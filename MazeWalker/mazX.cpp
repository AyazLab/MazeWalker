#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <fstream>
#include "mazX.h"
#include "unzip/unzip.h"
	
#define ZPOS64_T uint64_t 

// unz_file_info_interntal contain internal info about a file in zipfile
typedef struct unz_file_info_internal_s
{
	uLong offset_curfile;// relative offset of local header 4 bytes
} unz_file_info_internal;


typedef struct
{
	bool is_handle; // either a handle or memory
	bool canseek;
	// for handles:
	HANDLE h; bool herr; unsigned long initial_offset; bool mustclosehandle;
	// for memory:
	void *buf; unsigned int len, pos; // if it's a memory block
} LUFILE;



class TUnzip
{
public:
	TUnzip(const char *pwd) : uf(0), unzbuf(0), currentfile(-1), czei(-1), password(0) { if (pwd != 0) { password = new char[strlen(pwd) + 1]; strcpy(password, pwd); } }
	~TUnzip() { if (password != 0) delete[] password; password = 0; if (unzbuf != 0) delete[] unzbuf; unzbuf = 0; }

	unzFile uf; int currentfile; ZIPENTRY cze; int czei;
	char *password;
	char *unzbuf;            // lazily created and destroyed, used by Unzip
	TCHAR rootdir[MAX_PATH]; // includes a trailing slash

	ZRESULT Open(void *z, unsigned int len, DWORD flags);
	ZRESULT Get(int index, ZIPENTRY *ze);
	ZRESULT Find(const TCHAR *name, bool ic, int *index, ZIPENTRY *ze);
	ZRESULT Unzip(int index, void *dst, unsigned int len, DWORD flags);
	ZRESULT SetUnzipBaseDir(const TCHAR *dir);
	ZRESULT Close();
};



#define ZIP_HANDLE   1
#define ZIP_FILENAME 2
#define ZIP_MEMORY   3


// case sensitivity when searching for filenames
#define CASE_SENSITIVE 1
#define CASE_INSENSITIVE 2


#define UNZ_OK                  (0)
#define UNZ_END_OF_LIST_OF_FILE (-100)
#define UNZ_ERRNO               (Z_ERRNO)
#define UNZ_EOF                 (0)
#define UNZ_PARAMERROR          (-102)
#define UNZ_BADZIPFILE          (-103)
#define UNZ_INTERNALERROR       (-104)
#define UNZ_CRCERROR            (-105)
#define UNZ_PASSWORD            (-106)


LUFILE *lufopen(void *z, unsigned int len, DWORD flags, ZRESULT *err)
{
	if (flags != ZIP_HANDLE && flags != ZIP_FILENAME && flags != ZIP_MEMORY) { *err = ZR_ARGS; return NULL; }
	//
	HANDLE h = 0; bool canseek = false; *err = ZR_OK;
	bool mustclosehandle = false;
	if (flags == ZIP_HANDLE || flags == ZIP_FILENAME)
	{
		if (flags == ZIP_HANDLE)
		{
			HANDLE hf = z;
			h = hf; mustclosehandle = false;
#ifdef DuplicateHandle
			BOOL res = DuplicateHandle(GetCurrentProcess(), hf, GetCurrentProcess(), &h, 0, FALSE, DUPLICATE_SAME_ACCESS);
			if (!res) mustclosehandle = true;
#endif
		}
		else
		{
			h = CreateFile((const TCHAR*)z, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h == INVALID_HANDLE_VALUE) { *err = ZR_NOFILE; return NULL; }
			mustclosehandle = true;
		}
		// test if we can seek on it. We can't use GetFileType(h)==FILE_TYPE_DISK since it's not on CE.
		DWORD res = SetFilePointer(h, 0, 0, FILE_CURRENT);
		canseek = (res != 0xFFFFFFFF);
	}
	LUFILE *lf = new LUFILE;
	if (flags == ZIP_HANDLE || flags == ZIP_FILENAME)
	{
		lf->is_handle = true; lf->mustclosehandle = mustclosehandle;
		lf->canseek = canseek;
		lf->h = h; lf->herr = false;
		lf->initial_offset = 0;
		if (canseek) lf->initial_offset = SetFilePointer(h, 0, NULL, FILE_CURRENT);
	}
	else
	{
		lf->is_handle = false;
		lf->canseek = true;
		lf->mustclosehandle = false;
		lf->buf = z; lf->len = len; lf->pos = 0; lf->initial_offset = 0;
	}
	*err = ZR_OK;
	return lf;
}


int lufclose(LUFILE *stream)
{
	if (stream == NULL) return EOF;
	if (stream->mustclosehandle) CloseHandle(stream->h);
	delete stream;
	return 0;
}

int luferror(LUFILE *stream)
{
	if (stream->is_handle && stream->herr) return 1;
	else return 0;
}

long int luftell(LUFILE *stream)
{
	if (stream->is_handle && stream->canseek) return SetFilePointer(stream->h, 0, NULL, FILE_CURRENT) - stream->initial_offset;
	else if (stream->is_handle) return 0;
	else return stream->pos;
}

int lufseek(LUFILE *stream, long offset, int whence)
{
	if (stream->is_handle && stream->canseek)
	{
		if (whence == SEEK_SET) SetFilePointer(stream->h, stream->initial_offset + offset, 0, FILE_BEGIN);
		else if (whence == SEEK_CUR) SetFilePointer(stream->h, offset, NULL, FILE_CURRENT);
		else if (whence == SEEK_END) SetFilePointer(stream->h, offset, NULL, FILE_END);
		else return 19; // EINVAL
		return 0;
	}
	else if (stream->is_handle) return 29; // ESPIPE
	else
	{
		if (whence == SEEK_SET) stream->pos = offset;
		else if (whence == SEEK_CUR) stream->pos += offset;
		else if (whence == SEEK_END) stream->pos = stream->len + offset;
		return 0;
	}
}


size_t lufread(void *ptr, size_t size, size_t n, LUFILE *stream)
{
	unsigned int toread = (unsigned int)(size*n);
	if (stream->is_handle)
	{
		DWORD red; BOOL res = ReadFile(stream->h, ptr, toread, &red, NULL);
		if (!res) stream->herr = true;
		return red / size;
	}
	if (stream->pos + toread > stream->len) toread = stream->len - stream->pos;
	memcpy(ptr, (char*)stream->buf + stream->pos, toread); DWORD red = toread;
	stream->pos += red;
	return red / size;
}



typedef unsigned __int32 lutime_t;       // define it ourselves since we don't include time.h

FILETIME timet2filetime(const lutime_t t)
{
	LONGLONG i = Int32x32To64(t, 10000000) + 116444736000000000;
	FILETIME ft;
	ft.dwLowDateTime = (DWORD)i;
	ft.dwHighDateTime = (DWORD)(i >> 32);
	return ft;
}

FILETIME dosdatetime2filetime(WORD dosdate, WORD dostime)
{ // date: bits 0-4 are day of month 1-31. Bits 5-8 are month 1..12. Bits 9-15 are year-1980
	// time: bits 0-4 are seconds/2, bits 5-10 are minute 0..59. Bits 11-15 are hour 0..23
	SYSTEMTIME st;
	st.wYear = (WORD)(((dosdate >> 9) & 0x7f) + 1980);
	st.wMonth = (WORD)((dosdate >> 5) & 0xf);
	st.wDay = (WORD)(dosdate & 0x1f);
	st.wHour = (WORD)((dostime >> 11) & 0x1f);
	st.wMinute = (WORD)((dostime >> 5) & 0x3f);
	st.wSecond = (WORD)((dostime & 0x1f) * 2);
	st.wMilliseconds = 0;
	FILETIME ft; SystemTimeToFileTime(&st, &ft);
	return ft;
}

ZRESULT TUnzip::Open(void *z, unsigned int len, DWORD flags)
{
	if (uf != 0 || currentfile != -1) return ZR_NOTINITED;
	//
#ifdef GetCurrentDirectory
	GetCurrentDirectory(MAX_PATH, rootdir);
#else
	_tcscpy(rootdir, _T("\\"));
#endif
	TCHAR lastchar = rootdir[_tcslen(rootdir) - 1];
	if (lastchar != '\\' && lastchar != '/') _tcscat(rootdir, _T("\\"));
	//
	if (flags == ZIP_HANDLE)
	{ // test if we can seek on it. We can't use GetFileType(h)==FILE_TYPE_DISK since it's not on CE.
		DWORD res = SetFilePointer(z, 0, 0, FILE_CURRENT);
		bool canseek = (res != 0xFFFFFFFF);
		if (!canseek) return ZR_SEEK;
	}
	ZRESULT e; LUFILE *f = lufopen(z, len, flags, &e);
	if (f == NULL) return e;
	uf = unzOpen((char*) z);//f); ***
	if (uf == 0) return ZR_NOFILE;
	return ZR_OK;
}

ZRESULT TUnzip::SetUnzipBaseDir(const TCHAR *dir)
{
	_tcscpy(rootdir, dir);
	TCHAR lastchar = rootdir[_tcslen(rootdir) - 1];
	if (lastchar != '\\' && lastchar != '/') _tcscat(rootdir, _T("\\"));
	return ZR_OK;
}

ZRESULT TUnzip::Get(int index, ZIPENTRY *ze)
{
	unz_global_info_s* gi;
	gi = (unz_global_info_s*) malloc(sizeof(unz_global_info_s));
	unzGetGlobalInfo(uf, gi);
	unz_file_pos* fpos;
	fpos = (unz_file_pos*)malloc(sizeof(unz_file_pos));
	unzGetFilePos(uf, fpos);
	if (index<-1 || index >= (int)gi->number_entry) return ZR_ARGS;
	if (currentfile != -1) unzCloseCurrentFile(uf); currentfile = -1;
	if (index == czei && index != -1) { memcpy(ze, &cze, sizeof(ZIPENTRY)); return ZR_OK; }
	if (index == -1)
	{
		ze->index = gi->number_entry;
		ze->name[0] = 0;
		ze->attr = 0;
		ze->atime.dwLowDateTime = 0; ze->atime.dwHighDateTime = 0;
		ze->ctime.dwLowDateTime = 0; ze->ctime.dwHighDateTime = 0;
		ze->mtime.dwLowDateTime = 0; ze->mtime.dwHighDateTime = 0;
		ze->comp_size = 0;
		ze->unc_size = 0;
		return ZR_OK;
	}
	if (index<(int)fpos->num_of_file) unzGoToFirstFile(uf);
	while ((int)fpos->num_of_file < index)
	{
		unzGoToNextFile(uf);
		unzGetFilePos(uf, fpos);
	}
	unz_file_info ufi; char fn[MAX_PATH];
	unzGetCurrentFileInfo(uf, &ufi, fn, MAX_PATH, NULL, 0, NULL, 0);
	// now get the extra header. We do this ourselves, instead of
	// calling unzOpenCurrentFile &c., to avoid allocating more than necessary.
	unsigned int extralen, iSizeVar; unsigned long offset;
	//int res = unz64local_CheckCurrentFileCoherencyHeader(uf, &iSizeVar, &offset, &extralen);
	//if (res != UNZ_OK) return ZR_CORRUPT;
	//if (lufseek(uf->filestream, offset, SEEK_SET) != 0) return ZR_READ;
	//unsigned char *extra = new unsigned char[extralen];
	//if (lufread(extra, 1, (uInt)extralen, uf->filestream) != extralen) { delete[] extra; return ZR_READ; }
	//
	ze->index = fpos->num_of_file;
	TCHAR tfn[MAX_PATH];
#ifdef UNICODE
	MultiByteToWideChar(CP_UTF8, 0, fn, -1, tfn, MAX_PATH);
#else
	strcpy(tfn, fn);
#endif
	// As a safety feature: if the zip filename had sneaky stuff
	// like "c:\windows\file.txt" or "\windows\file.txt" or "fred\..\..\..\windows\file.txt"
	// then we get rid of them all. That way, when the programmer does UnzipItem(hz,i,ze.name),
	// it won't be a problem. (If the programmer really did want to get the full evil information,
	// then they can edit out this security feature from here).
	// In particular, we chop off any prefixes that are "c:\" or "\" or "/" or "[stuff]\.." or "[stuff]/.."
	const TCHAR *sfn = tfn;
	for (;;)
	{
		if (sfn[0] != 0 && sfn[1] == ':') { sfn += 2; continue; }
		if (sfn[0] == '\\') { sfn++; continue; }
		if (sfn[0] == '/') { sfn++; continue; }
		const TCHAR *c;
		c = _tcsstr(sfn, _T("\\..\\")); if (c != 0) { sfn = c + 4; continue; }
		c = _tcsstr(sfn, _T("\\../")); if (c != 0) { sfn = c + 4; continue; }
		c = _tcsstr(sfn, _T("/../")); if (c != 0) { sfn = c + 4; continue; }
		c = _tcsstr(sfn, _T("/..\\")); if (c != 0) { sfn = c + 4; continue; }
		break;
	}
	_tcscpy(ze->name, sfn);


	// zip has an 'attribute' 32bit value. Its lower half is windows stuff
	// its upper half is standard unix stat.st_mode. We'll start trying
	// to read it in unix mode
	unsigned long a = ufi.external_fa;
	bool isdir = (a & 0x40000000) != 0;
	bool readonly = (a & 0x00800000) == 0;
	//bool readable=  (a&0x01000000)!=0; // unused
	//bool executable=(a&0x00400000)!=0; // unused
	bool hidden = false, system = false, archive = true;
	// but in normal hostmodes these are overridden by the lower half...
	int host = ufi.version >> 8;
	if (host == 0 || host == 7 || host == 11 || host == 14)
	{
		readonly = (a & 0x00000001) != 0;
		hidden = (a & 0x00000002) != 0;
		system = (a & 0x00000004) != 0;
		isdir = (a & 0x00000010) != 0;
		archive = (a & 0x00000020) != 0;
	}
	ze->attr = 0;
	if (isdir) ze->attr |= FILE_ATTRIBUTE_DIRECTORY;
	if (archive) ze->attr |= FILE_ATTRIBUTE_ARCHIVE;
	if (hidden) ze->attr |= FILE_ATTRIBUTE_HIDDEN;
	if (readonly) ze->attr |= FILE_ATTRIBUTE_READONLY;
	if (system) ze->attr |= FILE_ATTRIBUTE_SYSTEM;
	ze->comp_size = ufi.compressed_size;
	ze->unc_size = ufi.uncompressed_size;
	//
	WORD dostime = (WORD)(ufi.dosDate & 0xFFFF);
	WORD dosdate = (WORD)((ufi.dosDate >> 16) & 0xFFFF);
	FILETIME ftd = dosdatetime2filetime(dosdate, dostime);
	FILETIME ft; LocalFileTimeToFileTime(&ftd, &ft);
	ze->atime = ft; ze->ctime = ft; ze->mtime = ft;
	//// the zip will always have at least that dostime. But if it also has
	//// an extra header, then we'll instead get the info from that.
	//unsigned int epos = 0;
	//while (epos + 4<extralen)
	//{
	//	char etype[3]; etype[0] = extra[epos + 0]; etype[1] = extra[epos + 1]; etype[2] = 0;
	//	int size = extra[epos + 2];
	//	if (strcmp(etype, "UT") != 0) { epos += 4 + size; continue; }
	//	int flags = extra[epos + 4];
	//	bool hasmtime = (flags & 1) != 0;
	//	bool hasatime = (flags & 2) != 0;
	//	bool hasctime = (flags & 4) != 0;
	//	epos += 5;
	//	if (hasmtime)
	//	{
	//		lutime_t mtime = ((extra[epos + 0]) << 0) | ((extra[epos + 1]) << 8) | ((extra[epos + 2]) << 16) | ((extra[epos + 3]) << 24);
	//		epos += 4;
	//		ze->mtime = timet2filetime(mtime);
	//	}
	//	if (hasatime)
	//	{
	//		lutime_t atime = ((extra[epos + 0]) << 0) | ((extra[epos + 1]) << 8) | ((extra[epos + 2]) << 16) | ((extra[epos + 3]) << 24);
	//		epos += 4;
	//		ze->atime = timet2filetime(atime);
	//	}
	//	if (hasctime)
	//	{
	//		lutime_t ctime = ((extra[epos + 0]) << 0) | ((extra[epos + 1]) << 8) | ((extra[epos + 2]) << 16) | ((extra[epos + 3]) << 24);
	//		epos += 4;
	//		ze->ctime = timet2filetime(ctime);
	//	}
	//	break;
	//}
	////
	//if (extra != 0) delete[] extra;
	memcpy(&cze, ze, sizeof(ZIPENTRY)); czei = index;
	return ZR_OK;
}

ZRESULT TUnzip::Find(const TCHAR *tname, bool ic, int *index, ZIPENTRY *ze)
{
	unz_file_pos* fpos;
	unzGetFilePos(uf,fpos);
	char name[MAX_PATH];
#ifdef UNICODE
	WideCharToMultiByte(CP_UTF8, 0, tname, -1, name, MAX_PATH, 0, 0);
#else
	strcpy(name, tname);
#endif
	int res = unzLocateFile(uf, name, ic ? CASE_INSENSITIVE : CASE_SENSITIVE);
	if (res != UNZ_OK)
	{
		if (index != 0) *index = -1;
		if (ze != NULL) { ZeroMemory(ze, sizeof(ZIPENTRY)); ze->index = -1; }
		return ZR_NOTFOUND;
	}
	if (currentfile != -1) unzCloseCurrentFile(uf); currentfile = -1;
	int i = (int)fpos->num_of_file;
	if (index != NULL) *index = i;
	if (ze != NULL)
	{
		ZRESULT zres = Get(i, ze);
		if (zres != ZR_OK) return zres;
	}
	return ZR_OK;
}

void EnsureDirectory(const TCHAR *rootdir, const TCHAR *dir)
{
	if (rootdir != 0 && GetFileAttributes(rootdir) == 0xFFFFFFFF) CreateDirectory(rootdir, 0);
	if (*dir == 0) return;
	const TCHAR *lastslash = dir, *c = lastslash;
	while (*c != 0) { if (*c == '/' || *c == '\\') lastslash = c; c++; }
	const TCHAR *name = lastslash;
	if (lastslash != dir)
	{
		TCHAR tmp[MAX_PATH]; memcpy(tmp, dir, sizeof(TCHAR)*(lastslash - dir));
		tmp[lastslash - dir] = 0;
		EnsureDirectory(rootdir, tmp);
		name++;
	}
	TCHAR cd[MAX_PATH]; *cd = 0; if (rootdir != 0) _tcscpy(cd, rootdir); _tcscat(cd, dir);
	if (GetFileAttributes(cd) == 0xFFFFFFFF) CreateDirectory(cd, NULL);
}



ZRESULT TUnzip::Unzip(int index, void *dst, unsigned int len, DWORD flags)
{
	unz_global_info_s* gi;
	gi = (unz_global_info_s*)malloc(sizeof(unz_global_info_s));
	unzGetGlobalInfo(uf, gi);
	unz_file_pos* fpos;
	fpos = (unz_file_pos*)malloc(sizeof(unz_file_pos));
	unzGetFilePos(uf, fpos);
	if (flags != ZIP_MEMORY && flags != ZIP_FILENAME && flags != ZIP_HANDLE) return ZR_ARGS;
	if (flags == ZIP_MEMORY)
	{
		if (index != currentfile)
		{
			if (currentfile != -1) unzCloseCurrentFile(uf); currentfile = -1;
			if (index >= (int)gi->number_entry) return ZR_ARGS;
			if (index<(int)fpos->num_of_file) unzGoToFirstFile(uf);
			while ((int)fpos->num_of_file < index)
			{
				unzGoToNextFile(uf);
				unzGetFilePos(uf, fpos);
			}
			unzOpenCurrentFilePassword(uf, password); currentfile = index;
		}
		//bool reached_eof;
		int res = unzReadCurrentFile(uf, dst, len);// , &reached_eof);
		if (res <= 0) { unzCloseCurrentFile(uf); currentfile = -1; }
		if (res==0) return ZR_OK;
		if (res>0) return ZR_MORE;
		if (res == UNZ_PASSWORD) return ZR_PASSWORD;
		return ZR_FLATE;
	}
	// otherwise we're writing to a handle or a file
	if (currentfile != -1) unzCloseCurrentFile(uf); currentfile = -1;
	if (index >= (int)gi->number_entry) return ZR_ARGS;
	if (index<(int)fpos->num_of_file) unzGoToFirstFile(uf);
	while ((int)fpos->num_of_file < index)
	{
		unzGoToNextFile(uf);
		unzGetFilePos(uf, fpos);
	}
	ZIPENTRY ze; Get(index, &ze);
	// zipentry=directory is handled specially
	if ((ze.attr&FILE_ATTRIBUTE_DIRECTORY) != 0)
	{
		if (flags == ZIP_HANDLE) return ZR_OK; // don't do anything
		const TCHAR *dir = (const TCHAR*)dst;
		bool isabsolute = (dir[0] == '/' || dir[0] == '\\' || (dir[0] != 0 && dir[1] == ':'));
		if (isabsolute) EnsureDirectory(0, dir); else EnsureDirectory(rootdir, dir);
		return ZR_OK;
	}
	// otherwise, we write the zipentry to a file/handle
	HANDLE h;
	if (flags == ZIP_HANDLE) h = dst;
	else
	{
		const TCHAR *ufn = (const TCHAR*)dst;
		// We'll qualify all relative names to our root dir, and leave absolute names as they are
		// ufn="zipfile.txt"  dir=""  name="zipfile.txt"  fn="c:\\currentdir\\zipfile.txt"
		// ufn="dir1/dir2/subfile.txt"  dir="dir1/dir2/"  name="subfile.txt"  fn="c:\\currentdir\\dir1/dir2/subfiles.txt"
		// ufn="\z\file.txt"  dir="\z\"  name="file.txt"  fn="\z\file.txt"
		// This might be a security risk, in the case where we just use the zipentry's name as "ufn", where
		// a malicious zip could unzip itself into c:\windows. Our solution is that GetZipItem (which
		// is how the user retrieve's the file's name within the zip) never returns absolute paths.
		const TCHAR *name = ufn; const TCHAR *c = name; while (*c != 0) { if (*c == '/' || *c == '\\') name = c + 1; c++; }
		TCHAR dir[MAX_PATH]; _tcscpy(dir, ufn); if (name == ufn) *dir = 0; else dir[name - ufn] = 0;
		TCHAR fn[MAX_PATH];
		bool isabsolute = (dir[0] == '/' || dir[0] == '\\' || (dir[0] != 0 && dir[1] == ':'));
		if (isabsolute) { wsprintf(fn, _T("%s%s"), dir, name); EnsureDirectory(0, dir); }
		else { wsprintf(fn, _T("%s%s%s"), rootdir, dir, name); EnsureDirectory(rootdir, dir); }
		//
		h = CreateFile(fn, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, ze.attr, NULL);
	}
	if (h == INVALID_HANDLE_VALUE) return ZR_NOFILE;
	unzOpenCurrentFilePassword(uf, password);
	if (unzbuf == 0) unzbuf = new char[16384]; DWORD haderr = 0;
	//  

	for (; haderr == 0;)
	{
		//bool reached_eof;
		int res = unzReadCurrentFile(uf, unzbuf, 16384);// , &reached_eof);
		if (res == UNZ_PASSWORD) { haderr = ZR_PASSWORD; break; }
		if (res<0) { haderr = ZR_FLATE; break; }
		if (res>0) { DWORD writ; BOOL bres = WriteFile(h, unzbuf, res, &writ, NULL); if (!bres) { haderr = ZR_WRITE; break; } }
		if (res==0) break;
		//if (res == 0) { haderr = ZR_FLATE; break; }
	}
	if (!haderr) SetFileTime(h, &ze.ctime, &ze.atime, &ze.mtime); // may fail if it was a pipe
	if (flags != ZIP_HANDLE) CloseHandle(h);
	unzCloseCurrentFile(uf);
	if (haderr != 0) return haderr;
	return ZR_OK;
}

ZRESULT TUnzip::Close()
{
	if (currentfile != -1) unzCloseCurrentFile(uf); currentfile = -1;
	if (uf != 0) unzClose(uf); uf = 0;
	return ZR_OK;
}





ZRESULT lasterrorU = ZR_OK;

unsigned int FormatZipMessageU(ZRESULT code, TCHAR *buf, unsigned int len)
{
	if (code == ZR_RECENT) code = lasterrorU;
	const TCHAR *msg = _T("unknown zip result code");
	switch (code)
	{
	case ZR_OK: msg = _T("Success"); break;
	case ZR_NODUPH: msg = _T("Culdn't duplicate handle"); break;
	case ZR_NOFILE: msg = _T("Couldn't create/open file"); break;
	case ZR_NOALLOC: msg = _T("Failed to allocate memory"); break;
	case ZR_WRITE: msg = _T("Error writing to file"); break;
	case ZR_NOTFOUND: msg = _T("File not found in the zipfile"); break;
	case ZR_MORE: msg = _T("Still more data to unzip"); break;
	case ZR_CORRUPT: msg = _T("Zipfile is corrupt or not a zipfile"); break;
	case ZR_READ: msg = _T("Error reading file"); break;
	case ZR_PASSWORD: msg = _T("Correct password required"); break;
	case ZR_ARGS: msg = _T("Caller: faulty arguments"); break;
	case ZR_PARTIALUNZ: msg = _T("Caller: the file had already been partially unzipped"); break;
	case ZR_NOTMMAP: msg = _T("Caller: can only get memory of a memory zipfile"); break;
	case ZR_MEMSIZE: msg = _T("Caller: not enough space allocated for memory zipfile"); break;
	case ZR_FAILED: msg = _T("Caller: there was a previous error"); break;
	case ZR_ENDED: msg = _T("Caller: additions to the zip have already been ended"); break;
	case ZR_ZMODE: msg = _T("Caller: mixing creation and opening of zip"); break;
	case ZR_NOTINITED: msg = _T("Zip-bug: internal initialisation not completed"); break;
	case ZR_SEEK: msg = _T("Zip-bug: trying to seek the unseekable"); break;
	case ZR_MISSIZE: msg = _T("Zip-bug: the anticipated size turned out wrong"); break;
	case ZR_NOCHANGE: msg = _T("Zip-bug: tried to change mind, but not allowed"); break;
	case ZR_FLATE: msg = _T("Zip-bug: an internal error during flation"); break;
	}
	unsigned int mlen = (unsigned int)_tcslen(msg);
	if (buf == 0 || len == 0) return mlen;
	unsigned int n = mlen; if (n + 1>len) n = len - 1;
	_tcsncpy(buf, msg, n); buf[n] = 0;
	return mlen;
}


typedef struct
{
	DWORD flag;
	TUnzip *unz;
} TUnzipHandleData;




HZIP OpenZipInternal(void *z, unsigned int len, DWORD flags, const char *password)
{
	TUnzip *unz = new TUnzip(password);
	lasterrorU = unz->Open(z, len, flags);
	if (lasterrorU != ZR_OK) { delete unz; return 0; }
	TUnzipHandleData *han = new TUnzipHandleData;
	han->flag = 1; han->unz = unz; return (HZIP)han;
}

ZRESULT GetZipItem(HZIP hz, int index, ZIPENTRY *ze)
{
	ze->index = 0; *ze->name = 0; ze->unc_size = 0;
	if (hz == 0) { lasterrorU = ZR_ARGS; return ZR_ARGS; }
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
	if (han->flag != 1) { lasterrorU = ZR_ZMODE; return ZR_ZMODE; }
	TUnzip *unz = han->unz;
	lasterrorU = unz->Get(index, ze);
	return lasterrorU;
}

HZIP OpenZip(const TCHAR *fn, const char *password)
{
	return OpenZipInternal((void*)fn, 0, ZIP_FILENAME, password);
}

HZIP OpenZip(void *z, unsigned int len, const char *password)
{
	return OpenZipInternal(z, len, ZIP_MEMORY, password);
}

HZIP OpenZipHandle(HANDLE h, const char *password)
{
	return OpenZipInternal((void*)h, 0, ZIP_HANDLE, password);
}

ZRESULT FindZipItem(HZIP hz, const TCHAR *name, bool ic, int *index, ZIPENTRY *ze)
{
	if (hz == 0) { lasterrorU = ZR_ARGS; return ZR_ARGS; }
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
	if (han->flag != 1) { lasterrorU = ZR_ZMODE; return ZR_ZMODE; }
	TUnzip *unz = han->unz;
	lasterrorU = unz->Find(name, ic, index, ze);
	return lasterrorU;
}

ZRESULT UnzipItemInternal(HZIP hz, int index, void *dst, unsigned int len, DWORD flags)
{
	if (hz == 0) { lasterrorU = ZR_ARGS; return ZR_ARGS; }
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
	if (han->flag != 1) { lasterrorU = ZR_ZMODE; return ZR_ZMODE; }
	TUnzip *unz = han->unz;
	lasterrorU = unz->Unzip(index, dst, len, flags);
	return lasterrorU;
}
ZRESULT UnzipItemHandle(HZIP hz, int index, HANDLE h) { return UnzipItemInternal(hz, index, (void*)h, 0, ZIP_HANDLE); }
ZRESULT UnzipItem(HZIP hz, int index, const TCHAR *fn) { return UnzipItemInternal(hz, index, (void*)fn, 0, ZIP_FILENAME); }
ZRESULT UnzipItem(HZIP hz, int index, void *z, unsigned int len) { return UnzipItemInternal(hz, index, z, len, ZIP_MEMORY); }

ZRESULT SetUnzipBaseDir(HZIP hz, const TCHAR *dir)
{
	if (hz == 0) { lasterrorU = ZR_ARGS; return ZR_ARGS; }
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
	if (han->flag != 1) { lasterrorU = ZR_ZMODE; return ZR_ZMODE; }
	TUnzip *unz = han->unz;
	lasterrorU = unz->SetUnzipBaseDir(dir);
	return lasterrorU;
}


ZRESULT CloseZipU(HZIP hz)
{
	if (hz == 0) { lasterrorU = ZR_ARGS; return ZR_ARGS; }
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
	if (han->flag != 1) { lasterrorU = ZR_ZMODE; return ZR_ZMODE; }
	TUnzip *unz = han->unz;
	lasterrorU = unz->Close();
	delete unz;
	delete han;
	return lasterrorU;
}

ZRESULT CloseZip(HZIP hz)
{
	return CloseZipU(hz);
}

bool IsZipHandleU(HZIP hz)
{
	if (hz == 0) return false;
	TUnzipHandleData *han = (TUnzipHandleData*)hz;
	return (han->flag == 1);
}


#include <io.h>
long CheckFile2(char* file)
{
	
	
	FILE* fp = fopen(file, "r");
	if (fp != NULL)
	{
		long fSize=0;

		fseek(fp, 0, SEEK_END);   // non-portable
		fSize = ftell(fp);

		fclose(fp);

		


		if (fSize != NULL)
			return fSize;
		else
			return 0;
		/*try
		{
			
			return in.tellg();
		}
		catch (int e)
		{
			return 1;
			//cout << "An exception occurred. Exception Nr. " << e << '\n';
		}
		
		*/

		//return true;
	}
	else
		
		return -1;
}

char* ExtractToTempDIR(char* fname)
{

	HZIP hz = OpenZip(fname, 0);
	
	SetUnzipBaseDir(hz, "_temp\\");
	ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
	int len = 0;
	char out[MAX_PATH] = "_temp\\";
	for (int zi = 0; zi<numitems; zi++)
	{
		GetZipItem(hz, zi, &ze);

		char* checkStr = new char[MAX_PATH];
		sprintf(checkStr, "_temp/%s", ze.name);


		long fSize = CheckFile2(checkStr);
		if (fSize<0||ze.unc_size!=fSize)
			UnzipItem(hz, zi, ze.name);

		len = strlen(ze.name);
		if (len>4&&tolower(ze.name[len - 3]) == 'm'&&tolower(ze.name[len - 2]) == 'a'&&tolower(ze.name[len - 1]) == 'z')
		{
			strcat_s(out, sizeof(char)*MAX_PATH, ze.name);
		}
	}
	CloseZip(hz);

	

	if (strlen(out)>1)
		return out;
	else
		return NULL;
}
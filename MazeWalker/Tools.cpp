
#include "Tools.h"
#include <math.h>
#include <ctype.h>
#include <climits>
#include <direct.h>
#include <vector>
#include <string>
//#include <minwinbase
#include <windows.h>

bool trueFalse(char *str)
{
	if (atoi(str) == 1)
		return true;
	
	char cStr[10];
	sprintf(cStr,"%s", str);


	if (cStr[0]=='T'||cStr[0]=='t')
		return true;
	else
		return false;
}

bool trueFalse(char *str, bool defaultResponse)
{
	if (str)
		return trueFalse(str);
	else
		return defaultResponse;
}

void autoSizeToWidth(char* str, int sz, int txtWidth)
{
	char tempStr[BIGSTRBUFFER];

	//	char prev;
	if (str == NULL)
	{
		return;
	}
	int len = strlen(str);
	sprintf_s(tempStr, sz,"%s", str);
	int j = 0;
	int lineLengthCount = 0;
	int lastRelativeSpace = 0;
	int numLines = 0;
	for (int i = 0; i < len; i++) //add \n to new string when line is longer than width
	{
		if (str[i] == '\n' || str[i] == '\a' || str[i] == NULL)
		{
			lineLengthCount = 0;
			lastRelativeSpace = txtWidth;
			numLines++;
		}
		else
		{
			if (str[i] == ' ')
				lastRelativeSpace = txtWidth - lineLengthCount;
			lineLengthCount++;
		}

		if (i + j >= sz)
		{
			tempStr[sz - 1] = NULL;
			break;
		}

		if (lineLengthCount >= txtWidth)
		{
			lineLengthCount = 0;
			numLines++;
			if (lastRelativeSpace < txtWidth / 3)
			{
				tempStr[i - lastRelativeSpace + j] = '\n'; //change that space to a \n
				lastRelativeSpace = txtWidth;
			}
			else
			{
				tempStr[i + j] = '\n';
				j++;
			}
		}

		tempStr[i + j]=str[i];
	}

	if (len + j + 1 >= sz)
		tempStr[sz - 1] = NULL;
	else
		tempStr[len+j+1] = NULL;
	strcpy_s(str,sz, tempStr);
}

void addLineBreaksAndMC(char* str,int sz) //replaces \\n with \n   and \\a with \a (multiple choice linebreak)
{
	//int sz = sizeof(&str);
	char tempStr[BIGSTRBUFFER];

	//	char prev;
	if (str == NULL)
	{
		return;
	}
	memcpy(tempStr,str,sz);
	int j = 0;
	for (int i = 0; i < sz - 1; i++) //replace \\n with line break except in errors and replace \\a with \a
	{
		if (tempStr[i] == '\\')
		{
			if (tempStr[i + 1] == 'n')
			{
				tempStr[i + 1] = '\n';
				tempStr[i] = '\n';
				i = i + 1; //skip one letter
				j = j - 1; //note the offset
			}
			else if (tempStr[i + 1] == 'a')
			{
				tempStr[i + 1] = '\a';
				tempStr[i] = '\a';
				i = i + 1;
				j = j - 1;
			}
		}
		str[i + j] = tempStr[i];
	}
	str[sz + j - 1] = 0;
	//strcpy(tempStr, str);
	//str[strlen(displayText)] = 0;
}

void trim(char *str)
{
	int i,j=0;
	int size=0;
	for(size=0;size<500;size++)
	{
		if(str[size]==NULL)
			break;
	}
		
	int flag = 0;
	for(i=0;str[i]!='\0';i++)
	{
	
		if (flag != 1 && (str[i] == ' ' || str[i] == '\t'))
		{
			for(j=i; str[j] != NULL; j++)
			{				
				str[j] = str[j+1];
			}
		}
		else if(str[i]=='.')
		{
			flag=-1;
		}
		else if(flag!=-1)
		{
			flag =1;
		}
	}
	str[i-1]='\0';
} 

bool GetUntilTab(FILE* fp,char * out,int len)
{
	if(fp==NULL)
		return false;
	if(out==NULL)
		return false;

	bool ret=false;
	int n=0;
	out[0]=1;
	while(true)
	{		
		fread(&(out[n]),1,1,fp);
		if(out[n]=='\t')
		{
			ret=true;
			break;
		}
		else if( out[n]==-2|| out[n]=='\n' || out[n]==EOF || out[n]=='\r')
		{
			ret=false;
			break;
		}
		else if(n==len ) 
			return false;
		n++;
	}
	//n++;
	out[n]=0;
	return ret;
}

bool IsEqual(float inp1,float inp2)
{
	if( fabs(inp1-inp2)<0.0001)
	{
		return true;
	}
	return false;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *textFileRead(char *fn) {


	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

int textFileWrite(char *fn, char *s) {

	FILE *fp;
	int status = 0;

	if (fn != NULL) {
		fp = fopen(fn,"w");

		if (fp != NULL) {

			if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
				status = 1;
			fclose(fp);
		}
	}
	return(status);
}

char* strtolower(char* s)
{
	char* d = (char*)malloc(strlen(s) + 1);
	int i = 0;
	while (*s)
	{
		*d = tolower(*s);
		d++;
		s++;
		i++;
	}

	*d = 0;
	d = d - i;

	return d;
}

char* getFileDir(char* fname)
{
	char dir[_MAX_PATH];
	strncpy(dir, fname,_MAX_PATH-1);
	for (int i = strlen(fname)-1; i > 0; i--)
	{
		if (dir[i] == '\\' || dir[i] == '/')
		{
			dir[i] = 0;
			return dir;
		}
	}
	
	dir[0] = 0;

	return dir;
}


char* getFilenameOnly(char* fname)
{
	return getFilenameOnly(fname, true);
}


char* getFilenameOnly(char* fname,bool withExt)
{
	char dir[_MAX_PATH];
	bool firstDot = false;
	strncpy(dir, fname, _MAX_PATH - 1);
	for (int i = strlen(fname) - 1; i >= 0; i--)
	{
		dir[i] = tolower(dir[i]);
		if (dir[i] == '.'&&!firstDot&&!withExt)
		{
			dir[i] = 0;
			firstDot = true;
		}
		if (dir[i] == '\\' || dir[i] == '/')
		{
			return &dir[i+1];
		}
	}

	//dir[0] = 0;

	return dir;
}


char* getFileExt(char* fname)
{
	char dir[_MAX_PATH];
	strncpy(dir, fname, _MAX_PATH - 1);
	for (int i = strlen(fname) - 1; i >= 0; i--)
	{
		dir[i] = tolower(dir[i]);
		if (dir[i] == '.')
		{
			return &dir[i];
		}
	}

	dir[0] = 0;

	return dir;
}



char* printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 1)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		//free(infoLog);
		return infoLog;
	}
	else
		return NULL;
}


char mazeWorkingDir[_MAX_PATH] = "";
char melWorkingDir[_MAX_PATH] = "";
char userLibraryPath[_MAX_PATH] = "";
char libraryPath[_MAX_PATH] = "";


char* getBestPath(char* filename)
{
	char* bestPath = getBestPath(filename, "");
	char ret[_MAX_PATH];
	if (bestPath)
	{
		strncpy(ret, bestPath, _MAX_PATH - 1);
		return ret;
	}
	else
		return NULL;
	
}

char* getBestPath(std::string filename_string)
{
	char fname[_MAX_PATH];
	strncpy(fname, filename_string.c_str(), _MAX_PATH - 1);
	char* bestPath = getBestPath(fname, "");
	char ret[_MAX_PATH];
	if (bestPath)
	{
		strncpy(ret, bestPath, _MAX_PATH - 1);
		return ret;
	}
	else
		return NULL;

}

char* getBestPath(std::string filename_string, char* folder)
{
	char fname[_MAX_PATH];
	strncpy(fname, filename_string.c_str(), _MAX_PATH - 1);

	char* bestPath = getBestPath(fname, folder);

	char ret[_MAX_PATH];
	if (bestPath)
	{
		strncpy(ret, bestPath, _MAX_PATH - 1);
		return ret;
	}
	else
		return NULL;

}
char* getBestPath(char* filename, std::vector<std::string> foldersToCheck)
{
	if (strlen(filename) < 3) //for too short paths
		return NULL;
	if (filename[1] == ':' && CheckFileExists(filename)) //for absolute paths
		return filename;
	else if (filename[1] == ':')
		return NULL;

	char pathBuffer[_MAX_PATH];
	std::vector<std::string> folderFileNameToCheck;
	std::vector<std::string> fullPathsToCheck;

	strncpy(filename, swapSlash(filename), _MAX_PATH - 1);

	//pathsToCheck.push_back(filename); 

	bool useFolder = foldersToCheck.size()>0;

	folderFileNameToCheck.push_back(filename); //first try the file at the root

	//Next Assemble the possible folders to look in (ex: img/images/textures/tex)
	for (std::vector<std::string>::iterator it = foldersToCheck.begin(); it != foldersToCheck.end(); ++it) {

		sprintf_s(pathBuffer, _MAX_PATH, "%s/%s", it->c_str(), filename);
		folderFileNameToCheck.push_back(pathBuffer);
	}

	std::vector<std::string> dirsToCheck;

	//Next try the local maze folder
	if (strlen(mazeWorkingDir) > 0)
	{
		dirsToCheck.push_back(mazeWorkingDir);
	}

	//Next try the local mel folder
	if (strlen(melWorkingDir) > 0)
	{
		dirsToCheck.push_back(melWorkingDir);
	}

	//Next try the user library folder
	if (strlen(userLibraryPath) > 0)
	{
		dirsToCheck.push_back(userLibraryPath);
	}

	//Next try the standard library folder (where .exe file is
	if (strlen(libraryPath) > 0)
	{
		dirsToCheck.push_back(libraryPath);
	}

	for (std::vector<std::string>::iterator idir = dirsToCheck.begin(); idir != dirsToCheck.end(); ++idir) {
		for (std::vector<std::string>::iterator ifold = folderFileNameToCheck.begin(); ifold != folderFileNameToCheck.end(); ++ifold) {

			sprintf(pathBuffer, "%s/%s", idir->c_str(), ifold->c_str());
			if (CheckFileExists(pathBuffer))
				return pathBuffer;

		}
	}
	return NULL;


}

char* getBestPath(char* filename,char* folder)
{
	std::vector<std::string> foldersToCheck;
	foldersToCheck.push_back(folder);

	char* bestPath;
	bestPath = getBestPath(filename, foldersToCheck);

	char ret[_MAX_PATH];
	if (bestPath)
	{
		strncpy(ret, bestPath, _MAX_PATH - 1);
		return ret;
	}
	else
		return NULL;

}

char* getBestPathNoExt(char* filename, char* folder)
{
	std::vector<std::string> foldersToCheck;
	foldersToCheck.push_back(folder);

	char* bestPath;
	bestPath = getBestPathNoExt(filename, foldersToCheck);

	char ret[_MAX_PATH];
	if (bestPath)
	{
		strncpy(ret, bestPath, _MAX_PATH - 1);
		return ret;
	}
	else
		return NULL;
}

char* getBestPathNoExt(char* filename, std::vector<std::string> foldersToCheck)
{
	if (strlen(filename) < 3) //for too short paths
		return NULL;
	if (filename[1] == ':' && CheckFileExists(filename)) //for absolute paths
		return filename;
	else if (filename[1] == ':')
		return NULL;

	char pathBuffer[_MAX_PATH];
	std::vector<std::string> folderFileNameToCheck;
	std::vector<std::string> fullPathsToCheck;

	strncpy(filename, swapSlash(filename), _MAX_PATH - 1);

	//pathsToCheck.push_back(filename); 

	bool useFolder = foldersToCheck.size() > 0;

	folderFileNameToCheck.push_back(filename); //first try the file at the root

	//Next Assemble the possible folders to look in (ex: img/images/textures/tex)
	for (std::vector<std::string>::iterator it = foldersToCheck.begin(); it != foldersToCheck.end(); ++it) {

		sprintf_s(pathBuffer, _MAX_PATH, "%s/%s", it->c_str(), filename);
		folderFileNameToCheck.push_back(pathBuffer);
	}

	std::vector<std::string> dirsToCheck;

	//Next try the local maze folder
	if (strlen(mazeWorkingDir) > 0)
	{
		dirsToCheck.push_back(mazeWorkingDir);
	}

	//Next try the local mel folder
	if (strlen(melWorkingDir) > 0)
	{
		dirsToCheck.push_back(melWorkingDir);
	}

	//Next try the user library folder
	if (strlen(userLibraryPath) > 0)
	{
		dirsToCheck.push_back(userLibraryPath);
	}

	//Next try the standard library folder (where .exe file is
	if (strlen(libraryPath) > 0)
	{
		dirsToCheck.push_back(libraryPath);
	}
	char* bestPath;
	for (std::vector<std::string>::iterator idir = dirsToCheck.begin(); idir != dirsToCheck.end(); ++idir) {
		for (std::vector<std::string>::iterator ifold = folderFileNameToCheck.begin(); ifold != folderFileNameToCheck.end(); ++ifold) {

			sprintf(pathBuffer, "%s/%s", idir->c_str(), ifold->c_str());
			bestPath = CheckFileExistsNoExt(pathBuffer);
			char ret[_MAX_PATH];
			if (bestPath)
			{
				strncpy(ret, bestPath, _MAX_PATH - 1);
				return ret;
			}

		}
	}
	return NULL;


}

char* swapSlash(char* pathIn)
{
	char d[_MAX_PATH];
	strcpy(d, pathIn);

	char* dP = d;

	int i = 0;
	while (*dP)
	{
		if (*dP == '\\')
			*dP = '/';
		else
			*dP = tolower(*dP);
		dP++;
		i++;
	}

	*dP = 0;
	dP = d;

	return dP;
}


bool CheckFileExists(char* file)
{
	FILE* fp = fopen(file, "r");
	if (fp != NULL)
	{
		fclose(fp);
		return true;
	}
	else
		return false;
}

bool CheckFileExists(char* file,bool noExt)
{
	if (noExt)
		return(CheckFileExistsNoExt(file));
	else
		return(CheckFileExists(file));

}

char* CheckFileExistsNoExt(char* file)
{
	char fileDir[_MAX_PATH];
	char filename[_MAX_PATH];

	strcpy(fileDir, getFileDir(file));
	strcpy(filename, getFilenameOnly(file,false));


	std::vector<std::string> out;
	std::string searchStr = std::string(getFilenameOnly(file, true));
	std::string nextBest = std::string(getFilenameOnly(file, false));
	char next[600];
	sprintf(next,"");

	out= std::vector<std::string>();
	GetFilesInDirectory(out, fileDir);
	if (out.size() < 1)
		return NULL;
	else
	{
		for(int j=0;j<out.size();j++)
		{
			char f[600];

			strcpy(f,(out.at(j).c_str()));
			strcpy(f, strtolower(f));
			std::string fmatch = std::string(&f[strlen(fileDir)]);
				
			if(fmatch.find(searchStr,0)!=std::string::npos)
			{
				return f;
			}
			else if(fmatch.find(nextBest,0)!=std::string::npos)
			{
				sprintf(next,"%s",f);
			}
		}
	}



	return NULL;
}


void GetFilesInDirectory(std::vector<std::string>& out, const std::string& directory)
{
	HANDLE dir;
	WIN32_FIND_DATA file_data;

	if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		return; /* No files found */

	do {
		const std::string file_name = file_data.cFileName;
		const std::string full_file_name = directory + "/" + file_name;
		const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

		if (file_name[0] == '.')
			continue;

		if (is_directory)
			continue;

		out.push_back(full_file_name);
	} while (FindNextFile(dir, &file_data));

	FindClose(dir);

} // GetFilesInDirectory



void updateMazeWorkingDir(char* newDir)
{
	if (newDir)
		strncpy(mazeWorkingDir,  swapSlash(newDir),_MAX_PATH-1);
	else
		mazeWorkingDir[0] = 0;
}

char* getMazeWorkingDir()
{
	char ret[_MAX_PATH];
	strcpy(ret, mazeWorkingDir);
	return ret;
}

void updateMelWorkingDir(char* newDir)
{
	if (newDir)
		strcpy_s(melWorkingDir, _MAX_PATH, swapSlash(newDir));
	else
		melWorkingDir[0] = 0;
}

char* getMelWorkingDir()
{
	char ret[_MAX_PATH];
	strcpy(ret, melWorkingDir);
	return ret;
}

void updateUserLibraryDir(char* newDir)
{
	if (newDir)
		strcpy_s(userLibraryPath, _MAX_PATH, swapSlash(newDir));
	else
		userLibraryPath[0] = 0;
}

char* getUserLibraryDir()
{
	char ret[_MAX_PATH];
	strcpy(ret, userLibraryPath);
	return ret;
}

void updateLibraryDir(char* newDir)
{
	if (newDir)
		strcpy_s(libraryPath, _MAX_PATH, swapSlash(newDir));
	else
		libraryPath[0] = 0;


}

char* getLibraryDir()
{
	char ret[_MAX_PATH];
	strcpy(ret, libraryPath);
	return ret;
}

char* getBestPathImg(char* input)
{
	return getTextureFilename(input);
}

char* getTextureFilename(char* input)
{


	char fname[_MAX_PATH] = "";

	std::vector<std::string> foldersToCheck;
	foldersToCheck.push_back("tex");
	foldersToCheck.push_back("textures");
	foldersToCheck.push_back("images");
	foldersToCheck.push_back("img");


	char* bestPath = getBestPath(input, foldersToCheck);
	char ret[_MAX_PATH];
	if (bestPath)
	{
		strncpy(ret, bestPath, _MAX_PATH - 1);
		return ret;
	}
	else
		return NULL;

	return NULL;


	//
	//return next;
}
#pragma once

#include "TextureDictionaryItem.h"
#include "GL/glew.h"

class TextureDictionary
{
public:
	TextureDictionaryItem* cListRoot;

	int Get_glKey(int i_mzKey);
	int Get_glKey_from_path(char* fname);
	//int Get_mzKey(int i_glKey);
	void Clear();
	void ClearUnused();
	void MarkAllUnused();
	void UnbindKey(int mzKey); // unbind ID and set to unused
	//void RemoveDuplicates();
	int GetLength();
	TextureDictionaryItem* GetLast();
	int Add(char* fname, int glKey, int mzKey);
	void SetWhite(GLuint* White){whiteID=White;};
	int GetWhite(){return whiteID[0];}

	GLuint* whiteID;

	TextureDictionary(void);
	~TextureDictionary(void);
};

#pragma once

class TextureDictionaryItem
{
public:
	TextureDictionaryItem(void);
	TextureDictionaryItem(char* filename,int i_glKey,int i_mzKey);
	~TextureDictionaryItem(void);

	int glKey;	// texture used in opengl, unqiue for each texture
	int mzKey;  // id value used in maze
	char fname[256]; // File name of texture
	bool unused; //if unused is markerd, cleared after maze load
	bool isDuplicate; //when maze has more than one key perGL
	
	TextureDictionaryItem *next;

};

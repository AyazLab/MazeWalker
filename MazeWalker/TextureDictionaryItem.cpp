#include "TextureDictionaryItem.h"
#include <stdio.h>
#include <string.h>

TextureDictionaryItem::TextureDictionaryItem(void)
{
	next=NULL;
	glKey = -100;
	mzKey = -100;
	unused = false;
	isDuplicate = false;
	sprintf(fname,"ROOT");
}

TextureDictionaryItem::TextureDictionaryItem(char* filename, int i_glKey, int i_mzValue)
{
	memset(&fname[0], 0, sizeof(fname));
	sprintf(fname, "%s", filename);
	next=NULL;
	glKey = i_glKey;
	mzKey = i_mzValue;
	unused = false;
	isDuplicate = false;
}

TextureDictionaryItem::~TextureDictionaryItem(void)
{
}


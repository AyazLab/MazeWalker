#include "stdio.h"
#include <stdlib.h>
#include "TextureDictionary.h"
#include <string.h>


TextureDictionary::TextureDictionary(void)
{
	cListRoot = new TextureDictionaryItem();
}

TextureDictionary::~TextureDictionary(void)
{

}

//int TextureDictionary::Get_mzKey(int i_glKey) //likely unused
//{
	//TextureDictionaryItem* cur = cListRoot;
	//while(cur->next)
	//{
	//	if(cur->glKey == i_glKey)
	//		return cur->mzKey;
//		cur=cur->next;
	//}
	//return -1;
//}

int TextureDictionary::Get_glKey(int i_mzKey) // returns the value used in TextureL[glKey]
{
	
	TextureDictionaryItem* cur = cListRoot;
	while(cur->next)
	{
		if(cur->mzKey == i_mzKey)
			return cur->glKey;
		cur=cur->next;
	}
	if(cur->mzKey == i_mzKey)
		return cur->glKey;
	return 0;
}

int TextureDictionary::Get_glKey_from_path(char* fname) // returns the value of previously loaded texture
{
	TextureDictionaryItem* cur = cListRoot;

	char fnameCMP[256];
	memset(&fnameCMP[0], 0, sizeof(fnameCMP));

	sprintf(fnameCMP, "%s", fname);
	cur = cur->next;
	while (cur)
	{
		if (!cur->isDuplicate&&strcmp(fnameCMP, cur->fname) == 0)
		{
			cur->unused = false; //checked textures are considered used
			return cur->glKey;
		}
		cur = cur->next;
	}
	//if (!cur->isDuplicate&&strcmp(fname, cur->fname) == 0)
	//{
		//cur->unused = false;
		//return cur->glKey;
	//}
	return -1; // doesn't exist
}



void TextureDictionary::Clear(void)
{
	//clear the list...
	TextureDictionaryItem *temp,*cur;
	cur = cListRoot->next;

	if(cur!=NULL)
	{	
		GLuint tex;
		tex=cur->glKey;
		glDeleteTextures(1, &tex);
		while(cur->next)
		{
			temp = cur;

			GLuint tex;
			tex=cur->glKey;
			glDeleteTextures(1, &tex); //frees texture from GL

			cur=cur->next;
			//delete temp;
			free(temp);
		}
	}
	cListRoot->next=NULL;
}

void TextureDictionary::ClearUnused(void)
{
	
	TextureDictionaryItem *last, *cur;
	cur = cListRoot->next;
	last = cListRoot;

	while (cur != NULL)
	{
		if (cur->unused) 
		{
			if (!cur->isDuplicate) //only delete "originals", but remove duplicates
			{
				GLuint tex;
				tex = cur->glKey;
				glDeleteTextures(1, &tex);
			}


			last->next = cur->next;
			free(cur);
			cur = last->next;
		}
		else
		{
			last = cur;
			cur = cur->next;
		}
		
	}
	last->next = NULL;
}


void TextureDictionary::MarkAllUnused(void)
{

	TextureDictionaryItem *last, *cur;
	cur = cListRoot->next;
	last = cListRoot;

	while (cur != NULL)
	{
		cur->unused = true; //mark unused
		cur->mzKey = -100; //unbind all textures
		last = cur;
		cur = cur->next;
	}
	last->next = NULL;
}

void TextureDictionary::UnbindKey(int mzKey)
{

	TextureDictionaryItem *last, *cur;
	cur = cListRoot->next;
	last = cListRoot;

	while (cur != NULL)
	{
		if (cur->mzKey==mzKey)
		{
			cur->unused = true; //mark unused
			cur->mzKey = -100; //unbind all textures
		}
		last = cur;
		cur = cur->next;
	}
	last->next = NULL;
}


int TextureDictionary::GetLength()
{
	TextureDictionaryItem *ret=cListRoot;
	int count=1;
	while(ret->next!=NULL)
	{
		ret = ret->next;
		count++;
	}
	return count;
}

TextureDictionaryItem* TextureDictionary::GetLast()
{
	TextureDictionaryItem *ret=cListRoot;
	while(ret->next!=NULL)
	{
		ret = ret->next;
	}
	return ret;
}

int TextureDictionary::Add(char* fname,int glKey,int mzKey) //Add new texture or duplicate and bind to mzKey, textures bound to current mzKey will be unbound and marked unused
{
	TextureDictionaryItem *cur = GetLast();
	//int i=GetLength()+1;

	if (Get_glKey_from_path(fname) < 0) // if texture is new
	{
		if (Get_glKey(mzKey) != 0) //already in use
		{
			UnbindKey(mzKey);
		}


		cur->next = new TextureDictionaryItem(fname, glKey, mzKey);
	}
	else //if texture is a duplicate key
	{
		cur->next = new TextureDictionaryItem(fname, glKey, mzKey);
		cur->next->isDuplicate = true;
	}

	
	return glKey;
}
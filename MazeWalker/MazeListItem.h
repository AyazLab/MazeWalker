#pragma once

#include "stdio.h"
#include <string.h>

#define TXTMSGBUFFERLIMIT_MAZELISTITEM 800


enum ItemType 
{
	Maze,Text
};

class MazeListItem
{
public:
    ItemType type;
	char value[TXTMSGBUFFERLIMIT_MAZELISTITEM];

	int showStyle;
	long lifeTime;
	char BGfname[TXTMSGBUFFERLIMIT_MAZELISTITEM];
	bool isMazX = false;

	MazeListItem* next;

	void AddMaze(char *val, bool isMazX);
	void AddBG(char* val);
	void AddText(char * val, long lftm,int shStyle);

	MazeListItem(void);
	~MazeListItem(void);
	
};

#pragma once

#include "stdio.h"
#include <string.h>


enum ItemType 
{
	Maze,Text
};

class MazeListItem
{
public:
    ItemType type;
	char value[400];

	int showStyle;
	long lifeTime;
	char BGfname[800];
	bool isMazX = false;

	MazeListItem* next;

	void AddMaze(char *val, bool isMazX);
	void AddBG(char* val);
	void AddText(char * val, long lftm,int shStyle);

	MazeListItem(void);
	~MazeListItem(void);
	
};

#pragma once

#include "stdio.h"
#include <string.h>

#define TXTMSGBUFFERLIMIT_MAZELISTITEM 800


enum ItemType 
{
	None,Maze,Text,Command
};

enum textboxStyle 
{  
	ON_DIALOG_CLEAR_BG, ON_SCREEN_CLEAR_BG, ON_DIALOG_WITH_BG, WARNING, FATAL_ERROR, ACTIVEREGION_ON_SCREEN
};

class MazeListItem
{
public:
    ItemType type=None;
	char value[TXTMSGBUFFERLIMIT_MAZELISTITEM];

	textboxStyle showStyle= ON_DIALOG_CLEAR_BG;
	long lifeTime;
	char BGfname[TXTMSGBUFFERLIMIT_MAZELISTITEM];
	bool isMazX = false;

	char audioFilename[TXTMSGBUFFERLIMIT_MAZELISTITEM];

	char mzCommand[TXTMSGBUFFERLIMIT_MAZELISTITEM];
	bool waitForExitCmd = false;

	MazeListItem* next;

	void AddMaze(char *val, bool isMazX);
	void AddBG(char* val);
	void AddText(char * val, long lftm,textboxStyle shStyle);
	void AddCommand(char* cmd,bool waitForExit);

	MazeListItem(void);
	~MazeListItem(void);
	
};

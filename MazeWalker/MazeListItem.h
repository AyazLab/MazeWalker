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
	bool audioLoop = false;
	bool audioStopOnEnd = true;
	bool audioPauseOnEnd = false;

	char mzCommand[TXTMSGBUFFERLIMIT_MAZELISTITEM];
	bool waitForExitCmd = false;

	bool recordAudio = false;

	typedef struct
	{
		int timeoutOverride = -999;

		char startPosition[TXTMSGBUFFERLIMIT_MAZELISTITEM] = "";
		char startMessage[TXTMSGBUFFERLIMIT_MAZELISTITEM] = "";

		int initialPoints = -999;
		bool setInitialPoints = true;

		int startTime = 0;
		bool continueTimeFrom = false;
	} MazeOptions;
	
	MazeOptions mazeOptions;

	MazeListItem* next;

	void AddMaze(char *val, bool isMazX);
	void AddMaze(char* val, bool isMazX, MazeOptions m);
	void AddBG(char* val);
	void AddRecordAudio();
	void AddAudio(char* val, bool loopAudio, bool stopOnEnd, bool pauseOnEnd);
	void AddText(char * val, long lftm,textboxStyle shStyle);
	void AddCommand(char* cmd,bool waitForExit);

	MazeListItem(void);
	~MazeListItem(void);
	
};

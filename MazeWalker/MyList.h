#pragma once

#include "MazeListItem.h"


#define TEXTBOXSTYLE_ONDIALOG_CLEAR_BK	0
#define TEXTBOXSTYLE_ONSCREEN_CLEAR_BK	1
#define TEXTBOXSTYLE_ONDIALOG_WITH_BK	2
#define TEXTBOXSTYLE_WARNING	3
#define TEXTBOXSTYLE_FATAL_ERROR	4
#define TEXTBOXSTYLE_ACTIVEREGION_ONSCREEN 5

class MyList
{
public:
	MazeListItem *cListRoot;

	void AddMaze(char *val);
	void AddText(char * val, long lftm,int shStyle);
	void AddBG(char* val);

	void Clear();

	MazeListItem* GetLast();
	MazeListItem* GetFirst();
	int GetLength();

	MyList(void);
	~MyList(void);
	int ReadMazeListFile(char *filename);
	int ReadFile(char *filename);
	char mListDIR[800];
};

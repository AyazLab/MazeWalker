#pragma once

#include "MazeListItem.h"



#define TEXTBOXSTYLE_ONDIALOG_CLEAR_BK	0
#define TEXTBOXSTYLE_ONSCREEN_CLEAR_BK	1
#define TEXTBOXSTYLE_ONDIALOG_WITH_BK	2
#define TEXTBOXSTYLE_WARNING	3
#define TEXTBOXSTYLE_FATAL_ERROR	4
#define TEXTBOXSTYLE_ACTIVEREGION_ONSCREEN 5

class MazeList
{
public:
	MazeListItem *cListRoot;

	void AddMaze(char *val);
	void AddText(char * val, long lftm,int shStyle);
	void AddBG(char* val);
	void AddCommand(char* cmd, bool waitForComplete);

	void Clear();

	MazeListItem* GetLast();
	MazeListItem* GetFirst();
	int GetLength();

	MazeList(void);
	~MazeList(void);
	int ReadMazeListClassic(char *filename);
	int ReadMazeList(char *filename);
	int ReadMazeListXML(char* filename);
	char mListDIR[800];
};

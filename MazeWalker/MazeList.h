#pragma once

#include "MazeListItem.h"



class MazeList
{
public:
	MazeListItem *cListRoot;

	void AddMaze(char *val);
	void AddText(char * val, long lftm,textboxStyle shStyle);
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

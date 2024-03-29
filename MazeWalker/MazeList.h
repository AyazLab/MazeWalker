#pragma once

#include "MazeListItem.h"



class MazeList
{
public:
	MazeListItem *cListRoot;

	void AddMaze(char *val);
	void AddMaze(char* val, MazeListItem::MazeOptions m);
	void AddText(char * val, long lftm,textboxStyle shStyle);
	void AddBG(char* val, int index, textboxImageStyle bgStyle);
	void AddAudio(char* val, bool loopAudio, bool stopOnEnd, bool pauseOnEnd);
	void AddRecordAudio();
	void AddCommand(char* cmd, char* cmdParams, bool waitForComplete, bool hideCmd);

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

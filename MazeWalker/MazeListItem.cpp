#include ".\mazelistitem.h"


MazeListItem::MazeListItem(void)
{
	type = Maze;
	lifeTime=0;
	showStyle=0;
	value[0]=NULL;
	next=NULL;
	BGfname[0] = 0;

}

void MazeListItem::AddMaze(char *val,bool isMazX)
{
	this->isMazX = isMazX;
	type = Maze;
	lifeTime=0;
	showStyle=0;
	if (strlen(val) > 150)
		int i = 1;

	sprintf(value,"%s",val);
	value[TXTMSGBUFFERLIMIT_MAZELISTITEM-1] = 0;

	next=NULL;
}
void MazeListItem::AddText(char * val, long lftm,int shStyle)
{
	type = Text;
	lifeTime=lftm;
	showStyle=shStyle;
	strcpy_s(value, TXTMSGBUFFERLIMIT_MAZELISTITEM,val);
	strcpy_s(BGfname, TXTMSGBUFFERLIMIT_MAZELISTITEM,"");
	next=NULL;
}

void MazeListItem::AddBG(char *fname)
{
	if(strlen(fname)>3)
	{
		sprintf(BGfname,"texture\\%s",fname);
	}

}

void MazeListItem::AddCommand(char* cmd, bool waitForExit)
{
	type = Command;
	strcpy_s(mzCommand,cmd);
	waitForExitCmd = waitForExit;
	next = NULL;
}

MazeListItem::~MazeListItem(void)
{
}

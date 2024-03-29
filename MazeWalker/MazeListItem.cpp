#include ".\mazelistitem.h"


MazeListItem::MazeListItem(void)
{
	type = Maze;
	lifeTime=0; //Time in MS to show
	showStyle=ON_DIALOG_CLEAR_BG;
	value[0]=NULL;
	next=NULL;
	BGfname[0] = 0;

}

void MazeListItem::AddMaze(char *val,bool isMazX)
{
	this->isMazX = isMazX;
	type = Maze;
	lifeTime=0;
	showStyle=ON_DIALOG_CLEAR_BG;


	if (strlen(val) > 150)
		int i = 1;

	sprintf(value,"%s",val);
	value[TXTMSGBUFFERLIMIT_MAZELISTITEM-1] = 0;

	next=NULL;
}

void MazeListItem::AddMaze(char* val, bool isMazX, MazeOptions m)
{
	this->isMazX = isMazX;
	type = Maze;
	lifeTime = 0;
	showStyle = ON_DIALOG_CLEAR_BG;

	this->mazeOptions = m;


	if (strlen(val) > 150)
		int i = 1;

	sprintf(value, "%s", val);
	value[TXTMSGBUFFERLIMIT_MAZELISTITEM - 1] = 0;

	next = NULL;
}
void MazeListItem::AddText(char * val, long lifetime_ms,textboxStyle shStyle)
{
	type = Text;
	lifeTime=lifetime_ms;
	showStyle=shStyle;
	strcpy_s(value, TXTMSGBUFFERLIMIT_MAZELISTITEM,val);
	strcpy_s(BGfname, TXTMSGBUFFERLIMIT_MAZELISTITEM,"");
	strcpy_s(audioFilename, TXTMSGBUFFERLIMIT_MAZELISTITEM, "");

	next=NULL;
}

void MazeListItem::AddBG(char *fname)
{
	MazeListItem::AddBG(fname, 33000,IMAGE_STRETCH);
}

void MazeListItem::AddBG(char* fname, int index,textboxImageStyle imageStyle)
{
	
	if (strlen(fname) > 3)
	{
		bgIndex = index;
		sprintf_s(BGfname, 800, "texture\\%s", fname);
		bgStyle = imageStyle;
	}
	else {
		bgIndex = -1;
		bgStyle = IMAGE_STRETCH;
	}

}

void MazeListItem::AddAudio(char* fname, bool loopAudio, bool stopOnEnd, bool pauseOnEnd)
{
	if (strlen(fname) > 3)
	{
		sprintf_s(audioFilename,800, "%s", fname);

		audioLoop = loopAudio;
		audioStopOnEnd = stopOnEnd;
		audioPauseOnEnd = pauseOnEnd;
	}

}

void MazeListItem::AddRecordAudio()
{
	recordAudio = true;

}

void MazeListItem::AddCommand(char* cmd, char* cmdParams, bool waitForExit,bool hideCmd)
{
	type = Command;
	strcpy_s(mzCommand, TXTMSGBUFFERLIMIT_MAZELISTITEM,cmd);
	strcpy_s(mzCommandParams, TXTMSGBUFFERLIMIT_MAZELISTITEM, cmdParams);
	waitForExitCmd = waitForExit;
	mzCommandHide = hideCmd;
	next = NULL;
}

MazeListItem::~MazeListItem(void)
{
}

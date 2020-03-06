#include "Log.h"
#include "stdio.h"

FILE *plog=NULL;
volatile bool LogLock=false;
extern DWORD startTime;
extern DWORD mazeStart;
extern int mazeStartedAndRunning;
extern DWORD GetQPC();
extern bool bUseLPT;
extern int iLptDelay;

extern void sendToAll(int code, int val);




void EventLog(int type,int val, int state,char* message)
{
	//Type 0: Debug Logs

	//Type 1: System Events
				//10  paused
				//11  unpaused
				//50  MazeWalker Start
				//51  MazeWalker Closing
				//58  MazeWalker Loading
				//59  MazeLoaded (waiting for devices)
				//60  MazeWalker waiting for cue
				//61  MazeStart t=0
				//70  MazeEnded
				//71  MazeRestarted
				//72  MazeSkipped
			    //80  Message Start
				//81  Message Ended



	//Type 2: User/Interacton Events
		//40 + cueType Cue received
		//40	Cue skipped
		//45	Cue timeout
		//Object Activated : 120 +objectActivationType
		//Object Highlighted (inside active radius) : 100
		//Object Unhighlighted (left active radius : 110
		//Object Action Completed (moved/destroyed/switched) : 90
	//Type 3: Maze Events

	//Type 4: API only events

					// Old code events
						//type: 1= object
						//	val: object number
						//  state: 1=activated, 2= finished activation
						//type: 2= Sync Cue events
						//  val = cueType or 0 for skip
						//  state: more information about cue
						//		for Skip 0=Skip with escape
						//				 1= Skip with jump
						//				 2= Timeout
						//		for DeviceCue: state= DevID
						//		for Joystick: state= joystickButton
						//		for Serial: state= triggerChar
	sendToAll(type, val);

	if(type==5) //BAR events only (legacy)
	{
		ComSend(200);
		ComSend(state*10);
	}
	if(LogStatus&&strlen(message)>0)
	{
		char msg[800];
		int mazeTime = 0;
		if(mazeStartedAndRunning==1)
			mazeTime = GetQPC()-mazeStart;		
		sprintf(msg,"%d\t%d\tEvent\t%d\t%d\t%d\t%s\n",GetQPC()-startTime,mazeTime,type,val,state,message);
		AddToLog(msg);
	}
	else
	{
		char msg[800];
		int mazeTime = 0;
		if(mazeStartedAndRunning==1)
			mazeTime = GetQPC()-mazeStart;		
		sprintf(msg,"%d\t%d\tEvent\t%d\t%d\t%d\n",GetQPC()-startTime,mazeTime,type,val,state);
		AddToLog(msg);
	}

	if (type == 1 && (bUseLPT))
	{  //resets the LPT port for the next round
		Sleep(iLptDelay);
		LPT_Reset();
	}


}

void EventLog(int type,int val, int state)
{
	EventLog(type,val, state,"");
}

void EventLog(int type, int val)
{
	EventLog(type, val, 0, "");
}

int InitLog(char file[])
{
	plog=NULL;
	plog=fopen(file,"w");
	if(plog!=NULL)
		return 1;
	else
		return 0;
}

int LogStatus()
{
	if(plog)
		return 1;
	else return 0;
}

void AddToLog(char line[])
{
	while(LogLock)
	{
	}
	LogLock=true;
	if(plog)
	{
		fprintf(plog,"%s",line);
	}
	LogLock=false;

}

void CloseLog()
{
	while(LogLock)
	{

	}
	LogLock=true;
	if(plog)
		fclose(plog);
	LogLock=false;
}


#include "com.h"
#include "Log.h"

HANDLE hCom=NULL;
extern HWND	hWnd;	// Holds Our Window Handle


static bool flag=false;
extern bool bListen;
extern bool bSendSync;
extern int cueType;
extern bool cue;
HANDLE comRec;
extern DWORD startTime;
extern DWORD mazeStart;
extern bool bUseLPT;
extern int iLptDelay;

extern int triggerChar;
volatile int bufferVar=0;
OVERLAPPED her;



char lastLPTchar = 0;

//#define TRIGGER_CHAR1	84 //'T'
//#define TRIGGER_CHAR2	116 // 't'

int ComConnect(int iPort,DWORD baud,bool parity,int stopBits)
{

	COMMTIMEOUTS CommTimeouts;
	DCB dcb;
	BOOL fSuccess;

	char port[20];
	sprintf_s(port,"COM%d",iPort);

	hCom = CreateFile(port,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_FLAG_OVERLAPPED,NULL);

	//hCom = CreateFile(port,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,NULL,NULL);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		char errMsg[256];
		sprintf(errMsg, "Could Not Open Port %s", port);
		ComError(errMsg);
		return 0;
	}

	fSuccess = GetCommState(hCom, &dcb);
	if (!fSuccess) 
	{
		ComError("Unable to get Serial Port State");
		return 0;
	}

	dcb.BaudRate = baud;
	dcb.ByteSize = 8;
	if(!parity)
		dcb.Parity = NOPARITY;
	else
		dcb.Parity=PARITY_ODD;
	if(stopBits==0)
	dcb.StopBits = ONESTOPBIT;
	else if (stopBits==1)
		dcb.StopBits=ONE5STOPBITS;
	else if (stopBits==2)
		dcb.StopBits=TWOSTOPBITS;

	fSuccess = SetCommState(hCom, &dcb);
	if(!fSuccess) 
	{
		ComError("Unable to set COM port state");
		return 0;
	}

	//fSuccess = SetCommMask(hCom, EV_RXCHAR);
	//if (!fSuccess) 
	//{
	//	ComError("SetMask");
	//	return 0;
	//}

	GetCommTimeouts (hCom, &CommTimeouts);

	// Change the COMMTIMEOUTS structure settings.
	CommTimeouts.ReadIntervalTimeout = MAXDWORD;  
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;  
	CommTimeouts.ReadTotalTimeoutConstant = 0;  

	CommTimeouts.WriteTotalTimeoutMultiplier = 10;  
	CommTimeouts.WriteTotalTimeoutConstant = 1000;    

	// Set the time-out parameters for all read and write operations
	if(!SetCommTimeouts (hCom, &CommTimeouts))
	{
		ComError("Unable to set COM port timeout");
		return 0;
	}

	PurgeComm(hCom, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

	// Direct the port to perform extended functions SETDTR and SETRTS
	// SETDTR: Sends the DTR (data-terminal-ready) signal.
	// SETRTS: Sends the RTS (request-to-send) signal. 
	EscapeCommFunction(hCom, SETDTR);
	EscapeCommFunction(hCom, SETRTS);

	memset( &her, 0, sizeof( OVERLAPPED ) ) ;
	her.hEvent = CreateEvent(
		NULL,   // default security attributes 
		FALSE,  // auto reset event 
		FALSE,  // not signaled 
		NULL    // no name 
		);

	//if (!SetCommMask(hCom, EV_RXCHAR)) 
	//{
	//	ComError("SetComMask");
	//}
	flag=false;

	if(bListen)
	{
		DWORD devID=0;
		comRec = CreateThread( 
			NULL,                   // default security attributes
			0,                      // use default stack size  
			comRecThread,       // thread function name
			NULL,          // argument to thread function 
			0,                      // use default creation flags 
			&devID);   // returns the thread identifier
	}

	return 1;
}


void ComError(LPCTSTR text)
{
	//wsprintf(d,"%s Error # : %d ",text,GetLastError());
	//AfxMessageBox(d);

	TCHAR d[500];

	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);

	wsprintf(d,"%s:\n%s",text,lpMsgBuf);
	//AfxMessageBox((LPCTSTR)d, MB_OK | MB_ICONINFORMATION );
	//CobiPrintText(d,error);
	MessageBox(hWnd,d,"Serial Port Error",MB_OK);
	LocalFree(lpMsgBuf);
}

int ComSend(LPCTSTR strInp)
{
	if(hCom==NULL||!bSendSync) return 0;
	int i=0;
	while(i<100 && strInp[i]!=NULL)
		i++;
	DWORD out=0;
	WriteFile(hCom,strInp,i,&out,&her);
	
	char msg[800];
	sprintf(msg,"%d\t%d\tEvent\t%d\t%d\t%s\r\n",GetQPC()-startTime,GetQPC()-mazeStart,31,0,strInp);
	AddToLog(msg);

	if(out!=i)
		return 0;
	return 1;
}

int ComSend(char cInp)
{
	if(hCom==NULL||!bSendSync) return 0;
	DWORD out=0;
	WriteFile(hCom,&cInp,1,&out,&her);

	
	char msg[800];
	sprintf(msg,"%d\t%d\tEvent\t%d\t%d\t%d\r\n",GetQPC()-startTime,GetQPC()-mazeStart,31,0,(int) cInp);
	AddToLog(msg);

	if(out!=1)
		return 0;
	return 1;
}


//int ComRead()
//{	
//	if(hCom==NULL) return 0;
//	LARGE_INTEGER now,freq;
//	DWORD out=0;
//	LPCTSTR cInp;
//	DWORD dwEvtMask,gelen;
//	unsigned char buffer[2];
//
//	if(flag)
//	{
//		if (!SetCommMask(hCom, EV_RXCHAR)) 
//		{
//			ComError("SetComMask");
//		}
//
//		memset( &her, 0, sizeof( OVERLAPPED ) ) ;
//		her.hEvent = CreateEvent(
//			NULL,   // default security attributes 
//			FALSE,  // auto reset event 
//			FALSE,  // not signaled 
//			NULL    // no name 
//			);
//		flag=true;
//	}
//
//	assert(her.hEvent);
//	if(false){
//		ReadFile(hCom,buffer,1,&gelen,&her);
//		return buffer[0];
//	}
//	else{
//	if(WaitCommEvent(hCom, &dwEvtMask, &her)) 
//	{ 
//		if (dwEvtMask & EV_RXCHAR) 
//		{
//			do{
//				ReadFile(hCom,buffer,1,&gelen,&her);
//				if(GetOverlappedResult(hCom,&her,&gelen,TRUE)==0)
//				{
//					//CheckError("ReadFile");
//				}
//				if(gelen>0)
//				{
//					return buffer[0];         
//				}
//
//			}while(gelen);                            
//		}
//	}
//	else
//	{
//		WaitForSingleObject(her.hEvent,500);
//		if(GetOverlappedResult(hCom,&her,&gelen,FALSE))
//		{
//			if (dwEvtMask & EV_RXCHAR) 
//			{
//				do{
//					ReadFile(hCom,buffer,1,&gelen,&her);
//					if(GetOverlappedResult(hCom,&her,&gelen,TRUE)==0)
//					{
//						//CheckError("ReadFile");
//					}
//					//if(gelen==1)
//					if(gelen>0)
//					{
//
//						return buffer[0]; 
//
//					}
//				}while(gelen);                            
//			}//if mask
//		}
//	}
//	//Sleep(10);
//	}
//	return (int)out;
//}

void ComClose()
{
	if(hCom!=NULL)
	{

		CloseHandle(hCom);
		hCom = NULL;
	}



}

DWORD WINAPI comRecThread( LPVOID lpParam)
{

	//LARGE_INTEGER freq; //now
	unsigned char buffer[2];
//	char daq[50];
	char msg[800];

	//Serial port initialization
	DWORD dwEvtMask,gelen;

	gelen = SetCommMask(hCom, EV_RXCHAR);
	if (!gelen)
	{
		return 0;
	}

	while(bListen)
	{

		if(WaitCommEvent(hCom, &dwEvtMask, &her))
		{
			if (dwEvtMask & EV_RXCHAR)
			{
				do{
					ReadFile(hCom,buffer,1,&gelen,&her);
					if(GetOverlappedResult(hCom,&her,&gelen,TRUE)==0)
					{
						//CheckError("ReadFile");
					}
					if(gelen>0)
					{
						 bufferVar=buffer[0]; 
						 //if(bufferVar==TRIGGER_CHAR1 || bufferVar==TRIGGER_CHAR2)
						 if(bufferVar == triggerChar)
						 {
							 EventLog(2,1,bufferVar);
							cue=true;
						 }

						 EventLog(30,0,bufferVar);

					}

				}while(gelen);                           
			}
		}
		else
		{
			WaitForSingleObject(her.hEvent,500);
			if(GetOverlappedResult(hCom,&her,&gelen,FALSE))
			{
				if (dwEvtMask & EV_RXCHAR)
				{
					do{
						ReadFile(hCom,buffer,1,&gelen,&her);
						if(GetOverlappedResult(hCom,&her,&gelen,TRUE)==0)
						{
							ComError("Unable to Read COM port");
						}

						if(gelen>0)
						{
							bufferVar=buffer[0];  
							//if(bufferVar==TRIGGER_CHAR1 || bufferVar==TRIGGER_CHAR2)
							if(bufferVar == triggerChar)
							{
								cue=true;
								sprintf(msg,"%d\t%d\tEvent\t2\t1\t%d\n",GetQPC()-startTime,GetQPC()-mazeStart,triggerChar);
									//Particular Log Event for Comm Cue, see EventLog()
								AddToLog(msg);
							}

							sprintf(msg,"%d\t%d\tEvent\t%d\t%d\t%d\r\n",GetQPC()-startTime,GetQPC()-mazeStart,30,0,bufferVar);
							AddToLog(msg);
						}
					}while(gelen);                           
				}//if mask
			}
		}
		//Sleep(10);
		Sleep(1);
	}
	if(hCom)
	{
		CloseHandle(hCom);
		hCom=NULL;
	}
	return 1;

}

WORD wLptPort = 0x378;
int LPT_Connect()
{
	//application then use InpOutx64.dll
	if (bUseLPT)
	{


		bool LPTloaded = false;
		LPTloaded = IsInpOutDriverOpen();
		if (!LPTloaded)
		{
			bUseLPT = false;
			printf("Unable to open InpOut32 Driver!\n");
			return -1;
		}
		else
		{
			LPT_Reset();
		}

	}
	else
	{
		return 0;
	}


	return 1;
}

int LPT_Connect(WORD iLPTport)
{
	//application then use InpOutx64.dll
	if (bUseLPT)
	{
		wLptPort = iLPTport;

		bool LPTloaded = false;
		LPTloaded = IsInpOutDriverOpen();
		if (!LPTloaded)
		{
			bUseLPT = false;
			printf("Unable to open InpOut32 Driver!\n");
			return -1;
		}
		else
		{
			LPT_Reset();
		}

	}
	else
	{
		return 0;
	}


	return 1;
}

void LPT_Reset()
{
	if (bUseLPT)
	{
		
		Out32(wLptPort, 0);
		
	}
}


int LPT_Send(char cInp)
{
	
	if (bUseLPT&&cInp == lastLPTchar)
	{
		
		
		lastLPTchar = 0;
	}
	
	if (bUseLPT)
	{
		WORD wData = (int)cInp;
		
		Out32(wLptPort, wData);
		
	}
	//LPT_Reset();

	lastLPTchar = cInp;
	/*if (BV_OLE)
	{

	}
	*/
	return 1;
}

void LPT_Free()
{
	if (bUseLPT)
	{
		bUseLPT = false;

	}
}

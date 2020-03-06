#pragma once
#include <stdio.h>
// ServerSocket command target

struct tcpMessage
{
	int command;	//Command code
	int iArg;		//int argument
	double dArgs[4]; //double arguments 

	char* toBytes()
	{
		unsigned char arr[sizeof(int)*2+sizeof(double)*4+4];
		unsigned char cmdArr[sizeof(command)],iArgArr[sizeof(iArg)],dArgArr[sizeof(double)*4];

		memcpy(cmdArr,&command,sizeof(command));
		memcpy(iArgArr,&iArg,sizeof(iArg));
		memcpy(dArgArr,dArgs,sizeof(double)*4);

		for (int x=3;x<sizeof(arr)-1;x++)
		{
			if(x-3<sizeof(int))
				arr[x]=cmdArr[x-3];
			else if(x-3<sizeof(int)*2)
				arr[x]=iArgArr[x-4-3];
			else
				arr[x]=dArgArr[x-8-3];
		}

		arr[0]=7;
		arr[1]=sizeof(arr)-4;
		arr[2]=sizeof(arr)-4;
		unsigned char chksum=0;
		
		for(int x=3;x<sizeof(arr)-1;x++)
		{
			chksum+=arr[x];
		}
		arr[sizeof(arr)-1]=chksum;

		char* ret;
		ret=(char*)malloc(sizeof(arr));
		memcpy(ret,arr,sizeof(arr));
		return ret;
	};
	tcpMessage()
	{
		command=0;
		iArg=0;
		dArgs[0]=0;
		dArgs[1]=0;
		dArgs[2]=0;
		dArgs[3]=0;
	}
	tcpMessage(int cmd)
	{
		command=cmd;
		iArg=0;
		dArgs[0]=0;
		dArgs[1]=0;
		dArgs[2]=0;
		dArgs[3]=0;
	}
	tcpMessage(int cmd,int arg)
	{
		command=cmd;
		iArg=arg;
		dArgs[0]=0;
		dArgs[1]=0;
		dArgs[2]=0;
		dArgs[3]=0;
	}
	tcpMessage(int cmd,int intArg,double doubleArgs[])
	{
		command=cmd;
		iArg=intArg;
		if(sizeof(doubleArgs)!=sizeof(double)*4)
		{
			dArgs[0]=0;
			dArgs[1]=0;
			dArgs[2]=0;
			dArgs[3]=0;
		}
		else
		{
			dArgs[0]=doubleArgs[0];
			dArgs[1]=doubleArgs[1];
			dArgs[2]=doubleArgs[2];
			dArgs[3]=doubleArgs[3];
		}
	}
	int storeStringInArr(char* input)
	{
		char dArgsArr[sizeof(dArgs)];

		sprintf_s(dArgsArr,sizeof(dArgsArr),"%.31s",input);
		memcpy(dArgs,dArgsArr,sizeof(dArgs));
		return 1;
	}
	
	char* getStringFromArr()
	{
		char dArgsArr[sizeof(dArgs)];
		char* string;
		memcpy(dArgsArr,dArgs,sizeof(dArgs));
		string=(char*) malloc(sizeof(dArgs));
		strcpy(string,dArgsArr);
		return string;
	}
};

class ServerSocket
{
public:
	bool unused;
	bool connected;
	bool connecting;
	tcpMessage message;

	ServerSocket();

	int Send(int code,int val);
	int Send(tcpMessage t);
	int Receive();
	void Disconnect();
	ServerSocket(int port,DWORD ipAddress);
	virtual ~ServerSocket();
private:
	SOCKET serverSocket;
	int Verify(unsigned char buf[]);
	void Process(unsigned char buf[]);
};




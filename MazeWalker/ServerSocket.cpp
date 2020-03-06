// ServerSocket.cpp : implementation file
//

#include <winsock2.h>
#include "ServerSocket.h"



// ServerSocket

ServerSocket::ServerSocket()
{
	connected=false;
	unused=true;
	connecting=false;
}
ServerSocket::ServerSocket(int port, DWORD ipAddress)
{
	connected=false;
	unused=false;
	connecting=true;
	if(port!=0)
	{	
		 WSADATA WsaDat;
		WSAStartup(MAKEWORD(1,1),&WsaDat);

		sockaddr_in serverInfo;
		serverInfo.sin_port=htons(port);
		serverInfo.sin_family=AF_INET;

		serverInfo.sin_addr.S_un.S_un_b.s_b1=(UCHAR) (ipAddress >> 24) & 255;  	
		serverInfo.sin_addr.S_un.S_un_b.s_b2=(UCHAR) (ipAddress >> 16) & 255;
		serverInfo.sin_addr.S_un.S_un_b.s_b3=(UCHAR) (ipAddress >> 8) & 255;
		serverInfo.sin_addr.S_un.S_un_b.s_b4=(UCHAR) (ipAddress >>0)  ;

		
		

		serverSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

		if(serverSocket==INVALID_SOCKET)
		{
			connected=false;
		}
		else
		{
			
			int ret=connect(serverSocket,(SOCKADDR *) &serverInfo,sizeof(serverInfo));
		     
			if(ret!=0)
			 {
				 closesocket(serverSocket);
				 WSACleanup();
				 connected=false;
				connecting=false;
			 }
			else
				connected=true;
			connecting=false;
		}
		
	}
	else
	{
		unused=true;
	}
}


ServerSocket::~ServerSocket()
{
}

void ServerSocket::Disconnect()
{
	if(!unused)
	{	
		WSACancelBlockingCall ( );
		closesocket(serverSocket);

		WSACleanup();
		connected=false;
		unused=true;
		Sleep(100);
	}
}


int ServerSocket::Send(int code,int val)
{
	if(connected)
	{
		tcpMessage t=tcpMessage(code,val);

		return Send(t);
	}
	else
		return 0;
}

int ServerSocket::Send(tcpMessage t)
{
	int BytesSent=0;
	if(connected)
	{
		char* mBytes=t.toBytes();

		BytesSent = send(serverSocket, mBytes, 44, 0);
	}
	else
		return 0;
	if(BytesSent<1)
		Disconnect();
	return BytesSent;
}



int ServerSocket::Receive()
{
	int result=0;
	if(connected)
	{
		char bufstar[sizeof(message)+4];
		unsigned char buf[sizeof(message)+4];
		result=recv(serverSocket,bufstar,sizeof(message)+4,0);
		memcpy(buf,bufstar,44);
		Process(buf);
	}
	

	if(result==SOCKET_ERROR)
		connected=false;
	return result;
}

double byteToDouble(unsigned char buf[], int offset)
{
	double ret;
	unsigned char dbuf[sizeof(double)];
	dbuf[0]=buf[offset];
	dbuf[1]=buf[offset+1];
	dbuf[2]=buf[offset+2];
	dbuf[3]=buf[offset+3];
	dbuf[4]=buf[offset+4];
	dbuf[5]=buf[offset+5];
	dbuf[6]=buf[offset+6];
	dbuf[7]=buf[offset+7];
	memcpy(&ret,dbuf,sizeof(double));
	return ret;
}

int byteToInt(unsigned char buf[],int offset)
{
	int ret;
	unsigned char dbuf[sizeof(int)];
	dbuf[0]=buf[offset];
	dbuf[1]=buf[offset+1];
	dbuf[2]=buf[offset+2];
	dbuf[3]=buf[offset+3];
	memcpy(&ret,dbuf,sizeof(int));
	return ret;
}

int ServerSocket::Verify(unsigned char buf[])
{
	if(buf[0]!=7||buf[1]<5)
		return 0;

	unsigned char chksum=0;

	for (int x=3; x<buf[1]+3;x++)
	{
		chksum+=buf[x];
	}

	return (chksum==buf[buf[1]+3]);
}

void ServerSocket::Process(unsigned char buf[])
{
	if(Verify(buf))
	{
		message.command=byteToInt(buf,0+3);
		message.iArg=byteToInt(buf,sizeof(int)+3);
		message.dArgs[0]=byteToDouble(buf,sizeof(int)*2+3);
		message.dArgs[1]=byteToDouble(buf,sizeof(int)*2+sizeof(double)+3);
		message.dArgs[2]=byteToDouble(buf,sizeof(int)*2+sizeof(double)*2+3);
		message.dArgs[3]=byteToDouble(buf,sizeof(int)*2+sizeof(double)*3+3);
	}
	else
	{
		message.command=-1000; //ignored as incorrect
		//message.command=byteToInt(buf,0);
		//message.iArg=byteToInt(buf,sizeof(int));
		//message.dArgs[0]=byteToDouble(buf,sizeof(int)*2);
		//message.dArgs[1]=byteToDouble(buf,sizeof(int)*2+sizeof(double));
		//message.dArgs[2]=byteToDouble(buf,sizeof(int)*2+sizeof(double)*2);
		//message.dArgs[3]=byteToDouble(buf,sizeof(int)*2+sizeof(double)*3);
	}
}

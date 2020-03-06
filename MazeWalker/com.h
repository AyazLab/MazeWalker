

#pragma once
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "stdio.h"
#include "QPC.h"
#include "Libs/inpout32/inpout32.h"

int ComConnect(int iPort,DWORD baud,bool parity,int stopBits);
void ComError(LPCTSTR text);
int  ComSend(LPCTSTR strInp);
int  ComSend(char cInp);
int ComRead();
void ComClose();
DWORD WINAPI comRecThread( LPVOID lpParam);
void LPT_Reset();
int LPT_Send(char cInp);
void LPT_Free();
int LPT_Connect();
int LPT_Connect(WORD iLptPort);
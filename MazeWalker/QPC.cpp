#include "QPC.h"

LARGE_INTEGER startTick;
LARGE_INTEGER curFreq;
LARGE_INTEGER curTick;

void InitializeQPC()
 {
	 QueryPerformanceFrequency(&curFreq);
	 QueryPerformanceCounter(&startTick);
 }

 DWORD GetQPC()
 {
	 QueryPerformanceCounter(&curTick);
	 return (curTick.QuadPart-startTick.QuadPart)/(curFreq.QuadPart/1000);

 }
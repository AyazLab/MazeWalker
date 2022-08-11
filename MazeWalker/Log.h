#include "com.h"
void EventLog(int type,int instance, int state);
void EventLog(int type, int val);
int InitLog(char* file);
void AddToLog(char* line);
void WriteHeader(char* walker, char* mazeListItem);
void CloseLog();
int LogStatus();
void LogPosition(float xPos, float yPos, float zPos, float xView, float yView, float zView, int mazeScore);
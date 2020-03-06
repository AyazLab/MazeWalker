#include "com.h"
void EventLog(int type,int instance, int state);
void EventLog(int type, int val);
int InitLog(char* file);
void AddToLog(char* Line);
void CloseLog();
int LogStatus();
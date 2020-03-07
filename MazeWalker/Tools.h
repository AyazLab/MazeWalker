
#pragma once

#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "GL/glew.h"


bool GetUntilTab(FILE* fp,char *out,int len);

bool trueFalse(char *str);
bool trueFalse(char *str,bool defaultResponse);
bool IsEqual(float inp1,float inp2);
char *textFileRead(char *fn);
int textFileWrite(char *fn, char *s);
char* printShaderInfoLog(GLuint obj);
void trim(char *str);
void addLineBreaksAndMC(char* str,int sz);


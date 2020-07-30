
#pragma once

#include "stdio.h"
#include <stdlib.h>
#include <string>
#include <vector>
#include "GL/glew.h"

#define BIGSTRBUFFER 10000


bool GetUntilTab(FILE* fp,char *out,int len);

bool trueFalse(char *str);
bool trueFalse(char *str,bool defaultResponse);
bool IsEqual(float inp1,float inp2);
char *textFileRead(char *fn);
int textFileWrite(char *fn, char *s);
char* printShaderInfoLog(GLuint obj);
void trim(char *str);
void addLineBreaksAndMC(char* str,int sz);
void autoSizeToWidth(char* str, int sz, int txtWidth);
char* strtolower(char* s);
bool CheckFileExists(char* file);
char* getFileDir(char* fname);
char* getFileExt(char* fname);
bool CheckFileExists(char* file);
bool CheckFileExists(char* file,bool noExt);
char* CheckFileExistsNoExt(char* file);
char* getBestPath(char* filename, char* folder);
char* getBestPath(char* filename);

char* getBestPath(std::string filename_string, char* folder);
char* getBestPath(std::string filename_string);

void GetFilesInDirectory(std::vector<std::string>& out, const std::string& directory);
char* getBestPathNoExt(char* filename, char* folder);
char* getFilenameOnly(char* fname, bool withExt);
char* getFilenameOnly(char* fname);

void updateMazeWorkingDir(char* newDir);
char* getMazeWorkingDir();

void updateMelWorkingDir(char* newDir);
char* getMelWorkingDir();

void updateUserLibraryDir(char* newDir);
char* getUserLibraryDir();

void updateLibraryDir(char* newDir);
char* getLibraryDir();

char* swapSlash(char* pathIn);
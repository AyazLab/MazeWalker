#pragma once
#include <windows.h>
#include "fmod\\inc\\fmod.hpp"
#include "fmod\\inc\\fmod_errors.h"

#ifndef _INC_STDIO
#include <stdio.h>
#endif

#ifndef _INC_MALLOC
#include <malloc.h>
#endif



DWORD WINAPI audioPlay( LPVOID lpParam);
class AudioDictionaryItem
{
public:
	int index;	// index value: 0,1,2...
	char fname[800];
	volatile int playing;
	AudioDictionaryItem(void);
	AudioDictionaryItem(int index,char* fname2);
	void Play();
	void Play(bool looped);
	void Pause(bool pause);
	void SetVolume(float volume);
	void Stop();
	void Release();
	FMOD::Sound *sound;
	FMOD::Channel *channel;

	AudioDictionaryItem *next;
};




class AudioDictionary
{
public:
	FMOD::System *fmodsys;
	AudioDictionary();
	AudioDictionaryItem* cListRoot;

	void Play(int index);
	void Play(int index,bool looped);
	void Stop(int index);
	void SetVolume(int index, float volume);
	void Pause(int index,bool pause);
	void Clear();
	void SetRecordDirPath(char* fname);
	int GetLength();
	int RecordStart();
	int RecordStop();
	AudioDictionaryItem* GetAudioItem(int index);
	int Add(int index, char* fname);

	char audioRecordDirPath[512];
};

void WriteWavHeader(FILE* fp, FMOD_SOUND* sound, int length);
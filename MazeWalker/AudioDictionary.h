#pragma once
#include <windows.h>
#include "fmod\\fmod.hpp"
#include "fmod\\fmod_errors.h"



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
	int GetLength();
	AudioDictionaryItem* GetAudioItem(int index);
	int Add(int index, char* fname);
};
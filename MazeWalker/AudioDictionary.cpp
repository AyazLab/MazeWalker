#include "AudioDictionary.h"
#include <memory.h>
#include <time.h>
HANDLE audioThread;

#if defined(WIN32) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
#define __PACKED                         /* dummy */
#else
#define __PACKED __attribute__((packed)) /* gcc packed */
#endif

extern AudioDictionary curAudioDict; //defined in main.cpp

	AudioDictionaryItem::AudioDictionaryItem(void)
	{
		this->next=NULL;
		fname[0] = 0;
		index=-1;
		playing=0;

	}
	AudioDictionaryItem::AudioDictionaryItem(int index,char* fname2)
	{
		this->index=index;
		strcpy(fname,fname2);
		this->next=NULL;
		FMOD_RESULT result;

		result=curAudioDict.fmodsys->createStream(fname,FMOD_DEFAULT,0,&sound);
			
		//MessageBox(NULL,FMOD_ErrorString(result),NULL,NULL);
	}
	void AudioDictionaryItem::Play()
	{
		curAudioDict.fmodsys->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
		channel->setMode(FMOD_LOOP_OFF);
		
	}
	void AudioDictionaryItem::Play(bool looping)
	{
		bool playing,paused;
		channel->isPlaying(&playing);
		channel->getPaused(&paused);
		if (paused)
			Pause(false);
		else if(!playing)
		{
			if(looping)
			{
				curAudioDict.fmodsys->playSound(FMOD_CHANNEL_FREE, sound, false, &channel);
				channel->setMode(FMOD_LOOP_NORMAL);
			}
			else
			{
				Play();
			}
		}

	}
	void AudioDictionaryItem::Stop()
	{
		channel->stop();
	}

	void AudioDictionaryItem::Release()
	{
		if (sound)
		{
			sound->release();
		}

	}

	void AudioDictionaryItem::Pause(bool pause)
	{
		channel->setPaused(pause);
	}

	void AudioDictionaryItem::SetVolume(float volume)
	{
		channel->setPaused(true);
		channel->setVolume(volume);
		channel->setPaused(false);
	}

	AudioDictionary::AudioDictionary()
	{
		FMOD::System_Create(&fmodsys);
		fmodsys->init(32,FMOD_INIT_NORMAL,0);
		cListRoot=new AudioDictionaryItem();

		strcpy(audioRecordDirPath, "");
	}
	AudioDictionaryItem* cListRoot;

	void AudioDictionary::Play(int index)
	{
		AudioDictionaryItem *cur=GetAudioItem(index);
		if(cur)
			cur->Play();
	}
	void AudioDictionary::Play(int index,bool looped)
	{
		AudioDictionaryItem *cur=GetAudioItem(index);
		if(cur)
			cur->Play(looped);
	}
	void AudioDictionary::Pause(int index, bool pause)
	{
		AudioDictionaryItem *cur=GetAudioItem(index);
		if(cur)
			cur->Pause(pause);
	}
	void AudioDictionary::Stop(int index)
	{
		 AudioDictionaryItem *cur=GetAudioItem(index);
		if(cur)
			cur->Stop();
		//PlaySound(NULL, NULL, NULL);
	}


	void AudioDictionary::SetVolume(int index,float volume)
	{
		AudioDictionaryItem *cur = GetAudioItem(index);
		if (cur)
			cur->SetVolume(volume);
		//PlaySound(NULL, NULL, NULL);
	}
	void AudioDictionary::Clear()
	{
		//clear the list...
		AudioDictionaryItem *temp,*cur;
		if(cListRoot)
			cur = cListRoot;

		if(cur!=NULL)
		{	
			while(cur)
			{
				if (cur->fname[0]!=0)
					cur->Release();
				temp = cur;
				cur=cur->next;
				//delete temp;
				free(temp);
			}
		}

		cListRoot=new AudioDictionaryItem();
	}
	int AudioDictionary::GetLength()
	{
		AudioDictionaryItem* item=cListRoot;
		int count=0;
		if(item)
			count++;
		while(item->next)
		{
			count++;
			item=item->next;
		}
		return count;
	}
	AudioDictionaryItem* AudioDictionary::GetAudioItem(int index)
	{
				if(index>0)
		{
			AudioDictionaryItem* item=cListRoot;
			while(item->next)
			{
				if(item->index==index)
					break;
				item=item->next;
			}
			if(item&&item->index==index)
				return item;
		}
				else return 0;
				return 0;
	}

	void AudioDictionary::SetRecordDirPath(char* fname)
	{
		strcpy_s(audioRecordDirPath,512, fname);
	}

	int AudioDictionary::Add(int index, char* fname)
	{
		char fnameTemp[800];
		strcpy_s(fnameTemp, 800, fname);
		AudioDictionaryItem* newitem= new AudioDictionaryItem(index, fnameTemp);
		if(cListRoot==NULL)
		{
			cListRoot=newitem;
			return 1;
		}
		else
		{
			AudioDictionaryItem* lastitem=cListRoot;
			while(lastitem->next)
			{
				lastitem=lastitem->next;
			}
			lastitem->next=newitem;
			return 1;
		}
	}


	unsigned int datalength = 0, soundlength;
	DWORD WINAPI audioRecord(LPVOID lpParam);
	volatile bool bRecord = false;
	int AudioDictionary::RecordStart()
	{

		bRecord = true;
		audioThread = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			audioRecord,       // thread function name
			(LPVOID)0,          // argument to thread function 
			0,                      // use default creation flags 
			0);   // returns the thread identifier

		return 0;
	}

	DWORD WINAPI audioRecord(LPVOID lpParam)
	{
		FMOD_RESULT            result;
		FMOD_SOUND* snd = 0;

		FMOD_CREATESOUNDEXINFO exinfo;
		memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));

		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.numchannels = 2;
		exinfo.format = FMOD_SOUND_FORMAT_PCM16;
		exinfo.defaultfrequency = 44100;
		exinfo.length = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * 2;

		result = FMOD_System_CreateSound((FMOD_SYSTEM*)curAudioDict.fmodsys, 0, FMOD_2D | FMOD_SOFTWARE | FMOD_OPENUSER, &exinfo, &snd);

		int recorddriver = 0;
		int numdrivers;
		FMOD_System_GetRecordNumDrivers((FMOD_SYSTEM*)curAudioDict.fmodsys, &numdrivers);
		if (numdrivers > 0)
			result = FMOD_System_RecordStart((FMOD_SYSTEM*)curAudioDict.fmodsys, recorddriver, snd, 1);


		result = FMOD_Sound_GetLength(snd, &soundlength, FMOD_TIMEUNIT_PCM);

		char rcrdfile[800];
		time_t rawtime;
		time(&rawtime);
		struct tm* timeinfo;
		timeinfo = localtime(&rawtime);


		sprintf(rcrdfile, "%s\\autoLog_%i_%i_%i_%i_%i_%i_Record.wav", curAudioDict.audioRecordDirPath, timeinfo->tm_year - 100, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);



		FILE* fp = fopen(rcrdfile, "wb");
		if (!fp)
		{
			printf("ERROR : could not open record.wav for writing.\n");
			return 1;
		}

		/*
		Write out the wav header.  As we don't know the length yet it will be 0.
	*/
		WriteWavHeader(fp, snd, datalength);

		do
		{
			static unsigned int lastrecordpos = 0;
			unsigned int recordpos = 0;

			FMOD_System_GetRecordPosition((FMOD_SYSTEM*)curAudioDict.fmodsys, recorddriver, &recordpos);

			if (recordpos != lastrecordpos)
			{
				void* ptr1, * ptr2;
				int blocklength;
				unsigned int len1, len2;

				blocklength = (int)recordpos - (int)lastrecordpos;
				if (blocklength < 0)
				{
					blocklength += soundlength;
				}

				/*
					Lock the sound to get access to the raw data.
				*/
				FMOD_Sound_Lock(snd, lastrecordpos * exinfo.numchannels * 2, blocklength * exinfo.numchannels * 2, &ptr1, &ptr2, &len1, &len2);   /* * exinfo.numchannels * 2 = stereo 16bit.  1 sample = 4 bytes. */

				/*
					Write it to disk.
				*/
				if (ptr1 && len1)
				{
					datalength += fwrite(ptr1, 1, len1, fp);
				}
				if (ptr2 && len2)
				{
					datalength += fwrite(ptr2, 1, len2, fp);
				}

				/*
					Unlock the sound to allow FMOD to use it again.
				*/
				FMOD_Sound_Unlock(snd, ptr1, ptr2, len1, len2);
			}

			lastrecordpos = recordpos;

			printf("%-23s. Record buffer pos = %6d : Record time = %02d:%02d\r", (timeGetTime() / 500) & 1 ? "Recording to record.wav" : "", recordpos, datalength / exinfo.defaultfrequency / exinfo.numchannels / 2 / 60, (datalength / exinfo.defaultfrequency / exinfo.numchannels / 2) % 60);

			FMOD_System_Update((FMOD_SYSTEM*)curAudioDict.fmodsys);

			Sleep(10);

		} while (bRecord);

		WriteWavHeader(fp, snd, datalength);

		fclose(fp);

		/*
			Shut down
		*/
		result = FMOD_Sound_Release(snd);

	}

	int AudioDictionary::RecordStop()
	{
		bRecord = false;
		return 0;
	}

	void WriteWavHeader(FILE* fp, FMOD_SOUND* sound, int length)
	{
		int             channels, bits;
		float           rate;

		if (!sound)
		{
			return;
		}

		fseek(fp, 0, SEEK_SET);

		FMOD_Sound_GetFormat(sound, 0, 0, &channels, &bits);
		FMOD_Sound_GetDefaults(sound, &rate, 0, 0, 0);

		{
#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
#pragma pack(1)
#endif

			/*
				WAV Structures
			*/
			typedef struct
			{
				signed char id[4];
				int 		size;
			} RiffChunk;

			struct
			{
				RiffChunk       chunk           __PACKED;
				unsigned short	wFormatTag      __PACKED;    /* format type  */
				unsigned short	nChannels       __PACKED;    /* number of channels (i.e. mono, stereo...)  */
				unsigned int	nSamplesPerSec  __PACKED;    /* sample rate  */
				unsigned int	nAvgBytesPerSec __PACKED;    /* for buffer estimation  */
				unsigned short	nBlockAlign     __PACKED;    /* block size of data  */
				unsigned short	wBitsPerSample  __PACKED;    /* number of bits per sample of mono data */
			} FmtChunk = { {{'f','m','t',' '}, sizeof(FmtChunk) - sizeof(RiffChunk) }, 1, channels, (int)rate, (int)rate * channels * bits / 8, 1 * channels * bits / 8, bits } __PACKED;

			struct
			{
				RiffChunk   chunk;
			} DataChunk = { {{'d','a','t','a'}, length } };

			struct
			{
				RiffChunk   chunk;
				signed char rifftype[4];
			} WavHeader = { {{'R','I','F','F'}, sizeof(FmtChunk) + sizeof(RiffChunk) + length }, {'W','A','V','E'} };

#if defined(WIN32) || defined(_WIN64) || defined(__WATCOMC__) || defined(_WIN32) || defined(__WIN32__)
#pragma pack()
#endif

			/*
				Write out the WAV header.
			*/
			fwrite(&WavHeader, sizeof(WavHeader), 1, fp);
			fwrite(&FmtChunk, sizeof(FmtChunk), 1, fp);
			fwrite(&DataChunk, sizeof(DataChunk), 1, fp);
		}
	}

//DWORD WINAPI audioPlay( LPVOID lpParam)
//	{
//		int index= (int) lpParam;
//		AudioDictionaryItem* audio=curAudioDict.GetAudioItem(index);
//
//		if(audio)
//		{
//			audio->playing=1;
//			PlaySound(audio->fname, NULL, SND_FILENAME|SND_ASYNC);
//			audio->playing=0;
//		}
//			
//		return 0;
//	}
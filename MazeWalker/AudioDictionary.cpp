#include "AudioDictionary.h"

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
		//if(!playing)
		//{
		//audioThread = CreateThread( 
		//					NULL,                   // default security attributes
		//					0,                      // use default stack size  
		//					audioPlay,       // thread function name
		//					(LPVOID)index,          // argument to thread function 
		//					0,                      // use default creation flags 
		//					0);   // returns the thread identifier
		//}
		//PlaySound(fname, NULL, SND_FILENAME|SND_ASYNC);
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
	int AudioDictionary::Add(int index, char* fname)
	{
		AudioDictionaryItem* newitem= new AudioDictionaryItem(index,fname);
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
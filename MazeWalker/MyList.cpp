#include ".\mylist.h"
#include "Tools.h"
#include <ctype.h>
#include <cmath>

#define TXTBUFFERLIMIT	800
#define MAXMSGTXTLIMIT	5000


MyList::MyList(void)
{
	cListRoot = new MazeListItem();
}

MyList::~MyList(void)
{
	Clear();
}

void MyList::Clear(void)
{
	//clear the list...
	MazeListItem *temp,*cur;
	cur = cListRoot;
	while(cur->next)
	{
		temp = cur;
		cur=cur->next;
		delete temp;
	}
}

void MyList::AddMaze(char* val)
{
	MazeListItem *cur = GetLast();

	bool isMazX = false;
	if (strlen(val) > 4)
	{
		isMazX = ('x' == tolower(val[strlen(val) - 1]));

	}

	cur->next = new MazeListItem();
	cur->next->AddMaze(val,isMazX);

}

void MyList::AddText(char* val,long lftm,int shStyle)
{
	MazeListItem *cur = GetLast();

	cur->next = new MazeListItem();
	cur->next->AddText(val,lftm,shStyle);
}

void MyList::AddBG(char* val)
{
	MazeListItem *cur = GetLast();

	cur->AddBG(val);
}

MazeListItem* MyList::GetLast()
{
	MazeListItem *ret=cListRoot;
	while(ret->next!=NULL)
	{
		ret = ret->next;
	}
	return ret;
}

MazeListItem* MyList::GetFirst()
{
	return cListRoot->next;
}

int MyList::GetLength()
{
	MazeListItem *ret=cListRoot;
	int count=0;
	while(ret->next!=NULL)
	{
		ret = ret->next;
		count++;
	}
	return count;
}

extern bool CheckFile(char* file);
//////////////////////////////////////////////////////////////////////////
//
//    returns
//
//			 2  loaded single maze file
//           1	loaded maze list file
//          -1	can not open the file
//			-2	incorrect file content   
//   
int MyList::ReadFile(char *filename)
{
	for (int i = 0; i < TXTBUFFERLIMIT; i++)
	{
		if (filename[i] == 0)
			break;
		filename[i] = tolower(filename[i]);
	}

	if(!CheckFile(filename)||strlen(filename)<4)
	{
		
		
		return -1;
	}
	int res=0;
	this->Clear();
	while(filename[res]!=NULL)
	{
		
		res++;
	}
	

	if (filename[res - 3] == 'm' && filename[res - 2] == 'a' && filename[res - 1] == 'z' || 
		filename[res - 4] == 'm' && filename[res - 3] == 'a' && filename[res - 2] == 'z' && filename[res - 1] == 'x' ||
		filename[res - 3] == 'x' && filename[res - 2] == 'm' && filename[res - 1] == 'l' ) //mazx file)
	{
		//single maze file...
		this->AddMaze(filename);
		mListDIR[0]=0;
		return 2;
	}
	
	else
	{
		return ReadMazeListFile(filename);
	}
}

//////////////////////////////////////////////////////////////////////////
//
//    returns 
//           1	successful operation
//          -1	can not open the file
//			-2	incorrect file content   
//   



int MyList::ReadMazeListFile(char *filename)
{
	char iDir[TXTBUFFERLIMIT],temp[TXTBUFFERLIMIT],temp2[TXTBUFFERLIMIT];
	char melType[TXTBUFFERLIMIT];
	char dlgType[TXTBUFFERLIMIT];
	char imgID[TXTBUFFERLIMIT];
	char curTxt[TXTBUFFERLIMIT];

	char msgTemp[MAXMSGTXTLIMIT];

	float inp1;
	int res,type;
	int msgLen = 0;
	int multiMsgNum = 0;
	bool ret=false;
	
	//maze list file...
	if(!CheckFile(filename))
	{
		
		
		return -1;
	}
	else
	{	
		FILE * fp = fopen(filename,"r");
		

		for(res=0;filename[res]!=NULL;res++)
			;
		for(type=res;filename[type]!='\\' && type>0;type--)
			;
		for(res=0;res<=type;res++)
			iDir[res]=filename[res];
		iDir[type+1]=NULL;

		GetUntilTab(fp,temp,TXTBUFFERLIMIT); // Get text Type
		if(sscanf_s(temp,"Maze List File %f",&inp1)!=1)
			return -2;

		while(GetUntilTab(fp, melType,TXTBUFFERLIMIT))
		{
			if(strcmp(melType,"Maze")==0)
			{
				strcpy_s(temp,iDir); //copy mel file directory first

				for (res = strlen(temp); res < TXTBUFFERLIMIT; res++)
				{
					temp[res] = 0; 
				}
				
				
				//strcat(temp,"\\");
				//i++;
				GetUntilTab(fp,&(temp[strlen(temp)]),TXTBUFFERLIMIT-strlen(temp)); //append mazefile to string

				
				
				this->AddMaze(temp);

			}
			else if (strcmpi(melType, "Text") == 0 || 
				strcmpi(melType, "MultipleChoice") == 0 ||
				strcmpi(melType, "Image") == 0) // All of these really are the same anyways
			{
				do
				{				
					ret=GetUntilTab(fp, msgTemp, MAXMSGTXTLIMIT);
					
				} while (ret == false && strlen(msgTemp)>10); // get extra long messages

				if (!ret)
					sprintf(msgTemp, "Message too Long");
				

				msgLen = strlen(msgTemp);

				multiMsgNum = floor(msgLen*1.0f / ((TXTBUFFERLIMIT-1)*1.0f))+1;

				GetUntilTab(fp, dlgType, TXTBUFFERLIMIT);
				res = TEXTBOXSTYLE_ONDIALOG_CLEAR_BK;
				if (strcmp(dlgType, "OnDialog") == 0 || strcmp(dlgType, "OnFramedDialog") == 0)
				{
					res = TEXTBOXSTYLE_ONDIALOG_CLEAR_BK;
				}
				else
				{
					res = TEXTBOXSTYLE_ONSCREEN_CLEAR_BK;
				}
				GetUntilTab(fp, temp2, TXTBUFFERLIMIT);
				long lf = atol(temp2); //Time shown
				GetUntilTab(fp, temp2, TXTBUFFERLIMIT);
				bool hasImg = GetUntilTab(fp, temp2, TXTBUFFERLIMIT);

				if (hasImg)
				{
					//since previous return value is not false, end of line is not reached.
					//get the next cell (background image)
					GetUntilTab(fp, imgID, TXTBUFFERLIMIT);
				}

				for (int i=0;i<multiMsgNum;i++)
				{
					int sz = strlen(msgTemp);
					memcpy(curTxt,&msgTemp[(i)*(TXTBUFFERLIMIT - 1)], TXTBUFFERLIMIT-1);
					curTxt[TXTBUFFERLIMIT - 1] = 0;
					this->AddText(curTxt,lf,res);
					if (hasImg)
					{
						if (strlen(imgID) > 2)
							this->AddBG(imgID);
					}

				}
			}

			//sprintf(cMazeList[iMazeList],"%s\\%s",iDir,temp);
			//iMazeList++;
		}
		fclose(fp);

		strcpy(mListDIR,iDir);
	}
	return 1;
}
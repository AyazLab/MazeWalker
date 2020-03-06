#include ".\mylist.h"
#include "Tools.h"
#include <ctype.h>

#define TXTBUFFERLIMIT	800

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
	float inp1;
	int res,type;
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

		GetUntilTab(fp,temp,TXTBUFFERLIMIT);
		if(sscanf_s(temp,"Maze List File %f",&inp1)!=1)
			return -2;

		while(GetUntilTab(fp,temp,TXTBUFFERLIMIT))
		{
			if(strcmp(temp,"Maze")==0)
			{
				strcpy_s(temp,iDir);

				for (res = strlen(temp); res < TXTBUFFERLIMIT; res++)
				{
					temp[res] = 0;
				}
				
				
				//strcat(temp,"\\");
				//i++;
				GetUntilTab(fp,&(temp[strlen(temp)]),TXTBUFFERLIMIT-strlen(temp));

				
				
				this->AddMaze(temp);

			}
			else if (strcmp(temp, "Text") == 0 || strcmp(temp, "MultipleChoice") == 0)
			{
				GetUntilTab(fp,temp,TXTBUFFERLIMIT);
				GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
				res=TEXTBOXSTYLE_ONDIALOG_CLEAR_BK;
				if(strcmp(temp2,"OnDialog")==0 || strcmp(temp2,"OnFramedDialog")==0)
				{
					res=TEXTBOXSTYLE_ONDIALOG_CLEAR_BK;
				}
				else
				{
					res=TEXTBOXSTYLE_ONSCREEN_CLEAR_BK;
				}
				GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
				long lf = atol(temp2);
				GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
				bool ret= GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
				
				this->AddText(temp,lf,res);

				if(ret)
				{				
					//since previous return value is not false, end of line is not reached.
					//get the next cell (background image)
					GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
					if(strlen(temp2)>2);
						this->AddBG(temp2);
				}
			}
			else if(strcmp(temp,"Image")==0)
			{
				GetUntilTab(fp,temp,TXTBUFFERLIMIT);
				GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
				res=TEXTBOXSTYLE_ONDIALOG_CLEAR_BK;
				if(strcmp(temp2,"OnDialog")==0 || strcmp(temp2,"OnFramedDialog")==0)
				{
					res=TEXTBOXSTYLE_ONDIALOG_CLEAR_BK;
				}
				else
				{
					res=TEXTBOXSTYLE_ONSCREEN_CLEAR_BK;
				}
				GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
				long lf = atol(temp2);
				GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
				bool ret= GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
				
				this->AddText(temp,lf,res);

				if(ret)
				{				
					//since previous return value is not false, end of line is not reached.
					//get the next cell (background image)
					GetUntilTab(fp,temp2,TXTBUFFERLIMIT);
					if(strlen(temp2)>2);
						this->AddBG(temp2);
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
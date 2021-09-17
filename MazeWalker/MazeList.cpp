#include ".\MazeList.h"
#include "Tools.h"
#include <ctype.h>
#include <cmath>
#include "Libs/rapidxml/rapidxml.hpp"

#include <fstream>
#include <sstream>

#include <map>

#define TXTBUFFERLIMIT	800
#define MAXMSGTXTLIMIT	5000

std::map<int, std::string> listMazeDict;
std::map<int, std::string> listAudioDict;
std::map<int, std::string> listImageDict;


MazeList::MazeList(void)
{
	cListRoot = new MazeListItem();
}

MazeList::~MazeList(void)
{
	Clear();
}

void MazeList::Clear(void)
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

void MazeList::AddMaze(char* val)
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

void MazeList::AddText(char* val,long lftm,int shStyle)
{
	MazeListItem *cur = GetLast();

	cur->next = new MazeListItem();
	cur->next->AddText(val,lftm,shStyle);
}

void MazeList::AddCommand(char* cmd, bool waitForComplete)
{
	MazeListItem* cur = GetLast();

	cur->next = new MazeListItem();
	cur->next->AddCommand(cmd, waitForComplete);
}

void MazeList::AddBG(char* val)
{
	MazeListItem *cur = GetLast();

	cur->AddBG(val);
}

MazeListItem* MazeList::GetLast()
{
	MazeListItem *ret=cListRoot;
	while(ret->next!=NULL)
	{
		ret = ret->next;
	}
	return ret;
}

MazeListItem* MazeList::GetFirst()
{
	return cListRoot->next;
}

int MazeList::GetLength()
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

//////////////////////////////////////////////////////////////////////////
//
//    returns
//
//			 2  loaded single maze file
//           1	loaded maze list file
//          -1	can not open the file
//			-2	incorrect file content   
//   





int MazeList::ReadMazeList(char *filename)
{
	char* filename_lower = strtolower(filename);

	int res = strlen(filename);
	if(!CheckFileExists(filename_lower)||res <4)
	{
		return -1;
	}

	this->Clear();
	
	char* newDir = getFileDir(filename);
	updateMelWorkingDir(newDir);

	char* fileExt = getFileExt(filename);
	if (strlen(fileExt)>0)
	{
		if (strcmpi(fileExt, ".maz") == 0 || strcmpi(fileExt, ".mazx") == 0)
		{
			this->AddMaze(filename);
			mListDIR[0] = 0;
			return 2;
		}
		else if(strcmpi(fileExt, ".mel") == 0)
		{
			return ReadMazeListClassic(filename);
		}
		else if(strcmpi(fileExt, ".melx") == 0)
		{
			return ReadMazeListXML(filename);
		}
		else
			return -1;

	}
	else
	{
		return -1; // not a file type?
	}
}

int MazeList::ReadMazeListXML(char* melFile)
{
	char iDir[TXTBUFFERLIMIT], temp[TXTBUFFERLIMIT], temp2[TXTBUFFERLIMIT];
	char melType[TXTBUFFERLIMIT];
	char dlgType[TXTBUFFERLIMIT];
	char imgID[TXTBUFFERLIMIT];
	char audioID[TXTBUFFERLIMIT];
	char curTxt[TXTBUFFERLIMIT];

	char errorTxt[TXTBUFFERLIMIT];

	char msgTemp[MAXMSGTXTLIMIT];





	float inp1;
	int res, type;
	int msgLen = 0;
	int multiMsgNum = 0;
	bool ret = false;

	bool hasImg = false;
	bool hasAudio = false;
	int lf = 0;
	int x = 0;
	int y = 0;
	
	bool isMultipleChoice = false;

	//maze list file...
	if (!CheckFileExists(melFile))
	{
		sprintf(errorTxt, "Error\nFile does not exist");
		//GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING); //return 0;
		return -1;
	}
	else
	{
		FILE* fp = fopen(melFile, "r");

		if (fp == NULL)
		{

			sprintf(errorTxt, "Couldn't open MazeList file!\n%s", melFile);
			if (strlen(melFile) > 300)
				sprintf(errorTxt, "Couldn't open MazeList file!\nMazeList Error");
			//GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
			return 0;
		}
		else
		{
			fclose(fp);

		}


		for (res = 0; melFile[res] != NULL; res++)
			;
		for (type = res; melFile[type] != '\\' && type > 0; type--)
			;
		for (res = 0; res <= type; res++)
			iDir[res] = melFile[res];
		iDir[type + 1] = NULL;

		//SetCurrentDirectoryA(mazeWorkingDir);

		try
		{
			using namespace rapidxml;

			std::ifstream file(melFile);
			float melFileEdition = 0;

			xml_document<> doc;
			std::stringstream buffer;
			buffer << file.rdbuf();
			file.close();
			std::string content(buffer.str());
			doc.parse<0>(&content[0]);
			xml_node<>* pRoot = doc.first_node("MazeList");

			if (pRoot == NULL)
			{
				sprintf(errorTxt, "Invalid MazeList File");
				//GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING); //return 0;
				return -1;
			}

			xml_attribute<>* pAttr;
			pAttr = pRoot->first_attribute("Version");
			if (pAttr)
				melFileEdition = atof(pAttr->value());


			if (melFileEdition > 2 || melFileEdition == 0)
			{
				if (melFileEdition > 2)
					sprintf(errorTxt, "Current File Version: 1.0\nMazeList File v%.1f is unsupported in this version of MazeWalker");
				else
					sprintf(errorTxt, "Current File Version: 1.0\nUnknown File Version Loaded");
				//GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING); //return 0;

				return -1;
			}

			char fPath[800] = "";

			int mazeID = -1;
			int autoID = 10000;

			xml_node<>* pListMazeLibraryNode = pRoot->first_node("MazeLibrary");
			listMazeDict.clear();

			if (pListMazeLibraryNode)
			{
				
				for (xml_node<>* pNode = pListMazeLibraryNode->first_node(); pNode; pNode = pNode->next_sibling())
				{
					if (pNode == NULL)
						break;
					if (0 == strcmp(pNode->name(), "Maze"))
					{
						pAttr = pNode->first_attribute("MazeID");
						if (pAttr)
							mazeID = atoi(pAttr->value());
						
						if(!pAttr||mazeID<0)
						{
							mazeID = autoID;
							autoID++;
						}



						pAttr = pNode->first_attribute("File");
						if(pAttr)
						{
							strcpy_s(temp, TXTBUFFERLIMIT, pAttr->value());
							listMazeDict[mazeID]=temp;
						}
					}
				}
			}

			xml_node<>* pListAudioLibraryNode = pRoot->first_node("AudioLibrary");
			listAudioDict.clear();

			if (pListAudioLibraryNode)
			{

				for (xml_node<>* pNode = pListAudioLibraryNode->first_node(); pNode; pNode = pNode->next_sibling())
				{
					if (pNode == NULL)
						break;
					if (0 == strcmp(pNode->name(), "Audio"))
					{
						pAttr = pNode->first_attribute("AudioID");

						if (pAttr)
							mazeID = atoi(pAttr->value());

						if (!pAttr || mazeID < 0)
						{
							mazeID = autoID;
							autoID++;

						}
						pAttr = pNode->first_attribute("File");
						if (pAttr)
						{
							strcpy_s(temp, TXTBUFFERLIMIT, pAttr->value());
							listAudioDict[mazeID] = temp;
						}
					}
				}
			}

			xml_node<>* pListImageLibraryNode = pRoot->first_node("ImageLibrary");
			listImageDict.clear();

			if (pListImageLibraryNode)
			{

				for (xml_node<>* pNode = pListImageLibraryNode->first_node(); pNode; pNode = pNode->next_sibling())
				{
					if (pNode == NULL)
						break;
					if (0 == strcmp(pNode->name(), "Image"))
					{
						pAttr = pNode->first_attribute("ID");
						if (pAttr)
							mazeID = atoi(pAttr->value());
						else
						{
							mazeID = autoID;
							autoID++;

						}
						pAttr = pNode->first_attribute("File");
						if (pAttr)
						{
							strcpy_s(temp, TXTBUFFERLIMIT, pAttr->value());
							listImageDict[mazeID] = temp;
						}
					}
				}
			}

			xml_node<>* pListItemsNode = pRoot->first_node("ListItems");

			if (!pListItemsNode) //If we can't find any List Item Node, then just use the root (undefined fallback behavior)
				pListItemsNode = pRoot;

			xml_attribute<>* pAttrID;
			xml_attribute<>* pAttrFile;

			if (pListItemsNode)
			{
				for (xml_node<>* pNode = pListItemsNode->first_node(); pNode; pNode = pNode->next_sibling())
				{
					if (pNode == NULL)
						break;
					if (strcmp(pNode->name(), "Maze")==0)
					{
						pAttrID = pNode->first_attribute("MazeID");
						pAttrFile = pNode->first_attribute("File");
						char* pValue = pNode->value();
						if (pAttrID)
						{
							mazeID = atoi(pAttrID->value());
							strcpy(temp, listMazeDict[mazeID].c_str());
						}
						else if (pAttrFile)
						{
							mazeID = autoID;
							autoID++;

							strcpy_s(temp, TXTBUFFERLIMIT, pAttrFile->value());
							listMazeDict[mazeID] = temp;
						}
						else if (strlen(pValue) > 4)
						{
							mazeID = autoID;
							autoID++;

							strcpy_s(temp, TXTBUFFERLIMIT, pValue);
							listMazeDict[mazeID] = temp;
						}
						else
							continue;

						this->AddMaze(temp);
					}

					else if (0 == strcmp(pNode->name(), "Command"))
					{
						bool waitForComplete;

						pAttr = pNode->first_attribute("WaitForComplete");
						if (pAttr)
							waitForComplete = strcmpi(pAttr->value(),"true");
						else
							waitForComplete = false;

						char* command = pNode->value();

						if (command)
						{
							this->AddCommand(command, waitForComplete);
						}

					}

					else if ((0 == strcmp(pNode->name(), "Image")|| (0 == strcmp(pNode->name(), "Text"))|| (0 == strcmp(pNode->name(), "MultipleChoice"))))
					{
						isMultipleChoice = (0 == strcmp(pNode->name(), "MultipleChoice"));

						pAttr = pNode->first_attribute("TextDisplayType");
						
						if (pAttr)
						{
							strcpy_s(dlgType, TXTBUFFERLIMIT, pAttr->value());
							if (strcmp(dlgType, "OnDialog") == 0 || strcmp(dlgType, "OnFramedDialog") == 0)
							{
								res = TEXTBOXSTYLE_ONDIALOG_CLEAR_BK;
							}
							else
							{
								res = TEXTBOXSTYLE_ONSCREEN_CLEAR_BK;
							}
						}
						else
							res = TEXTBOXSTYLE_ONDIALOG_CLEAR_BK;

						pAttr = pNode->first_attribute("LifeTime");
						if (pAttr)
							lf = atoi(pAttr->value());
						else
							lf = 0;

						pAttr = pNode->first_attribute("X");
						if (pAttr)
							x = atoi(pAttr->value());
						else
							x = 0;

						pAttr = pNode->first_attribute("Y");
						if (pAttr)
							y = atoi(pAttr->value());
						else
							y = 0;

						pAttr = pNode->first_attribute("Image");
						if (pAttr)
						{
							hasImg = true;
							strcpy_s(imgID, TXTBUFFERLIMIT, pAttr->value());
						}
						else
						{
							hasImg = false;
							strcpy_s(imgID, TXTBUFFERLIMIT, "");
						}

						pAttr = pNode->first_attribute("Audio");
						if (pAttr)
						{
							hasAudio = true;
							strcpy_s(audioID, TXTBUFFERLIMIT, pAttr->value());
						}
						else
						{
							hasAudio = false;
							strcpy_s(audioID, TXTBUFFERLIMIT, "");
						}

						if (isMultipleChoice)
						{
							strcpy_s(msgTemp, MAXMSGTXTLIMIT, pNode->value());
							xml_node<>* pQuestionNode = pNode->first_node("Question");
							if (pQuestionNode)
							{
								sprintf_s(msgTemp, MAXMSGTXTLIMIT, "%s%s",msgTemp, pQuestionNode->value());
							}else
								sprintf_s(msgTemp, MAXMSGTXTLIMIT, "%s",  msgTemp);

							for (xml_node<>* pChoiceNode = pNode->first_node(); pChoiceNode; pChoiceNode = pChoiceNode->next_sibling())
							{
								if (pChoiceNode == NULL)
									break;
								if (strcmp(pChoiceNode->name(), "Choice") == 0)
								{
									sprintf_s(msgTemp, MAXMSGTXTLIMIT, "%s\a%s", msgTemp, pChoiceNode->value());
								}
							}
						}
						else
							strcpy_s(msgTemp, MAXMSGTXTLIMIT, pNode->value());

						int bufEdge = TXTBUFFERLIMIT / 10;
						msgLen = strlen(msgTemp);

						if (!msgLen> TXTBUFFERLIMIT)
							sprintf(msgTemp, "Message too Long");

						multiMsgNum = floor(msgLen * 1.0f / ((TXTBUFFERLIMIT - 1 - bufEdge) * 1.0f)) + 1;

						for (int i = 0; i < multiMsgNum; i++)
						{
							int sz = strlen(msgTemp);
							memcpy(curTxt, &msgTemp[(i) * (TXTBUFFERLIMIT - 1 - bufEdge)], TXTBUFFERLIMIT - 1 - bufEdge);
							curTxt[TXTBUFFERLIMIT - 1 - bufEdge] = 0;
							this->AddText(curTxt, lf, res);
							if (hasImg)
							{
								if (strlen(imgID) > 2)
									this->AddBG(imgID);
							}

						}
					}
				}
			}
			else
			{
				sprintf(errorTxt, "Error! No Items in MazeList");
				return -1;
			}

		}
		catch (const rapidxml::parse_error& e)
		{
			char* err = new char[256];
			sprintf(err, "Corrupted MazeXML File:\n%s\n\n%s", e.what());
			//GUIMessageBox(err, 0, TEXTBOXSTYLE_WARNING);

			return 0;
		}
		strcpy(mListDIR, iDir);
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
//
//    returns 
//           1	successful operation
//          -1	can not open the file
//			-2	incorrect file content   
//   



int MazeList::ReadMazeListClassic(char *filename)
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

	char* bestPath;
	
	//maze list file...
	if(!CheckFileExists(filename))
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

				bestPath = getBestPathImg(temp);
				if (bestPath)
					strcpy(temp, bestPath);
				
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

				int bufEdge = TXTBUFFERLIMIT / 10;

				multiMsgNum = floor(msgLen*1.0f / ((TXTBUFFERLIMIT-1- bufEdge)*1.0f))+1;

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

					bestPath = getBestPathImg(imgID);
					if (bestPath)
						strcpy(imgID, bestPath);
				}

				for (int i=0;i<multiMsgNum;i++)
				{
					int sz = strlen(msgTemp);
					memcpy(curTxt,&msgTemp[(i)*(TXTBUFFERLIMIT - 1- bufEdge)], TXTBUFFERLIMIT-1- bufEdge);
					curTxt[TXTBUFFERLIMIT - 1- bufEdge] = 0;
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
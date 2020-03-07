
#include "Tools.h"
#include <math.h>

bool trueFalse(char *str)
{
	if (atoi(str) == 1)
		return true;
	
	char cStr[10];
	sprintf(cStr,"%s", str);


	if (cStr[0]=='T'||cStr[0]=='t')
		return true;
	else
		return false;
}

bool trueFalse(char *str, bool defaultResponse)
{
	if (str)
		return trueFalse(str);
	else
		return defaultResponse;
}

void addLineBreaksAndMC(char* str,int sz) //replaces \\n with \n   and \\a with \a (multiple choice linebreak)
{
	//int sz = sizeof(&str);
	char* tempStr;

	//	char prev;
	if (str == NULL)
	{
		return;
	}
	strcpy_s(tempStr,sz, str);
	int j = 0;
	for (int i = 0; i < sz - 1; i++) //replace \\n with line break except in errors and replace \\a with \a
	{
		if (tempStr[i] == '\\')
		{
			if (tempStr[i + 1] == 'n')
			{
				tempStr[i + 1] = '\n';
				tempStr[i] = '\n';
				i = i + 1; //skip one letter
				j = j - 1; //note the offset
			}
			else if (tempStr[i + 1] == 'a')
			{
				tempStr[i + 1] = '\a';
				tempStr[i] = '\a';
				i = i + 1;
				j = j - 1;
			}
		}
		str[i + j] = tempStr[i];
	}
	str[sz + j - 1] = 0;
	strcpy(tempStr, str);
	//str[strlen(displayText)] = 0;
}

void trim(char *str)
{
	int i,j=0;
	int size=0;
	for(size=0;size<500;size++)
	{
		if(str[size]==NULL)
			break;
	}
		
	int flag = 0;
	for(i=0;str[i]!='\0';i++)
	{
	
		if (flag != 1 && (str[i] == ' ' || str[i] == '\t'))
		{
			for(j=i; str[j] != NULL; j++)
			{				
				str[j] = str[j+1];
			}
		}
		else if(str[i]=='.')
		{
			flag=-1;
		}
		else if(flag!=-1)
		{
			flag =1;
		}
	}
	str[i-1]='\0';
} 

bool GetUntilTab(FILE* fp,char * out,int len)
{
	if(fp==NULL)
		return false;
	if(out==NULL)
		return false;

	bool ret=false;
	int n=0;
	out[0]=1;
	while(true)
	{		
		fread(&(out[n]),1,1,fp);
		if(out[n]=='\t')
		{
			ret=true;
			break;
		}
		else if( out[n]==-2|| out[n]=='\n' || out[n]==EOF || out[n]=='\r')
		{
			ret=false;
			break;
		}
		else if(n==len ) 
			return false;
		n++;
	}
	//n++;
	out[n]=0;
	return ret;
}

bool IsEqual(float inp1,float inp2)
{
	if( fabs(inp1-inp2)<0.0001)
	{
		return true;
	}
	return false;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *textFileRead(char *fn) {


	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

int textFileWrite(char *fn, char *s) {

	FILE *fp;
	int status = 0;

	if (fn != NULL) {
		fp = fopen(fn,"w");

		if (fp != NULL) {

			if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
				status = 1;
			fclose(fp);
		}
	}
	return(status);
}



char* printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 1)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
		//free(infoLog);
		return infoLog;
	}
	else
		return NULL;
}
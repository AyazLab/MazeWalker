#include "MyLightList.h"

MyLightList::MyLightList(void)
{	
	cListRoot = new LightItem();
}

MyLightList::~MyLightList(void)
{
	Clear();
	delete cListRoot;
}

void  MyLightList::Clear()
{
	//clear the list...
	LightItem *temp,*cur;
	cur = cListRoot->next;
	while(cur!=NULL)
	{
		temp = cur;
		cur=cur->next;
		delete temp;
		//free(temp);
	}
	cListRoot->next=NULL;
}

void MyLightList::AddLight()
{
	LightItem* cur = GetLast();
	cur->next = new LightItem();
}

void MyLightList::AddPositionToLast(float x,float y,float z, float i)
{
	LightItem* cur = GetLast();
	cur->LightPosition[0] = x;
	cur->LightPosition[1] = y;
	cur->LightPosition[2] = z;
	cur->LightPosition[3] = i;
}
void MyLightList::AddAttenToLast(float linear,float quadratic)
{
	LightItem* cur = GetLast();
	cur->linearAtten=linear;
	cur->quadraticAtten=quadratic;
}


void MyLightList::AddAmbientToLast(float ar,float ag,float ab,float ai)
{
	LightItem* cur = GetLast();
	cur->LightAmbient[0] = ar*ai;
	cur->LightAmbient[1] = ag*ai;
	cur->LightAmbient[2] = ab*ai;
	cur->LightAmbient[3] = 1;
}

void MyLightList::AddDiffuseToLast(float dr,float dg,float db,float di)
{
	LightItem* cur = GetLast();
	cur->LightDiffuse[0] = dr*di;
	cur->LightDiffuse[1] = dg*di;
	cur->LightDiffuse[2] = db*di;
	cur->LightDiffuse[3] = 1;
}

LightItem* MyLightList::GetLast()
{
	LightItem *ret=cListRoot;
	while(ret->next!=NULL)
	{
		ret = ret->next;
	}
	return ret;
}

LightItem* MyLightList::GetFirst()
{
	return cListRoot->next;
}

int MyLightList::GetLength()
{
	LightItem *ret=cListRoot;
	int count=0;
	while(ret->next!=NULL)
	{
		ret = ret->next;
		count++;
	}
	return count;
}
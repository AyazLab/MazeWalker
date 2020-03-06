#pragma once

#include "LightItem.h"

class MyLightList
{
public:
	LightItem* cListRoot;
	MyLightList(void);
	~MyLightList(void);

	void Clear();
	int GetLength();
	LightItem* GetLast();
	LightItem* GetFirst();
	void AddLight();
	void AddPositionToLast(float x,float y,float z, float i);
	void AddAmbientToLast(float ar,float ag,float ab,float ai);
	void AddDiffuseToLast(float dr,float dg,float db,float di);
	void AddAttenToLast(float linear,float quadratic);
};

#pragma once

#include "main.h"
#include <gl/GL.h>

class LightItem
{
public:
	LightItem(void);
	~LightItem(void);

	GLfloat LightPosition[4];
	GLfloat LightAmbient[4];
	GLfloat LightDiffuse[4];
	float quadraticAtten;
	float linearAtten;
	float distFromPlayer;
	int index;

	LightItem *next;
};


#include <stdio.h>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "GL/glew.h"	
#include "glmimg.h"
#include "glm.h"



#define	MAX_PARTICLES	9999		// Number Of Particles To Create
#define SPEED 100;

typedef struct						// Create A Structure For Particle
{
	bool	active;					// Active (Yes/No)
	float	life;					// Particle Life
	float	fade;					// Fade Speed
	float	r;						// Red Value
	float	g;						// Green Value
	float	b;						// Blue Value
	float	x;						// X Position
	float	y;						// Y Position
	float	z;						// Z Position
	float	xi;						// X Direction
	float	yi;						// Y Direction
	float	zi;						// Z Direction
	float	xg;						// X Gravity
	float	yg;						// Y Gravity
	float	zg;						// Z Gravity
	float	angle;					// circle location
}
particles;							// Particles Structure

typedef struct particleArray 
{
	bool activated;
	Vector pos;
	float radius;
	particles particle[MAX_PARTICLES];
	int color;
	bool up;
	particleArray *next;
	int particleCount;
	int dID;//itemID of corresponding dynamic Object
}
particleArray;

static GLfloat colors[12][3]=		// Rainbow Of Colors
{
	{1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
	{0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
	{0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};

void initParticles(particleArray* partArray);
GLvoid drawParticles(particleArray* partArray);
void updateParticles(particleArray* partArray);
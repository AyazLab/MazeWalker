#include"particle.h"
float zoom=0;
float xspeed=0;
float yspeed=SPEED;
float slowdown=50;





void initParticles(particleArray* partArray)
{
	for (int loop=0;loop<MAX_PARTICLES;loop++)				// Initials All The Textures
	{
		partArray->particle[loop].active=true;								// Make All The Particles Active
		partArray->particle[loop].life=1.0f;								// Give All The Particles Full Life
		partArray->particle[loop].fade=float(rand()%100)/1000.0f+0.003f;	// Random Fade Speed
		partArray->particle[loop].r=colors[partArray->color][0];	// Select Red Rainbow Color
		partArray->particle[loop].g=colors[partArray->color][1];	// Select Red Rainbow Color
		partArray->particle[loop].b=colors[partArray->color][2];	// Select Red Rainbow Color
		partArray->particle[loop].xi=float((rand()%50)-26.0f)*10.0f;		// Random Speed On X Axis
		partArray->particle[loop].yi=float((rand()%50)-25.0f)*10.0f;		// Random Speed On Y Axis
		partArray->particle[loop].zi=float((rand()%50)-25.0f)*10.0f;		// Random Speed On Z Axis
		partArray->particle[loop].xg=0.0f;// Set Horizontal Pull To Zero
		partArray->particle[loop].yg=-0.8f;								// Set Vertical Pull Downward
		partArray->particle[loop].zg=0.0f;// Set Pull On Z Axis To Zero
		partArray->particle[loop].angle=float((rand()%360));
	}
}

GLvoid drawParticles (particleArray* partArray)
{

	//glBindTexture(GL_TEXTURE_2D,partArray.textureID);
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glPushMatrix();
	glTranslatef(partArray->pos.x,0,partArray->pos.z);
	if(partArray->up)
	{
		yspeed=SPEED;
		yspeed*=-.5;
		glTranslatef(0,1,0);
	}
	if(!partArray->up)
	{
		yspeed=SPEED;
		yspeed*=2;
		glTranslatef(0,-1,0);
	}
	for (int loop=0;loop<partArray->particleCount;loop++)					// Loop Through All The partArray->particles
	{
		if (partArray->particle[loop].active)							// If The partArray->particle Is Active
		{
			glPushMatrix();
			glRotatef(partArray->particle[loop].angle,0,1.0f,0);
			glTranslatef(partArray->radius+rand()%3/10,0,0);
			float x=partArray->particle[loop].x;						// Grab Our partArray->particle X Position
			float y=partArray->particle[loop].y;						// Grab Our partArray->particle Y Position
			float z=partArray->particle[loop].z+zoom;					// partArray->particle Z Pos + Zoom

			// Draw The partArray->particle Using Our RGB Values, Fade The partArray->particle Based On It's Life
			glColor4f(partArray->particle[loop].r,partArray->particle[loop].g,partArray->particle[loop].b,partArray->particle[loop].life);

			glBegin(GL_TRIANGLE_STRIP);						// Build Quad From A Triangle Strip
			//glTexCoord2d(1,1); 
			glVertex3f(x+0.01f,y+0.01f,z); // Top Right
			//glTexCoord2d(0,1); 
			glVertex3f(x-0.01f,y+0.01f,z); // Top Left
			//glTexCoord2d(1,0); 
			glVertex3f(x+0.01f,y-0.01f,z); // Bottom Right
			//glTexCoord2d(0,0); 
			glVertex3f(x-0.01f,y-0.01f,z); // Bottom Left
			glEnd();										// Done Building Triangle Strip
			glPopMatrix();
		}
	}
	glEnable(GL_DEPTH_TEST);
	glPopMatrix();
	glDisable(GL_BLEND);
}

void updateParticles(particleArray* partArray)
{
	for (int loop=0;loop<partArray->particleCount;loop++)
	{
		partArray->particle[loop].x+=partArray->particle[loop].xi/(slowdown*1000);// Move On The X Axis By X Speed
		partArray->particle[loop].y+=partArray->particle[loop].yi/(slowdown*1000);// Move On The Y Axis By Y Speed
		partArray->particle[loop].z+=partArray->particle[loop].zi/(slowdown*1000);// Move On The Z Axis By Z Speed

		partArray->particle[loop].xi+=partArray->particle[loop].xg;			// Take Pull On X Axis Into Account
		partArray->particle[loop].yi+=partArray->particle[loop].yg;			// Take Pull On Y Axis Into Account
		partArray->particle[loop].zi+=partArray->particle[loop].zg;			// Take Pull On Z Axis Into Account
		partArray->particle[loop].life-=partArray->particle[loop].fade;		// Reduce partArray->particles Life By 'Fade'
		if (partArray->particle[loop].life<0.0f&&partArray->activated)					// If partArray->particle Is Burned Out
		{
			partArray->particle[loop].life=1.0f;					// Give It New Life
			partArray->particle[loop].fade=float(rand()%100)/1000.0f+0.003f;	// Random Fade Value
			partArray->particle[loop].x=0.0f;						// Center On X Axis
			partArray->particle[loop].y=0.0f;						// Center On Y Axis
			partArray->particle[loop].z=0.0f;						// Center On Z Axis
			partArray->particle[loop].xi=xspeed+float((rand()%60)-32.0f);	// X Axis Speed And Direction
			partArray->particle[loop].yi=yspeed+float((rand()%60)-30.0f);	// Y Axis Speed And Direction
			partArray->particle[loop].zi=float((rand()%60)-30.0f);	// Z Axis Speed And Direction
			partArray->particle[loop].r=colors[partArray->color][0];			// Select Red From Color Table
			partArray->particle[loop].g=colors[partArray->color][1];			// Select Green From Color Table
			partArray->particle[loop].b=colors[partArray->color][2];			// Select Blue From Color Table
			partArray->particle[loop].angle=float((rand()%360));
		}
		if(partArray->particle[loop].life<0.0f&&!partArray->activated)
			partArray->particle[loop].y=-19;
	}
}


//#include "windows.h"
#include "stdio.h"
//#include "Map.h"
#include "main.h"
#include "GL/glew.h"			// Header File For The OpenGL32 Library
#include ".\map.h"
#include "TextureDictionary.h"
#include "glm.h"
#include "MyLightList.h"



extern TextureDictionary curTextureDict;
//extern GLuint	textureL[MAX_TEXTURES];			// Storage Textures
extern FILE *plog;
extern DWORD startTime;
extern bool shader;
extern GLuint shaderProg;
extern GLuint shaderProgNoLights;
extern CCamera objCamera;
extern float specialVar2;

extern btDynamicsWorld*						m_dynamicsWorld; //this is the most important class
extern btAlignedObjectArray<btCollisionShape*>	m_collisionShapes; //keep the collision shapes, for deletion/cleanup

extern bool bMazeEndReached;
extern bool bMazeLoop;

#define MAX_OBJECTS 300
#define MAX_WALLS 100000

extern MapModel *MapModelHead;  //defined in main.cpp

extern MyLightList curLightList; //defined in main.cpp

GLuint queries[MAX_WALLS+MAX_OBJECTS];



#define SAFEGUARD1  0.3f
#define SAFEGUARD	0.8f
#define SAFEGUARD2	1.0f

Map::Map()
{
	root=NULL;
	bEndPos=false;
	maxMazeTime=0;
	numObjs=0;
	modelCount=0;
	dModelCount=0;
	startMessageEnabled = false;

	strcpy_s(timeoutMessage,500,"You could NOT reach the end of the MAZE!");
	strcpy_s(successMessage,500,"You reach the end of the MAZE!");
	strcpy_s(startMessage,500, "Welcome!");
}

Map::~Map()
{
	struct SPlane *temp;
	temp=root;
	if(root!=NULL)
	{
		while(temp)
		{
			temp=root->next;
			//delete root;
			free(root);
			root=temp;		
		}
	}

	EndRegion* endRegion;
	endRegion = endRegionHead;

	if (endRegionHead != NULL)
	{
		while (endRegionHead)
		{
			endRegion = endRegionHead->next;
			//delete root;
			free(endRegionHead);
			endRegionHead = endRegion;
		}
	}

	ActiveRegion* activeRegion;
	activeRegion = activeRegionHead;

	if (activeRegionHead != NULL)
	{
		while (activeRegionHead)
		{
			activeRegion = activeRegionHead->next;
			//delete root;
			free(activeRegionHead);
			activeRegionHead = activeRegion;
		}
	}

	StartPosition* startPos;
	//startPos = startPosHead;
	startPosDefault = NULL;

	if (startPosHead != NULL)
	{
		while (startPosHead)
		{
			startPos = startPosHead->next;
			//delete root;
			free(startPosHead);
			startPosHead = startPos;

		}
	}

	delete(colMesh);
}

//  parameter: visibility: 0-invisible, 1-visible
//





int Map::Add(int type,int texture,struct SColor color,struct SVertex v[4],int curVisibility)
{

	struct SPlane *temp=NULL;
	//temp=root;
	if(root==NULL)
	{
			root=new struct SPlane;
			root->type=type;
			root->texture=texture;
			
			root->col.b=color.b;
			root->col.g=color.g;
			root->col.r=color.r;
			root->pID=0;
			wallCount=1;
			for(int i=0;i<4;i++)
			{
				root->vertex[i].x=v[i].x;
				root->vertex[i].y=v[i].y;
				root->vertex[i].z=v[i].z;
				root->vertex[i].tex_x=v[i].tex_x;
				root->vertex[i].tex_y=v[i].tex_y;
			}
			
			
			root->visible=curVisibility;
			//////////////////////////////////////////////////////////////////////////
			
			float u[3],n[3],v[3];
			
			u[0] = root->vertex[1].x-root->vertex[0].x;
			u[1] = root->vertex[1].y-root->vertex[0].y;
			u[2] = root->vertex[1].z-root->vertex[0].z;

			v[0] = root->vertex[3].x-root->vertex[0].x;
			v[1] = root->vertex[3].y-root->vertex[0].y;
			v[2] = root->vertex[3].z-root->vertex[0].z;

			n[0] = u[1]*v[2] - u[2]*v[1];
			n[1] = u[2]*v[0] - u[0]*v[2];
			n[2] = u[0]*v[1] - u[1]*v[0];

			float l=1;			
			l = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
			n[0] /= l;
			n[1] /= l;
			n[2] /= l;

			root->normal[0]=n[0];
			root->normal[1]=n[1];
			root->normal[2]=n[2];

			//////////////////////////////////////////////////////////////////////////
			

			colMesh = new btTriangleMesh();
			btVector3 v1= btVector3(root->vertex[0].x,root->vertex[0].y,root->vertex[0].z);
			btVector3 v2= btVector3(root->vertex[1].x,root->vertex[1].y,root->vertex[1].z);
			btVector3 v3= btVector3(root->vertex[2].x,root->vertex[2].y,root->vertex[2].z);
			btVector3 v4= btVector3(root->vertex[3].x,root->vertex[3].y,root->vertex[3].z);

			double dist12 = std::pow(root->vertex[0].x - root->vertex[1].x, 2) + std::pow(root->vertex[0].y - root->vertex[1].y, 2) + std::pow(root->vertex[0].z - root->vertex[1].z, 2);
			double dist13 = std::pow(root->vertex[0].x - root->vertex[2].x, 2) + std::pow(root->vertex[0].y - root->vertex[2].y, 2) + std::pow(root->vertex[0].z - root->vertex[2].z, 2);
			double dist14 = std::pow(root->vertex[0].x - root->vertex[3].x, 2) + std::pow(root->vertex[0].y - root->vertex[3].y, 2) + std::pow(root->vertex[0].z - root->vertex[3].z, 2);

			if (dist12 > dist13&&dist12 > dist14)
			{
				colMesh->addTriangle(v1, v3, v2, false);
				colMesh->addTriangle(v1, v4, v2, false);
			}
			else if (dist13 > dist12&&dist13 > dist14)
			{
				colMesh->addTriangle(v1, v2, v3, false);
				colMesh->addTriangle(v1, v4, v3, false);
			}
			else
			{
				colMesh->addTriangle(v1, v2, v4, false);
				colMesh->addTriangle(v1, v3, v4, false);
			}

			


			root->polyOffset=0;

			
			
			root->distToPlayer=0.5;

			root->next=NULL;
			SetDirectionInfo(root);
			return 1;
	}
	else
	{
		temp = new struct SPlane;
		temp->type = type;
		temp->texture = texture;
		temp->col.b = color.b;
		temp->col.g = color.g;
		temp->col.r = color.r;
		for (int i = 0; i<4; i++)
		{
			temp->vertex[i].x = v[i].x;
			temp->vertex[i].y = v[i].y;
			temp->vertex[i].z = v[i].z;
			temp->vertex[i].tex_x = v[i].tex_x;
			temp->vertex[i].tex_y = v[i].tex_y;
		}
		temp->visible = curVisibility;

		//////////////////////////////////////////////////////////////////////////

		float u[3], n[3], v[3];

		u[0] = temp->vertex[1].x - temp->vertex[0].x;
		u[1] = temp->vertex[1].y - temp->vertex[0].y;
		u[2] = temp->vertex[1].z - temp->vertex[0].z;

		v[0] = temp->vertex[3].x - temp->vertex[0].x;
		v[1] = temp->vertex[3].y - temp->vertex[0].y;
		v[2] = temp->vertex[3].z - temp->vertex[0].z;

		n[0] = u[1] * v[2] - u[2] * v[1];
		n[1] = u[2] * v[0] - u[0] * v[2];
		n[2] = u[0] * v[1] - u[1] * v[0];

		float l;

		l = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		n[0] /= l;
		n[1] /= l;
		n[2] /= l;

		temp->normal[0] = n[0];
		temp->normal[1] = n[1];
		temp->normal[2] = n[2];


		//////////////////////////////////////////////////////////////////////////


		temp->pID = wallCount;
		wallCount++;

		btVector3 v1 = btVector3(temp->vertex[0].x, temp->vertex[0].y, temp->vertex[0].z);
		btVector3 v2 = btVector3(temp->vertex[1].x, temp->vertex[1].y, temp->vertex[1].z);
		btVector3 v3 = btVector3(temp->vertex[2].x, temp->vertex[2].y, temp->vertex[2].z);
		btVector3 v4 = btVector3(temp->vertex[3].x, temp->vertex[3].y, temp->vertex[3].z);

		double dist12 = std::pow(temp->vertex[0].x - temp->vertex[1].x, 2) + std::pow(temp->vertex[0].y - temp->vertex[1].y, 2) + std::pow(temp->vertex[0].z - temp->vertex[1].z, 2);
		double dist13 = std::pow(temp->vertex[0].x - temp->vertex[2].x, 2) + std::pow(temp->vertex[0].y - temp->vertex[2].y, 2) + std::pow(temp->vertex[0].z - temp->vertex[2].z, 2);
		double dist14 = std::pow(temp->vertex[0].x - temp->vertex[3].x, 2) + std::pow(temp->vertex[0].y - temp->vertex[3].y, 2) + std::pow(temp->vertex[0].z - temp->vertex[3].z, 2);

		if (dist12 > dist13&&dist12 > dist14)
		{
			colMesh->addTriangle(v1, v3, v2, false);
			colMesh->addTriangle(v1, v4, v2, false);
		}
		else if (dist13 > dist12&&dist13 > dist14)
		{
			colMesh->addTriangle(v1, v2, v3, false);
			colMesh->addTriangle(v1, v4, v3, false);
		}
		else
		{
			colMesh->addTriangle(v1, v2, v4, false);
			colMesh->addTriangle(v1, v3, v4, false);
		}




		temp->next = root;
		SetDirectionInfo(temp);
		temp->distToPlayer = 0.5;


		temp->polyOffset = countCollidingPlane(temp); //doesn't do this anymore, randomly assigns z buffers

		root = temp;

		return 1;
	}
	
	
	

	return 0;
}

int lastVal=0;
int Map::countCollidingPlane(SPlane* search)
{
	struct SPlane *temp=NULL;
	temp=root;
	int count=0;
	if(search->visible==0)
		return 0;

	if(lastVal>9) //whatever I give up 1/7 planes flashes
		lastVal=0;

	return lastVal++;

	//while(temp) //recursively searches for all colliding planes nakes N^2 and is not guarenteed to work anyways
	//{

	//	if(temp->next!=NULL&&temp->visible)
	//	{
	//		float dist=CalcDistanceToPoint(temp->midPoint.x,temp->midPoint.y,temp->midPoint.z,search->midPoint.x,search->midPoint.y,search->midPoint.z);
	//		if(dist>temp->radius+search->radius)
	//			continue;
	//		else if(temp->CollModel->collision(search->CollModel))
	//		{
	//			//see if coplanar
	//			float tri1[9],tri2[9];
	//			temp->CollModel->getCollidingTriangles(tri1,tri2);

	//			Vector3D triA1= Vector3D(tri1[3]-tri1[0],tri1[4]-tri1[1],tri1[5]-tri1[2]);
	//			Vector3D triA2= Vector3D(tri1[6]-tri1[0],tri1[7]-tri1[1],tri1[8]-tri1[2]);
	//			Vector3D triAcross = CrossProduct(triA1,triA2);
	//			triAcross=triAcross.Normalized();

	//			Vector3D triB1= Vector3D(tri2[3]-tri2[0],tri2[4]-tri2[1],tri2[5]-tri2[2]);
	//			Vector3D triB2= Vector3D(tri2[6]-tri2[0],tri2[7]-tri2[1],tri2[8]-tri2[2]);
	//			Vector3D triBcross = CrossProduct(triB1,triB2);
	//			triBcross=triBcross.Normalized();

	//			Vector3D test = triAcross-triBcross;
	//			if(test.Magnitude()>1)
	//				test=triBcross-triAcross;

	//			if(test.Magnitude()<0.001)
	//				count++;
	//		}
	//	}
	//	temp=temp->next;

	//}
	//count--; //counts
	//return count;
}

void Map::Count()
{
	struct SPlane *temp;
	temp=root;

	wallCount=0;

	while(temp)
	{
		wallCount++;
		temp=temp->next;		
	}


	glGenQueriesARB(wallCount+modelCount,queries);

}
extern bool useAniso;


void Map::Draw()
{
	int i;
	bool Count=true;

	
	GLuint sampleCount=0;
	GLfloat u[3];
	GLfloat v[3];
	GLfloat n[3];
	GLfloat l;

	struct SPlane *temp;
	temp=root;


	if(shader)
	{
		if(lights)
			glUseProgram(shaderProg);
		else
			glUseProgram(shaderProgNoLights);
	}
	else if (shadersCompiled)
		glUseProgram(0);
	
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(0.2, 1.0);

	wallsDrawn=0;
	while(temp)
	{
		if(temp->visible==0)
		{
			temp=temp->next;
			continue;
		}
		glGetQueryObjectuivARB(queries[temp->pID], GL_QUERY_RESULT_ARB, &sampleCount);
		if(sampleCount>0)
		{
			
			
			if(temp->texture>0)
			{
				glColor3f(1,1,1);

 				glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(temp->texture) );
				if(useAniso)
					glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,8 );
				else
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				}
				//glTexParameteri(GL_TEXTURE_2D, GL_MAX_TEXTURE_LOD_BIAS, -200);

				if( curTextureDict.Get_glKey(temp->texture) <=0)
				{
					glColor3f(temp->col.r,temp->col.g,temp->col.b);
					glBindTexture(GL_TEXTURE_2D,curTextureDict.GetWhite());
					
				}
			}


			else
			{
				glBindTexture(GL_TEXTURE_2D,curTextureDict.GetWhite());
				glColor3f(temp->col.r,temp->col.g,temp->col.b);
			}

			switch(temp->type)
			{
				case MO_PLANE:

					if(temp->polyOffset>0)
					{
						glEnable(GL_POLYGON_OFFSET_FILL);
						glPolygonOffset((temp->polyOffset-1)/10.0f,(temp->polyOffset-1)/10.0f);
					}

    				glBegin(GL_QUADS);
					
					
					glNormal3fv(temp->normal);
					for(i=0;i<4;i++)
					{	
						glTexCoord2f(temp->vertex[i].tex_x, temp->vertex[i].tex_y);
						glVertex3f(temp->vertex[i].x,temp->vertex[i].y,temp->vertex[i].z);
					}

					glEnd();
					if(temp->polyOffset>0)
					{
						glDisable(GL_POLYGON_OFFSET_FILL);
						
					}
					break;
				case MO_TRI:
					glBegin(GL_TRIANGLES );
					u[0] = temp->vertex[0].x-temp->vertex[1].x;
					u[1] = temp->vertex[0].y-temp->vertex[1].y;
					u[2] = temp->vertex[0].z-temp->vertex[1].z;

					v[0] = temp->vertex[0].x-temp->vertex[2].x;
					v[1] = temp->vertex[0].y-temp->vertex[2].y;
					v[2] = temp->vertex[0].z-temp->vertex[2].z;

					n[0] = u[1]*v[2] - u[2]*v[1];
					n[1] = u[2]*v[0] - u[0]*v[2];
					n[2] = u[0]*v[1] - u[1]*v[0];

					l = (GLfloat)sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
					n[0] /= l;
					n[1] /= l;
					n[2] /= l;

					glNormal3fv(n);
	//				glNormal3fv(temp->normal);
					for(i=0;i<3;i++)
					{
						//glTexCoord2f(temp->vertex[i].tex_x, temp->vertex[i].tex_y);
						glVertex3f(temp->vertex[i].x,temp->vertex[i].y,temp->vertex[i].z);
					}
				
				
					glEnd();
					break;
			}
			wallsDrawn++;
		}
		
		temp=temp->next;		
	}


	MapModel *temp2;
	temp2=MapModelHead;

	modelsDrawn=0;


	while(temp2!=NULL)
	{

		glGetQueryObjectuivARB(queries[temp2->instanceID+wallCount], GL_QUERY_RESULT_ARB, &sampleCount);
		if(sampleCount>0)
			modelsDrawn++;

		if(sampleCount>0&&!temp2->transparent&&temp2->isVisible)
		{
			glPushMatrix();
			glTranslatef(temp2->pos.x,temp2->pos.y,temp2->pos.z);
			glRotatef(temp2->rot.x, 1.0f, 0.0f, 0.0f);
			glRotatef(temp2->rot.y, 0.0f, 1.0f, 0.0f);
			glRotatef(temp2->rot.z, 0.0f, 0.0f, 1.0f);

			glmReducedDraw(temp2->modelData,temp2->scale,temp2->glowFactor,temp2->glowColor);
			
			glPopMatrix();


		}


		temp2=temp2->next;
	}

	float maxDistance=0;

	temp2=MapModelHead;
	int numTransparentModels=0;

	while(temp2!=NULL) //render dynamic objects, and find number of visible tranparent objects
	{
		if(temp2->isVisible&&temp2->transparent)
		{
			numTransparentModels++;
			if(temp2->distToPlayer>maxDistance)
				maxDistance=temp2->distToPlayer;

		}
		
		temp2=temp2->next;
	}


	//////////////////////////////////////////////////////////////////////////
	// 
	float nextMaxDist=0;
	temp2=MapModelHead;

	while(numTransparentModels>0)
	{
		while(temp2!=NULL)
		{
			glGetQueryObjectuivARB(queries[temp2->instanceID+wallCount], GL_QUERY_RESULT_ARB, &sampleCount);
			if (temp2->isVisible&&temp2->transparent&&IsEqual(maxDistance,temp2->distToPlayer))
			{
				if(sampleCount>0)
				{
					glPushMatrix();
					glTranslatef(temp2->pos.x,temp2->pos.y,temp2->pos.z);
					glRotatef(temp2->rot.x, 1.0f, 0.0f, 0.0f);
					glRotatef(temp2->rot.y, 0.0f, 1.0f, 0.0f);
					glRotatef(temp2->rot.z, 0.0f, 0.0f, 1.0f);

					glmReducedDraw(temp2->modelData,temp2->scale,temp2->glowFactor,temp2->glowColor);

					glPopMatrix();
				}

				numTransparentModels--;
			}
			if(temp2->distToPlayer>nextMaxDist&&temp2->distToPlayer<maxDistance)
				nextMaxDist=temp2->distToPlayer;
			if(temp2->distToPlayer<0)
				temp2->distToPlayer=0;
			temp2=temp2->next;
		}
		maxDistance=nextMaxDist;
		nextMaxDist=0;
		temp2=MapModelHead;

	}

	glBindTexture(GL_TEXTURE_2D,curTextureDict.GetWhite());
}
extern float iTopDownHeight;


void Map::DrawSimple()
{
	int i;
	bool Count = true;




	GLuint sampleCount = 0;
	GLfloat u[3];
	GLfloat v[3];
	GLfloat n[3];
	GLfloat l;

	struct SPlane *temp;
	temp = root;


	if (shader)
	{
		if (lights)
			glUseProgram(shaderProg);
		else
			glUseProgram(shaderProgNoLights);
	}
	else if (shadersCompiled)
		glUseProgram(0);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(0.2, 1.0);

	wallsDrawn = 0;
	while (temp) //cycle through all visible walls
	{
		if (temp->visible == 0)
		{
			temp = temp->next;
			continue;
		}
		glGetQueryObjectuivARB(queries[temp->pID], GL_QUERY_RESULT_ARB, &sampleCount);
		
		if (temp->distToPlayer<iTopDownHeight * 1 || sampleCount>0)
		{

				glBindTexture(GL_TEXTURE_2D, curTextureDict.GetWhite());
				glColor3f(temp->col.r, temp->col.g, temp->col.b);
				glEnable(GL_LINE_SMOOTH);
				glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glLineWidth(5.f);//reset

			switch (temp->type)
			{
			case MO_PLANE:

				if (temp->vertex[0].y == temp->vertex[1].y && temp->vertex[0].y == temp->vertex[2].y) //floor
				{
					break;
				}
				
				if (temp->vertex[0].x != temp->vertex[1].x&&temp->vertex[0].z != temp->vertex[1].z)
				{
					glBegin(GL_LINES);
					glVertex3f(temp->vertex[0].x, 0, temp->vertex[0].z);
					glVertex3f(temp->vertex[1].x, 0, temp->vertex[1].z);
					glEnd();
				}

				if (temp->vertex[1].x != temp->vertex[2].x||temp->vertex[1].z != temp->vertex[2].z)
				{
					glBegin(GL_LINES);
					glVertex3f(temp->vertex[1].x, 0, temp->vertex[1].z);
					glVertex3f(temp->vertex[2].x, 0, temp->vertex[2].z);
					glEnd();
				}

				if (temp->vertex[2].x != temp->vertex[3].x||temp->vertex[2].z != temp->vertex[3].z)
				{
					glBegin(GL_LINES);
					glVertex3f(temp->vertex[3].x, 0, temp->vertex[3].z);
					glVertex3f(temp->vertex[2].x, 0, temp->vertex[2].z);
					glEnd();
				}
				break;
			case MO_TRI:
				glBegin(GL_LINES);
				glVertex3f(temp->vertex[0].x, temp->vertex[0].z, 0);
				glVertex3f(temp->vertex[1].x, temp->vertex[1].z, 0);
				glEnd();

				glBegin(GL_LINES);
				glVertex3f(temp->vertex[1].x, temp->vertex[1].z, 0);
				glVertex3f(temp->vertex[2].x, temp->vertex[2].z, 0);
				glEnd();

				break;
			}
			wallsDrawn++;
		}

		temp = temp->next;
	}


	MapModel *temp2; 
	temp2 = MapModelHead;

	modelsDrawn = 0;


	while (temp2 != NULL) 
	{

		glGetQueryObjectuivARB(queries[temp2->instanceID + wallCount], GL_QUERY_RESULT_ARB, &sampleCount);
		if (sampleCount>0)
			modelsDrawn++;

		if (sampleCount>0 && !temp2->transparent&&temp2->isVisible)
		{
			glPushMatrix();
			glTranslatef(temp2->pos.x, temp2->pos.y, temp2->pos.z);
			glRotatef(temp2->rot.x, 1.0f, 0.0f, 0.0f);
			glRotatef(temp2->rot.y, 0.0f, 1.0f, 0.0f);
			glRotatef(temp2->rot.z, 0.0f, 0.0f, 1.0f);

			

			glmReducedDraw(temp2->modelData, temp2->scale, temp2->glowFactor, temp2->glowColor);

			glPopMatrix();


		}


		temp2 = temp2->next;
	}

	float maxDistance = 0;

	temp2 = MapModelHead;
	int numTransparentModels = 0;

	while (temp2 != NULL) //render dynamic objects, and find number of visible tranparent objects
	{
		if (temp2->isVisible&&temp2->transparent)
		{
			numTransparentModels++;
			if (temp2->distToPlayer>maxDistance)
				maxDistance = temp2->distToPlayer;

		}

		temp2 = temp2->next;
	}


	//////////////////////////////////////////////////////////////////////////
	// 
	float nextMaxDist = 0;
	temp2 = MapModelHead;

	while (numTransparentModels>0)
	{
		while (temp2 != NULL)
		{
			glGetQueryObjectuivARB(queries[temp2->instanceID + wallCount], GL_QUERY_RESULT_ARB, &sampleCount);
			if (temp2->isVisible&&temp2->transparent&&IsEqual(maxDistance, temp2->distToPlayer))
			{
				if (sampleCount>0)
				{
					glPushMatrix();
					glTranslatef(temp2->pos.x, temp2->pos.y, temp2->pos.z);
					glRotatef(temp2->rot.x, 1.0f, 0.0f, 0.0f);
					glRotatef(temp2->rot.y, 0.0f, 1.0f, 0.0f);
					glRotatef(temp2->rot.z, 0.0f, 0.0f, 1.0f);

					glmReducedDraw(temp2->modelData, temp2->scale, temp2->glowFactor, temp2->glowColor);

					

					glPopMatrix();
				}

				numTransparentModels--;
			}
			if (temp2->distToPlayer>nextMaxDist&&temp2->distToPlayer<maxDistance)
				nextMaxDist = temp2->distToPlayer;
			if (temp2->distToPlayer<0)
				temp2->distToPlayer = 0;
			temp2 = temp2->next;
		}
		maxDistance = nextMaxDist;
		nextMaxDist = 0;
		temp2 = MapModelHead;

	}

	glBindTexture(GL_TEXTURE_2D, curTextureDict.GetWhite());
}

void Map::SortWalls()
{

	SPlane *temp;
	temp=root;
	SPlane *nextNode;
	SPlane *prevNode;

	prevNode=nullptr;
	int i=0;
	
	for (int j=0; j<wallCount; j++)
	{
		bool sorted=true;
		if(j*wallCount>10000)
			break;
		while (temp->next)  //iterate through list until next is null
		{
			if (temp->distToPlayer > temp->next->distToPlayer)
			{
				nextNode=temp->next;
				temp->next=nextNode->next;
				nextNode->next=temp;

				if(prevNode)
					prevNode->next=nextNode;
				else
					root=nextNode;

				temp=nextNode;
				sorted=false;
			}
			else 
			{
				prevNode=temp;
				temp=temp->next;
				i++;
			}

	

		}	

		if(sorted)
			break;


		temp=root;
		prevNode=nullptr;
	}
	return;
}

int validQuerie = 0;

void Map::DrawQuery()
{
	
	
	int i;
	GLfloat u[3];
	GLfloat v[3];
	GLfloat n[3];
	GLfloat l;

	float maxDistance=0;
	float nextMaxDist=0;
	float x=0;

	struct SPlane *temp;
	temp=root;

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_TRUE);

	if(!shaders&&shadersCompiled)
		glUseProgram(0);

	
	

	while(temp)
		{
			if(temp->visible)
			{
				glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[temp->pID]);
				switch(temp->type)
				{
					case MO_PLANE:
    					glBegin(GL_QUADS);
				
						for(i=0;i<4;i++)
						{	
							glVertex3f(temp->vertex[i].x,temp->vertex[i].y,temp->vertex[i].z);
						}

						glEnd();
						break;
					case MO_TRI:

						for(i=0;i<3;i++)
						{
							glVertex3f(temp->vertex[i].x,temp->vertex[i].y,temp->vertex[i].z);
						}
				
						glEnd();
						break;
					
				}
				if (validQuerie==0)
					validQuerie = temp->pID;

				glEndQueryARB(GL_SAMPLES_PASSED_ARB);
				
			}
			temp=temp->next;	
		}



	MapModel *temp2;
	temp2=MapModelHead;
	int modelCount=0;
	maxDistance=0;
	nextMaxDist=0;

	while(temp2!=NULL) //get all objects, render furthest to closest
	{
		if (temp2->isVisible)
		{
			temp2->drawFlag = true;
			if (temp2->distToPlayer > maxDistance)
			{
				maxDistance = temp2->distToPlayer;
			}
			modelCount++;
		}
		else
			temp2->drawFlag = false;
		temp2=temp2->next;
	}
	
	temp2=MapModelHead;
	
	while(modelCount>0)
	{
		while(temp2!=NULL)
		{
			if(temp2->drawFlag)
			{
				if(IsEqual(maxDistance,temp2->distToPlayer))
				{
					temp2->drawFlag = false;
					glBeginQueryARB(GL_SAMPLES_PASSED_ARB, queries[temp2->instanceID+wallCount]);
				

					glmMinDrawQuery(temp2->BBoxCoords,temp2->pos,false);

				
					glEndQueryARB(GL_SAMPLES_PASSED_ARB);
					modelCount--;
				}

				if(temp2->distToPlayer>nextMaxDist&&temp2->distToPlayer<maxDistance)
					nextMaxDist=temp2->distToPlayer;
			}

			temp2=temp2->next;
		}

		

		maxDistance=nextMaxDist;
		nextMaxDist=0;
		temp2=MapModelHead;

	}

	///queries resolve after update

	glBindTexture(GL_TEXTURE_2D,curTextureDict.GetWhite());
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);

}

bool compareDist (SPlane * p1, SPlane * p2)
{
	return ( p1->distToPlayer<p2->distToPlayer );
}

extern DWORD GetQPC();
DWORD timeSinceCheckCollision =0;

int Map::CheckCollision(float* x,float* y,float* z)
{

	float playerPos[3];
	playerPos[0]=*x;
	playerPos[1]=*y-0.3; //view translation
	playerPos[2]=*z;

	
	SPlane *temp;
	

	int numPlanes=0;

	int w=0;
	if(GetQPC()-timeSinceCheckCollision>4300)
	{
	//Calculate Plane Distances from initial position
		temp=root;
		while(temp!=NULL)
		{
			CalcDistance(temp,playerPos[0],playerPos[1],playerPos[2],&temp->distToPlayer);

			temp=temp->next;
			w++;

		}
		timeSinceCheckCollision=GetQPC();
		wallCount=w;
		SortWalls();
	}
		
	
	MapModel *temp2;
	//Calculate Model Distances from initial position
	temp2=MapModelHead;
	while(temp2!=NULL)
	{
		temp2->distToPlayer=CalcDistanceToPoint(playerPos[0],playerPos[1],playerPos[2],temp2->pos.x,temp2->pos.y,temp2->pos.z);

		if(temp2->dynamic==true&&temp2->highlighted&&temp2->distToPlayer<= temp2->p2ActiveRadius)
		{			
			temp2->proximityTrigger=2;
				
		}
		else if(temp2->dynamic==true&&!temp2->activated&&temp2->distToPlayer<= temp2->p1ActiveRadius)
		{			
			temp2->proximityTrigger=1;
				
		}
		else
		{
			temp2->proximityTrigger=false;
		}
		temp2=temp2->next;
	}




	return 0;
}
int Map::SetDirectionInfo(struct SPlane* inp)
{
	if( inp== NULL)
		return -1;
	int i=0;
	inp->direction=NONE;
	if(inp->type != 0)
	{	
		inp->direction=NONE; 
		return 1;
	}
	CalcCoefs(inp);
	if(inp->A && !inp->C)
	{
		inp->direction=X_WISE;
		return 1;
	}
	if(!inp->A && inp->C)
	{
		inp->direction=Z_WISE;
		return 1;
	}
	if(inp->A && inp->C)
	{
		inp->direction=XZ_WISE;
		return 1;
	}
	return 0;
}

int  Map::CalcCoefs(struct SPlane *inp)
{
	if(inp == NULL) return 0;
	//first calculate two vectors
	struct SVertex v1,v2;

	v1.x = inp->vertex[1].x - inp->vertex[0].x;
	v1.y = inp->vertex[1].y - inp->vertex[0].y;
	v1.z = inp->vertex[1].z - inp->vertex[0].z;

	v2.x = inp->vertex[2].x - inp->vertex[0].x;
	v2.y = inp->vertex[2].y - inp->vertex[0].y;
	v2.z = inp->vertex[2].z - inp->vertex[0].z;

	//calculate A,B,C by cross product

	inp->A = v1.y * v2.z - v2.y * v1.z;
	inp->B = v2.x * v1.z - v1.x * v2.z;
	inp->C = v1.x * v2.y - v1.y * v2.x;

    //calculate D by substituting a point in the equation

	inp->D = -1 * (inp->A * inp->vertex[0].x + inp->B * inp->vertex[0].y + inp->C * inp->vertex[0].z );

	//test the calculations..

	//if(inp->D != -1 * (inp->A * inp->vertex[1].x + inp->B * inp->vertex[1].y + inp->C * inp->vertex[1].z ))
	//{
	//	MessageBox(0,"Wrong plane coef calculations","Test",MB_OKCANCEL);
	//}

	//calculate the necessary coef sqrt(A^2+B^2+C^2)

	inp->coef = sqrt(inp->A*inp->A + inp->B*inp->B + inp->C*inp->C);

	//calculate zmin, zmax, xmin, xmax

	inp->zmin = inp->vertex[0].z;
	inp->zmax = inp->zmin;

	inp->xmin = inp->vertex[0].x;
	inp->xmax = inp->xmin;

	for(int i=1;i<4;i++)
	{
		if(inp->vertex[i].x < inp->xmin)
		{
			inp->xmin = inp->vertex[i].x;
		}
		else if(inp->vertex[i].x > inp->xmax)
		{
			inp->xmax = inp->vertex[i].x;
		}
		if(inp->vertex[i].z < inp->zmin)
		{
			inp->zmin = inp->vertex[i].z;
		}
		else if(inp->vertex[i].z > inp->zmax)
		{
			inp->zmax = inp->vertex[i].z;
		}
	}
	inp->xmin -= 0.5;
	inp->xmax += 0.5;
	inp->zmin -= 0.5;
	inp->zmax += 0.5;

	//calculate the wall slope in xz plane...
	inp->m =  (inp->vertex[0].z - inp->vertex[2].z) / (inp->vertex[0].x - inp->vertex[2].x);

	return 1;
}

int Map::CalcDistance(struct SPlane *inp, float x,float y,float z,float *outDistance)
{
	if(outDistance==NULL) return -1;
	if(inp==NULL) return -1;
	
	//calculate the distance (A*x + B*y + C*z + D) / sqrt(A^2 + B^2 + C^2)
	*outDistance = inp->A*x +  inp->B*y + inp->C*z + inp->D;
	if(inp->coef==0)
		*outDistance=0;
	else
		*outDistance /= inp->coef;
	//check if the points' projection is on the wall
	float prX,prY,prZ; //prY is the same as the original point
	float k = *outDistance / inp->coef;
	*outDistance=abs(*outDistance);
	
	prX = x - inp->A*k;
	prY = y - inp->B*k;
	prZ = z - inp->C*k;
	
	
	if( prX >= inp->xmin && prX <= inp->xmax)
	{
		if(prZ>= inp->zmin && prZ <= inp->zmax)
		{
			return 1;
		}
	}
	//*outDistance=9999999;
	return 0;
}
bool Map::IsGettingCloser(float distNew, float distOriginal)
{
	//check if they have the same sign...
	if(distNew * distOriginal < 0)
		return false;
	//check if distance is getting smaller..
	//if(distNew - distOriginal < SAFEGUARD1)
	if(fabs(distNew) < fabs(distOriginal))
//	if( fabs(distNew - distOriginal) <SAFEGUARD1)
		return true;
	return false;
}

//calculate distance from current position to a point
float Map::CalcDistanceToPoint(float fromx, float fromz, float tox, float toz)
{
	return (abs(sqrt( pow(tox-fromx,2) + pow(toz-fromz,2))));
}
float Map::CalcDistanceToPoint(float fromx, float fromy, float fromz, float tox, float toy, float toz)
{
	return (abs(sqrt( pow(tox-fromx,2)+pow(toy-fromy,2) + pow(toz-fromz,2))));
}


void Map::WaitForReady()
{
	GLint ready;
	do {
		/* do useful work here, if any */
		glGetQueryObjectivARB(queries[validQuerie], GL_QUERY_RESULT_AVAILABLE_ARB, &ready);
	} while (!ready);
}

extern volatile int curMazePoints;
extern AudioDictionary curAudioDict; //defined in main.cpp
extern int Width;
extern int Height;
extern int xLoc;
extern int yLoc;

MapModel* getDynamicObjectByIndex(int index)
{
	MapModel *temp;
	temp = (MapModel*)malloc(sizeof(MapModel));
	temp = MapModelHead;

	while (temp != NULL&&temp->dID != index)
	{
		temp = temp->next;
	}

	return temp;
}


void ActiveRegion::Activate()
{
	if (!activated) //prevent activation of static or already activated objects
	{
		activated = true;
		Highlight(false);	//unhighlight

		curAudioDict.Play(triggerAudioID, triggerAudioLoop);  //play assigned audio

		EventLog(3,212, index, label); //log action (COM as well)


		if (moveToPos > 0)
		{

			StartPosition * sPos = getStartPosByIndex(moveToPos);
			if (sPos != NULL)
			{
				objCamera.mPos.x = sPos->x;
				objCamera.mPos.y = sPos->y;
				objCamera.mPos.z = sPos->z;
				objCamera.Position_Camera(objCamera.mPos.x, objCamera.mPos.y, objCamera.mPos.z, 0, 0, 0, 0, 1, 0);
				objCamera.InitialRotate(sPos->angle*PI / 180, xLoc + Width / 2, yLoc + Height / 2, sPos->vertAngle);
				AlignCamera();
			}
		}

		if (activatedObjectID > 0)
		{
			MapModel * temp = getDynamicObjectByIndex(activatedObjectID);
			if (temp)
				temp->Activate();
 		}

		if (strlen(messageText))
		{
			glPushMatrix();
			glLoadIdentity();
			GUIMessageBox(messageText, 0, 5);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
		}

		curMazePoints += pointsGranted;

	
		triggerAction = 0;
	}

}

void ActiveRegion::Highlight(bool highlight)
{
	if ((highlight != highlighted || activated)) //to prevent double logging/actions
	{
		highlighted = highlight;
		if (highlight) //highlight
		{
			highlightTime = GetQPC(); //get-time...
			EventLog(3, 211, index, label);
			curAudioDict.Play(highlightAudioID, highlightAudioLoop);
		}
		else if (!activated) //unhighlight
		{
			if (this->highlightAudioBehavior == 0) // stop audio immediately
				curAudioDict.Stop(highlightAudioID);
			else if (highlightAudioBehavior == 1) //pause audio
				curAudioDict.Pause(highlightAudioID, true);
			else if (highlightAudioBehavior == 2) // let it finish
			{

			}
			else
				curAudioDict.Stop(highlightAudioID);
			EventLog(3, 210, index, label);
		}
		else //if activated 
		{
			curAudioDict.Stop(highlightAudioID);
			highlighted = false;
		}
	}
}

void ActiveRegion::CalcDistance(float x, float y, float z)
{
	float x1 = x - (xmax + xmin)/2.0f;
	float y1 = y - (offset);
	float z1 = z - (zmax + zmin)/2.0f;
	x1 = x1*x1;
	y1 = y1*y1;
	z1 = z1*z1;
	distToPlayer = sqrt(x1 + y1 + z1);
}



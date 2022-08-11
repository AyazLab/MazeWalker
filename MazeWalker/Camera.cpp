
#include <stdio.h>
#include "main.h"
#include "Log.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

extern FILE *plog;
extern DWORD startTime;
extern DWORD mazeStart;
extern volatile int curMazePoints;

extern int Width, Height;
extern bool bTopDown;
extern bool bTopDownMouseAngleMode;
extern int iTopDownOrientation;
extern bool b34View;

#define MOUSE_FRAMES 5
POINT PrevMousePos[MOUSE_FRAMES];
DWORD prevTickCount;

#define _DEAD_ZONE_	1

CCamera::CCamera()
{
	j=Joystick();
}

void CCamera::Initialize()
{
	
	//ZeroMemory((LPVOID)joyInfoEx, sizeof(joyInfoEx));
	//x_offset=0;
	//y_offset=0;
	cameraSpeedMove = cameraSpeedMoveDefault; //movement speed
	//cameraSpeedView = 0.75f; //rotate speed
	cameraSpeedView = cameraSpeedViewDefault; //movement speed

	Camera3D=false;
}

void CCamera::Position_Camera(float pos_x,  float pos_y,  float pos_z,
							  float view_x, float view_y, float view_z,
							  float up_x,   float up_y,   float up_z)
{
	mPos	= tVector3(pos_x,  pos_y,  pos_z ); // set position
	mView	= tVector3(pos_x,  pos_y,  pos_z ) + tVector3(0,  pos_y, 10); 
	mUp		= tVector3(up_x,   up_y,   up_z  ); // set the up vector	
}

extern float fallSpeed;
extern float jumpSpeed;
extern float specialVar;

int CCamera::Move_Camera(float speed)
{
	tVector3 vVector = mView - mPos;	// Get the view vector
	tVector3 initPos = mPos;
	int changes=0;
	int ret=0;
	fallSpeed = 0;


	speed = speed / 10.0f; //adjusted for camera motion

	if(Camera3D)//3D test can you fly?
		changes=CheckCollision(&(mPos.x), vVector.x * speed,&(mView.x),&(mPos.z), vVector.z * speed,&(mView.z),&mPos.y,vVector.y *speed,&mView.y);
	else
		changes=CheckCollision(&(mPos.x), vVector.x * speed,&(mView.x),&(mPos.z), vVector.z * speed,&(mView.z),&mPos.y,0,&mView.y);

	if(changes>0)
		ret+=2;
	if(initPos.y==mPos.y)
		ret+=1;

	return ret;

}

extern btDynamicsWorld* m_dynamicsWorld;
extern btPairCachingGhostObject* ghostObject;

struct btResult : public btCollisionWorld::ContactResultCallback
{
	bool btResult::collided;
	int count;
	virtual	btScalar	addSingleResult(btManifoldPoint& cp,	const btCollisionObjectWrapper* colObj0Wrap,int partId0,int index0,const btCollisionObjectWrapper* colObj1Wrap,int partId1,int index1)
	{
		if(collided=false)
			count=0;
		collided=true;
		
		count=count+1;

		return 0;
	}
};


int CCamera::Move_Test(float distance,float gravDistance)
{
	tVector3 vVector = mView - mPos;	// Get the view vector
	tVector3 initPos = mPos;
	int ret=0;
	btResult t;
	t.collided=false;
	t.count=0;
	//changes=CheckCollision(&(mPos.x), vVector.x * speed,&(mView.x),&(mPos.z), vVector.z * speed,&(mView.z),&mPos.y,0,&mView.y);
	
	btTransform initTransform;
	initTransform=ghostObject->getWorldTransform();
	btTransform testTransform;
	testTransform.setIdentity();
	testTransform.setOrigin (btVector3(mPos.x+vVector.x * distance,mPos.y+gravDistance+0.2,mPos.z+vVector.z * distance));
	ghostObject->setWorldTransform(testTransform);

	m_dynamicsWorld->performDiscreteCollisionDetection();
	m_dynamicsWorld->contactTest(ghostObject,t);
	ret=t.count;
	ghostObject->setWorldTransform(initTransform);

	if(ret>0)
		return ret;

	


	return ret;

}

void CCamera::SetICD(float ICD)
{
	float eyeAngle = 7;
	Rotate_View(90.0f*PI/180.0f);
	Move_Camera(ICD);
	Rotate_View(-90.0f * PI / 180.0f);
	if (ICD > 0)
	{
		Rotate_View(eyeAngle * PI / 180.0f);
	}
	else if (ICD < 0)
	{
		Rotate_View(-eyeAngle * PI / 180.0f);
	}
}

void CCamera::SetView(float yaw, float pitch, float roll)
{

	tVector3 vVector = mView - mPos;	// Get the view vector
	tVector3 temp;

	vVector.normalize();

	tVector3 lookPlane;
	double curAngle;


	//YAW
	
	temp.z = (float)(sin(-yaw)*vVector.x + cos(-yaw)*vVector.z);
	temp.x = (float)(cos(-yaw)*vVector.x - sin(-yaw)*vVector.z);
	temp.y = vVector.y;
	vVector = temp;

	mView = mPos + vVector;

	
	//PITCH (overrides Current Pitch)
	
	lookPlane = vVector;
	lookPlane.y = 0;
	lookPlane.normalize();

	curAngle = atan2(lookPlane.x, lookPlane.z);

	CCamera::Rotate_View(curAngle);
	vVector = mView - mPos;

	if (pitch < -PI / 2)
		pitch = -PI / 2;
	if (pitch>PI / 2)
		pitch = PI / 2;

	temp.y = (float)(cos(pitch)*vVector.y + sin(pitch)*vVector.z);
	temp.z = (float)(-sin(pitch)*vVector.y + cos(pitch)*vVector.z);
	temp.x = vVector.x;
	vVector = temp;
	
	vVector.normalize();
	mView = mPos + vVector;



	CCamera::Rotate_View(-curAngle);
	return;
	vVector = mView - mPos;
	float curViewAngle = atan2(vVector.y, vVector.z);

	

	LookUpDown(-curViewAngle);
	Rotate_View(curAngle);
	vVector = mView - mPos;

	
	//Roll 
	temp.z = mUp.z;
	temp.y = (float)(cos(roll)*mUp.y - sin(roll)*mUp.x);
	temp.x = (float)(sin(roll)*mUp.y + cos(roll)*mUp.x);
	mUp = temp;

	mUp.normalize();


	//Rotate_View(-curAngle);
	//LookUpDown(curViewAngle);
}


tVector3 CCamera::Rotate_View(float angle)
{
	tVector3 vVector = mView - mPos;	// Get the view vector
	tVector3 Init_vVector = vVector;	// Get the view vector
	vVector.normalize();


	tVector3 temp;
	temp.z = (float)(sin(angle)*vVector.x + cos(angle)*vVector.z);
	temp.x = (float)(cos(angle)*vVector.x - sin(angle)*vVector.z);
	temp.y = vVector.y;
	vVector = temp;

	mView = mPos + vVector;

	return Init_vVector;
	
}

void CCamera::Set_vVector(tVector3 vVector)
{
	mView = mPos + vVector;

	
}

void CCamera::LookUpDown(float angle)
{
	tVector3 vVector = mView - mPos;	// Get the view vector

	vVector.normalize();
	tVector3 lookPlane=vVector;
	lookPlane.y=0;
	lookPlane.normalize();
	float curAngle=atan2(lookPlane.x,lookPlane.z);
	
	CCamera::Rotate_View(curAngle);
	curAngle=atan2(lookPlane.x,lookPlane.z);
	vVector = mView - mPos;
	double curViewAngle=atan2(vVector.y,vVector.z);
	if(curViewAngle+angle>=PI/2)
		angle=0;
	else if(curViewAngle+angle<=-PI/2)
		angle=0;

	tVector3 temp;
	temp.y = (float)(cos(angle)*vVector.y +sin(angle)*vVector.z);
	temp.z = (float)(-sin(angle)*vVector.y +cos(angle)*vVector.z);
	temp.x = vVector.x;
	vVector = temp;
	vVector.normalize();

	mView = mPos + vVector;

	CCamera::Rotate_View(-curAngle);

}

void CCamera::Rotate_View(float angle,bool rotateSpeed)
{
	//if(rotateSpeed)
		//angle*=cameraSpeedMultiply;
	tVector3 vVector = mView - mPos;	// Get the view vector
	vVector.normalize();

	mView.x = (float)(mPos.x + cos(angle)*vVector.x - sin(angle)*vVector.z);
	mView.z = (float)(mPos.z + sin(angle)*vVector.x + cos(angle)*vVector.z);
}

void CCamera::InitialRotate(float angle,int mid_x_pos, int mid_y_pos,float vertAngle)
{
	// PI/4 correction is due to initial view vector setting (10,0,10)

	// - is due to flipping z axis
	mid_x= mid_x_pos;
	mid_y= mid_y_pos;
	//SetCursorPos(mid_x, mid_y);
	Rotate_View( -angle-PI/2.0F); //Rotate to initial view
	mView.y=mPos.y; //Set initial view level with position

	LookUpDown(vertAngle);

	PrevMousePos[0].x=0;
	ResetMouseBuffer();
	
}


void CCamera::UpdateMidCursorPos(int mid_x_pos, int mid_y_pos)
{
	mid_x= mid_x_pos;
	mid_y= mid_y_pos;
	ResetMouseBuffer();
}

extern bool bMouseInput;

void CCamera::ResetMouseBuffer()
{
	POINT mousePos= POINT();	
	if (bMouseInput)
		SetCursorPos(mid_x, mid_y);
	GetCursorPos(&mousePos);	// Get the 2D mouse cursor (x,y) position	

	for (int i=0;i<(MOUSE_FRAMES);i++)
	{
		PrevMousePos[i].x=0;
		PrevMousePos[i].y=0;
	}
}

float CCamera::GetCurAngle()
{
	tVector3 vVector = mView - mPos;
	double curViewAngle = atan2(vVector.x, vVector.z);
	return curViewAngle;
}

int CCamera::Mouse_Move()
{
	POINT mousePos;	
	
	float angle_y  = 0.0f;				
	float angle_z  = 0.0f;	

	
	if (bTopDown&&bTopDownMouseAngleMode)
	{
		GetCursorPos(&mousePos);	// Get the 2D mouse cursor (x,y) position	
		float bufx = 0;
		float bufy = 0;
		bufx = mousePos.x - mid_x;
		bufy = mousePos.y - mid_y;

		tVector3 vVector = mView - mPos;
		double curViewAngle = atan2(vVector.x, vVector.z);

		
		Rotate_View(curViewAngle);
		curViewAngle = atan2(bufx,bufy);
		
		if (iTopDownOrientation == 2) //South
			curViewAngle -= PI;
		else if (iTopDownOrientation == 3) //East
			curViewAngle -= PI / 2;
		else if (iTopDownOrientation == 4) //West
			curViewAngle += PI / 2;
		Rotate_View(-curViewAngle);
		return 1;
	}

					
	//if (PrevMousePos[0].x==0)   // initializes Previous Mouse Position
	//{
		/*
		SetCursorPos(mid_x, mid_y);
		GetCursorPos(&mousePos);	// Get the 2D mouse cursor (x,y) position	

		for (int i=0;i<(MOUSE_FRAMES);i++)
		{
			PrevMousePos[i].x=mousePos.x;
			PrevMousePos[i].y=mousePos.y;
		}
		*/
	//	ResetMouseBuffer();
	//}
	//else

		GetCursorPos(&mousePos);	// Get the 2D mouse cursor (x,y) position	
	


	SetCursorPos(mid_x, mid_y);	// Set the mouse cursor in the center of the window						

	// Get the direction from the mouse cursor, set a reasonable maneuvering speed

	float bufx=0;
	float bufy=0;

	

	for (int i=MOUSE_FRAMES-1;i>=0;i--)
	{
		if(i!=0)
		{
			PrevMousePos[i].x=PrevMousePos[i-1].x; //shift back positions
			PrevMousePos[i].y=PrevMousePos[i-1].y;

		}
		else
		{
			if (abs(mousePos.x - mid_x) < _DEAD_ZONE_ && abs(mousePos.y - mid_y) < _DEAD_ZONE_ && abs(PrevMousePos[0].x) < _DEAD_ZONE_ && abs(PrevMousePos[0].y) < _DEAD_ZONE_)
			{
				PrevMousePos[0].x = 0;   //Stores Current mouse position
				PrevMousePos[0].y = 0;
			}
			else
			{
				PrevMousePos[0].x = mousePos.x - mid_x;   //Stores Current mouse position
				PrevMousePos[0].y = mousePos.y - mid_y;
			}

			
		}

		bufx+=PrevMousePos[i].x;
		bufy+=PrevMousePos[i].y;
	}

	bufx = bufx / ((MOUSE_FRAMES)*1.0f);
	bufy = bufy / (1.0f*(MOUSE_FRAMES));
	//if( (mousePos.x == mid_x) && (mousePos.y == mid_y) && (PrevMousePos[0].x == mid_x) && (PrevMousePos[0].y == mid_y) ) return 0;

	


	angle_y = -(bufx)*4.0f/(float)Height;		 //speed of up down is faster
	angle_z = -(bufy) *3.0f/(float)Height;

	

	// The higher the value is the faster the camera looks around.

	if(!bTopDown&&(!b34View))// no need to move z if topdown
		LookUpDown(angle_z);
	
	Rotate_View(-angle_y); // Rotate





	return 1;
}

float* CCamera::JoyStick_Move()
{
	
	/* joystick handling is done in main.cpp */

	return j.JoyStickMove();
}

void CCamera::DoLog()
{
	if(plog)
	{
		DWORD tickCount=GetQPC()-startTime;
		if(prevTickCount!=tickCount)
		{
			LogPosition(mPos.x, mPos.y, mPos.z, mView.x, mView.y, mView.z, curMazePoints);
			prevTickCount=tickCount;
		}
		
	}
}

void CCamera::MoveSpeedIncrement()
{
	cameraSpeedMove += 0.001f;
}

void CCamera::MoveSpeedDecrement()
{
	if(cameraSpeedMove > 0.005f)
		cameraSpeedMove -= 0.001f;	
}

void CCamera::MoveSpeedDefault()
{
	cameraSpeedMove = cameraSpeedMoveDefault;
	cameraSpeedView = cameraSpeedViewDefault;
}

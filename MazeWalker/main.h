
#ifndef _MAIN_H

#define _MAIN_H

//NEW//////////////////NEW//////////////////NEW//////////////////NEW////////////////
//#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "Map.h"
#include <math.h>
#include "glmimg.h"
#include "Mmsystem.h"
#include "QPC.h"
#include "Joystick.h"
#include "freetype.h"
#include "mazX.h"
#include <filesystem>


#define PI 3.14159265
#define PIOVER4 0.78539816

#define MAX_TEXTURES 1000


#ifdef _WINDOWS 
#define strcasecmp stricmp 
#endif

//#define CAMERASPEED	0.03f				// The view Speed
//#define CAMERASPEED2	0.005f				// The move Speed
//NEW//////////////////NEW//////////////////NEW//////////////////NEW////////////////


//////////////////////////////////////
//The tVector3 Struct
//////////////////////////////////////
typedef struct tVector3					// expanded 3D vector struct
{			
	tVector3() {}	// constructor
	tVector3 (float new_x, float new_y, float new_z) // initialize constructor	 
	{x = new_x; y = new_y; z = new_z;}
	// overload + operator so that we easier can add vectors
	tVector3 operator+(tVector3 vVector) {return tVector3(vVector.x+x, vVector.y+y, vVector.z+z);}
	// overload - operator that we easier can subtract vectors
	tVector3 operator-(tVector3 vVector) {return tVector3(x-vVector.x, y-vVector.y, z-vVector.z);}
	// overload * operator that we easier can multiply by scalars
	tVector3 operator*(float number)	 {return tVector3(x*number, y*number, z*number);}
	// overload * operator that we easier can multiply by scalars
	float operator*(tVector3 vVector)	 {return (x*vVector.x+ y*vVector.y+ z*vVector.z);}
	// overload * operator so that we can perform dot product
	tVector3 operator/(float number)	 {return tVector3(x/number, y/number, z/number);}
	
	bool operator==(tVector3 vVector)	 {return (x==vVector.x&&y==vVector.y&&z==vVector.z);}

	float x, y, z;						// 3D vector coordinates
	void normalize()
	{
		float magnitude=sqrt(x*x+y*y+z*z);
		x=x/magnitude;
		y=y/magnitude;
		z=z/magnitude;
	}
}tVector3;


//////////////////////////////////////
//The CCamera Class
//////////////////////////////////////
class CCamera 
{

	public:
		Joystick j;
		int x_offset,y_offset;
		tVector3 mPos;	
		tVector3 mView;		
		tVector3 mUp;	
		tVector3 startPos;
		tVector3 startView;

		BOOL Camera3D;
		int mid_x;
		int mid_y;

		float cameraSpeedView;
		//float cameraSpeedMultiply;
		float cameraSpeedViewDefault;
		float cameraSpeedMove;
		float cameraSpeedMoveDefault;
		
		float CCamera::GetCurAngle();//accessible angle 


		CCamera();

		void CCamera::SetICD(float ICD);
		int Mouse_Move();
		void CCamera::ResetMouseBuffer();
		void CCamera::SetView(float yaw, float pitch, float roll);
		float* JoyStick_Move();
		void CCamera::InitialRotate(float angle,int mid_x_pos, int mid_y_pos,float vertAngle);
		void CCamera::UpdateMidCursorPos(int mid_x_pos, int mid_y_pos);
		
		void Initialize();
		int Move_Camera(float speed);
		int CCamera::Move_Test(float distance, float gravdistance);
		tVector3 Rotate_View(float angle);
		void CCamera::LookUpDown(float angle);
		void CCamera::Rotate_View(float angle, bool rotateSpeed);
		void Position_Camera(float pos_x, float pos_y,float pos_z,
			 				 float view_x, float view_y, float view_z,
							 float up_x,   float up_y,   float up_z);
		void DoLog();
		void MoveSpeedIncrement();
		void MoveSpeedDecrement();
		void MoveSpeedDefault();
		void CCamera::Set_vVector(tVector3 vVector);
};



//////////////////////////////////////
//	Global Variables
//////////////////////////////////////
extern	HDC			hDC;				// Device context
extern	HGLRC		hRC;				// Permanent rendering context
extern	HWND		hWnd;				// Holds our window handle
extern	HINSTANCE	hInstance;			// Holds the instance of the application

#include <vector>

int CheckCollision(float* x,float dx,float *vx, float* z,float dz,float *vz,float* y,float dy,float *vy);
void GameUpdate();
int DrawHUD();
void BuildShaders();
 DWORD WINAPI comListenThread( LPVOID lpParam);

 void sendToAll(int code,int val);
 void sendToTCP(int code, int val);

 void GUIMessageBox (char* displayText,int showTime,int showStyle);
 void GUIMessageBox (char* displayText,int showTime, int showStyle, GLuint texID);
 void DrawLoadingScreen(int width, int height);
 void loadIniSettings();
 void clearModels();
  POINT GetOGLPos();
  void DrawSkyBox();

  BOOL CreateWnd(char* title, int width, int height, int bits, int monitor);
void EventLog(int type,int instance, int state);
void EventLog(int type,int instance, int state,char* message);
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK AboutProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SensorProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ShaderProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ViewProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ControlProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK JoystickProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SyncProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK OtherProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OculusProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CommProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI listenThread( LPVOID lpParam);
int Draw(GLvoid);
int DrawBuffer(GLuint framebuffer);
//int DrawOculus();

void Tcp_Instant();
int DrawTopDown();
int LoadTexture(char* fname,int index);
void SetLights();
void DrawMouse();
void DrawCrosshair();

char* ftoLower(const char* in);
int BuildWhite();
void MazeInit();
void AlignCamera();
int ReadMap(char* theFile);
StartPosition* getStartPosByIndex(int index); //negative values return head, 0 returns random ID, positive IDs will return indexed values
//GLMmodelNode* getModelByIndex(int index);



#endif

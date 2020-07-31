
#include <math.h>           // Header File For Mathematical Functions
#include <shlobj.h>

#include "process.h"

#include "string.h"
#include <olectl.h>	

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <crtdbg.h>

#include "main.h"
#include "resource.h"
#include "time.h"
#include "MyList.h"
#include "Tools.h"
#include "LightItem.h"
#include "MyLightList.h"
#include "AudioDictionary.h"
#include "TextureDictionary.h"
#include "particle.h"
#include "ServerSocket.h"
#include <btBulletDynamicsCommon.h>
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

//#include "Oculus.h"
bool oculusEnabled = false;

#include "com.h"
#include "Log.h"
#include "GL/glew.h"
#include "GL/wglew.h"
#include "Libs/rapidxml/rapidxml.hpp"
#include <fstream>
#include <sstream>



#define BUF_SIZE 10000	

#define BAR_THRESHOLD	85

//Display List names
 
#define ROOM        16
#define COR         17
#define ROOM2       18
#define COR2        19

#define LINEBUFFERLIMIT	600
#define TXTBUFFERLIMIT	800

#define MC_CHOICES 10 // max number of dialog multiple choice boxes

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance=NULL;		// Holds The Instance Of The Application

int updatesSec=0;
double frameRate=60;
double smoothFrameRate = 0;
int timeSince3=0;

bool bTopDown=false;
bool bTopDownMouseAngleMode = true;
bool bTopDownSimpleRender = false;
int iTopDownOrientation = 0;
float iTopDownHeight = 15;
float firstPersonHeight = 1;
bool bFixedCameraLocationX = false;
bool bFixedCameraLocationZ = false;
float fixedCameraLocationX = 20;
float fixedCameraLocationZ = 20;

bool b34View = false;
int model34_ID = -1;
float model34_scale = 1;
tVector3 model34_rot = tVector3(0, 0, 0);
float f34Angle = 45;
int devViewMode = 0;

bool firstLog=false;
float fallSpeed=-0.01f;
float jumpSpeed=0;
bool falling=true;
int skyboxID=900;
float specialVar = 0;
float specialVar2 = 0;
float fieldOfView = 45; //Field of view for MazeWalker

float OculusOffset = 0.0f;

RECT desktopRC;

char cmdOptions[800]="";

bool	developerKeys=false;
bool	devFPS=false;
int		hideRenderedMaze = false; // Hide rendered maze during dialogs and other on screen messages
int		devMsgStatus=0;
char	devMsg[200]="";
int		devMsgTime;
bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=false;	// Full screen Flag Set To Full screen Mode
bool	borderlessWindowed = false; //Flag for borderless windowed mode
bool	lights=true;
bool	shader=true;
int shaderOptionCode;
bool	shadersCompiled=false;
bool autoLog=false;
GLuint	shaderProg=0,shaderProgNoLights=0,shaderProgBasic=0;
GLuint screenCapID;

bool Camera3D=false;
bool bDisableGravity = false; //2D motion, but collisions
int iScoreBarMode = 1; //1 = Maze Points 0 = API
GLuint gQuery1    = -1; //Room list
GLuint gQuery2   = -1; //dynamic model list
GLuint *whiteID=NULL;

bool usingMazX = false;
MazeListItem *curMazeListItem = NULL;


DWORD updateTime,updateTimeKeys,updateTime2,updateTime3,toggleKeyTime;
#define UpdateTimeBufferCount 10
DWORD updateTimeFrame[UpdateTimeBufferCount];
char mwalkerIniPath[MAX_PATH],userLibraryDir[MAX_PATH],libraryDir[MAX_PATH],exePath[MAX_PATH];

BOOL JoyPresent=FALSE;
bool joyUp=false,joyLeft=false,joyRight=false,joyDown=false;
bool specularShader=false,texturesShader=true,diffuseShader=false,objectsShader=true;
bool useMSAA=true, useAniso=true, useVSYNC=false;

int ambientShader=0;
int maxlights=4;
int numHighlighted=0;
HANDLE deviceThread[3];
HANDLE comThread;

int devicePort[3];
bool deviceLocal[3];
bool bWaitForAPIdevices=true;
bool deviceEnabled[3];
DWORD deviceIP[3];
int mazeInputValue=-1;
int mazeReturnValue=-1;

bool videoPlayBack = false;
bool pausePlayback = false;
bool askForResponse =false;
bool interactivePlayback =false;
DWORD pauseTime = 0;

int Width=800,Height=600;            // Screen Dimensions
int xLoc=0,yLoc=0;
int bitz=32;
float x,y,z;

ServerSocket device[3];

typedef bool (APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	



particleArray *partArrayHead=NULL;
bool bMazeLoop=true;
bool bMazeEndReached=false;
bool bRestartMaze = false;

bool bStatusWaitingForAPI = false;
bool bStatusWaitingForCue = false;
bool bStatusMazeLoading = false;
bool bStatusMazeRunning = false;
bool bStatusMazeMessage = false;

bool bMouseInput=true;		//Get mouse input
bool bJoyStickInput=false;  //Get joystick input
int joystickNum=JOYSTICKID1;
int joyCueButton=2;
bool bShowCrosshair=true;      //Show Cursor
bool bShowTimer=true;		//Timer
bool bShowBar=true;			//bar
bool bSkipWarnings=false;
bool showDebugMessage=false;
DWORD debugMessageTime=0;
char* debugMessage="";

float barScore=0;

volatile int curMazePoints = 0;
volatile float targetBarScore=0;
float userResponse=0;


MyList curList;
MyLightList curLightList;
bool firstLightRun=true;
AudioDictionary curAudioDict;
TextureDictionary curTextureDict;

long lifeTime=0;

char filename[TXTBUFFERLIMIT+1];
char logfile[TXTBUFFERLIMIT];
char walker[100]="";

char curMazeFilename[TXTBUFFERLIMIT];

//char mazeWorkingDir[500] = "";
//extern char melWorkingDir[500] = "";

int linenumber; //for debugging

FILE *clog;
bool newclogType=true;
DWORD clogItemTime;
DWORD actualTime;
DWORD clogStartTime;
DWORD startTime;
DWORD mazeStart;
int mazeStartedAndRunning=0;  //0-not start, 1-running, 2-ended

GLuint	base;				// Base Display List For The Font Set
//GLuint	MAX_TEXTURES];			// Storage Textures
GLMmodelNode *ModelDictHead = NULL;
MapModel *MapModelHead =nullptr;

freetype::font_data fontUI;
int fontsize = 20;
int fontheight = 10;
char* fontname = new char[256];

/////////////////////////////////////////////////////////////
#define USER_STATUS_NONE	0
#define USER_STATUS_HIGHLIGHT	1
#define USER_STATUS_TRIGGER	2
#define USER_STATUS_REPORT 3
int iUserStatus= USER_STATUS_NONE;  //For fnir-bci study
////////////////////////////////////////////////////////////



extern volatile int bufferVar; //defined in com.cpp

CCamera objCamera; 
Map *objMap=NULL;

bool bSendSync=false;
bool bListen=false;
int cueType=0; //1: serial 2:TCP 3:Keyboard 4:Joystick
bool cueSkipButton=true;
bool cue=false;

int returned=0;
int comPort=1;
int baudRate=4;
bool parity=false;
int stopBits=0;

int triggerChar= 116;



bool bUseLPT = false;
int iLptPortNum = 0;
int iLptDelay = 1; //Time to wait before sending 0 after maze


//#define _MAZE_PROFILER


#ifdef _MAZE_PROFILER

FILE *debugFile;

#endif


//variables for to bullet physics API
btAlignedObjectArray<btCollisionShape*>	m_collisionShapes; //keep the collision shapes, for deletion/cleanup
btBroadphaseInterface*					m_broadphase;
btCollisionDispatcher*					m_dispatcher;
btConstraintSolver*						m_solver;
btDefaultCollisionConfiguration*		m_collisionConfiguration;
btDynamicsWorld*						m_dynamicsWorld; //this is the most important class
 btKinematicCharacterController* controller;
 btPairCachingGhostObject* ghostObject;
 


void ExitBulletPhysics();
void InitBulletPhysics();
void StepBulletPhysics(double stepSize);
BOOL CALLBACK MonitorEnumProc(
  _In_  HMONITOR hMonitor,
  _In_  HDC hdcMonitor,
  _In_  LPRECT lprcMonitor,
  _In_  LPARAM dwData
);

enum controls{
	Forward = 0,
	Backward = 1,
	StrafeLeft = 2,
	StrafeRight = 3,
	LookLeft = 4,
	LookRight = 5,
	Run = 6,
	LightToggle = 7,
	SpeedUp = 8,
	SpeedDown = 9,
	Jump = 10,
	Crouch = 11,
	CrosshairToggle = 12,
	TimerToggle = 13,
	BarToggle = 14,
	Double = 15,
	LookUp = 30,
	LookDown = 31,
	RestartMaze = 32,
	NextMaze = 33,
	Interact = 25
};

enum VK{
	NUMPAD7 = 0x67,
	NUMPAD8 = 0x68,
	NUMPAD9 = 0x69,
	MULTIPLY = 0x6A,
	ADD = 0x6B,
	SEPARATOR = 0x6C,
	SUBTRACT = 0x6D,
	DECIMALDOT = 0x6E,
	DIVIDE = 0x6F,
	F1 = 0x70,
	F2 = 0x71,
	F3 = 0x72,
	F4 = 0x73,
	F5 = 0x74,
	F6 = 0x75,
	F7 = 0x76,
	F8 = 0x77,
	F9 = 0x78,
	F10 = 0x79,
	F11 = 0x7A,
	F12 = 0x7B,
	NUMLOCK = 0x90,
	SCROLL = 0x91,
	LSHIFT = 0xA0,
	RSHIFT = 0xA1,
	LCONTROL = 0xA2,
	RCONTROL = 0xA3,
	LMENU = 0xA4,
	RMENU = 0xA5,
	BACK = 0x08,
	TAB = 0x09,
	RETURN = 0x0D,
	SHIFT = 0x10,
	CONTROL = 0x11,
	MENU = 0x12,
	PAUSE = 0x13,
	CAPITAL = 0x14,
	ESCAPE = 0x1B,
	SPACE = 0x20,
	END = 0x23,
	HOME = 0x24,
	LEFT = 0x25,
	UP = 0x26,
	RIGHT = 0x27,
	DOWN = 0x28,
	PRINT = 0x2A,
	SNAPSHOT = 0x2C,
	INSERT = 0x2D,
	DELETEKEY = 0x2E,
	LWIN = 0x5B,
	RWIN = 0x5C,
	NUMPAD0 = 0x60,
	NUMPAD1 = 0x61,
	NUMPAD2 = 0x62,
	NUMPAD3 = 0x63,
	NUMPAD4 = 0x64,
	NUMPAD5 = 0x65,
	NUMPAD6 = 0x66,
	A = 0x41,
	B = 0x42,
	C = 0x43,
	D = 0x44,
	E = 0x45,
	F = 0x46,
	G = 0x47,
	H = 0x48,
	I = 0x49,
	J = 0x4A,
	K = 0x4B,
	L = 0x4C,
	M = 0x4D,
	N = 0x4E,
	O = 0x4F,
	P = 0x50,
	Q = 0x51,
	R = 0x52,
	S = 0x53,
	T = 0x54,
	U = 0x55,
	V = 0x56,
	W = 0x57,
	X = 0x58,
	Y = 0x59,
	Z = 0x5A
};




int ControlLibrary[40];

struct condition //player condition
{
	bool state;	//is ? crouching, movindForward etc
	int moveCount;		//how many moves
	float instantMove;

	int condition::Move() //if moveCount is enough then move use one token
	{
		if(condition::moveCount>0)
		{
			condition::state=true;
			condition::moveCount--;
		}
		return condition::state;
	};
};

class MPlayer{
public:
	MPlayer(){}
	condition running;
	condition crouching;
	condition movingForward;
	condition movingBackwards;
	bool jumping;
	condition strafingLeft;
	condition strafingRight;
	condition lookingLeft;
	condition lookingRight;
	condition lookingUp;
	condition lookingDown;
	bool testPass;
	float jumpSpeed;
	float halfSpeed;
	condition justJumped;
	bool jumpReleased;
	bool goingUp;
	float offset; //current camera offset
	float defaultCameraOffset = 0.3f;
	bool gravityEnabled;
	bool collisionEnabled;
	bool toggleDevMode;
	bool interact;

	void ClearConditions()
	{
		
		running.instantMove=0;
		running.moveCount=0;
		crouching.instantMove=0;
		crouching.moveCount=0;
		strafingLeft.instantMove=0;
		strafingLeft.moveCount=0;
		strafingRight.instantMove=0;
		strafingRight.moveCount=0;
		lookingLeft.instantMove=0;
		lookingLeft.moveCount=0;
		lookingRight.instantMove=0;
		lookingRight.moveCount=0;
		lookingUp.instantMove=0;
		lookingUp.moveCount=0;
		lookingDown.instantMove=0;
		lookingDown.moveCount=0;
		movingForward.instantMove=0;
		movingForward.moveCount=0;
		movingBackwards.instantMove=0;
		movingBackwards.moveCount=0;
	};
};



MPlayer Player;




void ReportError(LPSTR fn,int expected,int actual)
{
	char mes[200];
	sprintf(mes,"File: %s\n\nLine Number\t\t: %d\nExpected parameters\t: %d\nFound paramteres\t\t: %d",fn,linenumber,expected,actual);
	MessageBox(hWnd,mes,"Error in maze file",MB_ICONEXCLAMATION|MB_OK);
}

GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	char * pSystemRoot;
	pSystemRoot = getenv("WINDIR");

	char* fName = new char[256];//"mcyh.ttf";
	//fName 
	sprintf(fName, "%s", fontname);
	

	char* fontpath = new char[512];

	sprintf(fontpath, "%s\\Fonts\\%s", pSystemRoot, fName);
	if (CheckFileExists(fontpath))
	{
		fontUI.init(fontpath, fontsize);                                  // Build The FreeType Font
		fontheight = (int)fontUI.h;
		return;
	}

	fontpath = new char[512];
	fName = "arialbd.ttf";
	sprintf(fontpath, "%s\\Fonts\\%s", pSystemRoot, fName);

	if (CheckFileExists(fontpath))
	{
		fontUI.init(fontpath, fontsize);                                  // Build The FreeType Font
		fontheight = (int)fontUI.h;
		return;
	}

	fontpath = new char[512];
	fName = "system.ttf";
	sprintf(fontpath, "%s\\Fonts\\%s", pSystemRoot, fName);

	if (CheckFileExists(fontpath))
	{
		fontUI.init(fontpath, fontsize);                                  // Build The FreeType Font
		fontheight = (int)fontUI.h;
		return;
	}


	/*
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping
	
	base = glGenLists(96);								// Storage For 96 Characters
	font = CreateFont(	-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Arial");					// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
	SelectObject(hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
	*/
}

GLvoid KillFont(GLvoid)									// Delete The Font List
{
	fontUI.clean();
	//glDeleteLists(base, 96);							// Delete All 96 Characters
}

GLvoid glPrint(int allign,const char *fmt, ...)					// Custom GL "Print" Routine
{
	//allign 0 = left, 1 = center, 2 = right

	if(shadersCompiled)
	{
		glUseProgram(0);
	}
	/*
	char text[256];										// Holds Our String
	va_list	ap;											// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
	*/

	char text[512] = "test";										// Holds Our String


	va_list	ap;											// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	wchar_t ws[512] = L"";
	//swprintf(ws, 512, L"%hs", text); //no unicode conversion
	
	freetype::ascii2utf8(ws, text, 512);

	float rpos[4];
	glGetFloatv(GL_CURRENT_RASTER_POSITION, rpos);
	freetype::print(fontUI, rpos[0], rpos[1], allign, L"%s", ws);
}
bool resizeFLAG = false;
bool initialResizeFlag = true;

GLvoid ReSizeGL(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}
	

	if (Width == width && Height == height&&!initialResizeFlag) //should work when initialized
	{
		return;
	}
	initialResizeFlag = false;
	resizeFLAG = true; //marks to tell menus to resize as well
	Width = width;
	Height = height;
	glViewport(0,0,width,height);						// Reset The Current Viewport
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	
	// Calculate The Aspect Ratio Of The Window
	/*if (oculusEnabled)
		gluPerspective(60.0f,(GLfloat)width/(GLfloat)height,0.1f,1000.0f);
	else*/
		gluPerspective(fieldOfView, (GLfloat)width / (GLfloat)height, 0.1f, 1000.0f);
	glPopMatrix();
}

GLMmodel* LoadModel (char* file)
{
	GLMmodel* Model;

	Model = glmReadOBJ(file);

	if (Model!=NULL)
	{
		glmUnitize(Model);
		glmFacetNormals(Model);
		glmVertexNormals(Model, 90.0);
	}
	return Model;
}

void clearModels()
{
	//clear the list...
	if (ModelDictHead)
	{
		GLMmodelNode *temp2, *cur2;
		cur2 = ModelDictHead;
		GLMmodel *temp3;
		while (cur2 != NULL)
		{
			temp3 = cur2->modelData;
			delete temp3;
			temp2 = cur2;
			cur2 = cur2->next;
			delete temp2;
		}
		ModelDictHead = NULL;
	}


	MapModel *temp,*cur;
	if(MapModelHead)
	{
		cur = MapModelHead;
		while(cur!=NULL)
		{
			temp = cur;
			cur=cur->next;

			//glmDelete(temp->modelData);
			delete temp;
			//free(temp);
		}
		MapModelHead=NULL;
	}
	
	particleArray *p,*pTemp;
	if (partArrayHead)
	{
		p = partArrayHead;
		while(p != NULL)
		{
			pTemp = p;
			p = p->next;

			delete pTemp;
		}
		partArrayHead = NULL;
	}
	
	
}

int defaultStartPosID = 0;

int ReadMapXML(char* mazeXMLfile)
{
	//sprintf(mazeXMLfile, "D:\\BasicMazeFail.xml");

	firstLog = false;



//	srand(GetQPC()*time(NULL)); //seed based on time
	FILE *fp = fopen(mazeXMLfile, "r");
	if (fp == NULL)
	{
		char message[900];
		sprintf(message, "Couldn't open Maze file!\n%s", mazeXMLfile);
		if (strlen(mazeXMLfile) > 300)
			sprintf(message, "Couldn't open Maze file!\nMazeList Error");
		GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
		return 0;
	}
	else
	{
		fclose(fp);
	}

	int texture=-1, type=-1;

	int curVisibility=1;   //0-invisible, 1-visible
	//float inp1, inp2, inp3, inp4, inp5, inp6, inp7;

	int res = 0;
	int bMouseInputDisabled = bMouseInput;//disable mouse input for loading
	bMouseInput = false;
	bool defaultLights = true;

	struct SColor col;
	struct SVertex v[4];
	struct SVertex v2[4];
	LightItem* lightItem = NULL;

	SetCurrentDirectoryA(getMazeWorkingDir());

	int index = 0, counter = 0;
	UINT i;
	char fname[LINEBUFFERLIMIT]; fname[0] = 0;
	int n=0;// , elementLineCount;
	linenumber = -1;

	MazeInit();

	GLMmodelNode *temp2 = NULL;
	MapModel *temp = NULL;
	StartPosition* startPos = NULL;
	EndRegion* endRegion = NULL;
	ActiveRegion* activeRegion = NULL;
	

	try
	{

		using namespace rapidxml;

		std::ifstream file(mazeXMLfile);

		float mzFileEdition = 0;

		xml_document<> doc;
		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();
		std::string content(buffer.str());
		doc.parse<0>(&content[0]);
		xml_node<> *pRoot = doc.first_node("MazeFile");

		if (pRoot == NULL)
		{
			char txt[250];
			sprintf(txt, "Invalid Maze File");
			GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING); //return 0;
			return false;
		}

		xml_attribute<> *pAttr;
		pAttr = pRoot->first_attribute("version");
		if (pAttr)
			mzFileEdition = atof(pAttr->value());


		if (mzFileEdition > 2 || mzFileEdition == 0)
		{
			char txt[250];
			if (mzFileEdition > 2)
				sprintf(txt, "Current File Version: 2.0\nMazeFile v%.1f is unsupported in this version of MazeWalker");
			else
				sprintf(txt, "Current File Version: 2.0\nUnknown File Version Loaded");
			GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING); //return 0;
		}

		char fPath[800] = "";

		bool clearDefaultLights = false;


		xml_node<> *pGlobalNode = pRoot->first_node("Global");
		if (pGlobalNode)
		{
			for (xml_node<> *pNode = pGlobalNode->first_node(); pNode; pNode = pNode->next_sibling())
			{
				if (pNode == NULL)
					break;

				if (0 == strcmp(pNode->name(), "DefaultStartPosition"))
				{

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						defaultStartPosID = atoi(pAttr->value());
					else
						defaultStartPosID = 0;

				}

				else if (0 == strcmp(pNode->name(), "AmbientLight"))
				{
					float ar = 1, ag = 1, ab = 1, intensity = 0.1; //Ambient Light
					//bool ambientEnabled = true;

					pAttr = pNode->first_attribute("globalLightEnabled");
					if (pAttr)
						clearDefaultLights = !trueFalse(pAttr->value(), false);

					pAttr = pNode->first_attribute("r");
					if (pAttr)
						ar = atof(pAttr->value());

					pAttr = pNode->first_attribute("g");
					if (pAttr)
						ag = atof(pAttr->value());

					pAttr = pNode->first_attribute("b");
					if (pAttr)
						ab = atof(pAttr->value());

					pAttr = pNode->first_attribute("intensity");
					if (pAttr)
						intensity = atof(pAttr->value());

					float vAmbientLightBright[4] = { ar*intensity, ag*intensity, ab*intensity, 1 };
					glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vAmbientLightBright);
				}

				else if (0 == strcmp(pNode->name(), "Avatar"))
				{

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						model34_ID = atoi(pAttr->value());

					pAttr = pNode->first_attribute("scale");
					if (pAttr)
						model34_scale = atoi(pAttr->value());

					pAttr = pNode->first_attribute("rotX");
					if (pAttr)
						model34_rot.x = atof(pAttr->value());

					pAttr = pNode->first_attribute("rotY");
					if (pAttr)
						model34_rot.y = atof(pAttr->value());

					pAttr = pNode->first_attribute("rotZ");
					if (pAttr)
						model34_rot.z = atof(pAttr->value());
				}

				else if (0 == strcmp(pNode->name(), "StartMessage"))
				{

					pAttr = pNode->first_attribute("enabled");
					if (pAttr)
						objMap->startMessageEnabled = trueFalse(pAttr->value(), false);

					pAttr = pNode->first_attribute("message");
					if (pAttr)
						strcpy_s(objMap->startMessage, pAttr->value());

				}

				else if (0 == strcmp(pNode->name(), "Timeout"))
				{
					pAttr = pNode->first_attribute("timeoutValue");
					if (pAttr)
						objMap->maxMazeTime = atof(pAttr->value())*1000.0f;

					pAttr = pNode->first_attribute("enabled");
					if (pAttr)
						objMap->maxMazeTime = objMap->maxMazeTime*trueFalse(pAttr->value(), false);

					pAttr = pNode->first_attribute("message");
					if (pAttr)
						strcpy_s(objMap->timeoutMessage, pAttr->value());
				}

				else if (0 == strcmp(pNode->name(), "PointOptions"))
				{
					pAttr = pNode->first_attribute("exitThreshold");
					if (pAttr)
						objMap->pointExitThreshold = atof(pAttr->value());

					pAttr = pNode->first_attribute("messageText");
					if (pAttr)
					{
						strcpy_s(objMap->pointMessage, pAttr->value());
						if (strlen(objMap->pointMessage) > 0)
							objMap->pointMessageEnabled = true;
						else
							objMap->pointMessageEnabled = false;
						
					}
					else
					{
						objMap->pointMessageEnabled = false;
						sprintf(objMap->pointMessage, "");
					}

				}

				else if (0 == strcmp(pNode->name(), "PerspectiveSettings"))
				{
					pAttr = pNode->first_attribute("avatarHeight");
					if (pAttr)
						Player.defaultCameraOffset = atof(pAttr->value())+0.3f;

					pAttr = pNode->first_attribute("cameraHeight");
					if (pAttr)
						iTopDownHeight = atof(pAttr->value());

					pAttr = pNode->first_attribute("topDownOrientation");
					if (pAttr)
					{
						if (strcmp(pAttr->value(), "Free") == 0)
							iTopDownOrientation = 0;
						else if (strcmp(pAttr->value(), "North") == 0)
							iTopDownOrientation = 1;
						else if (strcmp(pAttr->value(), "South") == 0)
							iTopDownOrientation = 2;
						else if (strcmp(pAttr->value(), "East") == 0)
							iTopDownOrientation = 3;
						else if (strcmp(pAttr->value(), "West") == 0)
							iTopDownOrientation = 4;
					}

					pAttr = pNode->first_attribute("fixCameraX");
					if (pAttr)
						bFixedCameraLocationX = trueFalse(pAttr->value(), false);

					pAttr = pNode->first_attribute("fixedCameraX");
					if (pAttr)
						fixedCameraLocationX = atof(pAttr->value());

					pAttr = pNode->first_attribute("fixCameraZ");
					if (pAttr)
						bFixedCameraLocationZ = trueFalse(pAttr->value(), false);

					pAttr = pNode->first_attribute("fixedCameraZ");
					if (pAttr)
						fixedCameraLocationZ = atof(pAttr->value());

					pAttr = pNode->first_attribute("cameraMode");
					if (pAttr)
					{
						if (strcmp(pAttr->value(), "First-Person") == 0)
						{
							bTopDown = false;
							b34View = false;
						}
						else if (strcmp(pAttr->value(), "Top-Down") == 0)
						{
							bTopDown = true;
							b34View = false;
						}
						else if (strcmp(pAttr->value(), "3/4 View or Fixed") == 0)
						{
							bTopDown = false;
							b34View = true;
						}

					}

					pAttr = pNode->first_attribute("xRayRendering");
					if (pAttr)
						bTopDownSimpleRender = trueFalse(pAttr->value(), false);


					///*pAttr = pNode->first_attribute("useMouseToOrient");
					//if (pAttr)
					//	bTopDownMouseAngleMode = trueFalse(pAttr->value(), false);*/

					pAttr = pNode->first_attribute("fieldOfView"); // move to just mazewalker?
					if (pAttr)
						fieldOfView = atof(pAttr->value());
				}

				else if (0 == strcmp(pNode->name(), "Speed"))
				{
					pAttr = pNode->first_attribute("moveSpeed");
					if (pAttr)
						objCamera.cameraSpeedMoveDefault = atof(pAttr->value()) / 3.0f*0.05f;

					pAttr = pNode->first_attribute("turnSpeed");
					if (pAttr)
						objCamera.cameraSpeedViewDefault = atof(pAttr->value()) / 45.0f*0.08f;

				}

				else if (0 == strcmp(pNode->name(), "Skybox"))
				{
					pAttr = pNode->first_attribute("id");
					if (pAttr)
					{
						skyboxID = atoi(pAttr->value());
						if (skyboxID <= 0)
							skyboxID = 0;
					}

				}

			}
		}

		xml_node<> *pImageLibraryNode = pRoot->first_node("ImageLibrary");
		if (pImageLibraryNode)
		{
			// Load Images
			for (xml_node<> *pNode = pImageLibraryNode->first_node("Image"); pNode; pNode = pNode->next_sibling("Image"))
			{
				xml_attribute<> *pAttr = pNode->first_attribute("file");
				xml_attribute<> *pAttrID = pNode->first_attribute("id");

				std::string fname;
				int index;

				if (pAttr != NULL&&pAttrID != NULL)
				{
					fname = pAttr->value();
					index = atoi(pAttrID->value());
					strcpy(fPath, fname.c_str());
					sprintf(fPath, "texture\\%s", fname.c_str());

					LoadTexture(fPath, index);
				}
			}
		}
		xml_node<> *pAudioLibraryNode = pRoot->first_node("AudioLibrary");
		if (pAudioLibraryNode)
		{
			// Load Audio
			for (xml_node<> *pNode = pAudioLibraryNode->first_node("Sound"); pNode; pNode = pNode->next_sibling("Sound"))
			{
				xml_attribute<> *pAttr = pNode->first_attribute("file");
				xml_attribute<> *pAttrID = pNode->first_attribute("id");

				std::string fname;
				int index;

				if (pAttr != NULL&&pAttrID != NULL)
				{
					fname = pAttr->value();
					index = atoi(pAttrID->value());

					strcpy(fPath, fname.c_str());


					char* soundPath;
					soundPath=getBestPath(fname, "audio");
					
					if (soundPath)
					{
						curAudioDict.Add(index, soundPath);
					}
					else
					{
						char txt[250];
						sprintf(txt, "Audio '%s' not found!\n", fPath);
						GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING);
						//MessageBoxA(NULL, txt, 0, 0);
					}

					//curAudioDict.Add(index, (char*)fname.c_str());
				}
			}
		}
		xml_node<> *pModelLibraryNode = pRoot->first_node("ModelLibrary");
		if (pModelLibraryNode)
		{
			// Load Models
			for (xml_node<> *pNode = pModelLibraryNode->first_node("Model"); pNode; pNode = pNode->next_sibling("Model"))
			{
				xml_attribute<> *pAttr = pNode->first_attribute("file");
				xml_attribute<> *pAttrID = pNode->first_attribute("id");

				std::string fname;
				int index;

				if (pAttr != NULL&&pAttrID != NULL)
				{
					GLMmodel* model;

					fname = pAttr->value();
					index = atoi(pAttrID->value());

					strcpy(fPath, fname.c_str());

					if (index <= 0)
					{
						//this model is not going to be used!
						model = NULL;
					}
					else
					{
						char* modelPathP;
						char modelDir[800] = "";

						char curDir[_MAX_PATH] = "";
						GetCurrentDirectory(MAX_PATH, curDir);

						char modelPath[_MAX_PATH];

						modelPathP =getBestPath(fname, "objs");


						if (modelPathP)
						{
							strncpy(modelPath, modelPathP, _MAX_PATH - 1);
							strcpy(modelDir, getFileDir(modelPath));
							
							SetCurrentDirectory(modelDir);
							model = LoadModel(modelPath);
							SetCurrentDirectory(curDir);
						}
						else
						{
							model = NULL;
						}

						if (model != NULL)		//if model was loaded
						{
							GLMmodelNode *temp;             //create a temporary node 
							temp = (GLMmodelNode*)malloc(sizeof(GLMmodelNode)); //allocate space for node 
							temp->modelData = model;			//assign model to node2
							temp->modelID = index;			//use index as modelID
							temp->next = ModelDictHead;				//assaign next address to temp
							ModelDictHead = temp;					//assign head to temp
							objMap->numObjs++;					//increment total .obj count

							temp->MaxCoords.x = temp->MinCoords.x = temp->modelData->vertices[3 + 0];
							temp->MaxCoords.y = temp->MinCoords.y = temp->modelData->vertices[3 + 1];
							temp->MaxCoords.z = temp->MinCoords.z = temp->modelData->vertices[3 + 2];
							for (i = 1; i < temp->modelData->numvertices; i++) {
								if (temp->MaxCoords.x < temp->modelData->vertices[3 * i + 0])
									temp->MaxCoords.x = temp->modelData->vertices[3 * i + 0];
								if (temp->MinCoords.x > temp->modelData->vertices[3 * i + 0])
									temp->MinCoords.x = temp->modelData->vertices[3 * i + 0];

								if (temp->MaxCoords.y < temp->modelData->vertices[3 * i + 1])
									temp->MaxCoords.y = temp->modelData->vertices[3 * i + 1];
								if (temp->MinCoords.y > temp->modelData->vertices[3 * i + 1])
									temp->MinCoords.y = temp->modelData->vertices[3 * i + 1];

								if (temp->MaxCoords.z < temp->modelData->vertices[3 * i + 2])
									temp->MaxCoords.z = temp->modelData->vertices[3 * i + 2];
								if (temp->MinCoords.z > temp->modelData->vertices[3 * i + 2])
									temp->MinCoords.z = temp->modelData->vertices[3 * i + 2];

							}
						}
						else
						{
							//fclose(fp);
							char txt[250];
							sprintf(txt, "Model not found!\n'%s'", fPath);
							GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING); //return 0;
						}
					}


				}
			}
		}

		xml_node<> *pMazeItemsNode = pRoot->first_node("MazeItems");
		if (pMazeItemsNode)
		{
			//xml_attribute<> *pAttr;
			xml_node<> *pSubNode;
			int itemID=-1;
			char label[256] = "";

			xml_node<> *pWallsNode = pMazeItemsNode->first_node("Walls");
			if (pWallsNode)
			{
				for (xml_node<> *pNode = pWallsNode->first_node("Wall"); pNode; pNode = pNode->next_sibling("Wall"))
				{
					itemID = -1;
					sprintf(label, "");
					texture = 0;
					curVisibility = 1;

					col.r = 1;
					col.g = 1;
					col.b = 1;

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						itemID = atoi(pAttr->value());

					pAttr = pNode->first_attribute("label");
					if (pAttr)
						sprintf(label, "%s", pAttr->value());

					pSubNode = pNode->first_node("Texture");
					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("id");
						if (pAttr)
							texture = atoi(pAttr->value());

						if (texture == -1)
							curVisibility = 0;
					}

					pSubNode = pNode->first_node("Color");
					
					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("r");
						if (pAttr)
							col.r = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("g");
						if (pAttr)
							col.g = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("b");
						if (pAttr)
							col.b = atof(pAttr->value());
					}

					pSubNode = pNode->first_node("Appearance");

					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("visible");
						if (pAttr)
							curVisibility = trueFalse(pAttr->value(), true);
					}


					char* nodeName = new char[30];
					bool wallError = false;

					for (i = 0; i < 4; i++)
					{
						sprintf(nodeName, "MzPoint%i", i + 1);
						pSubNode = pNode->first_node(nodeName);

						if (pSubNode)
						{
							pAttr = pSubNode->first_attribute("x");
							if (pAttr)
								v[i].x = atof(pAttr->value());
							else
								wallError = true;
							pAttr = pSubNode->first_attribute("y");
							if (pAttr)
								v[i].y = atof(pAttr->value());
							else
								wallError = true;
							pAttr = pSubNode->first_attribute("z");
							if (pAttr)
								v[i].z = atof(pAttr->value());
							else
								wallError = true;

							pAttr = pSubNode->first_attribute("texX");
							if (pAttr)
								v[i].tex_x = atof(pAttr->value());
							else
								v[i].tex_x = 0;

							pAttr = pSubNode->first_attribute("texY");
							if (pAttr)
								v[i].tex_y = atof(pAttr->value());
							else
								v[i].tex_y = 0;
						}
						else
							wallError = true;

						if (wallError)
							break;
					}


					if (wallError||objMap->Add(MO_PLANE, texture, col, v, curVisibility) == 0)
					{
						GUIMessageBox("Error adding Wall!\nInvalid construction", 0, TEXTBOXSTYLE_WARNING); return 0;
					}
				}

				for (xml_node<> *pNode = pWallsNode->first_node("CurvedWall"); pNode; pNode = pNode->next_sibling("CurvedWall"))
				{
					itemID = -1;
					sprintf(label, "");
					texture = 0;
					curVisibility = 1;

					col.r = 1;
					col.g = 1;
					col.b = 1;

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						itemID = atoi(pAttr->value());

					pAttr = pNode->first_attribute("label");
					if (pAttr)
						sprintf(label, "%s", pAttr->value());

					pSubNode = pNode->first_node("Texture");
					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("id");
						if (pAttr)
							texture = atoi(pAttr->value());

						if (texture == -1)
							curVisibility = 0;
					}

					pSubNode = pNode->first_node("Color");

					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("r");
						if (pAttr)
							col.r = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("g");
						if (pAttr)
							col.g = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("b");
						if (pAttr)
							col.b = atof(pAttr->value());
					}

					pSubNode = pNode->first_node("Appearance");

					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("visible");
						if (pAttr)
							curVisibility = trueFalse(pAttr->value(), true);
					}


					char* nodeName = new char[30];
					bool wallError = false;

					for (i = 0; i < 4; i++)
					{
						sprintf(nodeName, "MzPoint%i", i + 1);
						pSubNode = pNode->first_node(nodeName);

						if (pSubNode)
						{
							pAttr = pSubNode->first_attribute("x");
							if (pAttr)
								v[i].x = atof(pAttr->value());
							else
								wallError = true;
							pAttr = pSubNode->first_attribute("y");
							if (pAttr)
								v[i].y = atof(pAttr->value());
							else
								wallError = true;
							pAttr = pSubNode->first_attribute("z");
							if (pAttr)
								v[i].z = atof(pAttr->value());
							else
								wallError = true;

							pAttr = pSubNode->first_attribute("texX");
							if (pAttr)
								v[i].tex_x = atof(pAttr->value());
							else
								v[i].tex_x = 0;

							pAttr = pSubNode->first_attribute("texY");
							if (pAttr)
								v[i].tex_y = atof(pAttr->value());
							else
								v[i].tex_y = 0;
						}
						else
							wallError = true;

						if (wallError)
							break;
					}


					if (wallError)// || objMap->Add(MO_PLANE, texture, col, v, curVisibility) == 0)
					{
						GUIMessageBox("Error adding Wall!\nInvalid construction", 0, TEXTBOXSTYLE_WARNING); return 0;
					}
					else
					{
						pSubNode = pNode->first_node("Geometry");

						if (pSubNode)
						{
							int numVerts = 0,maxVerts=10000;
							char* geoText = pSubNode->value();
							char*pch = strtok(geoText, ",");
							float curVal;
							float x, y, z;
							float texX, texY;

							GLfloat* verticesTemp = new GLfloat[maxVerts * 5]; // create vertex array
							
							while (pch!=NULL&&numVerts<maxVerts)
							{
								curVal = atof(pch);
								x = curVal;
								pch = strtok(NULL, ",");
								if (pch == NULL)
									break;
								curVal = atof(pch);
								y = curVal;
								pch = strtok(NULL, ",");
								if (pch == NULL)
									break;
								curVal = atof(pch);
								z = curVal;
								pch = strtok(NULL, ",");
								if (pch == NULL)
									break;
								curVal = atof(pch);
								texX = curVal;
								pch = strtok(NULL, ",");
								if (pch == NULL)
									break;
								curVal = atof(pch);
								texY = curVal;
								pch = strtok(NULL, ",");
								if (pch == NULL)
									break;
								curVal = atof(pch);
								
								verticesTemp[numVerts*5] = x;
								verticesTemp[numVerts*5+1] = y;
								verticesTemp[numVerts*5+2] = z;
								verticesTemp[numVerts * 5 +3] = texX;
								verticesTemp[numVerts * 5 + 4] = texY;
								numVerts++;

							}

							pSubNode = pNode->first_node("Indicies");

							GLint* indiciesTemp;
							int numIndex = 0;
							if (pSubNode)
							{

								char* indexText = pSubNode->value();
								char*pch = strtok(indexText, ",");
								float curVal;
								float x, y, z;
								float texX, texY;
								int index1, index2, index3;

								indiciesTemp = new GLint[maxVerts * 4]; // create vertex array

								while (pch != NULL&&numIndex < maxVerts)
								{
									curVal = atoi(pch);
									index1 = curVal;
									pch = strtok(NULL, ",");
									if (pch == NULL)
										break;
									curVal = atoi(pch);
									index2 = curVal;
									pch = strtok(NULL, ",");
									if (pch == NULL)
										break;
									curVal = atoi(pch);
									index3 = curVal;
									
									indiciesTemp[numIndex * 3] = index1;
									indiciesTemp[numIndex * 3 + 1] = index2;
									indiciesTemp[numIndex * 3 + 2] = index3;

									if (index1 > numVerts || index2 > numVerts || index3 > numVerts || index1 < 0 || index2 < 0 || index3 < 0)
									{
										numVerts = 0;
										numIndex = 0;
										GUIMessageBox("Error adding Wall!\nInvalid goemetry", 0, TEXTBOXSTYLE_WARNING); return 0;
									}

									numIndex++;

									pch = strtok(NULL, ",");
									if (pch == NULL)
										break;
								}
							}

							if (numIndex > 0&&numVerts>0)
							{
								GLfloat* vertices = new GLfloat[numVerts * 5]; // create vertex array


								for (int i = 0;i<numVerts;i++)
								{
									vertices[i*5]=verticesTemp[i*5];
									vertices[i*5+1] = verticesTemp[i*5 + 1];
									vertices[i*5+2] = verticesTemp[i*5 + 2];
									vertices[i * 5 + 3] = verticesTemp[i * 5 + 3];
									vertices[i * 5 + 4] = verticesTemp[i * 5 + 4];
								}
								delete[] verticesTemp;

								GLint* indicies = new GLint[numIndex * 3];

								for (int i = 0; i<numIndex; i++)
								{
									indicies[i * 3] = indiciesTemp[i* 3];
									indicies[i * 3 + 1] = indiciesTemp[i * 3 + 1];
									indicies[i * 3 + 2] = indiciesTemp[i * 3 + 2];
								}
								delete[] indiciesTemp;
								
								int z;
								for (int i = 0; i < numIndex*3-3; i+=6)// for each (wall = 2triangle, = 4 verticies)
								{
									for (int j = 0; j <6; j++)
									{

										if (j == 3)
											z = 2;
										else if (j == 2)
											z = 3;
										else
											z = j;

										if (j == 3)
											j = 5;

										v[z].x = vertices[indicies[i+j]*5];
										v[z].y = vertices[indicies[i+j] *5 + 1];
										v[z].z = vertices[indicies[i+j] *5 + 2];
										v[z].tex_x = vertices[indicies[i+j] * 5 + 3];
										v[z].tex_y = vertices[indicies[i+j] * 5 + 4];


									}


									objMap->Add(MO_PLANE, texture, col, v, curVisibility);//such a bad idea
											//I'm ashamed
								}
								

								delete[] vertices;
								delete[] indicies;
							}
							

						}
					}
				}
			}
			xml_node<> *pFloorsNode = pMazeItemsNode->first_node("Floors");
			if (pFloorsNode)
			{
				for (xml_node<> *pNode = pFloorsNode->first_node("Floor"); pNode; pNode = pNode->next_sibling("Floor"))
				{
					itemID = -1;
					sprintf(label, "");
					texture = 0;
					col.r = 1;
					col.g = 1;
					col.b = 1;
					curVisibility = 1;
					bool hasCeiling = false;
					float ceilingHeight = 2;

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						itemID = atoi(pAttr->value());

					pAttr = pNode->first_attribute("label");
					if (pAttr)
						sprintf(label, "%s", pAttr->value());

					pSubNode = pNode->first_node("FloorTexture");

					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("id");
						if (pAttr)
							texture = atoi(pAttr->value());

						//pAttr = pSubNode->first_attribute("flip"); ///Not available for Floors /ceiling
						//if (pAttr)
						//	flip = trueFalse(pAttr->value(), false);
					}

					pSubNode = pNode->first_node("FloorColor");

					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("r");
						if (pAttr)
							col.r = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("g");
						if (pAttr)
							col.g = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("b");
						if (pAttr)
							col.b = atof(pAttr->value());

					}

					pSubNode = pNode->first_node("Appearance");
					
					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("hasCeiling");
						if (pAttr)
							hasCeiling = trueFalse(pAttr->value(), false);

						pAttr = pSubNode->first_attribute("visible");
						if (pAttr)
							curVisibility = trueFalse(pAttr->value(), true);

						pAttr = pSubNode->first_attribute("ceilingHeight");
						if (pAttr)
							ceilingHeight = atof(pAttr->value());
					}


					char* nodeName = new char[30];
					bool floorError = false;

					for (i = 0; i < 4; i++)
					{
						sprintf(nodeName, "MzPoint%i", i + 1);
						pSubNode = pNode->first_node(nodeName);

						if (pSubNode)
						{
							pAttr = pSubNode->first_attribute("x");
							if (pAttr)
								v[i].x = atof(pAttr->value());
							else
								floorError = true;

							pAttr = pSubNode->first_attribute("y");
							if (pAttr)
								v[i].y = atof(pAttr->value());
							else
								floorError = true;

							pAttr = pSubNode->first_attribute("z");
							if (pAttr)
								v[i].z = atof(pAttr->value());
							else
								floorError = true;

							pAttr = pSubNode->first_attribute("texX");
							if (pAttr)
								v[i].tex_x = atof(pAttr->value());
							else
								v[i].tex_x = 0;

							pAttr = pSubNode->first_attribute("texY");
							if (pAttr)
								v[i].tex_y = atof(pAttr->value());
							else
								v[i].tex_y = 0;

							if (hasCeiling)
							{
								v2[i].y = v[i].y + ceilingHeight;
								v2[i].x = v[i].x;
								v2[i].z = v[i].z;

								pAttr = pSubNode->first_attribute("texX_Ceiling");
								if (pAttr)
									v2[i].tex_x = atof(pAttr->value());
								else
									v2[i].tex_x = 0;

								pAttr = pSubNode->first_attribute("texY_Ceiling");
								if (pAttr)
									v2[i].tex_y = atof(pAttr->value());
								else
									v2[i].tex_y = 0;
							}
						}
						else
							floorError = true;
					}

					if (floorError||objMap->Add(MO_PLANE, texture, col, v, curVisibility) == 0)
					{
						GUIMessageBox("Error adding Floor!\nInvalid construction", 0, TEXTBOXSTYLE_WARNING);
					}
					else if (hasCeiling)
					{
						pSubNode = pNode->first_node("CeilingColor");
						col.r = 1;
						col.g = 1;
						col.b = 1;
						texture = 0;

						if (pSubNode)
						{
							pAttr = pSubNode->first_attribute("r");
							if (pAttr)
								col.r = atof(pAttr->value());

							pAttr = pSubNode->first_attribute("g");
							if (pAttr)
								col.g = atof(pAttr->value());

							pAttr = pSubNode->first_attribute("b");
							if (pAttr)
								col.b = atof(pAttr->value());
						}

						pSubNode = pNode->first_node("CeilingTexture");

						if (pSubNode)
						{
							pAttr = pSubNode->first_attribute("id");
							if (pAttr)
								texture = atoi(pAttr->value());

							//pAttr = pSubNode->first_attribute("Flip");
							//if (pAttr)
							//	flip = trueFalse(pAttr->value(), false);
						}

						if (objMap->Add(MO_PLANE, texture, col, v2, curVisibility) == 0)
						{
							GUIMessageBox("Error adding Ceiling!\n\nMAZE couldn't load plane to memory!", 0, TEXTBOXSTYLE_WARNING); 
						}
					}

				}
			}
			
			xml_node<>*pStaticModelsNode = pMazeItemsNode->first_node("StaticModels");
			if (pStaticModelsNode)
			{
				for (xml_node<> *pNode = pStaticModelsNode->first_node("StaticModel"); pNode; pNode = pNode->next_sibling("StaticModel"))
				{
					//////////////////////////////////////////////////////////////////////////////////

					//STATIC Objects

					/////////////////////////////////////////////////////////////////////////////////
					itemID = -1;
					sprintf(label, "");

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						itemID = atoi(pAttr->value());

					pAttr = pNode->first_attribute("label");
					if (pAttr)
						sprintf(label, "%s", pAttr->value());

					temp = (MapModel*)malloc(sizeof(MapModel));
					temp->next = MapModelHead;

					temp->modelID = -1;
					temp->scale = 1;
					temp->rot.x = 0;
					temp->rot.y = 0;
					temp->rot.z = 0;
					temp->dID = -1;

					temp2 = (GLMmodelNode*)malloc(sizeof(GLMmodelNode));
					temp2 = ModelDictHead;

					pSubNode = pNode->first_node("Model");

					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("id");
						if (pAttr)
							temp->modelID = atoi(pAttr->value());
							
						pAttr = pSubNode->first_attribute("scale");
						if (pAttr)
							temp->scale = atof(pAttr->value());
							
						pAttr = pSubNode->first_attribute("rotX");
						if (pAttr)
							temp->rot.x = atof(pAttr->value());
							
						pAttr = pSubNode->first_attribute("rotY");
						if (pAttr)
							temp->rot.y = atof(pAttr->value());
							
						pAttr = pSubNode->first_attribute("rotZ");
						if (pAttr)
							temp->rot.z = atof(pAttr->value());
					}

					while (temp2 != NULL&&temp2->modelID != temp->modelID)
					{
						temp2 = temp2->next;
					}
					if (temp->modelID<=0 ||temp2 == NULL)
					{
						temp2 = ModelDictHead;
						if ((temp2 == NULL || objMap->numObjs == 0))
						{
							char message[50];
							sprintf(message, "No valid Models!\n Model ID %i not loaded",temp->modelID );
							GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
							break;
						}
						else
						{
							char message[100];
							sprintf(message, "%i is not a valid Model ID!\nUsing primary model instead\n", temp->modelID);
							GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
							break;
						}
					}
					if (temp2 != NULL)
					{
						temp->name = new char[256];

						sprintf(temp->name, "S%i", temp->modelID);

						temp->modelData = temp2->modelData;
						temp->transparent = temp2->modelData->transparent;
						temp->MinCoords = temp2->MinCoords;
						temp->MaxCoords = temp2->MaxCoords;
						temp->isVisible = true;
						temp->distToPlayer = 1;
						temp->collide = 1;
						temp->pos.x = 0;
						temp->pos.y = 0;
						temp->pos.z = 0;
						temp->kinematic = 0;
						temp->mass = 1;
						temp->collide = true;

						temp->pointsGranted = 0;
						temp->p1PointThreshold = 0;
						temp->p2PointThreshold = 0;
						temp->p2InteractRequired = false;
						temp->p1TriggerTime = 0;
						temp->p2TriggerTime = 0;
						temp->p1ActiveRadius = 0;
						temp->p2ActiveRadius = 0;

						temp->highlightAudioID = -1;
						temp->triggerAudioID = -1;
						temp->highlightAudioLoop = false;
						temp->triggerAudioLoop = false;
						temp->highlightAudioBehavior = 0;
						bool modelError = false;

						pSubNode = pNode->first_node("MzPoint");

						if (pSubNode)
						{

							pAttr = pSubNode->first_attribute("x");
							if (pAttr)
								temp->pos.x = atof(pAttr->value());
							else
								modelError = true;

							pAttr = pSubNode->first_attribute("y");
							if (pAttr)
								temp->pos.y = atof(pAttr->value());
							else
								modelError = true;


								pAttr = pSubNode->first_attribute("z");
							if (pAttr)
								temp->pos.z = atof(pAttr->value());
							else
								modelError = true;

						}
						else
							modelError = true;



						pSubNode = pNode->first_node("Physics");

						if (pSubNode)
						{

							pAttr = pSubNode->first_attribute("collision");
							if (pAttr)
								temp->collide = trueFalse(pAttr->value());

							pAttr = pSubNode->first_attribute("kinematic");
							if (pAttr)
								temp->kinematic = trueFalse(pAttr->value());

							pAttr = pSubNode->first_attribute("mass");
							if (pAttr)
								temp->mass = atof(pAttr->value())*temp->kinematic;
						}


						temp->initRot = temp->rot;
						temp->initPos = temp->pos;
						temp->endPos = temp->pos;
						temp->endRot = temp->pos;
						temp->endScale = temp->scale;

						temp->initScale = temp->scale;
						glmCalcBounds(temp);


						//Initialization
						temp->dynamic = false;
						temp->activated = false;
						temp->destroy = false;
						temp->destroyed = false;
						temp->highlighted = false;
						temp->proximityTrigger = 0; //0 no proximity,1 highlight proximity, 2 activation proximity
						temp->moving = false;
						temp->switchModel = false;
						temp->currentPercentage = 100;
						temp->glowFactor = 0;
						temp->glowColor = 0;
						temp->highlightStyle = 0;

						

						temp->instanceID = objMap->modelCount;
						objMap->modelCount++;

						if (modelError)
						{
							GUIMessageBox("Missing Valid Model Position, placing at 0,0", 0, TEXTBOXSTYLE_WARNING);
						}

						MapModelHead = temp;
					}

				}
			}

			xml_node<>*pDynamicObjectsNode = pMazeItemsNode->first_node("DynamicObjects");
			if (pDynamicObjectsNode)
			{
				for (xml_node<> *pNode = pDynamicObjectsNode->first_node("DynamicObject"); pNode; pNode = pNode->next_sibling("DynamicObject"))
				{
					//////////////////////////////////////////////////////////////////////////////////

					//Dynamic Objects

					/////////////////////////////////////////////////////////////////////////////////
					itemID = -1;
					sprintf(label, "");

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						itemID = atoi(pAttr->value());

					pAttr = pNode->first_attribute("label");
					if (pAttr)
						sprintf(label, "%s", pAttr->value());

					temp = (MapModel*)malloc(sizeof(MapModel));
					temp->next = MapModelHead;
					temp->modelID = -1;
					temp->scale = 1;
					temp->rot.x = 0;
					temp->rot.y = 0;
					temp->rot.z = 0;
					temp->name = new char[256];
					if (strlen(label)>0)
						sprintf(temp->name, "%s", label);
					else
						sprintf(temp->name, "d%i", itemID);

					temp->dID = itemID;
					


					temp2 = (GLMmodelNode*)malloc(sizeof(GLMmodelNode));
					temp2 = ModelDictHead;

					pSubNode = pNode->first_node("Model");

					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("id");
						if (pAttr)
							temp->modelID = atoi(pAttr->value());
							

						pAttr = pSubNode->first_attribute("scale");
						if (pAttr)
							temp->scale = atof(pAttr->value());
							

						pAttr = pSubNode->first_attribute("rotX");
						if (pAttr)
							temp->rot.x = atof(pAttr->value());
							

						pAttr = pSubNode->first_attribute("rotY");
						if (pAttr)
							temp->rot.y = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("rotZ");
						if (pAttr)
							temp->rot.z = atof(pAttr->value());
					}

					while (temp2 != NULL&&temp2->modelID != temp->modelID)
					{
						temp2 = temp2->next;
					}
					if (temp->modelID<=0||temp2 == NULL)
					{
						temp2 = ModelDictHead;
						if ((temp2 == NULL || objMap->numObjs == 0))
						{
							char message[150];
							sprintf(message, "No valid Models!\n Model ID %i not loaded", temp->modelID);
							GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
	
						}
						else
						{
							char message[100];
							sprintf(message, "%i is not a valid Model ID!\nUsing primary model instead", temp->modelID);
							GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);

						}//MessageBox(NULL,"","Unkown ModelID",MB_OK | MB_ICONINFORMATION);}
					}
					if (temp2 != NULL)
					{
						
						temp->modelData = temp2->modelData;
						temp->transparent = temp2->modelData->transparent;
						temp->MinCoords = temp2->MinCoords;
						temp->MaxCoords = temp2->MaxCoords;
						temp->isVisible = true;
						temp->distToPlayer = 1;
						temp->collide = true;
						temp->kinematic = false;
						temp->mass = 1;
						temp->pos.x = 0;
						temp->pos.y = 0;
						temp->pos.z = 0;
						bool modelError = false;

						pSubNode = pNode->first_node("MzPoint");

						if (pSubNode)
						{

							pAttr = pSubNode->first_attribute("x");
							if (pAttr)
								temp->pos.x = atof(pAttr->value());
							else
								modelError = true;

							pAttr = pSubNode->first_attribute("y");
							if (pAttr)
								temp->pos.y = atof(pAttr->value());
							else
								modelError = true;

							pAttr = pSubNode->first_attribute("z");
							if (pAttr)
								temp->pos.z = atof(pAttr->value());
							else
								modelError = true;
						}
						else
							modelError = true;

						pSubNode = pNode->first_node("Physics");

						if (pSubNode)
						{

							pAttr = pSubNode->first_attribute("collision");
							if (pAttr)
								temp->collide = trueFalse(pAttr->value());

							pAttr = pSubNode->first_attribute("kinematic");
							if (pAttr)
								temp->kinematic = trueFalse(pAttr->value());

							pAttr = pSubNode->first_attribute("mass");
							if (pAttr)
								temp->mass = atof(pAttr->value())*temp->kinematic;
						}


						temp->initRot = temp->rot;
						temp->initPos = temp->pos;
						temp->endPos = temp->pos;
						temp->endRot = temp->rot;
						temp->endScale = temp->scale;

						temp->initScale = temp->scale;
						temp->targetModelID = temp->modelID;
						glmCalcBounds(temp);


						//Initialization
						temp->dynamic = true;
						temp->activated = false;
						temp->destroy = false;
						temp->destroyed = false;
						temp->highlighted = false;
						temp->proximityTrigger = 0; //0 no proximity,1 highlight proximity, 2 activation proximity
						temp->moving = false;
						temp->switchModel = false;
						temp->currentPercentage = 100;
						temp->glowFactor = 0;
						temp->glowColor = 0;
						temp->highlightStyle = 0;
						temp->highlightCriteria = 1; //proximity only
						temp->triggerAction = 0;
						temp->triggerCriteria = 0;
						temp->currentPercentage = 0;
						temp->targetPercentage = 100;


						temp->pointsGranted = 0;
						temp->p1PointThreshold = 0;
						temp->p2PointThreshold = 0;
						temp->p2InteractRequired = false;
						temp->p1TriggerTime = 0;
						temp->p2TriggerTime = 0;
						temp->p1ActiveRadius = 1;
						temp->p2ActiveRadius = 1;
						temp->actionTime = 0;
						

						temp->highlightAudioID = -1;
						temp->triggerAudioID = -1;
						temp->highlightAudioLoop = false;
						temp->triggerAudioLoop = false;
						temp->highlightAudioBehavior = 0;

						
							

						pSubNode = pNode->first_node("Phase1Highlight");
						xml_node<> *pSubSubNode;

						if (pSubNode)
						{

							pAttr = pSubNode->first_attribute("criteria");
							if (pAttr)
							{
								if (strcmp(pAttr->value(), "Skip") == 0)
									temp->highlightCriteria = 0;
								else if (strcmp(pAttr->value(), "Proximity") == 0)
									temp->highlightCriteria = 1;
								else if (strcmp(pAttr->value(), "Time") == 0)
									temp->highlightCriteria = 2;
								else if (strcmp(pAttr->value(), "Proximity and Time") == 0)
									temp->highlightCriteria = 3;
							}
							else
								temp->highlightCriteria = 0;

							pAttr = pSubNode->first_attribute("highlightStyle");
							if (pAttr)
							{
								if (strcmp(pAttr->value(), "Bounce") == 0)
									temp->highlightStyle = 1;
								else if (strcmp(pAttr->value(), "Shake") == 0)
									temp->highlightStyle = 2;
								else if (strcmp(pAttr->value(), "Rotate") == 0)
									temp->highlightStyle = 3;
								else if (strcmp(pAttr->value(), "GlowRed") == 0)
									temp->highlightStyle = 4;
								else if (strcmp(pAttr->value(), "GlowGreen") == 0)
									temp->highlightStyle = 5;
								else if (strcmp(pAttr->value(), "GlowBlue") == 0)
									temp->highlightStyle = 6;
								else if (strcmp(pAttr->value(), "ParticleColor1") == 0)
									temp->highlightStyle = 11;
								else if (strcmp(pAttr->value(), "ParticleColor2") == 0)
									temp->highlightStyle = 12;
								else if (strcmp(pAttr->value(), "ParticleColor3") == 0)
									temp->highlightStyle = 13;
								else if (strcmp(pAttr->value(), "ParticleColor4") == 0)
									temp->highlightStyle = 14;
								else if (strcmp(pAttr->value(), "ParticleColor5") == 0)
									temp->highlightStyle = 15;
								else if (strcmp(pAttr->value(), "ParticleColor6") == 0)
									temp->highlightStyle = 16;
								else if (strcmp(pAttr->value(), "ParticleColor7") == 0)
									temp->highlightStyle = 17;
								else if (strcmp(pAttr->value(), "ParticleColor8") == 0)
									temp->highlightStyle = 18;
								else if (strcmp(pAttr->value(), "ParticleColor9") == 0)
									temp->highlightStyle = 19;
								else if (strcmp(pAttr->value(), "ParticleColor10") == 0)
									temp->highlightStyle = 20;
								else if (strcmp(pAttr->value(), "ParticleColor11") == 0)
									temp->highlightStyle = 21;
								else if (strcmp(pAttr->value(), "ParticleColor12") == 0)
									temp->highlightStyle = 22;
								else if (strcmp(pAttr->value(), "FallingParticleColor1") == 0)
									temp->highlightStyle = 23;
								else if (strcmp(pAttr->value(), "FallingParticleColor2") == 0)
									temp->highlightStyle = 24;
								else if (strcmp(pAttr->value(), "FallingParticleColor3") == 0)
									temp->highlightStyle = 25;
								else if (strcmp(pAttr->value(), "FallingParticleColor4") == 0)
									temp->highlightStyle = 26;
								else if (strcmp(pAttr->value(), "FallingParticleColor5") == 0)
									temp->highlightStyle = 27;
								else if (strcmp(pAttr->value(), "FallingParticleColor6") == 0)
									temp->highlightStyle = 28;
								else if (strcmp(pAttr->value(), "FallingParticleColor7") == 0)
									temp->highlightStyle = 29;
								else if (strcmp(pAttr->value(), "FallingParticleColor8") == 0)
									temp->highlightStyle = 30;
								else if (strcmp(pAttr->value(), "FallingParticleColor9") == 0)
									temp->highlightStyle = 31;
								else if (strcmp(pAttr->value(), "FallingParticleColor10") == 0)
									temp->highlightStyle = 32;
								else if (strcmp(pAttr->value(), "FallingParticleColor11") == 0)
									temp->highlightStyle = 33;
								else if (strcmp(pAttr->value(), "FallingParticleColor12") == 0)
									temp->highlightStyle = 34;

							}

							pAttr = pSubNode->first_attribute("pointThreshold");
							if (pAttr)
								temp->p1PointThreshold = atoi(pAttr->value());


							pAttr = pSubNode->first_attribute("radius");
							if (pAttr)
								temp->p1ActiveRadius = atof(pAttr->value());

							pAttr = pSubNode->first_attribute("triggerTime");
							if (pAttr)
								temp->p1TriggerTime = 1000 * atof(pAttr->value());

							pSubSubNode = pSubNode->first_node("Audio");
							if (pSubSubNode)
							{
								pAttr = pSubSubNode->first_attribute("id");
								if (pAttr)
									temp->highlightAudioID = atoi(pAttr->value());

								pAttr = pSubSubNode->first_attribute("unhighlightBehavior");
								if (pAttr)
								{
									if (strcmp(pAttr->value(), "Stop") == 0)
										temp->highlightAudioBehavior = 0;
									else if (strcmp(pAttr->value(), "Pause") == 0)
										temp->highlightAudioBehavior = 1;
									else if (strcmp(pAttr->value(), "Continue") == 0)
										temp->highlightAudioBehavior = 2;
									else if (strcmp(pAttr->value(), "VolumeByDistance") == 0)
										temp->highlightAudioBehavior = 3;

								}

								pAttr = pSubSubNode->first_attribute("loop");

								if (pAttr)
									temp->highlightAudioLoop = trueFalse(pAttr->value(), false);
							}

						}

						pSubNode = pNode->first_node("Phase2Event");

						if (pSubNode)
						{

							pAttr = pSubNode->first_attribute("criteria");
							if (pAttr)
							{
								if (strcmp(pAttr->value(), "None") == 0)
									temp->triggerCriteria = 0;
								else if (strcmp(pAttr->value(), "Proximity") == 0)
									temp->triggerCriteria = 1;
								else if (strcmp(pAttr->value(), "Time") == 0)
									temp->triggerCriteria = 2;
								else if (strcmp(pAttr->value(), "Interact") == 0)
									temp->triggerCriteria = 3;
								else if (strcmp(pAttr->value(), "Proximity and Time") == 0)
									temp->triggerCriteria = 4;
								else if (strcmp(pAttr->value(), "Proximity and Interact") == 0)
									temp->triggerCriteria = 5;
								else if (strcmp(pAttr->value(), "Interact and Time") == 0)
									temp->triggerCriteria = 6;
								else if (strcmp(pAttr->value(), "Proximity, Interact and Time") == 0)
									temp->triggerCriteria = 7;
							}

							pAttr = pSubNode->first_attribute("pointThreshold");
							if (pAttr)
								temp->p2PointThreshold = atoi(pAttr->value());

							pAttr = pSubNode->first_attribute("pointsGranted");
							if (pAttr)
								temp->pointsGranted = atoi(pAttr->value());

							pAttr = pSubNode->first_attribute("triggerAction");
							if (pAttr)
							{
								if (strcmp(pAttr->value(), "No Action") == 0)
									temp->triggerAction = 0;
								else if (strcmp(pAttr->value(), "Move/Rotate") == 0)
									temp->triggerAction = 1;
								else if (strcmp(pAttr->value(), "Change Model") == 0)
									temp->triggerAction = 2;
								else if (strcmp(pAttr->value(), "Destroy Model") == 0)
									temp->triggerAction = 3;
							}

							pAttr = pSubNode->first_attribute("radius");
							if (pAttr)
								temp->p2ActiveRadius = atof(pAttr->value());

							pAttr = pSubNode->first_attribute("triggerTime");
							if (pAttr)
								temp->p2TriggerTime = 1000 * atof(pAttr->value());

							pAttr = pSubNode->first_attribute("actionTime");
							if (pAttr)
								temp->actionTime = 1000 * atof(pAttr->value());

							pSubSubNode = pSubNode->first_node("Audio");
							if (pSubSubNode)
							{
								pAttr = pSubSubNode->first_attribute("id");
								if (pAttr)
									temp->triggerAudioID = atoi(pAttr->value());

								pAttr = pSubSubNode->first_attribute("audioBehavior");
								if (pAttr)
									temp->triggerAudioBehavior = atoi(pAttr->value());

								pAttr = pSubSubNode->first_attribute("loop");

								if (pAttr)
									temp->triggerAudioLoop = trueFalse(pAttr->value(), false);
							}

							pSubSubNode = pSubNode->first_node("EndMzPoint");
							if (pSubSubNode)
							{
								pAttr = pSubSubNode->first_attribute("x");
								if (pAttr)
									temp->endPos.x += atof(pAttr->value());

								pAttr = pSubSubNode->first_attribute("y");
								if (pAttr)
									temp->endPos.y += atof(pAttr->value());

								pAttr = pSubSubNode->first_attribute("z");
								if (pAttr)
									temp->endPos.z += atof(pAttr->value());
							}

							pSubSubNode = pSubNode->first_node("EndModel");
							if (pSubSubNode)
							{
								pAttr = pSubSubNode->first_attribute("rotX");
								if (pAttr)
									temp->endRot.x += atof(pAttr->value());

								pAttr = pSubSubNode->first_attribute("rotY");
								if (pAttr)
									temp->endRot.y += atof(pAttr->value());

								pAttr = pSubSubNode->first_attribute("rotZ");
								if (pAttr)
									temp->endRot.z += atof(pAttr->value());

								pAttr = pSubSubNode->first_attribute("endScale");
								if (pAttr)
									temp->endScale *= atof(pAttr->value());

								pAttr = pSubSubNode->first_attribute("switchModelID");
								if (pAttr)
									temp->targetModelID = atoi(pAttr->value());

								if (temp->targetModelID > 0)
									temp->switchModel = true;
							}

						}

						if (temp->highlightCriteria != 1 && temp->highlightCriteria != 3) //disable p1 radius if not part of highlight criteria
							temp->p1ActiveRadius = -1;
						if (temp->highlightCriteria != 2 && temp->highlightCriteria != 3) //diable p1 time if not part of highlight criteria
							temp->p1TriggerTime = 0;

						if (temp->triggerCriteria == 3 || temp->triggerCriteria == 5 || temp->triggerCriteria == 6 || temp->triggerCriteria == 7) //diable p2interact if not part of criteria
							temp->p2InteractRequired = true;
						else
							temp->p2InteractRequired = false;

						if (temp->triggerCriteria != 1 && temp->triggerCriteria != 4 && temp->triggerCriteria != 6 && temp->triggerCriteria != 7) //diable p2 radius if not part of criteria
							temp->p2ActiveRadius = 0;

						if (temp->triggerCriteria != 2 && temp->triggerCriteria != 4 && temp->triggerCriteria != 5 && temp->triggerCriteria != 7) //diable p2 time if not part of criteria
							temp->p2TriggerTime = 0;

						

						temp->glowFactor = 0;
						temp->glowColor = 0;
						if (temp->highlightStyle > 3 && temp->highlightStyle < 8)
						{
							temp->glowColor = temp->highlightStyle - 4;
						}
						if (temp->highlightStyle > 10 && temp->highlightStyle <= 34)
						{
							particleArray *newCircle;             //create a temporary node 
							newCircle = (particleArray*)malloc(sizeof(particleArray)); //allocate space for node 
							newCircle->next = partArrayHead;				//assign next address to temp
							partArrayHead = newCircle;

							newCircle->dID = temp->dID;
							if (temp->highlightStyle <= 19)
							{
								newCircle->color = temp->highlightStyle - 6;
								newCircle->up = false;
							}
							else
							{
								newCircle->color = temp->highlightStyle - 22;
								newCircle->up = true;
							}

							newCircle->pos = temp->pos;
							newCircle->radius = (float)sqrt((temp->MaxCoords.x*temp->MaxCoords.x + temp->MaxCoords.z*temp->MaxCoords.z)*temp->scale*temp->scale);

							newCircle->activated = false;
							newCircle->particleCount = newCircle->radius*newCircle->radius * 1000;
							if (newCircle->particleCount > MAX_PARTICLES)
								newCircle->particleCount = MAX_PARTICLES;

						}


						temp->instanceID = objMap->modelCount;
						objMap->modelCount++;
						objMap->dModelCount++;


						if (modelError)
						{
							GUIMessageBox("Missing Valid Model Position, placing at 0,0", 0, TEXTBOXSTYLE_WARNING);
						}

						MapModelHead = temp;
					}

				}
			}


			xml_node<> *pStartPositionsNode = pMazeItemsNode->first_node("StartPositions");
			if (pStartPositionsNode)
			{
				for (xml_node<> *pNode = pStartPositionsNode->first_node("StartPosition"); pNode; pNode = pNode->next_sibling("StartPosition"))
				{
					itemID = -1;
					sprintf(label, "");

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						itemID = atoi(pAttr->value());

					

					pAttr = pNode->first_attribute("label");
					if (pAttr)
						sprintf(label, "%s", pAttr->value());

					startPos=(StartPosition*)malloc(sizeof(StartPosition));

					if (!objMap->bStartPos)
					{
						objMap->bStartPos = true;
						startPos->last = true;
					}
					else
					{
						startPos->last = false; //head is not last startPos
					}


					startPos->index = itemID;
					startPos->next = objMap->startPosHead;
					startPos->x = 0;
					startPos->y = 0;
					startPos->z = 0;
					startPos->angle = 0;
					startPos->vertAngle = 0;
					startPos->inputValue = -1;
					startPos->inputValue = 0; //initializing value

					bool startPosError = false;

					pSubNode = pNode->first_node("MzPoint");
					if (pSubNode)
					{

						pAttr = pSubNode->first_attribute("x");
						if (pAttr)
							startPos->x = atof(pAttr->value());
						else
							startPosError = true;

						pAttr = pSubNode->first_attribute("y");
						if (pAttr)
							startPos->y = atof(pAttr->value());
						else
							startPosError = true;

						pAttr = pSubNode->first_attribute("z");
						if (pAttr)
							startPos->z = atof(pAttr->value());
						else
							startPosError = true;
					}
					else
						startPosError = true;

					pSubNode = pNode->first_node("ViewAngle");
					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("angle");
						if (pAttr)
							startPos->angle = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("vertAngle");
						if (pAttr)
							startPos->vertAngle = atof(pAttr->value())*PI / 180;

						pAttr = pSubNode->first_attribute("randomAngle");
						if (pAttr && trueFalse(pAttr->value()))
						{
							startPos->angle = rand() / (RAND_MAX / 360.0); //int from 0 to 359
							EventLog(0,130,startPos->angle,"InitAngle");
						}

						pAttr = pSubNode->first_attribute("randomVertAngle");
						if (pAttr && trueFalse(pAttr->value()))
						{
							startPos->vertAngle = rand() / (RAND_MAX / 360.0); //int from 0 to 359
							EventLog(0,131,startPos->vertAngle,"InitVertAngle");
						}

					}

					objMap->startPosHead = startPos;
				}
			}
			xml_node<> *pEndRegionsNode = pMazeItemsNode->first_node("EndRegions");
			if (pEndRegionsNode)
			{
				for (xml_node<> *pNode = pEndRegionsNode->first_node("EndRegion"); pNode; pNode = pNode->next_sibling("EndRegion"))
				{
					itemID = -1;
					sprintf(label, "");

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						itemID = atoi(pAttr->value());

					

					pAttr = pNode->first_attribute("label");
					if (pAttr)
						sprintf(label, "%s", pAttr->value());

					endRegion = (EndRegion*)malloc(sizeof(EndRegion));
					endRegion->last = !objMap->bEndPos;
					

					if (objMap->bEndPos)
						endRegion->next = objMap->endRegionHead;
					else
						endRegion->next = NULL;
					objMap->bEndPos = true;

					endRegion->index = itemID;
					sprintf(endRegion->label, label);

					endRegion->xmin = 0;
					endRegion->xmax = 0;
					endRegion->zmin = 0;
					endRegion->zmax = 0;
					endRegion->height = 2;
					endRegion->offset = 0;
					endRegion->returnValue = 0;
					endRegion->mode = 0;
					endRegion->pointThreshold = 0;
					endRegion->moveToPos = 0;
					sprintf(endRegion->messageText, "");
					
					float x1 = 0, x2 = 0, z1 = 0, z2 = 0, y1 = -1, y2 = 1;
					bool endRegionError = false;

					pSubNode = pNode->first_node("MzCoord");

					if (pSubNode)
					{
						
						pAttr = pSubNode->first_attribute("x1");
						if (pAttr)
							x1 = atof(pAttr->value());
						else
							endRegionError = true;

						pAttr = pSubNode->first_attribute("x2");
						if (pAttr)
							x2 = atof(pAttr->value());
						else
							endRegionError = true;

						pAttr = pSubNode->first_attribute("z1");
						if (pAttr)
							z1 = atof(pAttr->value());
						else
							endRegionError = true;

						pAttr = pSubNode->first_attribute("z2");
						if (pAttr)
							z2 = atof(pAttr->value());
						else
							endRegionError = true;

						pAttr = pSubNode->first_attribute("y1");
						if (pAttr)
							y1 = atof(pAttr->value());
						else
							endRegionError = true;

						pAttr = pSubNode->first_attribute("y2");
						if (pAttr)
							y2 = atof(pAttr->value());
						else
							endRegionError = true;
					}
					else
						endRegionError = true;

					endRegion->xmin = min(x1, x2);
					endRegion->xmax = max(x1, x2);
					endRegion->zmin = min(z1, z2);
					endRegion->zmax = max(z1, z2);
					endRegion->height = y2 - y1;
					endRegion->offset = (y2 + y1) / 2.0f;

					pSubNode = pNode->first_node("Action");

					if (pSubNode)
					{

						pAttr = pSubNode->first_attribute("returnValue");
						if (pAttr)
							endRegion->returnValue = atoi(pAttr->value());

						pAttr = pSubNode->first_attribute("pointThreshold");
						if (pAttr)
							endRegion->pointThreshold = atoi(pAttr->value());

						pAttr = pSubNode->first_attribute("moveToPos");
						if (pAttr)
							endRegion->moveToPos = atoi(pAttr->value());

						pAttr = pSubNode->first_attribute("successMessage");
						if (pAttr)
							strcpy_s(endRegion->messageText, pAttr->value());
					}

					if (endRegionError)
					{
						GUIMessageBox("End Region not correctly defined 0,0", 0, TEXTBOXSTYLE_WARNING);
					}

					objMap->endRegionHead = endRegion;
				}
			}
			xml_node<> *pActiveRegionsNode = pMazeItemsNode->first_node("ActiveRegions");
			if (pActiveRegionsNode)
			{
				for (xml_node<> *pNode = pActiveRegionsNode->first_node("ActiveRegion"); pNode; pNode = pNode->next_sibling("ActiveRegion"))
				{
					itemID = -1;
					sprintf(label, "");

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						itemID = atoi(pAttr->value());

					pAttr = pNode->first_attribute("label");
					if (pAttr)
						sprintf(label, "%s", pAttr->value());

					activeRegion = (ActiveRegion*)malloc(sizeof(ActiveRegion));


					activeRegion->last = (objMap->activeRegionHead==NULL);
					if (objMap->activeRegionHead)
						activeRegion->next = objMap->activeRegionHead;
					else
						activeRegion->next = NULL;

					activeRegion->index = itemID;
					sprintf(activeRegion->label, label);

					activeRegion->xmin = 0;
					activeRegion->xmax = 0;
					activeRegion->zmin = 0;
					activeRegion->zmax = 0;
					activeRegion->height = 2;
					activeRegion->offset = 0;
					activeRegion->returnValue = 0;
					activeRegion->mode = 0;
					activeRegion->pointThreshold = 0;
					activeRegion->moveToPos = 0;
					sprintf(activeRegion->messageText, "");

					bool activeRegionError = false;

					pSubNode = pNode->first_node("MzCoord");
					float x1 = 0, x2 = 0, z1 = 0, z2 = 0, y1 = -1, y2 = 1;

					if (pSubNode)
					{
						
						pAttr = pSubNode->first_attribute("x1");
						if (pAttr)
							x1 = atof(pAttr->value());
						else
							activeRegionError = true;

						pAttr = pSubNode->first_attribute("x2");
						if (pAttr)
							x2 = atof(pAttr->value());
						else
							activeRegionError = true;

						pAttr = pSubNode->first_attribute("z1");
						if (pAttr)
							z1 = atof(pAttr->value());
						else
							activeRegionError = true;

						pAttr = pSubNode->first_attribute("z2");
						if (pAttr)
							z2 = atof(pAttr->value());
						else
							activeRegionError = true;

						pAttr = pSubNode->first_attribute("y1");
						if (pAttr)
							y1 = atof(pAttr->value());
						else
							activeRegionError = true;

						pAttr = pSubNode->first_attribute("y2");
						if (pAttr)
							y2 = atof(pAttr->value());
						else
							activeRegionError = true;
					}
					else
					{
						activeRegionError = true;
					}

					activeRegion->xmin = min(x1, x2);
					activeRegion->xmax = max(x1, x2);
					activeRegion->zmin = min(z1, z2);
					activeRegion->zmax = max(z1, z2);
					activeRegion->height = y2 - y1;
					activeRegion->offset = (y2 + y1) / 2.0f;
			
					activeRegion->p1TriggerTime = 0;
					activeRegion->p2TriggerTime = 0;
					activeRegion->pointsGranted = 0;
					activeRegion->p1PointThreshold = 0;
					activeRegion->p2PointThreshold = 0;
					activeRegion->p2InteractRequired = false;
					activeRegion->activatedObjectID = -1;

					activeRegion->highlightAudioID = -1;
					activeRegion->triggerAudioID = -1;
					activeRegion->highlightAudioLoop = false;
					activeRegion->triggerAudioLoop = false;
					activeRegion->highlightAudioBehavior = 0;
					activeRegion->highlighted = false;
					activeRegion->activated = false;

					pSubNode = pNode->first_node("Phase1Highlight");

					if (pSubNode)
					{
						pAttr = pSubNode->first_attribute("pointThreshold");
						if (pAttr)
							activeRegion->p1PointThreshold = atoi(pAttr->value());

						pAttr = pSubNode->first_attribute("timeElapsedThreshold");
						if (pAttr)
							activeRegion->p1TriggerTime = 1000.0f* atoi(pAttr->value());


						xml_node<> *pSubSubNode = pSubNode->first_node("Audio");
						if (pSubSubNode)
						{
							pAttr = pSubSubNode->first_attribute("id");
							if (pAttr)
								activeRegion->highlightAudioID = atoi(pAttr->value());

							pAttr = pSubSubNode->first_attribute("unhighlightBehavior");
							if (pAttr)
							{
								if (strcmp(pAttr->value(), "Stop") == 0)
									activeRegion->highlightAudioBehavior = 0;
								if (strcmp(pAttr->value(), "Pause") == 0)
									activeRegion->highlightAudioBehavior = 1;
								if (strcmp(pAttr->value(), "Continue") == 0)
									activeRegion->highlightAudioBehavior = 2;
								if (strcmp(pAttr->value(), "VolumeByDistance") == 0)
									activeRegion->highlightAudioBehavior = 3;

							}

							pAttr = pSubSubNode->first_attribute("loop");

							if (pAttr)
								activeRegion->highlightAudioLoop = trueFalse(pAttr->value(), false);

						}
					}

					pSubNode = pNode->first_node("Phase2Event");

					if (pSubNode)
					{

						pAttr = pSubNode->first_attribute("returnValue");
						if (pAttr)
							activeRegion->returnValue = atoi(pAttr->value());

						pAttr = pSubNode->first_attribute("pointThreshold");
						if (pAttr)
							activeRegion->p2PointThreshold = atoi(pAttr->value());

						pAttr = pSubNode->first_attribute("pointsGranted");
						if (pAttr)
							activeRegion->pointsGranted = atoi(pAttr->value());

						pAttr = pSubNode->first_attribute("moveToPos");
						if (pAttr)
							activeRegion->moveToPos = atoi(pAttr->value());

						pAttr = pSubNode->first_attribute("activatedObjectID");
						if (pAttr)
							activeRegion->activatedObjectID = atoi(pAttr->value());

						pAttr = pSubNode->first_attribute("activatedMessageText");
						if (pAttr)
							strcpy_s(activeRegion->messageText, pAttr->value());

						pAttr = pSubNode->first_attribute("interactRequired");
						if (pAttr)
							activeRegion->p2InteractRequired = trueFalse(pAttr->value(), false);

						pAttr = pSubNode->first_attribute("highlightTimeElapsedThreshold");
						if (pAttr)
							activeRegion->p2TriggerTime = 1000.0f* atof(pAttr->value());

						xml_node<> *pSubSubNode = pSubNode->first_node("Audio");
						if (pSubSubNode)
						{
							pAttr = pSubSubNode->first_attribute("id");
							if (pAttr)
								activeRegion->triggerAudioID = atoi(pAttr->value());

							pAttr = pSubSubNode->first_attribute("audioBehavior");
							if (pAttr)
								activeRegion->triggerAudioBehavior = atoi(pAttr->value());

							pAttr = pSubSubNode->first_attribute("loop");
							if (pAttr)
								activeRegion->triggerAudioLoop = trueFalse(pAttr->value(), false);

						}
					}

					if (activeRegionError)
					{
						GUIMessageBox("Active Region not correctly defined 0,0", 0, TEXTBOXSTYLE_WARNING);
					}

					objMap->activeRegionHead = activeRegion;
				}
			}
			xml_node<> *pLightsNode = pMazeItemsNode->first_node("Lights");
			if (pLightsNode)
			{

				for (xml_node<> *pNode = pLightsNode->first_node("Light"); pNode; pNode = pNode->next_sibling("Light"))
				{	
					itemID = -1;
					sprintf(label, "");

					pAttr = pNode->first_attribute("id");
					if (pAttr)
						itemID = atoi(pAttr->value());
	
					pAttr = pNode->first_attribute("label");
					if (pAttr)
						sprintf(label, "%s", pAttr->value());
						

					if (clearDefaultLights)
					{
						clearDefaultLights = false;
						curLightList.Clear();
					}
					curLightList.AddLight();

					float x = 0, y = 0, z = 0, intensity = 1;
					float r = 1, g = 1, b = 1; //Diffuse light
					float ar = 1, ag = 1, ab = 1; //Ambient Light
					float attenuation = 0.08;
					int lightType = 1; //1 = Normal, 0= Ambulatory (torch)

					pSubNode = pNode->first_node("Appearance");

					if (pSubNode)
					{

						pAttr = pSubNode->first_attribute("attenuation");
						if (pAttr)
							attenuation = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("intensity");
						if (pAttr)
							intensity = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("type");
						if (pAttr)
						{
							if (strcmp(pAttr->value(), "Stationary") == 0)
								lightType = 1;
							else
								lightType = 0;
						}
					}


					pSubNode = pNode->first_node("MzPoint");

					if (pSubNode)
					{

						pAttr = pSubNode->first_attribute("x");
						if (pAttr)
							x = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("y");
						if (pAttr)
							y = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("z");
						if (pAttr)
							z = atof(pAttr->value());
					}

					if (lightType)
						curLightList.AddPositionToLast(x, y, z, intensity);
					else
						curLightList.AddPositionToLast(0, 0, 0, 0);

					curLightList.AddAttenToLast(0, attenuation);

					pSubNode = pNode->first_node("Color");

					if (pSubNode)
					{

						pAttr = pSubNode->first_attribute("r");
						if (pAttr)
							r = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("g");
						if (pAttr)
							g = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("b");
						if (pAttr)
							b = atof(pAttr->value());


					}
					curLightList.AddDiffuseToLast(r, g, b, intensity);

					pSubNode = pNode->first_node("AmbientColor");


					if (pSubNode)
					{

						pAttr = pSubNode->first_attribute("r");
						if (pAttr)
							ar = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("g");
						if (pAttr)
							ag = atof(pAttr->value());

						pAttr = pSubNode->first_attribute("b");
						if (pAttr)
							ab = atof(pAttr->value());


					}

					curLightList.AddAmbientToLast(ar, ag, ab, 0.1);
					n = GL_LIGHT0 + curLightList.GetLength() - 1;

					lightItem = curLightList.GetLast();

					lightItem->index = n;


				}
			}
		}

		doc.clear();

	}
	catch (const rapidxml::parse_error& e)
	{
		char* err = new char[256];
		sprintf(err, "Corrupted MazeXML File:\n%s\n\n%s", e.what());
		GUIMessageBox(err, 0, TEXTBOXSTYLE_WARNING);
		
		return 0;
	}
	
	curTextureDict.ClearUnused();

	objMap->lights = lights;
	objMap->shaders = objectsShader;
	objMap->shadersCompiled = shadersCompiled;
	updateTime2 = GetQPC();
	updateTime3 = GetQPC();

	objCamera.startPos.x = 2;
	objCamera.startPos.y = 0;
	objCamera.startPos.z = 2;
	objCamera.Position_Camera(objCamera.startPos.x, objCamera.startPos.y, objCamera.startPos.z, 0, 0, 0, 0, 1, 0);
	if (objMap->bStartPos &&objMap->startPosHead) //starting position exists
	{
		startPos = objMap->startPosHead;

		if (mazeInputValue != -1) //if specified by maze Input value, use that instead
			defaultStartPosID = mazeInputValue;

		startPos = getStartPosByIndex(defaultStartPosID); //will give random if 0, head if negative, otherwise as specified
		if (startPos == NULL)
		{
			startPos = objMap->startPosHead;
			objMap->startPosDefault = objMap->startPosHead;
		}
		else
		{
			objMap->startPosDefault = startPos;
			objCamera.startPos.x = startPos->x;
			objCamera.startPos.y = startPos->y;
			objCamera.startPos.z = startPos->z;
			objCamera.Position_Camera(objCamera.startPos.x, objCamera.startPos.y , objCamera.startPos.z, 0, 0, 0, 0, 1, 0);
			objCamera.InitialRotate(startPos->angle*PI / 180, xLoc + Width / 2, yLoc + Height / 2, startPos->vertAngle);
		}
	}
	else
	{
		objCamera.InitialRotate(0, xLoc + Width / 2, yLoc + Height / 2, 0);
		GUIMessageBox("No Start Position Specified",0, TEXTBOXSTYLE_WARNING);
	}

	objCamera.startView = objCamera.mView;
	Player.offset = Player.defaultCameraOffset;
	objCamera.MoveSpeedDefault();

	bMouseInput = bMouseInputDisabled; //restore mouse input to original settings


	if (objMap->wallCount <= 0)
	{
		GUIMessageBox("Maze has no Listed walls or objects\nMaze will now Exit", 0, TEXTBOXSTYLE_FATAL_ERROR);
		return 0;
	}
		

	///////////////////
	//Move to Map BuildCollision()
	InitBulletPhysics();

	updateMazeWorkingDir("");

	return 1;
}



int ReadMap(char* theFile)
{
	updateMazeWorkingDir("");

	char* bestPath=getBestPath(theFile,"Mazes");
	if (bestPath)
	{
		strncpy(curMazeFilename, bestPath,_MAX_PATH-1); //try to find best path for mazefile
	}
	else
	{
		char message[900];
		sprintf(message, "Couldn't open Maze file!\n%s", theFile);
		if (strlen(theFile) > 300)
			sprintf(message, "Couldn't open Maze file!\nMazeList Error");
		GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
		return 0;
	}
	updateMazeWorkingDir(getFileDir(curMazeFilename));


	firstLog=false;
	srand(GetQPC()*time(NULL)); //seed based on time
	
	char* ext = new char[500];

	ext = getFileExt(curMazeFilename);
	if (strcmp(ext, ".xml") == 0)
	{
		return(ReadMapXML(curMazeFilename));
	}


	FILE* fp = fopen(curMazeFilename, "r");
	if (fp == NULL)
	{
		char message[900];
		sprintf(message, "Couldn't open Maze file!\n%s", theFile);
		if (strlen(theFile) > 300)
			sprintf(message, "Couldn't open Maze file!\nMazeList Error");
		GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
		return 0;
	}

	float inp1, inp2, inp3, inp4, inp5, inp6, inp7;
	char linebuf[LINEBUFFERLIMIT];
	

	NULL != fgets(linebuf, LINEBUFFERLIMIT, fp);
	if (1 != sscanf_s(linebuf, "Maze File %f\n", &inp1))
	{
		sscanf_s(linebuf, "<?xml version=\"%f\" encoding=\"UTF - 8\"?>", &inp1);
		if (inp1 > 0)
		{
			fclose(fp);
			return ReadMapXML(curMazeFilename);
		}
		else
		{
			if (strlen(linebuf) > 4) // Check for BOM character
			{
				std::string lineBuf_noBOM= std::string(linebuf,LINEBUFFERLIMIT);
				lineBuf_noBOM.erase(0, 3);

				sscanf_s(lineBuf_noBOM.c_str(), "<?xml version=\"%f\" encoding=\"UTF - 8\"?>", &inp1);
				if (inp1 > 0)
				{
					fclose(fp);
					return ReadMapXML(curMazeFilename);
				}
			}
		}
		fclose(fp);

		char message[800];
		sprintf(message, "Corrupted or old maze file.\nTry opening and saving with MazeMaker\n%s", theFile);
		GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
		return 0;
	}


	int texture,type;
	int res=0;
	int bMouseInputDisabled=bMouseInput;//disable mouse input for loading
	bMouseInput=false;
	bool defaultLights=true;
	
	bool newLightType=false;
	int curVisibility;   //0-invisible, 1-visible
	curVisibility=1;


	MazeInit();

	curTextureDict.MarkAllUnused();
	////////////
	
	 //to record the directory of the current maze file
	
	


	double mzFileEdition=inp1;
	if(mzFileEdition>(1.20001))
	{
		char message[800];
		sprintf(message,"Maze version is newer than MazeWalker\nPlease Update");
		GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
	}
	

	struct SColor col;
	struct SVertex v[4];
	LightItem* lightItem=NULL;

	
	SetCurrentDirectoryA(getMazeWorkingDir());

	int index=0,counter=0;
	UINT i;
	char fname[LINEBUFFERLIMIT]; 
	fname[0]=0;
	int n,elementLineCount; 
	linenumber=1;
	bool foundNextItem=false;

	GLMmodelNode *temp2=NULL;
	MapModel *temp=NULL;
	objMap->bEndPos=false;
	objMap->bStartPos=false;

	StartPosition* startPos=NULL;
	startPos = (StartPosition*)malloc(sizeof(StartPosition));
	startPos->angle=0;
	startPos->vertAngle = 0;


	//while(fscanf_s(fp,"%f\t%f\n",&inp1,&inp2)>=1)
	char* itemComment= new char[256];
	bool hasComment;

	while(NULL!=fgets(linebuf,LINEBUFFERLIMIT,fp))
	{
		if( 1 > sscanf_s(linebuf,"%f\t%f\n",&inp1,&inp2) )
			break;
		type=inp1;
		elementLineCount=inp2;

		hasComment=false;
		for(res=0,n=0;n<LINEBUFFERLIMIT-1;n++)
		{
			if(linebuf[n]=='\t')
				res++;
			if(linebuf[n]=='\n'||linebuf[n]==0)
				break;
			if(res==2&&!hasComment)
			{
				sscanf(&linebuf[n],"\t%s\n",itemComment);
				hasComment=true;
				
			}
			else if(res==3&&hasComment)
			{
				sscanf(&linebuf[n],"\t%s\n",itemComment);
				hasComment=true;
				break;
			}
		}
		
				////

		linenumber++;
		switch(type)
		{
			case MO_PLANE:			

				/*Old Format (1.10)
				0	6	0						MO_Plane NumLines ID
				0	1	1	1					RGB Color
				1	1	0	1	15				tex_X1	tex_Y1	X1	Y1	Z1
				1	0	0	-1	15				tex_X2	tex_Y2	X2	Y2	Z2
				0	0	15	-1	15				tex_X3	tex_Y3	X3	Y3	Z3
				0	1	15	1	15				tex_X4	tex_Y4	X4	Y4	Z4
				0	0	0	0	0				0	0	CurVisible?	0	0
				*/

				//res=fscanf(fp,"%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4);
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;
				texture=inp1; col.r=inp2; col.g=inp3; col.b=inp4;
				if(texture==-1)
				{
					curVisibility=0;
					texture=0;
				}
				else
					curVisibility=1;

				//res=fscanf(fp,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,5,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(res!=5) {ReportError(theFile,5,res); return 0;}
				else linenumber++;
				v[0].tex_x=inp1; v[0].tex_y=inp2; v[0].x=inp3; v[0].y=inp4; v[0].z=inp5;
			
				//res=fscanf(fp,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,5,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(res!=5) {ReportError(theFile,5,res); return 0;}
				else linenumber++;
				v[1].tex_x=inp1; v[1].tex_y=inp2; v[1].x=inp3; v[1].y=inp4; v[1].z=inp5;
			
				//res=fscanf(fp,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,5,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(res!=5) {ReportError(theFile,5,res); return 0;}
				else linenumber++;
				v[2].tex_x=inp1; v[2].tex_y=inp2; v[2].x=inp3; v[2].y=inp4; v[2].z=inp5;
				
				//res=fscanf(fp,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,5,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(res!=5) {ReportError(theFile,5,res); return 0;}
				else linenumber++;
				v[3].tex_x=inp1; v[3].tex_y=inp2; v[3].x=inp3; v[3].y=inp4; v[3].z=inp5;

				
				if(elementLineCount>=6)
				{
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,5,0); return 0;}
					res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
					if(res!=5) {ReportError(theFile,5,res); return 0;}
					else linenumber++;

					if(inp3>=0)
						curVisibility=1;
					else
						curVisibility=0;
				}

				//ASSERT(objMap);
				if(objMap->Add(type,texture,col,v,curVisibility)==0)
				{
					GUIMessageBox("Error with planes!!!\n\nMAZE couldn't load plane to memory!", 0, TEXTBOXSTYLE_WARNING); return 0;
				}
			

				break;
			case MO_TRI:
				//res=fscanf(fp,"%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4);
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;
				texture=inp1; col.r=inp2; col.g=inp3; col.b=inp4;

				//res=fscanf(fp,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,5,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(res!=5) {ReportError(theFile,5,res); return 0;}
				else linenumber++;
				v[0].tex_x=inp1; v[0].tex_y=inp2; v[0].x=inp3; v[0].y=inp4; v[0].z=inp5;
			
				//res=fscanf(fp,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,5,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(res!=5) {ReportError(theFile,5,res); return 0;}
				else linenumber++;
				v[1].tex_x=inp1; v[1].tex_y=inp2; v[1].x=inp3; v[1].y=inp4; v[1].z=inp5;
			
				//res=fscanf(fp,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,5,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4,&inp5);
				if(res!=5) {ReportError(theFile,5,res); return 0;}
				else linenumber++;
				v[2].tex_x=inp1; v[2].tex_y=inp2; v[2].x=inp3; v[2].y=inp4; v[2].z=inp5;

				//ASSERT(objMap);
				if(objMap->Add(type,texture,col,v,1)==0)
				{
					GUIMessageBox("Error with triangles!!\n\nMAZE couldn't be loaded to memory!", 0, TEXTBOXSTYLE_WARNING); return 0;
				}

				break;
			case MO_INITIALPOS:
				{

				/*Current Format
				10	1	12	180 45 5					X Y Z ViewAngle VerticalViewAngle ManualIndex

				*/
				 
				//res=fscanf(fp,"%f\t%f\t%f\n",&inp1,&inp2,&inp3);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4, &inp5, &inp6); //grab XYZ coordinates + viewAngle + Index Number
				if(res!=4 && res!=6) {ReportError(theFile,6,res); return 0;}
				else linenumber++;
				
				if(!objMap->bStartPos)
				{
					objMap->bStartPos=true;
					startPos->last=true;
				}
				else
				{
					startPos->last=false; //head is not last startPos
					startPos->index=-1;
				}
				int lastIndex=startPos->index;

				startPos->next=objMap->startPosHead;
				startPos->x=inp1;
				startPos->y=inp2;
				startPos->z=inp3;
				startPos->angle=inp4;
				startPos->vertAngle = inp5*PI/180;
				startPos->inputValue=0; //initializing value
				startPos->index=lastIndex+1; //auto-index startPos

				//Process RandomViewAngle and StartPosIndex
				if(res==6)
				{
					if ((inp4==-999))
					{
						startPos->angle=rand() /(RAND_MAX/360.0); //int from 0 to 359
						EventLog(0,130,startPos->angle,"InitAngle");
						
					}

					if ((inp5 == -999))
					{
						startPos->vertAngle = (rand() / (RAND_MAX / 360.0)-180) *PI /180; //int from 0 to 359
						EventLog(0, 131, startPos->vertAngle, "InitVertAngle");
					}
					
					if(inp6>10)
						startPos->index=inp6; //override reauto-index (could cause issues if manual index doesn't agree so added +10)
				}

				

				if(elementLineCount==2)
				{
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,1,0); return 0;}
					res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1);
					if(res!=1) {ReportError(theFile,1,res); return 0;}
					else linenumber++;
					startPos->inputValue=inp1;
				}
				objMap->startPosHead=startPos;
				break;
				}
			case MO_SPEED:
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,2,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\n",&inp1,&inp2);
				if(res!=2) {ReportError(theFile,2,res); return 0;}
				else linenumber++;
				//objCamera.cameraSpeedMove = inp1*10;
				//objCamera.cameraSpeedMultiply = inp2/0.1f;
				// DISABLED AFTER XML RELEASE
				break;
			case MO_ENDPOS:
				//res=fscanf(fp,"%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4);
				if(elementLineCount==1)
				{
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,4,0); return 0;}
					res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
					if(res!=4) {ReportError(theFile,4,res); return 0;}
					else linenumber++;
					//ASSERT(objMap);
					EndRegion *endRegion;
					endRegion = (EndRegion*)malloc(sizeof(EndRegion));
					endRegion->last = !objMap->bEndPos;
					if (objMap->bEndPos)
						endRegion->next = objMap->endRegionHead;
					else
						endRegion->next = NULL;
					objMap->bEndPos = true;
					
					endRegion->xmin=inp1;
					endRegion->xmax=inp2;
					endRegion->zmin=inp3;
					endRegion->zmax=inp4;
					endRegion->height=2;
					endRegion->offset=0;
					endRegion->returnValue=0;
					endRegion->mode=0;
					endRegion->pointThreshold = 0;
					endRegion->moveToPos = 0;
					
					for(res=0,n=0;n<LINEBUFFERLIMIT-1;n++)
					{
						if(linebuf[n]=='\t')
							res++;
						if(res==4)
							break;
					}
					for(res=n+1;res<LINEBUFFERLIMIT;res++)
						if(linebuf[res]=='\n')
							linebuf[res]=NULL;
					strcpy_s(endRegion->messageText,&(linebuf[n+1]));
					objMap->endRegionHead=endRegion;
				}
				if(elementLineCount==2)
				{
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,4,0); return 0;}
					res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
					if(res!=4) {ReportError(theFile,4,res); return 0;}
					else linenumber++;
					//ASSERT(objMap);
					EndRegion *endRegion;
					endRegion = (EndRegion*)malloc(sizeof(EndRegion));
					endRegion->last = !objMap->bEndPos;
					if (objMap->bEndPos)
						endRegion->next = objMap->endRegionHead;
					else
						endRegion->next = NULL;
					objMap->bEndPos = true;


					endRegion->xmin=inp1;
					endRegion->xmax=inp2;
					endRegion->zmin=inp3;
					endRegion->zmax=inp4;
					endRegion->moveToPos = 0;
					for(res=0,n=0;n<LINEBUFFERLIMIT-1;n++)
					{
						if(linebuf[n]=='\t')
							res++;
						if(res==4)
							break;
					}
					for(res=n+1;res<LINEBUFFERLIMIT;res++)
						if(linebuf[res]=='\n')
							linebuf[res]=NULL;
					strcpy_s(endRegion->messageText,&(linebuf[n+1]));
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,4,0); return 0;}
					res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
					if(res!=4) {ReportError(theFile,4,res); return 0;}
					else linenumber++;
					endRegion->height=inp1;
					endRegion->offset=inp2;
					endRegion->returnValue=inp3;
					endRegion->mode=inp4;
					objMap->endRegionHead=endRegion;
				}
				break;
			case MO_TIMELIMIT:
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,1,0); return 0;}
				res=sscanf_s(linebuf,"%f\t",&inp1);
				if(res!=1) {ReportError(theFile,1,res); return 0;}
				else linenumber++;
				objMap->maxMazeTime = inp1;// 100;
				for(res=0,n=0;n<LINEBUFFERLIMIT-1;n++)
				{
					if(linebuf[n]=='\t'&&n!=0)
						res++;
					if(res==1)
						break;
				}
				for(res=n+1;res<LINEBUFFERLIMIT;res++)
					if(linebuf[res]=='\n')
						linebuf[res]=NULL;
				strcpy_s(objMap->timeoutMessage,&(linebuf[n+1]));
				break;
			case MO_STARTMESSAGE:
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,1,0); return 0;}
				res=sscanf_s(linebuf,"%f\t",&inp1);
				if(res!=1) {ReportError(theFile,1,res); return 0;}
				else linenumber++;
				objMap->startMessageEnabled=(inp1!=0);
				for(res=0,n=0;n<LINEBUFFERLIMIT-1;n++)
				{
					if(linebuf[n]=='\t'&&n!=0)
						res++;
					if(res==1)
						break;
				}
				for(res=n+1;res<LINEBUFFERLIMIT;res++)
					if(linebuf[res]=='\n')
						linebuf[res]=NULL;
				strcpy_s(objMap->startMessage,&(linebuf[n+1]));
				break;
			case MO_DESIGNER:
				fname[0]=0; 
				n=0;
				while(1)
				{					
					fread(&(fname[n]),1,1,fp);
					if( fname[n]=='\n' || fname[n]==EOF || fname[n]=='\r')
						break;
					n++;
				}
				fname[n]=0;
				linenumber++;
				//if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				//{ReportError(a.m_ofn.lpstrFile,1,0); return ;}
				break;
			case MO_COMMENTS:
				fname[0]=0; 
				n=0;
				while(1)
				{				
					fread(&(fname[n]),1,1,fp);
					if( fname[n]=='\n' || fname[n]==EOF || fname[n]=='\r')
						break;
					n++;
				}
				fname[n]=0;
				linenumber++;
				//if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				//{ReportError(a.m_ofn.lpstrFile,1,0); return ;}
				break;
			case MO_SCALE:
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,1,0); return 0;}
				res=sscanf_s(linebuf,"%f\n",&inp1);
				if(res!=1) {ReportError(theFile,1,res); return 0;}
				else linenumber++;				
				//TODO: use inp to set scale!!!
				break;
			case MO_SKYBOX:
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,1,0); return 0;}
				res=sscanf_s(linebuf,"%f\n",&inp1);
				if(res!=1) {ReportError(theFile,1,res); return 0;}
				else linenumber++;				
				if(inp1>0)
					skyboxID=inp1;
				else
					skyboxID=0;
				break;
			case MO_AMBIENT:
				{
				//res=fscanf(fp,"%f\t%f\t%f\n",&inp1,&inp2,&inp3);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4); //grab RGB colors &intensity
				if(res!=4 ) {ReportError(theFile,4,res); return 0;}
				else linenumber++;

				float vAmbientLightBright[4] = {inp1*inp4, inp2*inp4, inp3*inp4, 1};
				glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vAmbientLightBright);
				break;
				}
			case MO_LIGHT:
				//glDisable(GL_LIGHT0);  //disable the default light
				if(inp2==4)
					newLightType=true;
				if(defaultLights)
				{
					defaultLights=false;
					curLightList.Clear();
				}
				curLightList.AddLight();

				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp)) //position
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;
				curLightList.AddPositionToLast(inp1,inp2,inp3,inp4);

				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))	//ambient
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;

				curLightList.AddAmbientToLast(inp1,inp2,inp3,inp4);

				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp)) //diffuse
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;

				curLightList.AddDiffuseToLast(inp1,inp2,inp3,inp4);

				n = GL_LIGHT0 + curLightList.GetLength() -1;
				
				lightItem = curLightList.GetLast();

				lightItem->index=n;
				inp1=0;

				if(newLightType)
				{
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,1,0); return 0;}
					res=sscanf_s(linebuf,"%f",&inp1);
					if(res!=1) {ReportError(theFile,1,res); return 0;}
					else linenumber++;
				}
				if(inp1>0)
					curLightList.AddAttenToLast(0,inp1);
				else
					curLightList.AddAttenToLast(0,0.08f);

				break;


			case MO_TEXTURE:
				//memset(TextureImage,0,sizeof(void *)*TEXT_NUM);           	// Set The Pointer To NULL				
				while(fscanf_s(fp,"\t%d\t",&index)==1)
				{
					if(index==-1) break;
					int n=7;
						strcpy_s(fname,"texture\\");
						////if(index>=TEXT_NUM || index<0){ fclose(fp); return false;}
						while(fname[n]!='\n' && fname[n]!=EOF && fname[n]!='\r')
						{
							n++;
							fread(&(fname[n]),1,1,fp);
						}
						linenumber++;
						fname[n]=0;	



					LoadTexture(fname,index);
					
		
				}
				linenumber++;
				break;



			case MO_AUDIO_LIST:
					while(fscanf_s(fp,"\t%d\t",&index)==1) //searches for /t AudioID /t
					{
						char soundSTR[256];
						if(index==-1) break;
						n=-1;
						while( n<0 || (fname[n]!='\n' && fname[n]!=EOF && fname[n]!='\r'))
						{
							n++;
							fread(&(fname[n]),1,1,fp);
						}
						linenumber++;
						fname[n]=0; //terminates the string
						if(fname[n-1]==' ')  //adding a space skips loading
						{
							sprintf(soundSTR,"");
						}
						else
						{
							char* soundPath;

							soundPath=getBestPath(fname, "audio");

							if (soundPath)
							{
								curAudioDict.Add(index, soundPath);
							}
							else
							{
								char txt[250];
								sprintf(txt,"Audio '%s' not found!\nLine: %i", fname,linenumber);
								GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING); //return 0;
							}
						
						}
					}
					linenumber++;
				break;
			case MO_MODEL_LIST:
				GLMmodel* model;

				while(fscanf_s(fp,"\t%d\t",&index)==1)
				{
					//if(modelCount==2)
					if(counter++>MAX_TEXTURES) break;
					if(index==-1) break;
					n=-1;
					////if(index>=TEXT_NUM || index<0){ fclose(fp); return false;}
					while(n<0 || (fname[n]!='\n' && fname[n]!=EOF && fname[n]!='\r'))
					{
						n++;
						fread(&(fname[n]),1,1,fp);
					}
					linenumber++;
					fname[n]=0;					


					if(fname[n-1]==' ')
					{
						//this model is not going to be used!
						model=NULL;
					}
					else
					{					
						char* modelPathP;
						char modelDir[800] = "";

						char curDir[_MAX_PATH] = "";
						GetCurrentDirectory(MAX_PATH, curDir);

						char modelPath[_MAX_PATH];

						modelPathP = getBestPath(fname, "objs");


						if (modelPathP)
						{
							strncpy(modelPath, modelPathP, _MAX_PATH - 1);
							strcpy(modelDir, getFileDir(modelPath));

							SetCurrentDirectory(modelDir);
							model = LoadModel(modelPath);
							SetCurrentDirectory(curDir);
						}
						else
						{
							model=NULL;
						}	

						if (model!=NULL)		//if model was loaded
						{
							GLMmodelNode *temp;             //create a temporary node 
							temp = (GLMmodelNode*)malloc(sizeof(GLMmodelNode)); //allocate space for node 
							temp->modelData=model;			//assign model to node2
							temp->modelID=index;			//use index as modelID
							temp->next=ModelDictHead;				//assaign next address to temp
							ModelDictHead = temp;					//assign head to temp
							objMap->numObjs++;					//increment total .obj count

							temp->MaxCoords.x = temp->MinCoords.x = temp->modelData->vertices[3 + 0];
							temp->MaxCoords.y = temp->MinCoords.y = temp->modelData->vertices[3 + 1];
							temp->MaxCoords.z = temp->MinCoords.z = temp->modelData->vertices[3 + 2];
							for (i = 1; i < temp->modelData->numvertices; i++) {
								if (temp->MaxCoords.x < temp->modelData->vertices[3 * i + 0])
									temp->MaxCoords.x = temp->modelData->vertices[3 * i + 0];
								if (temp->MinCoords.x > temp->modelData->vertices[3 * i + 0])
									temp->MinCoords.x = temp->modelData->vertices[3 * i + 0];

								if (temp->MaxCoords.y < temp->modelData->vertices[3 * i + 1])
									temp->MaxCoords.y = temp->modelData->vertices[3 * i + 1];
								if (temp->MinCoords.y > temp->modelData->vertices[3 * i + 1])
									temp->MinCoords.y = temp->modelData->vertices[3 * i + 1];

								if (temp->MaxCoords.z < temp->modelData->vertices[3 * i + 2])
									temp->MaxCoords.z = temp->modelData->vertices[3 * i + 2];
								if (temp->MinCoords.z > temp->modelData->vertices[3 * i + 2])
									temp->MinCoords.z = temp->modelData->vertices[3 * i + 2];

							}
						}
						else
						{
							//fclose(fp);
							char txt[250];
							sprintf(txt,"Model not found!\n'%s'", fname);
							GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING); //return 0;
						}
					}
				}
				linenumber++;
				break;

		case MO_MODEL:
				/*Old Format (1.10)
					10	8						MO_MODEL NumLines
					-1	38.82	0	4.82		ModelID	XPos	YPos	ZPos
					1	0	0	0				Scale	RotX	RotY	RotZ
					1							Collision
				*/
				/*Current Format (1.20)
					10	8						MO_MODEL NumLines
					-1	38.82	0	4.82		ModelID	XPos	YPos	ZPos
					1	0	0	0				Scale	RotX	RotY	RotZ
					1	1	1.5					Collision KinematicEnabled	Mass
				*/
				//res=fscanf(fp,"%f\t%f\t%f\t%f\n",&inp1,&inp2,&inp3,&inp4);
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;

				
				temp = (MapModel*)malloc(sizeof(MapModel));
				temp->next = MapModelHead;

				
				temp2 = (GLMmodelNode*)malloc(sizeof(GLMmodelNode));
				temp2 = ModelDictHead;
				while(temp2!=NULL&&temp2->modelID!=inp1)
				{temp2=temp2->next;}
				if (temp2==NULL)
				{
					temp2=ModelDictHead;
					if ((temp2==NULL||objMap->numObjs==0))
					{
						char message[50];
						sprintf(message, "No valid Models!\n Model ID %i not loaded\nLine: %i",(int)inp1,linenumber);
						GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
					}
					else
					{
						char message[100];
						sprintf(message,"%f is not a valid Model ID!\nUsing primary model instead\nLine: %i",inp1,linenumber);
						GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
					}//MessageBox(NULL,"","Unkown ModelID",MB_OK | MB_ICONINFORMATION);}
					}
				if (temp2!=NULL) 
				{
					temp->modelID=inp1;
					temp->name=new char[256];
					if(hasComment)
						strcpy(temp->name,itemComment);
					else
						sprintf(temp->name,"S%i",temp->modelID);
					
					temp->modelData=temp2->modelData;
					temp->transparent=temp2->modelData->transparent;
					temp->MinCoords=temp2->MinCoords;
					temp->MaxCoords=temp2->MaxCoords;
					temp->isVisible=true;
					temp->distToPlayer=1;

					temp->pos.x=inp2;
					temp->pos.y=inp3;
					temp->pos.z=inp4;
					MapModelHead=temp;
				}
				
				for(res=0,n=0;n<LINEBUFFERLIMIT-1;n++)
				{
					if(linebuf[n]=='\t')
						res++;
					if(res==4)
						break;
				}
				for(res=n+1;res<LINEBUFFERLIMIT;res++)
					if(linebuf[res]=='\n')
						linebuf[res]=NULL;
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;

				temp->scale=inp1;
				temp->rot.x=inp2;
				
				temp->rot.y=inp3;
				temp->rot.z=inp4;
				
				temp->initRot=temp->rot;
				temp->initPos=temp->pos;

				temp->initScale=temp->scale;

				for(res=0,n=0;n<LINEBUFFERLIMIT-1;n++)
				{
					if(linebuf[n]=='\t')
						res++;
					if(res==4)
						break;
				}
				for(res=n+1;res<LINEBUFFERLIMIT;res++)
					if(linebuf[res]=='\n')
						linebuf[res]=NULL;
						if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
							{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f",&inp1);
				if(res!=1) {ReportError(theFile,1,res); return 0;}
				else linenumber++;
				
				if (inp1==1)
					temp->collide=true;
				else
					temp->collide=false;

				
				for(res=0;res<LINEBUFFERLIMIT;res++)
				{
					if(linebuf[res]=='\t')
					{
						foundNextItem=true;
					}
					if(linebuf[res]=='\n'||linebuf[res]==NULL)
						break;
				}


				if(foundNextItem)
				{
					res=sscanf_s(linebuf,"%f\t%f\t%f\n",&inp1,&inp2,&inp3);
					if(res!=3) {ReportError(theFile,3,res); return 0;}
					else linenumber++;

					if (inp2==1&&temp->collide)
						temp->kinematic=true;
					else
						temp->kinematic=false;
					if(inp3<=0) //mass must be positive
						inp3=0;
					temp->mass= inp3*temp->kinematic;
					
					foundNextItem=false;
				}
				else
				{
					temp->kinematic=false;
					temp->mass=0;
				}
			
				

				glmCalcBounds(temp);
				
				
				//Initialization
				temp->dynamic=false;
				temp->activated=false;
				temp->destroy=false;
				temp->destroyed = false;
				temp->highlighted=false;
				temp->proximityTrigger=0; //0 no proximity,1 highlight proximity, 2 activation proximity
				temp->moving=false;
				temp->switchModel=false;
				temp->currentPercentage=100;
				temp->glowFactor=0;
				temp->glowColor=0;
				temp->highlightStyle=0;
				temp->p1PointThreshold = -1;
				temp->p2PointThreshold = -1;
				temp->pointsGranted = 0;

				

				temp->instanceID=objMap->modelCount;
				objMap->modelCount++;
				
				

				for(res=0,n=0;n<LINEBUFFERLIMIT-1;n++)
				{
					if(linebuf[n]=='\t')
						res++;
					if(res==1)
						break;
				}
				for(res=n+1;res<LINEBUFFERLIMIT;res++)
					if(linebuf[res]=='\n')
						linebuf[res]=NULL;
				break;
			case MO_DYNAMICMODEL:
				/*OldFormat (1.08)
					11	6						MO_DYNAMIC MODEL NumLines
					-1	38.82	0	4.82		ModelID	XPos	YPos	ZPos
					1	0	0	0				Scale	RotX	RotY	RotZ
					1	1	1	1				Collision	triggerCriteria	ActiveRadius	TriggerAction
					1	38.82	0	4.82		EndScale	EndX	EndY	EndZ
					3	0	0	0				ActionTime	EndRotX	EndRotY	EndRotZ
					0	1	30	40				TargetModelID	HighlightStyle
				*/
				/*Current Format (1.10)
					11	8						MO_DYNAMIC MODEL NumLines
					-1	38.82	0	4.82		ModelID	XPos	YPos	ZPos
					1	0	0	0				Scale	RotX	RotY	RotZ
					1							Collision
					1	1	2.5	30	0	1		P1triggerCriteria	P1HighlightStyle	P1ActiveRadius	p1TriggerTime	P1AudioID	P1AudioLoop	P1AudioBehavior
					1	1	1.5	40	0	0		P2triggerCriteria	P2TriggerAction	P2ActiveRadius	p2TriggerTime	P2AudioID	P2AudioLoop	P2AudioBehavior
					1	38.82	0	4.82		EndScale	EndX	EndY	EndZ					//phase 2
					3	0	0	0				ActionTime	EndRotX	EndRotY	EndRotZ					//phase 2
					-1							TargetModelID	
				*/
				/*Current Format (1.20)
					11	8						MO_DYNAMIC MODEL NumLines
					-1	38.82	0	4.82		ModelID	XPos	YPos	ZPos
					1	0	0	0				Scale	RotX	RotY	RotZ
					1	1	1.5					Collision	KinematicEnabled	Mass
					1	1	2.5	30	0	1		P1triggerCriteria	P1HighlightStyle	P1ActiveRadius	p1TriggerTime	P1AudioID	P1AudioLoop	P1AudioBehavior
					1	1	1.5	40	0	0		P2triggerCriteria	P2TriggerAction	P2ActiveRadius	p2TriggerTime	P2AudioID	P2AudioLoop	P2AudioBehavior
					1	38.82	0	4.82		EndScale	EndX	EndY	EndZ					//phase 2
					3	0	0	0				ActionTime	EndRotX	EndRotY	EndRotZ					//phase 2
					-1							TargetModelID	
				*/


				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))  //ModelID	XPos	YPos	ZPos
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;


				temp = (MapModel*)malloc(sizeof(MapModel));
				temp->next = MapModelHead;

				temp2 = (GLMmodelNode*)malloc(sizeof(GLMmodelNode));
				temp2 = ModelDictHead;
				while(temp2!=NULL&&temp2->modelID!=inp1)
				{temp2=temp2->next;}
				if (temp2==NULL)
				{
					temp2=ModelDictHead;
					char message[100];
					
					if ((temp2==NULL||objMap->numObjs==0))
					{
						//GUIMessageBox("Model is improperly defined",0,TEXTBOXSTYLE_ONDIALOG_CLEAR_BK);
						sprintf(message, "No valid Models!\n Model ID %i not loaded\nLine: %i",(int)inp1,linenumber);
						GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
					}
					else
					{
						sprintf(message,"%i is not a valid Model ID!\nUsing primary model instead\nLine: %i",(int)inp1,linenumber);
						GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
					}
					
					
					
				}
				if (temp2!=NULL) 
				{
					
					temp->modelID=inp1;

					temp->modelID=inp1;
					temp->dID=objMap->dModelCount;

					temp->name=new char[256];
					if(hasComment)
						strcpy(temp->name,itemComment);
					else
						sprintf(temp->name,"M%iD%i",temp->modelID,temp->dID);

					
					temp->instanceID=objMap->modelCount;
					objMap->dModelCount++;
					objMap->modelCount++;
					

					temp->modelData=temp2->modelData;
					temp->transparent=temp2->modelData->transparent;

					temp->MinCoords=temp2->MinCoords;
					temp->MaxCoords=temp2->MaxCoords;

					temp->pos.x=inp2;
					temp->pos.y=inp3;
					temp->pos.z=inp4;
					MapModelHead=temp;
				}

				temp->isVisible=true;
				temp->distToPlayer=1;
				
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))		//Scale	RotX	RotY	RotZ
				{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;



				temp->scale=inp1;
				temp->rot.x=inp2;
				
				temp->rot.y=inp3;
				temp->rot.z=inp4;
				
				temp->initRot=temp->rot;
				temp->initPos=temp->pos;

				temp->initScale=temp->scale;
				if(elementLineCount==6) //old version 1.08
				{
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,4,0); return 0;}

					res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4);	//Collision	triggerCriteria	ActiveRadius	TriggerAction
					if(res!=4) {ReportError(theFile,4,res); return 0;}
					else linenumber++;
				
					if (inp1==1)
						temp->collide=true;
					else
						temp->collide=false;


					glmCalcBounds(temp);
				
					

					
					temp->dynamic=true;
					temp->activated=false;
					temp->moving=false;
					temp->proximityTrigger=false;
					temp->destroy=false;
					temp->destroyed = false;
					temp->p1ActiveRadius=inp3;
					temp->p2ActiveRadius=inp3;
					temp->triggerAction=(int)inp4;
					temp->currentPercentage=0;
					temp->targetPercentage=100;
					temp->highlighted=false;
					temp->kinematic=false;
					temp->mass=0;

					temp->highlightCriteria = 1; //proximity only
					temp->triggerCriteria = inp2;

					if (temp->triggerCriteria == 3 || temp->triggerCriteria == 5 || temp->triggerCriteria == 6 || temp->triggerCriteria == 7)
						temp->p2InteractRequired = true;
					else
						temp->p2InteractRequired = false;

					if (temp->triggerCriteria != 1 && temp->triggerCriteria != 4 && temp->triggerCriteria != 6 && temp->triggerCriteria != 7)
						temp->p2ActiveRadius = 0;

					if (temp->triggerCriteria != 2 && temp->triggerCriteria != 4 && temp->triggerCriteria != 5 && temp->triggerCriteria != 7)
						temp->p2TriggerTime = 0;
				}
				else if(elementLineCount==8)	//version 1.10
				{
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,1,0); return 0;}
					res=sscanf_s(linebuf,"%f",&inp1);	//Collision
					if(res!=1) {ReportError(theFile,1,res); return 0;}
					else linenumber++;

					

					if (inp1==1)
						temp->collide=true;
					else
						temp->collide=false;

					for(res=0;res<LINEBUFFERLIMIT;res++)
					{
						if(linebuf[res]=='\t')
						{
							foundNextItem=true;
						}
						if(linebuf[res]=='\n'||linebuf[res]==NULL)
							break;
					}

					if(foundNextItem)
					{
						res=sscanf_s(linebuf,"%f\t%f\t%f\n",&inp1,&inp2,&inp3);
						if(res!=3) {ReportError(theFile,3,res); return 0;}
						else linenumber++;

						if (inp2==1&&temp->collide) //collision required
							temp->kinematic=true;
						else
							temp->kinematic=false;
						if(inp3<=0) //mass must be positive
							inp3=0;
						temp->mass= inp3*temp->kinematic;
					
						foundNextItem=false;
					}
					else
					{
						temp->kinematic=false;
						temp->mass=0;
					}
			

					glmCalcBounds(temp);

					
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp)) //P1triggerCriteria	P1HighlightStyle	P1ActiveRadius	P1autoTriggerTime	P1AudioID	P1AudioLoop	P1AudioBehavior
					{ReportError(theFile,7,0); return 0;}
					res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4,&inp5,&inp6,&inp7);	
					if(res!=7) {ReportError(theFile,7,res); return 0;}
					else linenumber++;			
					
					temp->highlighted=false;
					temp->highlightCriteria=inp1;	
					temp->highlightStyle=inp2;
					if(temp->highlightCriteria==0)
					{
						temp->highlightStyle=0;
						temp->Highlight(true);
					}
					temp->p1ActiveRadius=inp3;

					temp->p1TriggerTime=inp4*1000;

					temp->highlightAudioID=inp5;
					temp->highlightAudioLoop=(inp6>0);
					temp->highlightAudioBehavior=inp7;


					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp)) //P2triggerCriteria	P2TriggerAction	P2ActiveRadius	P2autoTriggerTime	P2AudioID	P2AudioLoop	P2AudioBehavior
					{ReportError(theFile,7,0); return 0;}
					res=sscanf_s(linebuf,"%f\t%f\t%f\t%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4,&inp5,&inp6,&inp7);	
					if(res!=7) {ReportError(theFile,7,res); return 0;}
					else linenumber++;	

					temp->triggerCriteria=inp1;
					temp->triggerAction=(int)inp2;
					temp->p2ActiveRadius=inp3;
					//if(temp->p1ActiveRadius<temp->p2ActiveRadius)	//highlight radius should be greater than or equal to activation radius
					//	temp->p1ActiveRadius=temp->p2ActiveRadius;
					
					temp->p2TriggerTime=inp4*1000;
					temp->triggerAudioID=inp5;
					temp->triggerAudioLoop = (inp6>0);
					temp->triggerAudioBehavior=inp7; //(not yet implemented)

					temp->dynamic=true;
					temp->activated=false;
					temp->moving=false;
					temp->proximityTrigger=0;
					temp->destroy=false;
					temp->destroyed = false;
					temp->currentPercentage=0;
					temp->targetPercentage=100;

					if (temp->highlightCriteria != 1 && temp->highlightCriteria != 3)
						temp->p1ActiveRadius = -1;
					if (temp->highlightCriteria != 2 && temp->highlightCriteria != 3)
						temp->p1TriggerTime = 0;

					if (temp->triggerCriteria == 3 || temp->triggerCriteria == 5 || temp->triggerCriteria == 6 || temp->triggerCriteria == 7)
						temp->p2InteractRequired = true;
					else
						temp->p2InteractRequired = false;

					if (temp->triggerCriteria != 1 && temp->triggerCriteria != 4 && temp->triggerCriteria != 6 && temp->triggerCriteria != 7)
						temp->p2ActiveRadius = 0;

					if (temp->triggerCriteria != 2 && temp->triggerCriteria != 4 && temp->triggerCriteria != 5 && temp->triggerCriteria != 7)
						temp->p2TriggerTime = 0;
					
				}
				
				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp)) //
					{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4); //EndScale	EndX	EndY	EndZ					//phase 2
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;

				temp->endScale=inp1;
				temp->endPos.x=inp2;
				temp->endPos.y=inp3;
				temp->endPos.z=inp4;

				if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,4,0); return 0;}
				res=sscanf_s(linebuf,"%f\t%f\t%f\t%f",&inp1,&inp2,&inp3,&inp4); //ActionTime	EndRotX	EndRotY	EndRotZ					//phase 2
				if(res!=4) {ReportError(theFile,4,res); return 0;}
				else linenumber++;

				temp->actionTime=inp1;
				temp->endRot.x=inp2;
				temp->endRot.y=inp3;
				temp->endRot.z=inp4;
				
				
				if(elementLineCount==6) //nosound dynamics (1.08)
				{
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,1,0); return 0;}
					res=sscanf_s(linebuf,"%f\t%f",&inp1,&inp2);
					if(res!=2) {ReportError(theFile,2,res); return 0;}
					else linenumber++;
				
					temp->targetModelID=inp1;
					temp->switchModel=false;
					temp->highlightStyle=inp2;
					temp->highlighted=false;
				}
				else if (elementLineCount==8)// sound 1.10
				{
					if(NULL==fgets(linebuf,LINEBUFFERLIMIT,fp))
					{ReportError(theFile,1,0); return 0;}
					res=sscanf_s(linebuf,"%f",&inp1);
					if(res!=1) {ReportError(theFile,1,res); return 0;}
					else linenumber++;
				
					temp->targetModelID=inp1;
					temp->switchModel=false;
				}
					

				temp->glowFactor=0;
				temp->glowColor=0;

				temp->highlightStyle = 0;
				temp->p1PointThreshold = -1;
				temp->p2PointThreshold = -1;
				temp->pointsGranted = 0;
				
				if(temp->highlightStyle>3&&temp->highlightStyle<7)
				{
					temp->glowColor=temp->highlightStyle-4;
				}
				if(temp->highlightStyle>10&&temp->highlightStyle<=34)
				{
					particleArray *newCircle;             //create a temporary node 
					newCircle = (particleArray*)malloc(sizeof(particleArray)); //allocate space for node 
					newCircle->next = partArrayHead;				//assign next address to temp
					partArrayHead = newCircle;
					newCircle->dID = temp->dID;
					if(temp->highlightStyle<=22)
					{
						newCircle->color=temp->highlightStyle-10;
						newCircle->up=false;
					}
					else
					{
						newCircle->color=temp->highlightStyle-22;
						newCircle->up=true;
					}

					newCircle->pos=temp->pos;
					newCircle->radius=(float)sqrt((temp->MaxCoords.x*temp->MaxCoords.x+temp->MaxCoords.z*temp->MaxCoords.z)*temp->scale*temp->scale);

					newCircle->activated=false;
					newCircle->particleCount=newCircle->radius*newCircle->radius*1000;
					if(newCircle->particleCount>MAX_PARTICLES)
						newCircle->particleCount=MAX_PARTICLES;
					
				}
				
				break;
			default:
				for(i=0;i<inp2;i++)
				{
					fgets(linebuf,LINEBUFFERLIMIT,fp);
					linenumber++;
				}
				break;
		}				
	}
	fclose(fp);

	curTextureDict.ClearUnused();



	objMap->lights=lights;
	objMap->shaders=objectsShader;
	objMap->shadersCompiled=shadersCompiled;
	updateTime2=GetQPC();
	updateTime3=GetQPC();
	objCamera.startPos.x=2;
	objCamera.startPos.y=0;
	objCamera.startPos.z=2;
	objCamera.Position_Camera(objCamera.startPos.x,objCamera.startPos.y,objCamera.startPos.z, 0,0,0 ,0,1,0);	
	if(objMap->bStartPos) //starting position exists
	{
		bool twice=false;	//second readthrough
		StartPosition* startPos;
		startPos=objMap->startPosHead;
		while(startPos!=NULL&&startPos->inputValue!=mazeInputValue)
		{
			if(startPos->last&&!twice) 
			{
				mazeInputValue==0;
				twice=true;
				startPos=objMap->startPosHead;
			}
			if(startPos->last&&twice)
			{
				startPos=objMap->startPosHead;
				break;
			}
			startPos=startPos->next;
		}
		if(startPos!=NULL)
		{
				
			objCamera.startPos.x=startPos->x;
			objCamera.startPos.y=startPos->y;
			objCamera.startPos.z=startPos->z;
			objCamera.Position_Camera(objCamera.startPos.x,objCamera.startPos.y,objCamera.startPos.z, 0,0,0 ,0,1,0);
			objCamera.InitialRotate(startPos->angle*PI/180,xLoc+Width/2,yLoc+Height/2,startPos->vertAngle);	
			
		}
	}
	else
		objCamera.InitialRotate(0,xLoc+Width/2,yLoc+Height/2,0);


	objCamera.startView=objCamera.mView;

	bMouseInput=bMouseInputDisabled; //restore mouse input to original settings

	if (objMap->wallCount <= 0)
	{
		GUIMessageBox("Maze has no Listed walls or objects\nMaze will now Exit", 0, TEXTBOXSTYLE_FATAL_ERROR);
		return 0;
	}

	///////////////////
	//Move to Map BuildCollision()
	InitBulletPhysics();

	//sprintf(mazeWorkingDir, ""); //clear mazeworkgdir
	updateMazeWorkingDir("");
	
	return 1;
}



char* ftoLower(const char* in)
{
	char ret[600]="";
	sprintf(ret,"%s",in);
	for (int i=0;i<strlen(in);i++)
	{
		ret[i]=tolower(ret[i]);
		if(ret[i]=='/')
			ret[i]='\\';
	}
	ret[strlen(in)]=0;
	return ret;
}

bool preloadUnused = true; //discard "unused"

int LoadTexture(char* fname, int mazeTexKey) //Load texture and MazeKey returns GL texture for 
{
		char  fname2[600];
		
		int n=7;
		n = strlen(fname);

		if (fname[n - 1] == ' ' || strlen(fname)<4)
		{
			
			if (!preloadUnused || strlen(fname)<4)//do not load image..
			{
				return 0;
			}
			fname[n - 1] = 0; //null Terminate
			//Otherwise continue to load
		}
		
						
		char* matchedfname;
			
			sprintf(fname2,"%s",&fname[8]);
			matchedfname=getTextureFilename(&fname[8]); //find best match

			int gl_key = -1;
						
			if(matchedfname) //check
			{

				int temp_gl_key = -1;
				//Check fname
				temp_gl_key=curTextureDict.Get_glKey_from_path(matchedfname);

				if (temp_gl_key > 0)
					gl_key = temp_gl_key;
				else //if doens't exist, load texture
					gl_key = glmLoadTexture(matchedfname);
			}
						
			if (gl_key<= 0) //Texture couldn't be loaded
			{
					
					char txt[250];
					sprintf(txt,"Texture not found!\n%s",fname2);
					GUIMessageBox(txt, 0, TEXTBOXSTYLE_WARNING);
					return 0;
			}	
			else
			{
				
				curTextureDict.Add(matchedfname, gl_key, mazeTexKey); //bind mazeKey +fname to gl_key 
			}
			return gl_key;
		
}


int Init(GLvoid)										// All Setup For OpenGL Goes Here
{
	
	GLenum err = glewInit();
	

	
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		char errorText[500];
		sprintf(errorText, "Error: %s\n", glewGetErrorString(err));
		MessageBox(hWnd,errorText,"Error",MB_OK);
		shadersCompiled=false;
		shader=false;
		return 0;
	}


	if ( !GLEW_VERSION_3_0)
	{
		/* OpenGL 3.0 required */
		char errorText[500];

		const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
		const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model

		sprintf(errorText, "Error: Graphics Card Does not Support OpenGL 3.0\nGraphics Card Make: %s\nDetected OpenGL version %s", renderer, glGetString(GL_VERSION));
		MessageBox(hWnd,errorText,"Minimum Requirements Error",MB_OK);
		return 0;
	}


		//////////////////////////////////////////////////////////////////////////
	if(shader&&GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
	{
		BuildShaders();

		glUseProgram(shaderProg);

		shadersCompiled=true;
	}
	else
	{
		maxlights=8;
		shadersCompiled=false;
		shader=false;
	}
		//////////////////////////////////////////////////////////////////////////
	float As[4] = {0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, As );

	objCamera.Position_Camera(0.0, 0.0, 0.0,	0, 0, 0,   0, 1, 0);
    
	glEnable( GL_MULTISAMPLE );
	glEnable(GL_MULTISAMPLE_ARB);
	

	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping 
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	            // Clear The Screen And The Depth Buffer
	glLoadIdentity();


	glDisable(GL_LIGHTING);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing							
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// For Nice Perspective Calculations
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	



	BuildFont();

	char *ext = (char*)glGetString( GL_EXTENSIONS );

	glGenQueriesARB( 1, &gQuery1 );
	glGenQueriesARB( 1, &gQuery2 );
	

	#ifdef _MAZE_PROFILER
	debugFile = fopen("out.txt","w");
	#endif


	
	
	MazeInit();

	//if(oculusEnabled)
		//InitOculus();

	return TRUE;										
}

void MazeInit()
{
	srand(GetQPC()*time(NULL)); //seed based on time

	for (int z=0;z<UpdateTimeBufferCount;z++)
	{
		updateTimeFrame[z]=0;
	}


	Player.offset=Player.defaultCameraOffset;
	Player.jumping=false;
	Player.goingUp=false;
	Player.gravityEnabled=!bDisableGravity;
	Player.collisionEnabled=true;
	Player.jumpSpeed=0;
	Player.defaultCameraOffset = 0.3f;

	objCamera.cameraSpeedMoveDefault = 0.05f;
	objCamera.cameraSpeedViewDefault = 0.08f;

	
	firstLightRun=true;
	Player.ClearConditions();
	Player.gravityEnabled=false;
	Player.collisionEnabled=false;

	curMazePoints = 0; //reset maze points

	clearModels();
	BuildWhite();
	//curTextureDict.MarkAllUnused();
	curTextureDict.SetWhite(whiteID);
	curTextureDict.MarkAllUnused();
	/////////////
	// Clear previous lights + Create Default Light
	curLightList.Clear();
	curLightList.AddLight();
	curLightList.AddAmbientToLast(0.3f,0.3f,0.3f,1.0f);
	curLightList.AddDiffuseToLast(0.7f,0.7f,0.7f,0.7f);
	curLightList.AddPositionToLast(-5,20,-5,1);
	curLightList.AddAttenToLast(0,0.000001f);
	curLightList.GetLast()->index=GL_LIGHT0;
	
	float As[4] = {0.1f, 0.1f, 0.1f, 1.0f };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, As );


	GLfloat	white[] = { 1.0, 1.0, 1.0, 1.0 }; //resets materials
	GLfloat	black[] = { 0.0, 0.0, 0.0, 0.0 }; //resets materials
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);

	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_RESCALE_NORMAL);
	
	/////////////

	////////////
	// Clear previous texture dictionary + Audio dictionary
	
	curAudioDict.Stop(0);
	curAudioDict.Clear();
	
	ExitBulletPhysics();
	if(bMouseInput)
		objCamera.ResetMouseBuffer();

	
	if(objMap)
	{
		objMap->startMessageEnabled=false;
		sprintf(objMap->startMessage, "");
		objMap->pointMessageEnabled = false;
		sprintf(objMap->pointMessage, "");
		objMap->pointExitThreshold = -1;
		objMap->bEndPos = false;
		objMap->bStartPos = false;
		objMap->wallCount = 0;
		objMap->numObjs = 0;
		objMap->modelCount = 0;
		objMap->dModelCount = 0;
		objMap->activeRegionHead = NULL;
		objMap->endRegionHead = NULL;
		objMap->startPosHead = NULL;
		objMap->startPosDefault = NULL;

		objMap->maxMazeTime = 0;
		sprintf(objMap->timeoutMessage, "");

		defaultStartPosID = 0;

		model34_ID = -1;
		model34_scale = 1;
		model34_rot = tVector3(0, 0, 0);
	}
	
	
}

StartPosition* getStartPosByIndex(int index) //negative values return head, 0 returns random ID, positive IDs will return indexed values
{
	StartPosition* temp = objMap->startPosHead;

	if(temp == NULL)
		return NULL;

	if (index < 0)
		return temp;

	if (index == 0)
	{
		int startPosCount = 0;
		while (temp)
		{
			startPosCount++;
			temp = temp->next;
		}
		temp = objMap->startPosHead;
		int randomNumber = rand() % startPosCount;
		for (int i = 0; i < randomNumber;i++)
		{
			temp = temp->next;
		}
		return temp;
	}

	while (temp)
	{
		if (temp->index == index)
			return temp;
		temp = temp->next;

	}

	return NULL;
}

GLMmodelNode* getModelByIndex(int index)
{
	GLMmodelNode *temp;
	temp = (GLMmodelNode*)malloc(sizeof(GLMmodelNode));
	temp = ModelDictHead;

	while (temp != NULL&&temp->modelID != index)
	{
		temp = temp->next;
	}
	
	return temp;
}



int BuildWhite()
{
	glPushMatrix();

	whiteID=new GLuint;
	static GLubyte checkImage[1][1][4];
	
	checkImage[0][0][0] = (GLubyte) 255;
	checkImage[0][0][1] = (GLubyte) 255;
	checkImage[0][0][2] = (GLubyte) 255;
	checkImage[0][0][3] = (GLubyte) 255;

   glClearColor (0.0, 0.0, 0.0, 0.0);
   glShadeModel(GL_FLAT);
   glEnable(GL_DEPTH_TEST);

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
   
   glGenTextures(1, whiteID);
   glBindTexture(GL_TEXTURE_2D, whiteID[0]);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                   GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                   GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1,
                1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                checkImage);
   glPopMatrix();
   return 1;

}

void BuildShaders()
{
	char *vs = NULL,*fs = NULL;

	char fragShadeCharArray[BUF_SIZE];

	std::string fragShade;

	GLuint v = glCreateShader(GL_VERTEX_SHADER);
	GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

	if(ambientShader!=0) //ambientshader override
		ambientShader=1;

	//vs = textFileRead("C:\\shaders\\vert.c");
	//fs = textFileRead("C:\\shaders\\frag.c");
	vs="varying vec3 N; varying vec3 v; varying vec2 varTexCoord; void main(void) { gl_FrontColor = gl_Color;  v = vec3(gl_ModelViewMatrix * gl_Vertex); N = normalize(gl_NormalMatrix * gl_Normal); gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; varTexCoord = vec2(gl_MultiTexCoord0);}";

	//fragShade="uniform sampler2D baseMap; varying vec3 N; varying vec3 v; varying vec2 varTexCoord; vec4 lightSrc (in int lightNum) { vec3 L = gl_LightSource[lightNum].position.xyz - v; float dist = length(L); float atten = (gl_LightSource[lightNum].constantAttenuation + gl_LightSource[lightNum].linearAttenuation * dist + gl_LightSource[lightNum].quadraticAttenuation * dist * dist);\n ";
	strcpy_s(fragShadeCharArray,BUF_SIZE,  "uniform sampler2D baseMap; varying vec3 N; varying vec3 v; varying vec2 varTexCoord; \n vec4 lightSrc (in int lightNum) { vec3 L = gl_LightSource[lightNum].position.xyz - v; float dist = length(L); float atten = (gl_LightSource[lightNum].constantAttenuation + gl_LightSource[lightNum].linearAttenuation * dist + gl_LightSource[lightNum].quadraticAttenuation * dist * dist);\n "); 
	if(specularShader)
		strcat_s(fragShadeCharArray,BUF_SIZE, "vec3 E = normalize(-v); vec3 R = normalize(-reflect(L,N)); vec4 Ispec = gl_FrontMaterial.specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);");
	//fragShade+="vec3 E = normalize(-v); vec3 R = normalize(-reflect(L,N)); vec4 Ispec = gl_FrontMaterial.specular * pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess);";
	if(diffuseShader)
		strcat_s(fragShadeCharArray,BUF_SIZE,"L = normalize(L); vec4 Idiff = (gl_LightSource[lightNum].diffuse * abs(dot(L,N)));\n");


	strcat_s(fragShadeCharArray,BUF_SIZE," vec4 color = (");

	if(diffuseShader)
	{
		strcat_s(fragShadeCharArray,BUF_SIZE,"Idiff");
		if(specularShader)
			strcat_s(fragShadeCharArray,BUF_SIZE,"+Ispec");
	}
	else if (specularShader)
		strcat_s(fragShadeCharArray,BUF_SIZE,"Ispec");
	else
		strcat_s(fragShadeCharArray,BUF_SIZE,"gl_Color");

	strcat_s(fragShadeCharArray,BUF_SIZE,")/atten; return color; } void main (void) { vec4 buff=((");

	if(maxlights>0)
		strcat_s(fragShadeCharArray,BUF_SIZE,"lightSrc(0)");
	if(maxlights>1)
		strcat_s(fragShadeCharArray,BUF_SIZE,"+lightSrc(1)");
	if(maxlights>2)
		strcat_s(fragShadeCharArray,BUF_SIZE,"+lightSrc(2)");
	if(maxlights>3)
		strcat_s(fragShadeCharArray,BUF_SIZE,"+lightSrc(3)");
	if(maxlights>4)
		strcat_s(fragShadeCharArray,BUF_SIZE,"+lightSrc(4)");
	if(maxlights>5)
		strcat_s(fragShadeCharArray,BUF_SIZE,"+lightSrc(5)");
	if(maxlights>6)
		strcat_s(fragShadeCharArray,BUF_SIZE,"+lightSrc(6)");
	if(maxlights>7)
		strcat_s(fragShadeCharArray,BUF_SIZE,"+lightSrc(7)");
	strcat_s(fragShadeCharArray,BUF_SIZE,")*gl_FrontMaterial.diffuse)");
	if (ambientShader==1)
		strcat_s(fragShadeCharArray,BUF_SIZE,"+(gl_LightModel.ambient*gl_FrontMaterial.ambient)");
	strcat_s(fragShadeCharArray,BUF_SIZE,";\n ");
	if (texturesShader)
		strcat_s(fragShadeCharArray,BUF_SIZE,"gl_FragColor =buff*texture2D(baseMap, varTexCoord)*gl_Color;}");
	else
		strcat_s(fragShadeCharArray,BUF_SIZE,"gl_FragColor =buff*gl_Color;}");

	
	fs=fragShadeCharArray;
	//MessageBox(NULL,fs,0,0);


	const char * vv = vs;
	const char * ff = fs;

	if(vs!=NULL&&fs!=NULL)
	{
		glShaderSource(v, 1, &vv,NULL);
		glShaderSource(f, 1, &ff,NULL);

		glCompileShader(v);
		glCompileShader(f);

		char* vlog=printShaderInfoLog(v);
		char* flog=printShaderInfoLog(f);
		//MessageBox(NULL,printShaderInfoLog(v),0,0);
		//MessageBox(NULL,printShaderInfoLog(f),0,0);
	
		char* temp = printShaderInfoLog(f);
		if(temp!=NULL)
		{
			if(!strcmp(temp,"Fragment shader was successfully compiled to run on hardware.\\n"))
			{
				MessageBox(NULL,printShaderInfoLog(f),"Shader Compile Error: Please Disable Shaders",0);
				shader=false;
			}
		}



		shaderProg = glCreateProgram();
		glAttachShader(shaderProg,v);
		glAttachShader(shaderProg,f);

		glLinkProgram(shaderProg);
	}


	//////////////////////////////////////////////////////////////////////////

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	//vs = textFileRead("C:\\shaders\\vertnolights.c");
	vs="varying vec2 varTexCoord; void main(void) { gl_FrontColor = gl_Color; gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; varTexCoord = vec2(gl_MultiTexCoord0);}";
	//fs = textFileRead("C:\\shaders\\fragnolights.c");
	fs="uniform sampler2D baseMap; varying vec2 varTexCoord;  void main (void) { vec4 buff=gl_FrontMaterial.diffuse*gl_Color; gl_FragColor =buff*texture2D(baseMap, varTexCoord); }";

	vv = vs;
	ff = fs;

	if(vs!=NULL&&fs!=NULL)
	{
		glShaderSource(v, 1, &vv,NULL);
		glShaderSource(f, 1, &ff,NULL);

		glCompileShader(v);
		glCompileShader(f);

		//MessageBox(NULL,printShaderInfoLog(v),0,0);
		//MessageBox(NULL,printShaderInfoLog(f),0,0);

		char* temp =printShaderInfoLog(f);
		if(temp!=NULL)
		{
			if(!strcmp(temp,"Fragment shader was successfully compiled to run on hardware.\\n"))
			{
				MessageBox(NULL,printShaderInfoLog(f),"Shader Compile Error: Please Disable Shaders",0);
			}
		}

		shaderProgNoLights = glCreateProgram();
		glAttachShader(shaderProgNoLights,v);
		glAttachShader(shaderProgNoLights,f);

		glLinkProgram(shaderProgNoLights);
	}

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	//vs = textFileRead("C:\\shaders\\vertnolights.c");
	vs="varying vec2 varTexCoord;  void main(void) {gl_FrontColor = gl_Color; gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; varTexCoord = vec2(gl_MultiTexCoord0);}";
	//fs = textFileRead("C:\\shaders\\fragnolights.c");
	fs="uniform sampler2D baseMap; varying vec2 varTexCoord; void main (void) { vec4 buff=gl_Color; gl_FragColor =buff*texture2D(baseMap, varTexCoord);}";

	vv = vs;
	ff = fs;

	if(vs!=NULL&&fs!=NULL)
	{
		glShaderSource(v, 1, &vv,NULL);
		glShaderSource(f, 1, &ff,NULL);

		glCompileShader(v);
		glCompileShader(f);

		//MessageBox(NULL,printShaderInfoLog(v),0,0);
		//MessageBox(NULL,printShaderInfoLog(f),0,0);

		
		char* temp=printShaderInfoLog(f);
		if(temp!=NULL)
		{
			if(!strcmp(temp,"Fragment shader was successfully compiled to run on hardware.\\n"))
			{
				MessageBox(NULL,printShaderInfoLog(f),"Shader Compile Error: Please Disable Shaders",0);
				shader=false;
			}
		}




		shaderProgBasic = glCreateProgram();
		glAttachShader(shaderProgBasic,v);
		glAttachShader(shaderProgBasic,f);

		glLinkProgram(shaderProgBasic);
	}

	//if (oculusEnabled)
	//	CompileOculusShaders();

}

//GLuint oProgL,oProgR;
//void CompileOculusShaders()
//{
//
//
//	if (oculusEnabled)
//	{
//		GLint oFL,oFR, oV;
//
//
//		oFL = glCreateShader(GL_FRAGMENT_SHADER);
//		oFR = glCreateShader(GL_FRAGMENT_SHADER);
//		oV = glCreateShader(GL_VERTEX_SHADER);
//
//
//		glShaderSource(oV, 1, &oVert, NULL);
//		glShaderSource(oFL, 1, &oFragL, NULL);
//		glShaderSource(oFR, 1, &oFragR, NULL);
//
//		glCompileShader(oV);
//		glCompileShader(oFR);
//		glCompileShader(oFL);
//
//		//MessageBox(NULL,printShaderInfoLog(oV),0,0);
//		//MessageBox(NULL,printShaderInfoLog(oF),0,0);
//
//
//		//MessageBox(NULL, printShaderInfoLog(bS), 0, 0);
//		//MessageBox(NULL, printShaderInfoLog(bR), 0, 0);
//		//MessageBox(NULL, printShaderInfoLog(bL), 0, 0);
//
//
//
//
//		oProgL = glCreateProgram();
//		glAttachShader(oProgL, oV);
//		glAttachShader(oProgL, oFL);
//		glLinkProgram(oProgL);
//
//		oProgR = glCreateProgram();
//		glAttachShader(oProgR, oV);
//		glAttachShader(oProgR, oFR);
//		glLinkProgram(oProgR);
//
//
//
//		glUseProgram(oProgL);
//		GLuint _lensCenterOffsetLoc = glGetUniformLocation(oProgL, "u_lensCenterOffset");
//		GLuint _hmdWarpParamLoc = glGetUniformLocation(oProgL, "u_hmdWarpParam");
//		GLuint _scaleLoc = glGetUniformLocation(oProgL, "u_scale");
//		GLuint _scaleInLoc = glGetUniformLocation(oProgL, "u_scaleIn");
//
//		glUniform4f(_hmdWarpParamLoc, 1.0f,0.22f,0.24f,0.0f);
//		float aspect = float(1280 / 2) / float(800);
//		float scaleFactor = 1.0f; ///;SLOculus::scale;
//		float w = float(1280 / 2) / float(800);
//		float h = 1.0f;
//		glUniform1f(_lensCenterOffsetLoc, (w + 0.151976 * 0.5f)*0.5f);
//		glUniform2f(_scaleLoc, (w / 2) * scaleFactor, (h / 2) * scaleFactor * aspect);
//		glUniform2f(_scaleInLoc, (4 *h/w), 4);
//
//		GLuint _lensCenterOffsetLocL = glGetUniformLocation(oProgR, "lensCenterOffset");
//		glUniform1f(_lensCenterOffsetLocL, OculusOffset);
//
//		glUseProgram(oProgR);
//		GLuint _hmdWarpParamLocR = glGetUniformLocation(oProgR, "u_hmdWarpParam");
//		GLuint _scaleLocR = glGetUniformLocation(oProgR, "u_scale");
//		GLuint _scaleInLocR = glGetUniformLocation(oProgR, "u_scaleIn");
//		GLuint _lensCenterOffsetLocR = glGetUniformLocation(oProgR, "lensCenterOffset");
//		glUniform1f(_lensCenterOffsetLocR, OculusOffset);
//
//		glUniform4f(_hmdWarpParamLocR, 1.0f,0.22f,0.24f,0.0f);
//
//		glUniform1f(_lensCenterOffsetLocR, (w + 0.151976 * 0.5f)*0.5f);
//		glUniform2f(_scaleLocR, (w / 2) * scaleFactor, (h / 2) * scaleFactor * aspect);
//		glUniform2f(_scaleInLocR, (4 * h / w), 4);
//
//		glUseProgram(0);
//	}
//
//
//}

void DrawPlayer(tVector3 mPos,tVector3 mView)
{
	glPushMatrix();

	glColor3f( 1, 0, 0);

	if (model34_ID > 0)
	{
		glTranslatef(mPos.x, mPos.y-Player.offset, mPos.z);
		glRotatef(model34_rot.x, 1.0f, 0.0f, 0.0f);
		glRotatef(model34_rot.y+objCamera.GetCurAngle()*180/PI, 0.0f, 1.0f, 0.0f);
		glRotatef(model34_rot.z, 0.0f, 0.0f, 1.0f);

		GLMmodelNode* temp2 = getModelByIndex(model34_ID);

		if (temp2)
			glmReducedDraw(temp2->modelData, model34_scale);

	}
	else{
		GLUquadricObj *quadric;
		quadric = gluNewQuadric();
		glTranslatef(mPos.x, mPos.y-Player.offset, mPos.z);

		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluSphere(quadric, .33, 36, 18);

		gluDeleteQuadric(quadric);

		glColor3f(1, 1, 0);
		quadric = gluNewQuadric();
		glTranslatef((mView.x - mPos.x) / 2, (mView.y - mPos.y) / 2, (mView.z - mPos.z) / 2);



		gluQuadricDrawStyle(quadric, GLU_FILL);
		gluSphere(quadric, .2, 12, 18);

		gluDeleteQuadric(quadric);
	}
	glPopMatrix();
}


GLuint FramebufferR = 0;
GLuint FramebufferL = 1;
GLuint renderedTextureL,renderedTextureR;
int oHeight=1920;
int oWidth=1080;

//OCULUS TESTING/
/*
void InitOculus()
{
	if (hmd)
		ovrHmd_Destroy(hmd);
	
	ovr_Shutdown();

	ovr_Initialize();

	hmd = ovrHmd_Create(0);
	if (!hmd) {
		MessageBox(NULL, "Unable to detect Rift Display", "OCULUS ERROR", MB_OK | MB_ICONINFORMATION);
		oculusEnabled = false;
		return;
	}
	ovrHmd_StartSensor(hmd, ovrSensorCap_Orientation, 0);

	//ovrHmd_GetDesc(hmd, &hmdDesc);
	CalculateHmdValues();

	glGenFramebuffers(1, &FramebufferL);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferL);

	int height=oHeight;
	int width=oWidth;

	// The texture we're going to render to

	glGenTextures(1, &renderedTextureL);
 
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTextureL);


 
	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);
 
	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// The depth buffer
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTextureL, 0);
	
 
	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	//RIGHT
	
	glGenFramebuffers(1, &FramebufferR);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferR);

	glGenTextures(1, &renderedTextureR);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTextureR);
	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTextureR, 0);
	DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


	
	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return;
	}
}
*/

float ICD;
float ICDset = 0.05;
bool skipUpdate = false;

int Draw(GLvoid)
{
	/*
	if (oculusEnabled)
	{
		//hmdFrameTiming = ovrHmd_BeginFrame(hmd, 0);
		//ss = ovrHmd_GetSensorState(hmd, 0);
		HeadPose = ss.Recorded.Pose;

		ICD = -ICDset;
		DrawBuffer(FramebufferL);
		ICD = ICDset;
		skipUpdate = true;
		DrawBuffer(FramebufferR);
		DrawOculus();

		//ovrHmd_EndFrame(hmd);
		
	}
	else
	{
		*/
		DrawBuffer(0);
		ICD = 0;
	//}

	return TRUE;
}
/*
int DrawOculus()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Width, Height); // Render on the whole framebuffer, complete from the lower left corner to the upper right



	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);

	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);

	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, renderedTextureL);
	glColor3f(1, 1, 1);

	glUseProgram(oProgL);
	GLuint _lensCenterOffsetLocL = glGetUniformLocation(oProgL, "lensCenterOffset");
	glUniform1f(_lensCenterOffsetLocL, OculusOffset);

	GLuint _hmdWarpParamLocL = glGetUniformLocation(oProgL, "hmdWarpParam");
	glUniform4f(_hmdWarpParamLocL, 1.0f, 0.22f, 0.3f, 0.0f);

	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(0, 0);		glVertex3f(0.0f, 0.0f, 0.0f); //vertex 1
	glTexCoord2d(0, 1);		glVertex3f(0, 1, 0.0f); //vertex 2
	glTexCoord2d(1, 0);		glVertex3f(0.5f, 0, 0.0f); //vertex 4
	glTexCoord2d(1, 1);		glVertex3f(0.5f, 1, 0.0f); //vertex 3
	glEnd();

	glUseProgram(oProgR);

	GLuint _lensCenterOffsetLocR = glGetUniformLocation(oProgR, "lensCenterOffset");
	glUniform1f(_lensCenterOffsetLocR, OculusOffset);


	GLuint _hmdWarpParamLocR = glGetUniformLocation(oProgR, "hmdWarpParam");
	glUniform4f(_hmdWarpParamLocR, 1.0f, 0.22f, 0.3f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, renderedTextureR);

	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(0, 0);		glVertex3f(0.5f, 0.0f, 0.0f); //vertex 1
	glTexCoord2d(0, 1);		glVertex3f(0.5f, 1, 0.0f); //vertex 2
	glTexCoord2d(1, 0);		glVertex3f(1, 0, 0.0f); //vertex 4
	glTexCoord2d(1, 1);		glVertex3f(1, 1, 0.0f); //vertex 3
	glEnd();


	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing		
	glEnable(GL_TEXTURE_2D);



	if (shadersCompiled&&shader)
		glUseProgram(shaderProg);
	else
		glUseProgram(shaderProgBasic);
	return true;
}

*/
#ifdef _MAZE_PROFILER
	LARGE_INTEGER start, now, freq;
	float mp1, mp2, mp3, mp4, mp5,mp6;
	float lastStepSize;
#endif
int DrawBuffer(GLuint framebuffer)								
{
	glFinish();

	if(oculusEnabled)
	{
		// Render to our framebuffer
		glViewport(0,0,oWidth,oHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	}

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

#ifdef _MAZE_PROFILER
	
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	DWORD firstUpdateVar = GetQPC();
	DWORD secondUpdateVar;// = updateTime2;

#endif
	
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	            // Clear The Screen And The Depth Buffer
	glLoadIdentity();
	
	// Reset The View

	tVector3 lastPos = objCamera.mPos;
	tVector3 lastView = objCamera.mView;
	tVector3 lastUp = objCamera.mUp;
	bool r = Player.collisionEnabled;


	

	//if (oculusEnabled)
	//{
	//	Player.collisionEnabled = false; //temporarily disable collision
	//	
	//	float hmdYaw, hmdPitch, hmdRoll;
	//	HeadPose.Rotation.GetEulerAngles<Axis_Y, Axis_X, Axis_Z>(&hmdYaw, &hmdPitch, &hmdRoll);

	//	specialVar = hmdYaw;
	//	
	//	objCamera.SetView(hmdYaw, hmdPitch, hmdRoll);
	//	objCamera.SetICD(ICD);

	//	gluLookAt(objCamera.mPos.x, objCamera.mPos.y, objCamera.mPos.z,
	//		objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,
	//		objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);
	//}
	//else 
	if(bTopDown)
	{
		
		//objCamera.cameraSpeedView = 5;
		//tVector3 cameraTarget = objCamera.mPos;
		if (bFixedCameraLocationX)
		{
			objCamera.mPos.x = fixedCameraLocationX;
			
		}
		if (bFixedCameraLocationZ)
		{
			objCamera.mPos.z = fixedCameraLocationZ;
		}

		if (iTopDownOrientation==1) //north
		
			gluLookAt(objCamera.mPos.x, objCamera.mPos.y + iTopDownHeight, objCamera.mPos.z,
			objCamera.mPos.x, objCamera.mPos.y, objCamera.mPos.z - 1e-10,
			objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);

		else if (iTopDownOrientation == 2) //South

			gluLookAt(objCamera.mPos.x, objCamera.mPos.y + iTopDownHeight, objCamera.mPos.z,
			objCamera.mPos.x, objCamera.mPos.y, objCamera.mPos.z + 1e-10,
			objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);
		else if (iTopDownOrientation == 3) //East

			gluLookAt(objCamera.mPos.x, objCamera.mPos.y + iTopDownHeight, objCamera.mPos.z,
			objCamera.mPos.x + 1e-10, objCamera.mPos.y, objCamera.mPos.z,
			objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);

		else if (iTopDownOrientation == 4) //West

			gluLookAt(objCamera.mPos.x, objCamera.mPos.y + iTopDownHeight, objCamera.mPos.z,
			objCamera.mPos.x - 1e-10, objCamera.mPos.y, objCamera.mPos.z,
			objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);


		else //Free
		{
			gluLookAt(objCamera.mPos.x, objCamera.mPos.y + iTopDownHeight, objCamera.mPos.z,
				objCamera.mPos.x - (objCamera.mPos.x - objCamera.mView.x) / 15, objCamera.mPos.y, objCamera.mPos.z - (objCamera.mPos.z - objCamera.mView.z) / 15,
				objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);
			
		}

		

	}
	else if (b34View)
	{
		float angle = f34Angle*PI / 180.0f;
		if (bFixedCameraLocationX||bFixedCameraLocationZ)
		{
			//objCamera.mPos.x = fixedCameraLocationX;
			//objCamera.mPos.z = fixedCameraLocationZ;
			float xPos = objCamera.mPos.x +(objCamera.mPos.x - objCamera.mView.x)*iTopDownHeight*cos(angle);
			float zPos = objCamera.mPos.z +(objCamera.mPos.z - objCamera.mView.z)*iTopDownHeight*cos(angle);
			if (bFixedCameraLocationX)
				xPos = fixedCameraLocationX;
			if (bFixedCameraLocationZ)
				zPos = fixedCameraLocationZ;
			

			
			gluLookAt(xPos, objCamera.mPos.y + iTopDownHeight*sin(angle), zPos,
				objCamera.mPos.x, objCamera.mPos.y + Player.defaultCameraOffset, objCamera.mPos.z,
				objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);
		}
		else
		{
	
			gluLookAt(objCamera.mPos.x + (objCamera.mPos.x - objCamera.mView.x)*iTopDownHeight*cos(angle), objCamera.mPos.y + iTopDownHeight*sin(angle), objCamera.mPos.z + (objCamera.mPos.z - objCamera.mView.z)*iTopDownHeight*cos(angle),
				objCamera.mPos.x, objCamera.mPos.y+Player.defaultCameraOffset, objCamera.mPos.z,
				objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);
		}
		//bTopDownMouseAngleMode = false;
	}
	else // First Person
	{

		gluLookAt(objCamera.mPos.x, objCamera.mPos.y, objCamera.mPos.z,
			objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,
			objCamera.mUp.x, objCamera.mUp.y, objCamera.mUp.z);
	}

	objCamera.mPos=lastPos;
	objCamera.mView=lastView;
	objCamera.mUp = lastUp;


	

	Player.collisionEnabled = r; //re-enable collision if enabled

#ifdef _MAZE_PROFILER
	QueryPerformanceCounter(&now);
	 mp1 = ((float)(now.QuadPart - start.QuadPart)*1000.0)/freq.QuadPart;  //Pre Update
	start = now;
#endif
	if (!skipUpdate)
		GameUpdate();
	else
		skipUpdate = false;

#ifdef _MAZE_PROFILER
	QueryPerformanceCounter(&now);
	 mp2 = ((float)(now.QuadPart - start.QuadPart)*1000.0) / freq.QuadPart;  //Post  Update
	start = now;
	 secondUpdateVar = updateTime2;
#endif 
	
	objMap->DrawQuery();

	
	
#ifdef _MAZE_PROFILER
	QueryPerformanceCounter(&now);
	 mp3 = ((float)(now.QuadPart - start.QuadPart)*1000.0)/freq.QuadPart; //Draw Query
	start = now;
#endif
	
	


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	            // Clear The Screen And The Depth Buffer

	
	if(shader)
	{
		if(lights)
			glUseProgram(shaderProg);
		else
			glUseProgram(shaderProgNoLights);
	}
	else if(lights) 
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	
	objMap->WaitForReady();

	#ifdef _MAZE_PROFILER
	QueryPerformanceCounter(&now);
	 mp4 = ((float)(now.QuadPart - start.QuadPart)*1000.0)/freq.QuadPart; //Wait for ready
	start = now;

#endif

	DrawSkyBox();
    //glCallList(ROOM);
	
	
	
	if (!bTopDown&&!b34View)
		objMap->Draw();
	else if(bTopDownSimpleRender)//Draws sphere for topdown or 34 view
	{
		objMap->DrawSimple();
		DrawPlayer(objCamera.mPos,objCamera.mView);
	}
	else
	{

		objMap->Draw();	
		DrawPlayer(objCamera.mPos, objCamera.mView);
	}
	

	
#ifdef _MAZE_PROFILER 
	QueryPerformanceCounter(&now);
	 mp5 = ((float)(now.QuadPart - start.QuadPart)*1000.0)/freq.QuadPart; //Post Draw
	start = now;
#endif
	//////////////////////////////////////////////////////////////////////////

	particleArray *tempCircles;
	tempCircles=partArrayHead;
	while(tempCircles!=NULL)
	{
		drawParticles(tempCircles);
		tempCircles=tempCircles->next;
	}
	
	
	
	DrawHUD();


#ifdef _MAZE_PROFILER 
	QueryPerformanceCounter(&now);
	mp6 = ((float)(now.QuadPart - start.QuadPart)*1000.0)/freq.QuadPart; //draw finished
	start = now;

	if(debugFile!=NULL)
	{
		fprintf(debugFile, "%f\t%f\t%f\t%f\t%f\t%f\t\t%d\t%d\t%d\t%f\t", mp1, mp2, mp3, mp4, mp5, mp6, firstUpdateVar, secondUpdateVar, GetQPC(), lastStepSize);
		//mp1 = Pre Update
		//mp2 = Draw Query
		//mp3 = Update
		//mp4 = Query Ready
		//mp5 = Draw
		//mp6 = DrawFinished
	}
#endif

	if(devMsgStatus==1)
	{
			
		GUIMessageBox(devMsg,devMsgTime,TEXTBOXSTYLE_ONDIALOG_CLEAR_BK);
		devMsgStatus=0;
	}

	//gluPerspective(45.0f, (GLfloat)Width / (GLfloat)Height, 0.1f, 1000.0f);
	

	return TRUE;
}

void DrawMouse()
{
	POINT mouse;						// Stores The X And Y Coords For The Current Mouse Position
	mouse=GetOGLPos();						// Gets The Current Cursor Coordinates (Mouse Coordinates)

	glColor3f(0,0,0);
	glBegin(GL_QUADS); //DrawMouse
		glVertex2f(mouse.x, mouse.y+1);
		glVertex2f(mouse.x+17, mouse.y-16);
		glVertex2f(mouse.x, mouse.y-16);
		glVertex2f(mouse.x, mouse.y-16);
	glEnd();

	glBegin(GL_QUADS); //DrawMouse
		glVertex2f(mouse.x, mouse.y-14);
		glVertex2f(mouse.x+10, mouse.y-14);
		glVertex2f(mouse.x+1, mouse.y-24);
		glVertex2f(mouse.x, mouse.y-24);
	glEnd();

	glBegin(GL_QUADS); //DrawMouse
		glVertex2f(mouse.x+3, mouse.y-14);
		glVertex2f(mouse.x+10, mouse.y-14);
		glVertex2f(mouse.x+14, mouse.y-28);
		glVertex2f(mouse.x+9, mouse.y-28);
	glEnd();
	
	glColor3f(1,1,1);

	glBegin(GL_QUADS); //DrawMouse
		glVertex2f(mouse.x+1, mouse.y-1);
		glVertex2f(mouse.x+15, mouse.y-15);
		glVertex2f(mouse.x+1, mouse.y-15);
		glVertex2f(mouse.x+1, mouse.y-15);
	glEnd();

	glBegin(GL_QUADS); //DrawMouse
		glVertex2f(mouse.x+1, mouse.y-15);
		glVertex2f(mouse.x+9, mouse.y-15);
		glVertex2f(mouse.x+1, mouse.y-23);
		glVertex2f(mouse.x+1, mouse.y-23);
	glEnd();

	glBegin(GL_QUADS); //DrawMouse
		glVertex2f(mouse.x+6, mouse.y-15);
		glVertex2f(mouse.x+9, mouse.y-15);
		glVertex2f(mouse.x+13, mouse.y-27);
		glVertex2f(mouse.x+10, mouse.y-27);
	glEnd();
}

void DrawCrosshair()
{

	glColor3f(1,1,1);
	glBegin(GL_QUADS); //DrawCrosshair
		glVertex2f(Width/2-Width/400, Height/2-Height/50-8);
		glVertex2f(Width/2+Width/400, Height/2-Height/50-8);
		glVertex2f(Width/2+Width/400, Height/2+Height/50-8);
		glVertex2f(Width/2-Width/400, Height/2+Height/50-8);
	glEnd();
	glBegin(GL_QUADS); //DrawCrosshair
		glVertex2f(Width/2-Width/67, Height/2-Height/300-8);
		glVertex2f(Width/2+Width/67, Height/2-Height/300-8);
		glVertex2f(Width/2+Width/67, Height/2+Height/300-8);
		glVertex2f(Width/2-Width/67, Height/2+Height/300-8);
	glEnd();
}

#define SKYBOX_FRONT 901
#define SKYBOX_LEFT 902
#define SKYBOX_RIGHT 904
#define SKYBOX_TOP 905
#define SKYBOX_BOTTOM 906
#define SKYBOX_BACK 903

void DrawSkyBox()
{
	glPushMatrix();
 
 
    // Enable/Disable features
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

	if(shader)
	{
			glUseProgram(0);
	}
    // Just in case we set all vertices to white.
    glColor4f(1,1,1,1);

	int mode=0;
	float skyScale=1000.0f;
	float playerOffsetX = objCamera.mPos.x;
	float playerOffsetZ = objCamera.mPos.z;
	 // Render the front quad
	GLuint texID = curTextureDict.Get_glKey(SKYBOX_FRONT);
	if(texID>0)
		mode=2;
    	texID=curTextureDict.Get_glKey(skyboxID);
	if(texID>0)
		mode=1;
	if(mode==1)
	{
		// Render the front quad
		glBindTexture(GL_TEXTURE_2D,curTextureDict.Get_glKey(skyboxID));
		glBegin(GL_QUADS);
		glTexCoord2f(0.75f, 0.333f); glVertex3f(0.5f*skyScale + playerOffsetX, -0.5f*skyScale, -0.5f*skyScale+ playerOffsetZ);
		glTexCoord2f(1, 0.333f); glVertex3f(-0.5f*skyScale + playerOffsetX, -0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 0.666f); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0.75f, 0.666f); glVertex3f(0.5f*skyScale + playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glEnd();
 
		// Render the left quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(skyboxID));
		glBegin(GL_QUADS);
		glTexCoord2f(0.50f, 0.334f); glVertex3f(0.5f*skyScale + playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0.75f, 0.334f); glVertex3f(0.5f*skyScale +playerOffsetX, -0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0.75f, 0.665f); glVertex3f(0.5f*skyScale +playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0.50f, 0.665f); glVertex3f(0.5f*skyScale +playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glEnd();
 
		// Render the back quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(skyboxID));
		glBegin(GL_QUADS);
		glTexCoord2f(0.25f, 0.334f); glVertex3f(-0.5f*skyScale +playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0.50f, 0.334f); glVertex3f(0.5f*skyScale +playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0.50f, 0.665f); glVertex3f(0.5f*skyScale +playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0.25f, 0.665f); glVertex3f(-0.5f*skyScale +playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
 
		glEnd();
 
		// Render the right quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(skyboxID));
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0.334f); glVertex3f(-0.5f*skyScale + playerOffsetX, -0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0.25f, 0.334f); glVertex3f(-0.5f*skyScale + playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0.25f, 0.665f); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0, 0.665f); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glEnd();
 
		// Render the top quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(skyboxID));
		glBegin(GL_QUADS);
		glTexCoord2f(0.2501f, 0.999f); glVertex3f(-0.50f*skyScale + playerOffsetX, 0.50f*skyScale, -0.50f*skyScale + playerOffsetZ);
		glTexCoord2f(0.2501f, 0.665f); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.50f*skyScale, 0.50f*skyScale + playerOffsetZ);
		glTexCoord2f(0.499f, 0.665f); glVertex3f(0.5f*skyScale + playerOffsetX, 0.50f*skyScale, 0.50f*skyScale + playerOffsetZ);
		glTexCoord2f(0.499f, 0.999f); glVertex3f(0.5f*skyScale + playerOffsetX, 0.50f*skyScale, -0.50f*skyScale + playerOffsetZ);
		glEnd();
 
		// Render the bottom quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(skyboxID));
		glBegin(GL_QUADS);
		glTexCoord2f(0.251f, 0); glVertex3f(-0.51f*skyScale + playerOffsetX, -0.5f*skyScale, -0.51f*skyScale + playerOffsetZ);
		glTexCoord2f(0.251f, 0.334f); glVertex3f(-0.51f*skyScale + playerOffsetX, -0.5f*skyScale, 0.51f*skyScale + playerOffsetZ);
		glTexCoord2f(0.49f, 0.334f); glVertex3f(0.51f*skyScale + playerOffsetX, -0.5f*skyScale, 0.51f*skyScale + playerOffsetZ);
		glTexCoord2f(0.49f, 0); glVertex3f(0.51f*skyScale + playerOffsetX, -0.5f*skyScale, -0.51f*skyScale + playerOffsetZ);
		glEnd();
	}

	else if(mode==2)
	{
		// Render the front quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(SKYBOX_FRONT));
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(0.5f*skyScale + playerOffsetX, -0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 0); glVertex3f(-0.5f*skyScale + playerOffsetX, -0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 1); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0, 1); glVertex3f(0.5f*skyScale + playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glEnd();
 
		// Render the left quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(SKYBOX_LEFT));
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(0.5f*skyScale + playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 0); glVertex3f(0.5f*skyScale + playerOffsetX, -0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 1); glVertex3f(0.5f*skyScale + playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0, 1); glVertex3f(0.5f*skyScale + playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glEnd();
 
		// Render the back quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(SKYBOX_BACK));
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-0.5f*skyScale + playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 0); glVertex3f(0.5f*skyScale + playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 1); glVertex3f(0.5f*skyScale + playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0, 1); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
 
		glEnd();
 
		// Render the right quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(SKYBOX_RIGHT));
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-0.5f*skyScale + playerOffsetX, -0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 0); glVertex3f(-0.5f*skyScale + playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 1); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0, 1); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glEnd();
 
		// Render the top quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(SKYBOX_TOP));
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0, 0); glVertex3f(-0.5f*skyScale + playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 0); glVertex3f(0.5f*skyScale + playerOffsetX, 0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 1); glVertex3f(0.5f*skyScale + playerOffsetX, 0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glEnd();
 
		// Render the bottom quad
		glBindTexture(GL_TEXTURE_2D, curTextureDict.Get_glKey(SKYBOX_BOTTOM));
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex3f(-0.5f*skyScale + playerOffsetX, -0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(0, 1); glVertex3f(-0.5f*skyScale + playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 1); glVertex3f(0.5f*skyScale + playerOffsetX, -0.5f*skyScale, 0.5f*skyScale + playerOffsetZ);
		glTexCoord2f(1, 0); glVertex3f(0.5f*skyScale + playerOffsetX, -0.5f*skyScale, -0.5f*skyScale + playerOffsetZ);
		glEnd();
	}
 
    // Restore enable bits and matrix
    glPopAttrib();
    glPopMatrix();
}

//showStyle param defined in mylist.h
void GUIMessageBox (char* displayText, int showTime, int showStyle)
{
	GUIMessageBox (displayText, showTime, showStyle, 0);
}



class OptionBox
{
	struct Color
	{
		float r;
		float g;
		float b;
	};

	char text[TXTBUFFERLIMIT];
	int selected = 0;
	int px = 0;
	int py = 0;
	int width = 100;
	int height = 30;
	int res = -1;
	int numLines = 0;
	Color bgColor;
	Color textColor;
	Color borderColor;
	Color selectColor;
	GLuint texID;

	int borderW = 1;
	bool drawBG = true;

public: OptionBox()
	{

	};
public: OptionBox(char* msg, int result, int posX,int posY,int w, int h)
	{
		texID = 0;
		px = posX;
		py = posY;
		width = w;
		height = h;
		res=result;
		strcpy(text, msg);

		SetTextColor(1, 1, 1);
		SetBorderColor(1, 1, 1);
		SetBGColor(0, 0, 0);
		SetSelectColor(0.4, 0, 0);

		for (int i = 0; i<TXTBUFFERLIMIT - 1; i++)
		{

			if (text[i] == NULL || text[i] == '\n')
			{
				numLines++;
			}

			if (text[i] == NULL)
				break;
		}

		selected = false;
	};

	public :void Draw()
	{
		x = Width / 2 + px;
		y = Height / 2 + py;
		glColor3f(borderColor.r, borderColor.g, borderColor.b);
		if (drawBG)
		{
			glBegin(GL_QUADS); //Box Border
			glVertex2f(x + width + borderW, y + height + borderW);
			glVertex2f(x - borderW, y + height + borderW);
			glVertex2f(x - borderW, y - borderW);
			glVertex2f(x + width + borderW, y - borderW);
			glEnd();

			if (texID != 0)
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texID);
				bgColor.r = 1;
				bgColor.g = 1;
				bgColor.b = 1;
			}

			if (selected)
				glColor3f(selectColor.r, selectColor.g, selectColor.b);
			else
			{
				glColor3f(bgColor.r, bgColor.g, bgColor.b);
			}
			glBegin(GL_QUADS); //Box fill

			glTexCoord2d(0, 0); glVertex2f(x, y);
			glTexCoord2d(1, 0); glVertex2f(x + width, y);
			glTexCoord2d(1, 1); glVertex2f(x + width, y + height);
			glTexCoord2d(0, 1); glVertex2f(x, y + height);
			glEnd();

			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, NULL);
		}

		char prev;
		int startPos = 0;	//will act as startpos
		int counter = 0;  //will act as line number

		float spacing = 1.5;
		float offset = spacing/2;
		float top = y + height;

		for (int i = 0; i<TXTBUFFERLIMIT - 1; i++)
		{

			if (text[i] == NULL&&i!=0 || text[i] == '\n')
			{
				glColor3f(textColor.r, textColor.g, textColor.b);
				

				prev = text[i];
				text[i] = NULL;
				glRasterPos2f(x + (width / 2), top - ((float)counter)*spacing*fontheight);// - fontheight*offset);
				glPrint(true,"%s", &(text[startPos]));
				text[i] = prev;
				counter++;
				startPos = i + 1;
			}

			if (text[i] == NULL)
				break;
		}

		glColor3f(1, 1, 1);
	};


public:int IsMouseOver(int mouseX, int mouseY)
{
	x = Width / 2 + px;
	y = Height / 2 + py;
	if (res == -1)
		return false;
	else if (mouseX > x&&mouseX<x + width&&mouseY>y&&mouseY < y + height)
		return true;
	else
		return false;
};

public: void SetTextColor(float red, float green, float blue)
{
	textColor.r = red;
	textColor.g = green;
	textColor.b = blue;
};

public: void SetBorderColor(float red, float green, float blue)
{
	borderColor.r = red;
	borderColor.g = green;
	borderColor.b = blue;
};

public: void SetBGColor(float red, float green, float blue)
{
	bgColor.r = red;
	bgColor.g = green;
	bgColor.b = blue;
};

public: void SetSelectColor(float red, float green, float blue)
{
	selectColor.r = red;
	selectColor.g = green;
	selectColor.b = blue;
};

public: void SetTexID(GLuint ID)
{
	texID = ID;
};

public :int Select(int select)
	{
		if (res!=-1)
			selected = select;
		return selected;
	};

public :int Selected()
	{
		return selected;
	};

public:int GetRes()
{
	return res;
};

public:int SetDrawBG(bool draw)
{
	drawBG = draw;
	return drawBG;
};

public:int SetBorderWidth(int borderWidth)
{
	borderW = borderWidth;
	return borderW;
};

public:char* GetText()
{
	return text;
};

};



void GUIMessageBox (char* displayText,int showTime, int showStyle, GLuint texID)
{
	int i,res=0,numOfLines=0;

	int yOffset = -fontheight*4;

	if (bSkipWarnings)
		res = -1; //skip non-fatal error messages

	keys[ControlLibrary[Jump]]=false;

	

	bool okSelected=true;
	keys[VK_ESCAPE]=false;
	MSG msg;
	DWORD messageTime=GetQPC();
	
	char displayTextChar[TXTBUFFERLIMIT];
	//char tempDisplayTextChar[TXTBUFFERLIMIT];
//	char prev;

	if (displayText == NULL)
	{
		return;
	}
	strcpy_s(displayTextChar, TXTBUFFERLIMIT, displayText);
	if (showStyle != TEXTBOXSTYLE_WARNING && showStyle != TEXTBOXSTYLE_FATAL_ERROR)
	{
		addLineBreaksAndMC(displayTextChar, TXTBUFFERLIMIT);
	}
	displayTextChar[strlen(displayText)] = 0;


	
	//count lines and the string length...
	int counter=0;

	int spaceLength = fontUI.getPixelLength(L" ");
	int fontHeight = fontUI.h;
	int longestMCpart = 8 * spaceLength;
	int longestPart = 8 * spaceLength;;

	//Convert to multiline depending on how many lines fit
	autoSizeToWidth(displayTextChar, TXTBUFFERLIMIT, (int)Width / (spaceLength * 12.0f / 10.0f));
	

	int MultipleChoice = 0;
	int MC[MC_CHOICES];
	int MClen[MC_CHOICES];

	std::list<OptionBox> boxList;
	boxList.clear();
	
	
	//OptionBox MCoption[5];
	//OptionBox OKbox;
	int lineStartIndex = 0;
	
	wchar_t wdisplayText[TXTBUFFERLIMIT];
	char tempTxt[TXTBUFFERLIMIT];
	strcpy_s(tempTxt, TXTBUFFERLIMIT, displayTextChar);

	freetype::ascii2utf8(wdisplayText, tempTxt, TXTBUFFERLIMIT);

	strcpy_s(displayTextChar, TXTBUFFERLIMIT, tempTxt);

	int origLen = strlen(displayTextChar);
	char* newSubStr;
	newSubStr=displayTextChar;

	for(i=0;i< origLen+1;i++)
	{
		newSubStr = &displayTextChar[i];
		if (MultipleChoice > 0)
		{
			if (strlen(newSubStr) == 0 ||(MultipleChoice == MC_CHOICES))
			{
				break;
			}
			else
				MClen[MultipleChoice - 1]++;
		}
		else if(displayTextChar[i]==NULL ||displayTextChar[i]=='\n')
		{

			wdisplayText[i] = 0;
			
			numOfLines++;
			
			
			counter = fontUI.getPixelLength(&wdisplayText[lineStartIndex]);
			if (counter>longestPart)
				longestPart = counter;
			lineStartIndex = i+1;
			wdisplayText[i] = '\n';

			if (displayTextChar[i] == NULL)
			{
				wdisplayText[i+1] = 0;
				break;
			}
		}

		if ((displayTextChar[i] == NULL&&MultipleChoice > 0) || displayTextChar[i] == '\a')
		{
			wdisplayText[i] = 0;
			counter = fontUI.getPixelLength(&wdisplayText[lineStartIndex]);
			lineStartIndex = i + 1;

			if (MultipleChoice == 0)
				numOfLines++;
			if (counter > longestPart&&MultipleChoice == 0)
				longestPart = counter;
			else if (counter > longestMCpart&&MultipleChoice != 0)
			{
				longestMCpart = counter;
			}

			if (displayTextChar[i] != NULL)
			{
				MultipleChoice++;
				MClen[MultipleChoice - 1] = 0;

				displayTextChar[i] = 0;
				
				MC[MultipleChoice - 1] = i;
			}
		}
	}


	if (numOfLines > 5)//Height/fontHeight / 4)
		yOffset=-numOfLines*fontHeight;

	bool drawOKbox = false;

	if (showTime == 0 && MultipleChoice == 0)
	{
		drawOKbox = true;
	}
	
	if(!pausePlayback)
		pauseTime=messageTime-mazeStart;

	if (hideRenderedMaze&&showStyle!=TEXTBOXSTYLE_ACTIVEREGION_ONSCREEN) // Manual override of textbox styles for certain cases
		showStyle = TEXTBOXSTYLE_ONDIALOG_CLEAR_BK;

	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	if(showStyle==TEXTBOXSTYLE_ONDIALOG_CLEAR_BK || showStyle == TEXTBOXSTYLE_ONSCREEN_CLEAR_BK || showStyle==TEXTBOXSTYLE_WARNING)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	            // Clear The Screen And The Depth Buffer

						
	glMatrixMode(GL_PROJECTION);					
						
	glPushMatrix();
	glLoadIdentity();	
	glOrtho(0, Width, 0, Height,-1,1);					

	glEnable(GL_TEXTURE_2D);

	GLubyte* screenCap;	// Captures current screen and sets as background
	screenCap=new GLubyte[4*Width*Height];

	glReadPixels(0,0,Width,Height,GL_RGBA,GL_UNSIGNED_BYTE,screenCap);
	glGenTextures(1, &screenCapID);

	glBindTexture(GL_TEXTURE_2D, screenCapID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,Width, Height, 0,GL_RGBA, GL_UNSIGNED_BYTE, screenCap);		

	free(screenCap);
	if(shadersCompiled)
		glUseProgram(0);

	char* message;
	

	
	
	/*glBegin(GL_QUADS); //Black Background
	glVertex2f(Width / 2 - longestPart / 2 * 13 - 20, Height / 2 - numOfLines * 30 + 20 - 40 * (showTime == 0 && MultipleChoice == 0));
	glVertex2f(Width / 2 + longestPart / 2 * 13 + 20, Height / 2 - numOfLines * 30 + 20 - 40 * (showTime == 0 && MultipleChoice == 0));
	glVertex2f(Width / 2 + longestPart / 2 * 13 + 20, Height / 2 + 80);
	glVertex2f(Width / 2 - longestPart / 2 * 13 - 20, Height / 2 + 80);
	glEnd();*/

	int mw = longestPart +8*spaceLength;
	int mh = (numOfLines+1) * 1.5*fontheight;
	OptionBox mainText = OptionBox(&displayTextChar[0], -1, -mw / 2, 80+yOffset, mw, mh);
	if (showStyle == TEXTBOXSTYLE_ONSCREEN_CLEAR_BK)
		mainText.SetDrawBG(false);
	
	
	int OKlength = fontUI.getPixelLength(L"Ok")*1.5;
	int OKheight = fontHeight * 2;

	if (drawOKbox)
	{
		mainText.SetBorderWidth(0);
		
		OptionBox bigBorder = OptionBox("", -1, -mw / 2, 80 - OKheight*1.5 + yOffset, mw + 2, mh + 2 + OKheight*1.5);
		boxList.push_back(bigBorder);
		
		
	}
	
	if(strlen(displayTextChar)>0) //draw text at all?
		boxList.push_back(mainText);
	
	
	if (showStyle == TEXTBOXSTYLE_WARNING)
	{
		OptionBox header = OptionBox("WARNING", -1, -mw / 2 + 0, mh + 80 + yOffset, mw + 2, 2 * fontHeight);
		header.SetBorderWidth(1);
		header.SetBorderColor(1, 1, 0);
		boxList.push_back(header);
	}
	else if(showStyle== TEXTBOXSTYLE_FATAL_ERROR)
	{
		OptionBox header = OptionBox("ERROR", -1, -mw / 2 + 0, mh + 80 + yOffset, mw + 2, 2 * fontHeight);
		header.SetBorderWidth(1);
		header.SetBorderColor(1, 0, 0);
		boxList.push_back(header);
	}

	int w = 300;
	int h = fontHeight*2;

	for (i = 0; i < MultipleChoice; i++)
	{

		message = &displayTextChar[MC[i] + 1];
		
		w = longestMCpart+60;
	
		boxList.push_back(OptionBox(message, 2, -w / 2, 40 - 1*fontheight - (h + 10) * (i + 1) + yOffset, w, h));
	}

	if (drawOKbox)
	{
		// move ok code here
		w = OKlength;
		h = OKheight;
		OptionBox okBox = OptionBox("Ok", 1, -w / 2, 80 - OKheight + yOffset, w, h);
		if (showStyle == TEXTBOXSTYLE_WARNING||TEXTBOXSTYLE_FATAL_ERROR)
		{
			okBox.SetSelectColor(0.2, 0.2, 0.2);
			okBox.SetBorderColor(1, 1, 0);
			okBox.SetBorderWidth(3);
		}
		boxList.push_back(okBox);
	}

	int selectedIndex = 0;
	POINT prevMousePos = GetOGLPos();
	POINT mouse = GetOGLPos();						// Stores The X And Y Coords For The Current Mouse Position
	glPushMatrix();
	bool clicked = false;
	bool mouseSelect = false;

	bool selectNext = false;
	bool selectPrev = false;

	DWORD minTime = 100;
	DWORD curMsgTime;

	while(res==0)
	{
		
		curMsgTime = GetQPC() - messageTime;
		
		if(curMsgTime>showTime&& (showTime>0))
			res=1;
		else if(curMsgTime>minTime){
			if (keys[VK_ESCAPE])
			{
				res = 1;
				keys[VK_ESCAPE] = false;
			}
			if (keys[ControlLibrary[Jump]] && (GetQPC() - messageTime) > minTime)
			{
				if (showTime == 0 || MultipleChoice > 0)
				{
					clicked = true;
				}
				keys[ControlLibrary[Jump]] = false;
			}
			if (keys[VK_RETURN])
			{
				if (showTime == 0 || MultipleChoice > 0)
				{
					clicked = true;
					mouseSelect = false;
				}
				keys[VK_RETURN] = false;
			}
			if (keys[VK_RIGHT] || keys[VK_DOWN])
			{
				selectNext = true;
				selectPrev = false;
				mouseSelect = false;
				keys[VK_RIGHT] = false;
				keys[VK_DOWN] = false;
			}
			if (keys[VK_LEFT] || keys[VK_UP])
			{
				selectPrev = true;
				selectNext = false;
				mouseSelect = false;
				keys[VK_LEFT] = false;
				keys[VK_UP] = false;
			}

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Check for message in the queue
			{
				if (msg.message == WM_LBUTTONDOWN)
				{
					clicked = true;
				}
				else									// If Not, Deal With Window Messages
				{
					TranslateMessage(&msg);				// Translate The Message
					DispatchMessage(&msg);				// Dispatch The Message
				}
			}
		}
		else
		{
			keys[VK_LEFT] = false;
			keys[VK_UP] = false;
			keys[VK_RIGHT] = false;
			keys[VK_DOWN] = false;
			keys[ControlLibrary[Jump]] = false;
			keys[VK_RETURN] = false;
		}
		/*
		if (shadersCompiled)
		{
			glUseProgram(shaderProgBasic);
		}
		*/
		
		glLoadIdentity();
		gluOrtho2D(0, Width, 0, Height);
		glColor3f(1, 1, 1);
		glEnable(GL_TEXTURE_2D);

		if(texID!=0)
			glBindTexture(GL_TEXTURE_2D, texID);
		else if(!oculusEnabled)
			glBindTexture(GL_TEXTURE_2D, screenCapID);
		else
			glBindTexture(GL_TEXTURE_2D, screenCapID);
		
			
		glBegin(GL_QUADS); //Background Box
			glTexCoord2d(0,0); glVertex2f(0, 0);
			glTexCoord2d(1,0); glVertex2f(Width, 0);
			glTexCoord2d(1,1); glVertex2f(Width, Height);
			glTexCoord2d(0,1); glVertex2f(0, Height);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, NULL);
		
		
		prevMousePos = mouse;
		mouse = GetOGLPos();						// Gets The Current Cursor Coordinates (Mouse Coordinates)
		
		if (mouse.x != prevMousePos.x||mouse.y!=prevMousePos.y)
			mouseSelect = true;

		int boxIndex = -1;

		
		bool selectCheck = false;



		for (std::list<OptionBox>::const_iterator iterator = boxList.begin(), end = boxList.end(); iterator != end; ++iterator) 
		{
			boxIndex++;
			OptionBox box = (OptionBox)*iterator;

			if (selectedIndex == boxIndex)
			{
				if (box.GetRes() == -1)
				{
					selectNext = true;
					if (selectCheck)
					{
						selectNext = false;
						selectPrev = true;
						selectCheck = false;
					}
				}
				if (selectNext)
				{
					selectedIndex++;
					selectNext = false;
				}
				if (selectPrev)
				{
					selectedIndex--;
					selectCheck = true;
					selectPrev = false;
				}

				if (selectedIndex >= boxList.size())
					selectedIndex = boxList.size() - 1;
				if (selectedIndex < 0)
				{
					selectPrev = false;
					selectCheck = false;
					selectedIndex = 0;
				}
			}
			
			if (mouseSelect)
			{
				box.Select(box.IsMouseOver(mouse.x, mouse.y));
			}
			else
			{
				if (boxIndex == selectedIndex)
				{
					box.Select(true);
				}
				else
					box.Select(false);
			}

			if (box.Selected())
			{
				selectedIndex = boxIndex;
				selectCheck = false;
				selectNext = false;
				selectPrev = false;
			}

			if (box.Selected()&&clicked)
			{
				EventLog(2,box.GetRes() + 200, boxIndex, box.GetText());
				res = box.GetRes();
			}
			box.Draw();
		}


		DrawMouse();


		SwapBuffers(hDC); //GUIMessageBox menu
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	            // Clear The Screen And The Depth Buffer
		clicked = false;
	}
	
	boxList.clear();
	glPopMatrix();
	for (int z=UpdateTimeBufferCount;z>0;z--)
	{
		updateTimeFrame[z]=0;
	}
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing		
	glEnable(GL_TEXTURE_2D);

	 glDeleteTextures(1, &screenCapID);
     screenCapID=0;


	if(shadersCompiled&&shader)
		glUseProgram(shaderProg);

	if(res!=0)
	{
		if(bMouseInput&&curMazeListItem!=NULL&&curMazeListItem->type==Maze)
			SetCursorPos(objCamera.mid_x,objCamera.mid_y);
		

		if(!pausePlayback)
		{	
			
			mazeStart=GetQPC()-pauseTime;
			pauseTime=0;
			updateTime=GetQPC();
			updateTime2=GetQPC();
		}
	}
}

int DrawHUD()
{
	if (shadersCompiled)
	{
		glUseProgram(shaderProgBasic);
	}
	

	glDisable(GL_LIGHTING);
	glLoadIdentity();

	if(barScore<targetBarScore)
		barScore+=1;
	if(barScore>targetBarScore)
		barScore-=1;
	if(barScore>100)
		barScore=100;
	if(barScore<0)
		barScore=0;

	float width=Width;
	float height=Height;

	glColor3f(1,1,1);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	gluOrtho2D(0, width, 0, height);

	
	if(bShowBar && pausePlayback==false)
	{
		
	   glBegin(GL_QUADS); //White Border
		glVertex2f(width-31, 19);
	   glVertex2f(width-9, 19);
	   glVertex2f(width-9, height-19);
	   glVertex2f(width-31, height-19);
	   glEnd();



	   glColor3f(0,0,0);
	   glBegin(GL_QUADS); //Black Background
	   glVertex2f(width-30, 20);
	   glVertex2f(width-10, 20);
	   glVertex2f(width-10, height-20);
	   glVertex2f(width-30, height-20);
	   glEnd();

	   float minVal=0, maxVal,barVal;

	   if (iScoreBarMode == 1) //MazePoints
	   {
		   minVal = 0;
		   maxVal = max(objMap->pointExitThreshold, 0);
		   if (objMap->pointExitThreshold == 0)
			   maxVal = 10;
		   if (objMap->pointExitThreshold < 0)
			   maxVal = -1*objMap->pointExitThreshold;
		   barVal = min(max(curMazePoints, 0), maxVal);
		   
	   }
	   else if (iScoreBarMode == 0) //API Score
	   {
		   minVal = 0;
		   barVal = min(max(barScore,0),100);
		   maxVal = 100;
	   }
	   
	   

	   glColor3f(((barVal - minVal) / (maxVal - minVal)),0,(1- (barVal - minVal) / (maxVal - minVal)));
	   glBegin(GL_QUADS); //Red Bar
	   glVertex2f(width-30, 30);
	   glVertex2f(width-10, 30);
	   glVertex2f(width-10, (height-60)*(barVal-minVal) /(maxVal-minVal)+30);
	   glVertex2f(width-30, (height-60)*(barVal - minVal) / (maxVal - minVal) +30);
	   glEnd();

	   glBegin(GL_TRIANGLE_FAN); //circle barometer top
		   glVertex2f(width-20,(height-60)*(barVal - minVal) / (maxVal - minVal) +30);
		   for (float angle = 0; angle<360; angle+=5)
			   glVertex2f(width-20 + sin(angle) * 10.0, (height-60)*(barVal - minVal) / (maxVal - minVal) +30 + cos(angle) * 10.0);
			   glEnd();

		glBegin(GL_TRIANGLE_FAN); //circle curving at bottom
			   glVertex2f(width-20,30);
			   for (float angle = 0; angle<360; angle+=5)
				   glVertex2f(width-20 + sin(angle) * 10.0, 30 + cos(angle) * 10.0);
			   glEnd();

	}
	char temp[100];
	if(bShowTimer||videoPlayBack)
	{
		
		if(!pausePlayback||interactivePlayback)
		{
			glColor3f(0.1f,0.1f,0.9f);
			sprintf(temp,"%.2f",(GetQPC()-mazeStart)/1000.0);

		}
		else
		{
			glColor3f(1,0,0);
			sprintf(temp,"Paused %.2f",(pauseTime/1000.0));
		}
		/*glColor3f(1,1,1);
		glBegin(GL_QUADS); //White Border
		glVertex2f(width-10-(70+14*strlen(temp)), 16);
		glVertex2f(width-75, 16);
		glVertex2f(width-75, 40);
		glVertex2f(width-10-(70+14*strlen(temp)), 40);
		glEnd();
		glColor3f(0,0,0);
		glBegin(GL_QUADS); //White Border
		glVertex2f(width-9-(70+14*strlen(temp)), 17);
		glVertex2f(width-76, 17);
		glVertex2f(width-76, 39);
		glVertex2f(width-9-(70+14*strlen(temp)), 39);
		glEnd();*/

		if(!pausePlayback||interactivePlayback)
			glColor3f(0.1f,0.1f,0.9f);
		else
			glColor3f(1,0,0);
		if(bShowBar)
			glRasterPos2f(width-45,2*fontheight*1.1);
		else
			glRasterPos2f(width - 15, 2 * fontheight*1.1);
		glPrint(2,"%s",temp);
	}

	if(!Player.collisionEnabled&&objCamera.Camera3D)
	{
		glColor3f(1,0,0);
		glRasterPos2f(20,height-fontheight*2*1.1);
		glPrint(false,"%s","GhostMode");
	}

#ifdef _DEBUG
	devFPS=true;
#endif

	if(devFPS)
	{
		glColor3f(0,0,0);

	   glBegin(GL_QUADS); //Black Background
	   glVertex2f(width-200, height-120);
	   glVertex2f(width-200, height-120);
	   glVertex2f(width, height);
	   glVertex2f(width, height);
	   glEnd();

		glColor3f(1,0,0); //FrameRate
		glRasterPos2f(width - 250, height - 1 * fontheight*1.1);
		glPrint(false,"FPS:%.0f",frameRate);

		glColor3f(1,0,0); //Item 2
		glRasterPos2f(width - 250, height - 2 * fontheight*1.1);
		glPrint(false,"POS:%.1f %.1f  %.1f",objCamera.mPos.x,objCamera.mPos.y,objCamera.mPos.z);

		glColor3f(1,0,0); //Item 3
		glRasterPos2f(width - 275, height - 3 * fontheight*1.1);
		//glPrint("SV:%.2f",fallSpeed);
		glPrint(false, "SV:%0.2f %10.8f", specialVar, specialVar2);


		glColor3f(1,0,0); //Item 3
		glRasterPos2f(width - 250, height - 4 * fontheight*1.1);
		glPrint(false,"%i  %i",objMap->modelsDrawn,objMap->wallsDrawn);//objCamera.cameraSpeedView/frameRate*100000);
	}

	
	if(showDebugMessage)
	{
		DWORD debugDT=GetQPC()-debugMessageTime;
		if(debugDT>1500)
			glColor4f(1-(float)(debugDT-1500.0f)/500.0f,0,0,1-(float)(debugDT-1500.0f)/500.0f);
		if(debugDT>2000)
			showDebugMessage=false;
		
		glRasterPos2f(20,height-1*fontheight*1.1);
		glPrint(false,"%s",debugMessage);
		
	}

	if((interactivePlayback&&pausePlayback&&askForResponse&&Player.testPass)||devMsgStatus==2)
	{
		
		//bShowBar=false;
		glColor3f(1,0,0);
		glRasterPos2f(width/2,height/2);
		if(devMsgStatus!=2)
			glPrint(false,"%s","Please Select Task Difficulty");
		else
		{
			glPrint(false,"%s",devMsg);
		}
		glColor3f(0,0,0);
		glBegin(GL_TRIANGLE_FAN); //circle background
		glVertex2f(width/2,height/2-30);
		for (float angle = 0; angle<360; angle+=5)
			glVertex2f(width/2 + sin(angle) * 20.0, height/2-30 + cos(angle) * 20.0);
		glEnd();

		if(userResponse!=0)
		{
			sprintf(temp,"%.0f",userResponse);
			glColor3f(1,1,1);
			glRasterPos2f(width/2,height/2-1*fontheight*1.1);			
			glPrint(false,"%s",temp);
		}
		glColor3f(1,1,1);
		glBegin(GL_QUADS); //White Border
		glVertex2f(98, height/2-59);
		glVertex2f(width-99, height/2-59);
		glVertex2f(width-99, height/2-91);
		glVertex2f(98, height/2-91);
		glEnd();
		glColor3f(0,0,0);
		glBegin(GL_QUADS); //Black Background
		glVertex2f(100, height/2-60);
		glVertex2f(width-100, height/2-60);
		glVertex2f(width-100, height/2-90);
		glVertex2f(100, height/2-90);
		glEnd();

		glColor3f(1*userResponse/10,0,0);
		glBegin(GL_QUADS); //Red diagonal Background
		glVertex2f(100, height/2-90);
		glVertex2f(width-100, height/2-60);
		glVertex2f(width-100, height/2-90);
		glVertex2f(100, height/2-90);
		glEnd();
		if(userResponse>=0.5)
		{
			glColor3f(1,1,1);
			glBegin(GL_QUADS); //Fill Bar
			glVertex2f(100+(width-220)*(userResponse-1)/9, height/2-60);
			glVertex2f(120+(width-220)*(userResponse-1)/9, height/2-60);
	
			glVertex2f(120+(width-220)*(userResponse-1)/9, height/2-90);
			glVertex2f(100+(width-220)*(userResponse-1)/9, height/2-90);
			glEnd();
		}
	}

	if(bShowCrosshair)
		DrawCrosshair();

	//if (bTopDownMouseAngleMode&&bTopDown)
		//DrawMouse();
	
	
	glMatrixMode(GL_PROJECTION);

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	glColor3f(1,1,1);
   return 0;
}

void setDebugMessage(char* str)
{
	debugMessage = new char[256];
	strcpy(debugMessage,str);
	//sprintf(debugMessage,"");
	//strcat(debugMessage,str);
	debugMessageTime=GetQPC();
	showDebugMessage=true;
}

int frameUpdate=10; //updateTime for FrameRate and Movement
DWORD tcpStepTime=9999999; //update time for tcp step
#include <string.h>
#include <locale.h>

bool DeleteDirectory(LPCTSTR lpszDir, bool noRecycleBin = true)
{
	LPTSTR iDir = new char[MAX_PATH];

	DWORD a = GetCurrentDirectory(MAX_PATH, iDir);

	
	char* substr = &iDir[strlen(iDir) - strlen(lpszDir)];
	
	for (int i = 0; i < strlen(substr) && i < strlen(lpszDir); i++)
	{
		if (substr[i] != lpszDir[i])
			return false;
	}

	int len = strlen(iDir);
	TCHAR *pszFrom = new TCHAR[len + 2];
	strcpy(pszFrom, iDir);
	pszFrom[len] = 0;
	pszFrom[len + 1] = 0;

	SHFILEOPSTRUCT fileop;
	fileop.hwnd = NULL;    // no status display
	fileop.wFunc = FO_DELETE;  // delete operation
	fileop.pFrom = pszFrom;  // source file name as double null terminated string
	fileop.pTo = NULL;    // no destination needed
	fileop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;  // do not prompt the user

	if (!noRecycleBin)
		fileop.fFlags |= FOF_ALLOWUNDO;

	fileop.fAnyOperationsAborted = FALSE;
	fileop.lpszProgressTitle = NULL;
	fileop.hNameMappings = NULL;

	int ret = SHFileOperation(&fileop);
	delete[] pszFrom;

//	RemoveDirectory(iDir);

	return (ret == 0);
}

float* quaternionToEuler (float qY, float qX, float qZ, float qW)
{
    float matrix[3][3];
    float cx,sx;
    float cy,sy,yr;
    float cz,sz;
	float eX,eY,eZ;

    matrix[0][0] = 1.0f - 2.0f * (qY * qY + qZ * qZ);
    matrix[0][1] = (2.0f * qX * qY) - (2.0f * qW * qZ);
    matrix[1][0] = 2.0f * (qX * qY + qW * qZ);
    matrix[1][1] = 1.0f - (2.0f * qX * qX) - (2.0f * qZ * qZ);
    matrix[2][0] = 2.0f * (qX * qZ - qW * qY);
    matrix[2][1] = 2.0f * (qY * qZ + qW * qX);
    matrix[2][2] = 1.0f - 2.0f * (qX * qX - qY * qY);


    sy = -matrix[2][0];
    cy = sqrt(1 - (sy * sy));
    yr = (float)atan2(sy,cy);
    eY = (yr * 180.0f) / (float)M_PI;

    if (sy != 1.0f && sy != -1.0f)   
    {
        cx = matrix[2][2] / cy;
        sx = matrix[2][1] / cy;
        eX = ((float)atan2(sx,cx) * 180.0f) / (float)M_PI;   // RAD TO DEG

        cz = matrix[0][0] / cy;
        sz = matrix[1][0] / cy;
        eZ = ((float)atan2(sz,cz) * 180.0f) / (float)M_PI;   // RAD TO DEG
    }
    else
    {
        cx = matrix[1][1];
        sx = -matrix[1][2];
        eX = ((float)atan2(sx,cx) * 180.0f) / (float)M_PI;   // RAD TO DEG

        cz = 1.0f;
        sz = 0.0f;
        eZ = ((float)atan2(sz,cz) * 180.0f) / (float)M_PI;   // RAD TO DEG
    }    


	float ret[3];
	ret[0]=eY;
	ret[1]=eX;
	ret[2]=eZ;
	
	
	return ret;
}
volatile bool alignCameraFlag = false; //used to force alignment via API
tVector3 newAPIpos; //Assigned Via API
tVector3 newAPIview; //Assigned Via API
volatile float apiRotateViewAngleTo=-999; //Assigned Via API

void GameUpdate()
{
	if(!pausePlayback)
	{


		if (alignCameraFlag)
		{
			objCamera.mPos = newAPIpos;
			objCamera.mView = newAPIview;

			objCamera.mPos.y -= Player.offset; //adjust view down to real positions
			objCamera.mView.y -= Player.offset;
			if (apiRotateViewAngleTo > -900)
			{
				objCamera.Rotate_View(apiRotateViewAngleTo);
				apiRotateViewAngleTo = -999;
			}
			alignCameraFlag = false;
			AlignCamera();
		}
		else
		{
			objCamera.mPos.y -= Player.offset; //adjust view down to real positions
			objCamera.mView.y -= Player.offset;
		}


		

		
		int timeSince;

		timeSince=GetQPC()-updateTime;

		//if(score>50) //for frame rate modification testing
		//	Sleep(1);
		//else
		//	Sleep(50);
		//
		
		updatesSec++;
		if(GetQPC()-timeSince3>frameUpdate)//gets framerate
		{	
			frameRate = ((double)updatesSec*1000.0f / (double)(GetQPC() - timeSince3));
			smoothFrameRate=smoothFrameRate*(double)(2.0f/3.0f)+frameRate/(double)3.0f;
			timeSince3=GetQPC();
			updatesSec=0;
		}


		SetLights();

		//if(timeSince<5) //maybe everything is too fast
		//	Sleep(1);
		
		double movePercent;

		if(timeSince<1000)
		{
			
			
			int updateTimeFrameSum=0;
			tVector3 initPos=objCamera.mPos,initView=objCamera.mView;
			float move_angle = -1000;

			int ret;

			
			for (int z=UpdateTimeBufferCount;z>0;z--)
			{
				if(updateTimeFrame[z-1]>=0)
					updateTimeFrame[z]=updateTimeFrame[z-1];
				updateTimeFrameSum+=(int)updateTimeFrame[z];
			}
			
			float moveSpeed=objCamera.cameraSpeedMove/Player.halfSpeed;
			//(float)updateTimeFrameSum/(UpdateTimeBufferCount*20.0f);
			float timeA=objCamera.cameraSpeedMove;//*(float)updateTimeFrameSum/(UpdateTimeBufferCount*20.0f);

			//objCamera.cameraSpeedView=1.0f;

			if(objCamera.Camera3D) // Move twice as fast in ghostmode
				moveSpeed *= 30;

			for(int tcpSteps=(GetQPC()-tcpStepTime)/10;tcpSteps>0;tcpSteps--) //1 step for each 10ms
			{
				Tcp_Instant();
				tcpStepTime=GetQPC();
				if(GetQPC()-mazeStart<500)
					tcpSteps=0;
				if(tcpSteps>1000000)
					break;
			}


			Player.gravityEnabled = !bDisableGravity;

			if(Player.movingForward.state)
				move_angle = 0;
			if(Player.movingBackwards.state)
			{
				move_angle = 180;
			}
			if(Player.strafingRight.state)
			{
				move_angle = 90;
				if(Player.movingForward.state)
					move_angle -= 45;
				if(Player.movingBackwards.state)
					move_angle += 45;
			}
			if(Player.strafingLeft.state)
			{
				move_angle = -90;
				if(Player.movingForward.state)
					move_angle += 45;
				if(Player.movingBackwards.state)
					move_angle -= 45;
			}

			if(Player.movingBackwards.state&&objCamera.Camera3D)
			{
				moveSpeed = -moveSpeed;
				move_angle = 0;
				if(Player.strafingLeft.state)
					move_angle += 45;
				if(Player.strafingRight.state)
					move_angle -= 45;
			}


			
			if (move_angle>-500 && moveSpeed != 0)
			{	
				move_angle *= PI / 180;
				
				tVector3 vVector=objCamera.Rotate_View(move_angle);
				initPos=objCamera.mPos;
				initView=objCamera.mView;
				ret = objCamera.Move_Camera(moveSpeed); 
				
				
				objCamera.Set_vVector(vVector);
			}
			else // No walking direction
			{
				ret=objCamera.Move_Camera(0); //allow gravity and stop regular movement
			}

			if(Player.interact==true)
			{
				//barScore=100;
				//targetScore=100;


			}
			


			if(Player.justJumped.state==true&&controller->canJump()&&Player.gravityEnabled)
			{
  				Player.justJumped.state=false;
				controller->setJumpSpeed(3);
				controller->setFallSpeed(2);
				controller->setMaxJumpHeight(1);
				
				controller->jump(btVector3(0, 2.95f, 0));

				/*Player.jumpSpeed=3.0f;
				Player.jumping=true;*/
				
			}

			/*

			if(Player.jumpSpeed<=0.1f&&Player.jumping)
			{
				jumpSpeed=0;
				Player.jumpSpeed=0;
				if(!falling)wwwwwwwwwwwwww
					Player.jumping=false;
			}
*/

			if(Player.lookingLeft.state)
			{	
				objCamera.Rotate_View(-objCamera.cameraSpeedView / smoothFrameRate * 10.0f, true);
			}

			if(Player.lookingRight.state)
			{	
				objCamera.Rotate_View(objCamera.cameraSpeedView / smoothFrameRate * 10.0f, true);
			}
			if(Player.lookingUp.state)
			{	
				objCamera.LookUpDown(objCamera.cameraSpeedView / 2.0f / smoothFrameRate);
			}

			if(Player.lookingDown.state)
			{	
				objCamera.LookUpDown(-objCamera.cameraSpeedView / 2.0f / smoothFrameRate); 
				
			}


			if (!Player.gravityEnabled)
			{
				controller->setFallSpeed(0);
			}
			
			if(objCamera.mPos.y<-30||abs(objCamera.mPos.x)>100000||abs(objCamera.mPos.z)>100000||_isnan(objCamera.mPos.x)||_isnan(objCamera.mPos.z)||_isnan(objCamera.mPos.y)) //Reset if player falls
			{
				objCamera.mPos=objCamera.startPos;
				objCamera.mView=objCamera.startView;

				AlignCamera();

			}
			

			if (objMap->bEndPos)
			{
				//check for end...
				EndRegion* endRegion = objMap->endRegionHead;
				while(endRegion)
				{
					if (curMazePoints >= endRegion->pointThreshold&&objCamera.mPos.x> endRegion->xmin && objCamera.mPos.x<endRegion->xmax && objCamera.mPos.z>endRegion->zmin && objCamera.mPos.z<endRegion->zmax&&objCamera.mPos.y<(endRegion->height / 2 + endRegion->offset) && objCamera.mPos.y>(-1 * endRegion->height / 2 + endRegion->offset))
					{
						if (endRegion->moveToPos == 0)
						{
							bMazeEndReached = true;
							bMazeLoop = false;
							if (LogStatus())
							{
								char msg[800];
								sprintf(msg, "-1\tMaze is completed: Return Value %i\n", endRegion->returnValue);
								AddToLog(msg);
							}
							EventLog(3,100+endRegion->returnValue,0,"Return Value");
							mazeReturnValue = endRegion->returnValue;
							sprintf(objMap->successMessage, endRegion->messageText);
						}
						else
						{

							StartPosition * sPos = getStartPosByIndex(endRegion->moveToPos);
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
					}
					if (endRegion->last)
						break;
					endRegion = endRegion->next;
				} 
			}


			specialVar = curMazePoints;

			if (objMap->activeRegionHead)
			{
				//check for active Region
				ActiveRegion* activeRegion = objMap->activeRegionHead;
				while (activeRegion)
				{
					if (activeRegion->activated)
					{
						activeRegion = activeRegion->next;
						continue;
					}

					if (objCamera.mPos.x > activeRegion->xmin && objCamera.mPos.x<activeRegion->xmax && objCamera.mPos.z>activeRegion->zmin && objCamera.mPos.z < activeRegion->zmax&&objCamera.mPos.y<(activeRegion->height / 2 + activeRegion->offset) && objCamera.mPos.y>(-1 * activeRegion->height / 2 + activeRegion->offset))
					{
						if (!activeRegion->highlighted)
						{
							bool highlightObject = true;
							if (GetQPC() - mazeStart < activeRegion->p1TriggerTime) //if time is specified then its required
								highlightObject = false;

							if (curMazePoints < activeRegion->p1PointThreshold)
								highlightObject = false;

							if (activeRegion->p2InteractRequired)
							{
								iUserStatus = USER_STATUS_HIGHLIGHT;
							}

							activeRegion->Highlight(highlightObject);
						}
						else if (!activeRegion->activated&&activeRegion->highlighted)
						{
							bool activateObject = true;

							if (GetQPC() - activeRegion->highlightTime < activeRegion->p2TriggerTime) //if P2Trigertime is specified then it is required
								activateObject = false;

							if (curMazePoints < activeRegion->p2PointThreshold)
								activateObject = false;

							if (!Player.interact&&activeRegion->p2InteractRequired)//barScore < BAR_THRESHOLD
							{
								activateObject = false;
							}
							else
								iUserStatus = USER_STATUS_TRIGGER;

							if (activateObject)
								activeRegion->Activate();
						}

						if (activeRegion->highlighted&&activeRegion->highlightAudioBehavior == 3)
						{
							activeRegion->CalcDistance(objCamera.mPos.x, objCamera.mPos.y, objCamera.mPos.z);
							float factor = min(activeRegion->xmax - activeRegion->xmin, activeRegion->zmax - activeRegion->zmin)/2.0f;
							float volume = (factor-activeRegion->distToPlayer)/factor;
							volume = min(max(volume,0),1);
							curAudioDict.SetVolume(activeRegion->highlightAudioID, volume);
						}
						//
					}
					else
					{
						activeRegion->Highlight(false);

					}
				


				activeRegion = activeRegion->next;
				}
			}

			

		

			//////////////////////////////////////////////
			//Object Status Code Here
			numHighlighted=0; //reset Highlight Count
			MapModel *temp2;
			temp2=MapModelHead;
				
			while(temp2)
			{
				
				if (temp2->destroyed||!temp2->kinematic&&!temp2->dynamic) // no static (non-kinematic) objects past this point
				{
					temp2 = temp2->next;
					continue;
				}
				if(!temp2->moving&&temp2->collide&&temp2->kinematic) //If its a kinematic object, move it
				{
					btTransform objWorldTrans=temp2->body->getWorldTransform();
					btVector3 origin=objWorldTrans.getOrigin();
					btQuaternion rotTrans=objWorldTrans.getRotation();

					float* euler;
					euler=quaternionToEuler(rotTrans.getY(),rotTrans.getX(),rotTrans.getZ(),rotTrans.getW());

					temp2->rot.y=euler[0];
					temp2->rot.x=euler[1];
					temp2->rot.z=euler[2];

					temp2->pos.x=origin.getX();
					temp2->pos.y=origin.getY();
					temp2->pos.z=origin.getZ();
				}

				if (temp2->pos.y<-30)
				{
					temp2->destroy = true;
				}

				if (temp2->destroy && !temp2->destroyed) //destroy flag check
				{
					temp2->activated = true;
					temp2->destroyed = true;

					temp2->actionTime = 0.5;
					temp2->moving = true;
					temp2->collide = false;
					temp2->endPos = temp2->pos;
					temp2->endRot = temp2->rot;
					temp2->endRot.y += 720;
					temp2->endScale = 0.001;
					temp2->targetPercentage = 100;
					temp2->isVisible = false;

					if (temp2->body)
						m_dynamicsWorld->removeRigidBody(temp2->body);

				}

				if (!temp2->dynamic||temp2->destroyed) // no regular static models past this point
				{
					temp2 = temp2->next;
					continue;
				}

				if (!temp2->activated)//&&!temp2->highlighted) //If not active/highlighted should we highlight?
				{
					bool highlightObject = true;
					if (GetQPC() - mazeStart < temp2->p1TriggerTime) //if time is specified then its required
						highlightObject = false;

					if (temp2->proximityTrigger < 1 && temp2->p1ActiveRadius > 0) //if not in range and range is required
						highlightObject = false;
					if (curMazePoints < temp2->p1PointThreshold)
						highlightObject = false;

					if (temp2->p2InteractRequired)
					{
						iUserStatus = USER_STATUS_HIGHLIGHT;
					}

					temp2->Highlight(highlightObject);
				}
				if (!temp2->activated&&temp2->highlighted) // if highlighted, should we activate?
				{
					bool activateObject = true;

					if (GetQPC() - temp2->highlightTime<temp2->p2TriggerTime) //if P2Trigertime is specified then it is required
						activateObject = false;

					if (temp2->proximityTrigger < 2 && temp2->p2ActiveRadius > 0) //if not in range and range is required
						activateObject = false;

					if (curMazePoints < temp2->p2PointThreshold)
						activateObject = false;
					
					if (!Player.interact&&temp2->p2InteractRequired) //barScore<BAR_THRESHOLD
					{
						activateObject = false;
					}
					else
						iUserStatus = USER_STATUS_TRIGGER;

					if (activateObject)
						temp2->Activate();
				}

				//if(temp2->highlighted&&GetQPC()-temp2->highlightTime>temp2->p2TriggerTime) //check if past highlight time and not activated, then highlight
				//{
				//	if(temp2->triggerCriteria==2)
				//		temp2->Activate();
				//	else if(temp2->triggerCriteria==4) //change from prox+time to prox
				//		temp2->triggerCriteria=1;
				//	else if(temp2->triggerCriteria==5) //change from device+time to device
				//		temp2->triggerCriteria=3;
				//	else if(temp2->triggerCriteria==7) //change from prox+device+time to prox+device
				//		temp2->triggerCriteria=6;
				//}


				//	
				//if(temp2->highlighted&&(temp2->triggerCriteria==3||(temp2->triggerCriteria==6&&temp2->proximityTrigger))) //trigger if device, or proximity and device
				//{
				//	//test types here						
				//		
				//	iUserStatus = USER_STATUS_HIGHLIGHT;
				//	if(score>BAR_THRESHOLD)
				//	{			
				//		if(GetQPC() - temp2->highlightTime > temp2->p2TriggerTime)
				//		{							
				//			temp2->Activate();
				//			numHighlighted = 0;
				//			iUserStatus = USER_STATUS_TRIGGER;
				//		}
				//	}
				//}



				//if(temp2->proximityTrigger!=0&&!temp2->activated) //Player is in active radius, but object is inactive
				//{
				//	if(temp2->highlightCriteria==1)
				//	{
				//		temp2->Highlight(true);
				//	}
				//	if(temp2->highlighted&&temp2->triggerCriteria==1&&temp2->proximityTrigger==2) //prox activation criteria
				//	{
				//		temp2->Activate();
				//	}
				//		
				//}

				//if(temp2->proximityTrigger==0&&(temp2->highlightCriteria==1||temp2->highlightCriteria==3)) //if player not in radius AND proximity is part of highlighting, then unhighlight
				//{
				//	temp2->Highlight(false);
				//}



				if(temp2->highlighted&&!temp2->activated)
				{
					numHighlighted++;

					if (temp2->highlightAudioBehavior == 3) //Update conditional audio volume
					{
						curAudioDict.SetVolume(temp2->highlightAudioID, 1 - temp2->distToPlayer / temp2->p1ActiveRadius);
					}

					if(temp2->highlightStyle==1)
					{
						if (temp2->bouncingUp)
							temp2->pos.y+=.003;
						if (!temp2->bouncingUp)
							temp2->pos.y-=.003;
						if (temp2->pos.y-.2>temp2->initPos.y)
							temp2->bouncingUp=false;
						if (temp2->pos.y+.2<(temp2->initPos.y))
							temp2->bouncingUp=true;
						temp2->moving=true;
					}
					if(temp2->highlightStyle==2)
					{
						if (temp2->bouncingUp)
							temp2->pos.x+=.03;
						if (!temp2->bouncingUp)
							temp2->pos.x-=.03;
						if (temp2->pos.x-.05>temp2->initPos.x)
							temp2->bouncingUp=false;
						if (temp2->pos.x+.05<(temp2->initPos.x))
							temp2->bouncingUp=true;
						temp2->moving=true;
					}
					if(temp2->highlightStyle==3)
					{
						temp2->rot.y+=1;
						if(temp2->rot.y>359)
							temp2->rot.y=1;
						temp2->moving=true;
					}
					if(temp2->highlightStyle>3&&temp2->highlightStyle<7)
					{
						if(smoothFrameRate==0)
							smoothFrameRate=1.0f;
						if(temp2->bouncingUp)
							temp2->glowFactor+=1/smoothFrameRate;
						else
							temp2->glowFactor-=1/smoothFrameRate;
						if(temp2->glowFactor>1)
						{
							temp2->bouncingUp=false;
							temp2->glowFactor=1;
						}
						else if (temp2->glowFactor<0)
						{
							temp2->bouncingUp=true;
							temp2->glowFactor=0;
						}
					}
					if(temp2->highlightStyle>=10&&temp2->highlightStyle<=34)
					{
						particleArray *tempCircles;
						tempCircles=partArrayHead;
						while(tempCircles!=NULL)
						{
							if(tempCircles->dID==temp2->dID&&!tempCircles->activated)
							{
								initParticles(tempCircles);
								tempCircles->activated=true;
							}

							tempCircles=tempCircles->next;
						}
					}
						

					if(temp2->highlightStyle>9&&temp2->highlightStyle<35)
					{
						particleArray *tempCircles;
						tempCircles=partArrayHead;
						while(tempCircles!=NULL)
						{
							updateParticles(tempCircles);
							if(tempCircles->dID==temp2->dID&&temp2->highlighted==false)
								tempCircles->activated=false;

							tempCircles=tempCircles->next;
						}
					}
				}
				else if(temp2->glowFactor>0&&!temp2->highlighted)
				{
					temp2->glowFactor-=0.05;
				}
				else if(!temp2->highlighted&&!temp2->activated)
					temp2->moving=false;

				if (temp2->activated)
				{
					if (temp2->switchModel&&temp2->targetModelID != temp2->modelID) //switch model
					{
						GLMmodelNode *temp;
						temp = (GLMmodelNode*)malloc(sizeof(GLMmodelNode));
						temp = ModelDictHead;
						while (temp != NULL&&temp->modelID != temp2->targetModelID)
						{
							temp = temp->next;
						}
						if (temp != NULL)
						{
							temp2->modelData = temp->modelData;
							temp2->MinCoords = temp->MinCoords;
							temp2->MaxCoords = temp->MaxCoords;
							temp2->modelID = temp->modelID;
						}
						temp2->moving = true;
					}


					if (temp2->moving) // handle moving
					{
						if (temp2->currentPercentage <= 0.01)
						{
							temp2->activatedTime = GetQPC();
							temp2->currentPercentage = 0.1;
						}


						temp2->actionTimer = GetQPC() - temp2->activatedTime;

						if (temp2->currentPercentage >= temp2->targetPercentage || temp2->actionTime <= 0) //if at end or time<0 go to end
							movePercent = temp2->targetPercentage;
						else if (temp2->targetPercentage > temp2->currentPercentage)
						{
							double timer = temp2->actionTimer;
							double totalTime = temp2->actionTime;
							movePercent = timer / totalTime*100.0f;
							movePercent = min(movePercent, 100.0f);
							movePercent = max(movePercent, 2.0f);

						}
						/*else if (temp2->targetPercentage<temp2->currentPercentage)
						{
						movePercent=-1/(temp2->actionTime)+temp2->currentPercentage;
						if(movePercent<temp2->targetPercentage)
						movePercent=temp2->targetPercentage;
						}*/

						glmMovePercent(temp2, movePercent);
						if (movePercent >= temp2->targetPercentage&&temp2->dynamic&&temp2->moving)
						{
							EventLog(3, 203, temp2->dID, temp2->name);

							temp2->moving = false;
							temp2->dynamic = false;
							temp2->activated = false;
							if (temp2->collide)
							{
								btTransform groundTransform;
								groundTransform.setIdentity();
								groundTransform.setRotation(btQuaternion(PI / 180.0f*temp2->rot.y, PI / 180.0f*temp2->rot.x, PI / 180.0f*temp2->rot.z));
								groundTransform.setOrigin(btVector3(temp2->pos.x, temp2->pos.y, temp2->pos.z));

								temp2->body->setWorldTransform(groundTransform);
							}
						}

						if (temp2->collide && !temp2->destroy)
						{
							btTransform groundTransform;
							groundTransform.setIdentity();
							groundTransform.setRotation(btQuaternion(PI / 180.0f*temp2->rot.y, PI / 180.0f*temp2->rot.x, PI / 180.0f*temp2->rot.z));
							groundTransform.setOrigin(btVector3(temp2->pos.x, temp2->pos.y, temp2->pos.z));

							temp2->body->setWorldTransform(groundTransform);
						}


					}

				}

				temp2=temp2->next;
			}

			if(Player.interact)
			{
				Player.interact=false;
				//barScore=0;
				//targetScore=0;
			}
		}



		updateTimeFrame[0]=GetQPC()-updateTime2;
		updateTime2=GetQPC();
	
		objCamera.mPos.y+=Player.offset;
		objCamera.mView.y+=Player.offset;
	}

	updateTime=GetQPC();
	if(iUserStatus == USER_STATUS_TRIGGER)
	{	
		if(barScore>BAR_THRESHOLD&&interactivePlayback&&!askForResponse)
		{
			Player.testPass=true;
			pausePlayback=false;
			EventLog(5,0,0);
			iUserStatus = USER_STATUS_NONE;
		}
		if((barScore>BAR_THRESHOLD&&interactivePlayback&&askForResponse)||devMsgStatus==2)
		{
			Player.testPass=true;
			pausePlayback=true;
			EventLog(5,0,0);
			iUserStatus = USER_STATUS_REPORT;
		}
	}
	
}

int lightNum[8];	//array of light indexes
void SetLights() //sorts lights
{
		LightItem* tempL;
		tempL=curLightList.GetFirst();

		
		

		for(int count=0;count<8;count++)
		{
			glDisable(GL_LIGHT0+count); //disable all lights

			if(firstLightRun)
			{
				lightNum[count]=999;
			}
		}
		
		if(curLightList.GetLength()>8||firstLightRun)
		{
			float closestDist=10000; //large number that would be unrealistically far from player
			float maxDist=0;

			while(tempL!=NULL)
			{
				tempL->distFromPlayer=objMap->CalcDistanceToPoint(objCamera.mPos.x,objCamera.mPos.y,objCamera.mPos.z,tempL->LightPosition[0],tempL->LightPosition[1],tempL->LightPosition[2]);
					
				if(tempL->distFromPlayer<closestDist)
				{
					closestDist=tempL->distFromPlayer;
					lightNum[0]=tempL->index;
				}
				if(tempL->distFromPlayer>maxDist)
					maxDist=tempL->distFromPlayer;

				tempL = tempL->next;
			}

			float curDist=10000;
			int curLight=1;

			while(closestDist<maxDist&&curLight<8)
			{
				tempL=curLightList.GetFirst();
				curDist=10000;

				while(tempL!=NULL)
				{
					if(tempL->distFromPlayer<curDist&&tempL->distFromPlayer>closestDist)
					{
						curDist=tempL->distFromPlayer;
						lightNum[curLight]=tempL->index;
					}

					tempL=tempL->next;
				}
				curLight++;
				closestDist=curDist;
			}

			firstLightRun=false;
		}
		tempL=curLightList.GetFirst();

		while(tempL!=NULL)
		{
			
			for(int count=0;count<maxlights;count++)
			{
				if(tempL->index==lightNum[count])
				{
					
					glLightfv(GL_LIGHT0+count, GL_AMBIENT, tempL->LightAmbient);     // Setup The Ambient Light
					glLightfv(GL_LIGHT0+count, GL_DIFFUSE, tempL->LightDiffuse);		// Setup The Diffuse Light
					glLightfv(GL_LIGHT0+count, GL_POSITION,tempL->LightPosition);
					//glLightfv(GL_LIGHT0+count, GL_SPECULAR,tempL->LightAmbient);
					if(tempL->linearAtten>0)
						glLightf(GL_LIGHT0+count, GL_LINEAR_ATTENUATION,  tempL->linearAtten);
					if(tempL->quadraticAtten>0)
						glLightf(GL_LIGHT0+count, GL_QUADRATIC_ATTENUATION,  tempL->quadraticAtten);
					glEnable(GL_LIGHT0+count);
				}
			}
			tempL = tempL->next;
		} //Lighting Setup

}
GLvoid Kill(GLvoid)								// Properly Kill The Window
{
	CloseLog();


	Sleep(20);

	/*
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
	}
	*/

		if (hRC)
		{
			if (!wglMakeCurrent(hDC, 0))
			{
				MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}
			if (!wglDeleteContext(hRC))						// Delete The RC
			{
				MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
			}
			hRC = NULL;										// Set RC To NULL

		}
	


	if (hDC && !ReleaseDC(hWnd,hDC))					// Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("MWView",hInstance))			// Unregister the Window Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
	ShowCursor(true);
	KillFont();

	if (usingMazX)
		DeleteDirectory("_temp",true);
	

#ifdef _MAZE_PROFILER

	fclose(debugFile);

#endif

}


bool WGLisExtensionSupported(const char *extension)
{
    const size_t extlen = strlen(extension);
    const char *supported = NULL;
 
    // Try To Use wglGetExtensionStringARB On Current DC, If Possible
    PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");
 
    if (wglGetExtString)
        supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());
 
    // If That Failed, Try Standard Opengl Extensions String
    if (supported == NULL)
        supported = (char*)glGetString(GL_EXTENSIONS);
 
    // If That Failed Too, Must Be No Extensions Supported
    if (supported == NULL)
        return false;
 
    // Begin Examination At Start Of String, Increment By 1 On False Match
    for (const char* p = supported; ; p++)
    {
        // Advance p Up To The Next Possible Match
        p = strstr(p, extension);
 
        if (p == NULL)
            return false;                       // No Match
 
        // Make Sure That Match Is At The Start Of The String Or That
        // The Previous Char Is A Space, Or Else We Could Accidentally
        // Match "wglFunkywglExtension" With "wglExtension"
 
        // Also, Make Sure That The Following Character Is Space Or NULL
        // Or Else "wglExtensionTwo" Might Match "wglExtension"
        if ((p==supported || p[-1]==' ') && (p[extlen]=='\0' || p[extlen]==' '))
            return true;                        // Match
    }
}

/*	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*/
 


BOOL CreateWnd(char* title, int width, int height, int bits, int monitor)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=WindowRect.left+(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=WindowRect.top+(long)height;		// Set Bottom Value To Requested Height

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));        // Load The Icon
	wc.hCursor			= LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));		// Load The Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "MWView";						// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","MazeWalker",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}	

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		
	}
	else if(borderlessWindowed)
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_POPUP;							// Windows Style
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
	}



	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		(BYTE)bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	//Test MSAA valid

	bool MSAAvalid=false;

	int pixelFormat;
	if ((hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"MWView",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
			hDC = GetDC(hWnd);
			PixelFormat=ChoosePixelFormat(hDC,&pfd);
			SetPixelFormat(hDC,PixelFormat,&pfd);
			hRC=wglCreateContext(hDC);
			glewInit();
			wglMakeCurrent(hDC,hRC);
			
		if (WGLisExtensionSupported("WGL_ARB_multisample")&&useMSAA)
		{
			// Get Our Pixel Format
			PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
		

			if(wglChoosePixelFormatARB!=NULL)
			{
		 
			UINT numFormats;
			float fAttributes[] = {0,0};
			int iAttributes[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, 1, //Number of buffers (must be 1 at time of writing)
			WGL_SAMPLES_ARB, 4,        //Number of samples
			0
			};
			

			MSAAvalid = wglChoosePixelFormatARB(hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats);
			}
		}
		////////////////DESTROY HERE/////////////////
		ReleaseDC (hWnd, hDC);            // Release Our Device Context
        hDC = 0;                 // Zero The Device Context
        DestroyWindow (hWnd);                // Destroy The Window
        hWnd = 0;                    // Zero The Window Handle
	}
	
	

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"MWView",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		Kill();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	

	
	hDC = GetDC(hWnd);
	if (!(hDC))							
	{
		Kill();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	


	if(MSAAvalid)
	{
		pfd=PIXELFORMATDESCRIPTOR();
		SetPixelFormat(hDC,pixelFormat,&pfd);
	}
	
	
	if (!MSAAvalid&&!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		Kill();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	
	if(!MSAAvalid&&!SetPixelFormat(hDC,PixelFormat,&pfd))
	{
		Kill();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	
	if (!(hRC=wglCreateContext(hDC)))		
	{
		Kill();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		Kill();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (WGLisExtensionSupported("WGL_EXT_swap_control")&&useVSYNC&&!oculusEnabled) //If compatible enable V-Sync
	{
		PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
		wglSwapIntervalEXT = 
			(PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(1);
	}
	

	if (!Init())									// Initialize Our Newly Created GL Window
	{
		Kill();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SwapBuffers(hDC); //initial swap
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGL(width, height);					// Set Up Our Perspective GL Screen

	if(!fullscreen)
	{
		//Center the window
		MoveWindow(hWnd,desktopRC.left+(desktopRC.right-desktopRC.left-width)/2,desktopRC.top+(desktopRC.bottom-desktopRC.top-height)/2,width,height,true);
	}

	if(shadersCompiled)
		glUseProgram(shaderProgBasic);
	else
		glDisable(GL_LIGHTING);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	 
	glColor3f(1,1,1);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, (double)width, 0, (double)height);
	           // Clear The Screen And The Depth Buffer
	
	glRasterPos2f((float)width/2.0f,(float)height/2.0f);

	bStatusMazeLoading = true;
	glPrint(true,"%s","Loading");
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	SwapBuffers(hDC); //to show loading screen

	MSG		msg;
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Check for message in the queue
				{
					if (msg.message==WM_QUIT)				// Check Quit Message
					{
													// If So done=TRUE
						bMazeLoop=TRUE;
						return false;
					}
					else									// If Not, Deal With Window Messages
					{
						TranslateMessage(&msg);				// Translate The Message
						DispatchMessage(&msg);				// Dispatch The Message
					}
				}
	return TRUE;									// Success
}



LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_CREATE:
			ShowCursor(false);									// Hide Mouse Pointer
			startTime=GetQPC();


			break;
	    case WM_PAINT:
			SwapBuffers(hDC);
			break;
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}
			return 0;								// Return To The Message Loop
		}

	
		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}
		case WM_WINDOWPOSCHANGED:
			{
				WINDOWINFO windINF;
				GetWindowInfo(hWnd,&windINF);
				xLoc=windINF.rcClient.left;
				yLoc=windINF.rcClient.top;
				int height=windINF.rcWindow.bottom-windINF.rcWindow.top;
				if(!fullscreen)
					height=height-30;
				int width=windINF.rcWindow.right-windINF.rcWindow.left-windINF.cxWindowBorders*2;
				
				//offset for cursor
				xLoc=xLoc-16;
				yLoc=yLoc-16;

				objCamera.UpdateMidCursorPos(xLoc+Width/2,yLoc+Height/2);
				ReSizeGL(width, height);					// Set Up Our Perspective GL Screen

				return 0;
			}
		case WM_CLOSE:								// Close Message
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;						
		}
		case WM_TIMER:
		{
			return 0;
		}
		case WM_KEYDOWN:							// A Key Down
		{
			keys[wParam] = TRUE;					// If So, Mark It As TRUE
			return 0;								
		}
		case WM_KEYUP:								// A Key Released
		{
			keys[wParam] = FALSE;					// If So, Mark It As FALSE		
			return 0;							
		}
	}
	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
LRESULT CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT RCdesktop,RCdlg;
	HWND parent;
	int res;
	OPENFILENAME ofn;
	float inp1;
	FILE *fp;
	char iDir[TXTBUFFERLIMIT],temp[TXTBUFFERLIMIT],temp2[TXTBUFFERLIMIT];
	JOYINFOEX joyInfoEx;
	SHELLEXECUTEINFO mzParams;
	switch (message)
	{
	case WM_INITDIALOG:
		
		parent=GetDesktopWindow();
		GetWindowRect(parent,&RCdesktop);
		GetWindowRect(hDlg,&RCdlg);
		SetWindowPos(hDlg,HWND_TOP,(RCdesktop.right - (RCdlg.right-RCdlg.left))/2,(RCdesktop.bottom - (RCdlg.bottom-RCdlg.top))/2,RCdlg.right-RCdlg.left,RCdlg.bottom-RCdlg.top,SWP_NOSIZE);
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"640x480 (4:3)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"800x600 (4:3)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"1024x768 (4:3)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"1280x720 (16:9)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"1280x800 (16:10)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"1280x1024 (4:3)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"1600x1200 (4:3)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"1680x1050 (16:10)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"1920x1080 (16:9)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"1920x1200 (16:10)");
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_ADDSTRING,0,(LPARAM)"3840x1024 (15:4)");
		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)"3840x2160 (16:9)");

		int xWidth; 
		xWidth = GetPrivateProfileInt("Display", "CustomWidth", 5760, mwalkerIniPath);
		int xHeight; 
		xHeight = GetPrivateProfileInt("Display", "CustomHeight", 1080, mwalkerIniPath);

		sprintf(temp, "%d", xWidth);
		WritePrivateProfileString("Display", "CustomWidth", temp, mwalkerIniPath);
		sprintf(temp, "%d", xHeight);
		WritePrivateProfileString("Display", "CustomHeight", temp, mwalkerIniPath);

		char ResString[256];
		sprintf(ResString, "Custom: %ix%i", xWidth, xHeight);

		SendDlgItemMessage(hDlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)ResString);
		SendDlgItemMessage(hDlg,IDC_COMBO2,CB_ADDSTRING,0,(LPARAM)"8 Bits");
		SendDlgItemMessage(hDlg,IDC_COMBO2,CB_ADDSTRING,0,(LPARAM)"16 Bits");
		SendDlgItemMessage(hDlg,IDC_COMBO2,CB_ADDSTRING,0,(LPARAM)"24 Bits");
		SendDlgItemMessage(hDlg,IDC_COMBO2,CB_ADDSTRING,0,(LPARAM)"32 Bits");

		int dSetting;
		dSetting=GetPrivateProfileInt("Display","Resolution",1,mwalkerIniPath);
		SendDlgItemMessage(hDlg,IDC_COMBO1,CB_SETCURSEL,dSetting,0);
		dSetting=GetPrivateProfileInt("Display","Bits",3,mwalkerIniPath);		
		SendDlgItemMessage(hDlg,IDC_COMBO2,CB_SETCURSEL,dSetting,0);
		
		autoLog=GetPrivateProfileInt("options","autoLog",0,mwalkerIniPath);
		SendDlgItemMessage(hDlg,IDC_AUTOLOGCHECK,BM_SETCHECK,autoLog,0);

		borderlessWindowed = GetPrivateProfileInt("Display", "BorderlessWindowed", 0, mwalkerIniPath);

		if(fullscreen) SendDlgItemMessage(hDlg,IDC_CHECK1,BM_SETCHECK,BST_CHECKED,0);
		if(bMouseInput) SendDlgItemMessage(hDlg,IDC_CHECK3,BM_SETCHECK,BST_CHECKED,0);
		if(bJoyStickInput) SendDlgItemMessage(hDlg,IDC_CHECK5,BM_SETCHECK,BST_CHECKED,0);
		if(bShowCrosshair) SendDlgItemMessage(hDlg,IDC_CHECK2,BM_SETCHECK,BST_CHECKED,0);
		if(shader) SendDlgItemMessage(hDlg,IDC_CHECK10,BM_SETCHECK,BST_CHECKED,0);
		if(bShowBar) SendDlgItemMessage(hDlg,IDC_CHECK9,BM_SETCHECK,BST_CHECKED,0);
		
		if(bShowTimer)  SendDlgItemMessage(hDlg,IDC_CHECK4,BM_SETCHECK,BST_CHECKED,0);
		else SendDlgItemMessage(hDlg,IDC_CHECK4,BM_SETCHECK,BST_UNCHECKED,0);


		//GetCurrentDirectory(TXTBUFFERLIMIT, iDir);
		SHGetFolderPath(hWnd, CSIDL_PERSONAL, NULL, 0, iDir);		
		sprintf(temp,"%s\\MazeSuite\\Samples\\Sample0.maz",iDir);
		GetPrivateProfileString("File","Map File",temp,temp2,TXTBUFFERLIMIT,mwalkerIniPath);
		fp = fopen(temp2,"r");
		if (fp != NULL)
		{
			//file exists
			fclose(fp);
			SetDlgItemText(hDlg, IDC_FILENAME, temp2);
		}
		else
		{
			SetDlgItemText(hDlg, IDC_FILENAME, "");
		}



		bSendSync=GetPrivateProfileInt("Com","Send",0,mwalkerIniPath);
		bListen=GetPrivateProfileInt("Com","Listen",0,mwalkerIniPath);
		bUseLPT = GetPrivateProfileInt("LPT", "Enable", 0, mwalkerIniPath);
		iLptPortNum = GetPrivateProfileInt("LPT", "Port", 1, mwalkerIniPath);
		iLptDelay = GetPrivateProfileInt("LPT", "Delay",1, mwalkerIniPath);


		comPort=GetPrivateProfileInt("Com","Port",1,mwalkerIniPath);
		baudRate=GetPrivateProfileInt("Com","BaudRate",3,mwalkerIniPath);
		parity=GetPrivateProfileInt("Com","Parity",0,mwalkerIniPath);
		stopBits=GetPrivateProfileInt("Com","StopBits",0,mwalkerIniPath);

		dSetting=GetPrivateProfileInt("Com","Send",0,mwalkerIniPath);

		if(dSetting) SendDlgItemMessage(hDlg,IDC_CHECK6,BM_SETCHECK,BST_CHECKED,0);

		if(lights) SendDlgItemMessage(hDlg,IDC_CHECK8,BM_SETCHECK,BST_CHECKED,0);

		triggerChar=GetPrivateProfileInt("Com","triggerChar",116,mwalkerIniPath);

		return TRUE;

	case WM_COMMAND:
		//switch(LOWORD(wParam))
		switch(wParam)
		{
			case IDC_BUTTON1:
				// Initialize OPENFILENAME
				//strcpy(temp,filename);
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = temp;

				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(temp);
				ofn.lpstrFilter = "Maze List Files (*.mel,*.melx)\0*.mel;*.melx\0Maze Files (*.maz)\0*.MAZ\0Maze Files (*.mazX)\0*.MAZX\0All MazeSuite Files\0*.mel;*.melx;*.maz;*.mazx";
				ofn.nFilterIndex = 4;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				//GetCurrentDirectory(TXTBUFFERLIMIT,iDir); //default behavior is opening last folder...
				//ofn.lpstrInitialDir = iDir;end
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetOpenFileName(&ofn)==TRUE)
				{
					SetDlgItemText(hDlg,IDC_FILENAME,ofn.lpstrFile);
				}

				//SetCurrentDirectory(iDir);
				
				break;
			case IDC_BUTTON2: //load log file

				strcpy_s(temp,logfile);
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = temp;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(temp);
				ofn.lpstrFilter = "Text File\0*.txt\0Excel File\0*.xls\0";
				ofn.nFilterIndex =1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				//GetCurrentDirectory(TXTBUFFERLIMIT,iDir);
				//ofn.lpstrInitialDir = iDir;
				ofn.Flags = OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT; // | OFN_OVERWRITEPROMPT;

				if (GetSaveFileName(&ofn)==TRUE)
				{
					strcpy_s(logfile,ofn.lpstrFile);
					if(ofn.nFileExtension==NULL)
					{
						if(ofn.nFilterIndex==1)
							strncat_s(logfile,".txt",4);
						else
							strncat_s(logfile,".xls",4);
					}
					SetDlgItemText(hDlg,IDC_FILENAME2,logfile);
				}

				if((strlen(logfile)>2)&&!InitLog(logfile))
				{
					SetDlgItemText(hDlg,IDC_FILENAME2,"");
					MessageBox(hDlg,"Can not create log file!. Make sure the folder is accessible and you read/write permissions.","Error",MB_ICONERROR);
				}
				SetCurrentDirectory(iDir);
				EnableWindow(GetDlgItem(hDlg,IDC_EDIT1),true);

				break;
			case IDC_BUTTON3:
				MessageBox(hDlg,"Baud Rate:9600\nByte Size:8\nParity:No\nStop Bits:1\n","Com Settings",MB_OK);
				break;
			case ID_HELP_MAZEUPDATEUTILITY:

				sprintf(temp,"MazeUpdate.exe");
				sprintf(temp2,"open");
				mzParams.cbSize = sizeof(SHELLEXECUTEINFO);
				mzParams.fMask = NULL;
				mzParams.hwnd = NULL;
				mzParams.lpFile = temp;
				mzParams.lpParameters = NULL;
				mzParams.lpDirectory = NULL;
				mzParams.nShow = SW_SHOWNORMAL;
				mzParams.hInstApp = NULL;
				mzParams.lpVerb = temp2;
				if(ShellExecuteEx(&mzParams) )
				{
					//WaitForSingleObject(mzUp, INFINITE);
					//successfully launched
					PostQuitMessage(0);
				}
				else
				{
					MessageBox(hDlg,"Couldn't find MazeUpdate Utility!\n\nPlease download and re-install latest MazeSuite setup!","MazeWalker",MB_ICONERROR | MB_OK);					
				}				
				break;
			case ID_INFO_ABOUT:
				DialogBox(hInstance, (LPCTSTR)IDD_DIALOG2, hDlg, (DLGPROC)AboutProc);
				break;
			case ID_OTHER_CONTROLS:
				DialogBox(hInstance, (LPCTSTR)IDD_DIALOG3, hDlg, (DLGPROC)ControlProc);
				break;
			case ID_ADVANCED_SHADEROPTIONS:
				DialogBox(hInstance, (LPCTSTR)IDD_DIALOG_SHADERS, hDlg, (DLGPROC)ShaderProc);
				break;
			case ID_ADVANCED_PERSPECTIVESETTINGS:
				DialogBox(hInstance, (LPCTSTR)IDD_DIALOG_VIEW_OPTIONS, hDlg, (DLGPROC)ViewProc);
				break;
			
			case ID_ADVANCED_SENSOROPTIONS:
				DialogBox(hInstance, (LPCTSTR)IDD_DIALOG_SENSORS, hDlg, (DLGPROC)SensorProc);
				break;
			case ID_ADVANCED_OTHER:
				DialogBox(hInstance, (LPCTSTR)IDD_DIALOG4, hDlg, (DLGPROC)OtherProc);
				break;
			case ID_ADVANCED_SYNC:
				DialogBox(hInstance, (LPCTSTR)IDD_DIALOG_SYNC, hDlg, (DLGPROC)SyncProc);
				break;
			case ID_ADVANCED_SERIALOPTIONS:
				DialogBox(hInstance, (LPCTSTR)IDD_DIALOG_COMM, hDlg, (DLGPROC)CommProc);
				break;
			case ID_ADVANCED_JOYSTICK:
				DialogBox(hInstance, (LPCTSTR)IDD_JOYSTICK, hDlg, (DLGPROC)JoystickProc);
				break;
			case ID_ADVANCED_OCULUS:
				DialogBox(hInstance, (LPCTSTR)IDD_DIALOG_OCULUS, hDlg, (DLGPROC)OculusProc);
				break;
			case ID_FILE_START:
			case IDOK:
				if(deviceEnabled[0])
				{
					DWORD devID=0;
					deviceThread[0] = CreateThread( 
						NULL,                   // default security attributes
						0,                      // use default stack size  
						listenThread,       // thread function name
						(LPVOID)0,          // argument to thread function 
						0,                      // use default creation flags 
						&devID);   // returns the thread identifier
			
				}
				if(deviceEnabled[1])
				{
					DWORD devID=1;
					deviceThread[1] = CreateThread( 
						NULL,                   // default security attributes
						0,                      // use default stack size  
						listenThread,       // thread function name
						(LPVOID)1,          // argument to thread function 
						0,                      // use default creation flags 
						&devID);   // returns the thread identifier

				}
				if(deviceEnabled[2])
				{
					DWORD devID=2;
					deviceThread[0] = CreateThread( 
						NULL,                   // default security attributes
						0,                      // use default stack size  
						listenThread,       // thread function name
						(LPVOID)2,          // argument to thread function 
						0,                      // use default creation flags 
						&devID);   // returns the thread identifier

				}

				EnumDisplayMonitors(NULL, &RCdesktop,MonitorEnumProc,0);
				////
				Sleep(150);
				while(device[0].connecting||device[1].connecting||device[2].connecting)
				{
					Sleep(150);
				}

				if (true)// old API wait function !bWaitForAPIdevices||((deviceEnabled[0] && device[0].connected) || !deviceEnabled[0]) && ((deviceEnabled[1] && device[1].connected) || !deviceEnabled[1]) && ((deviceEnabled[2] && device[2].connected) || !deviceEnabled[2]))
				
				{
					curList.Clear();
					filename[0]=0;
					GetDlgItemText(hDlg,IDC_FILENAME,filename,TXTBUFFERLIMIT);

					if (filename)
					{

						res = curList.ReadFile(filename);
					}
					if(res==-1)
					{
						char* errormsg= new char[512];
						sprintf(errormsg,"Can not open maze file\n%s",filename);
						MessageBox(hDlg,errormsg,"Error",MB_ICONERROR);
						break;
					}
					else if(res==-2)
					{
						char* errormsg= new char[512];
						sprintf(errormsg,"Incorrect file format\n%s",filename);
						MessageBox(hDlg,errormsg,"Error",MB_ICONERROR);
						break;
					}		

					res = SendDlgItemMessage(hDlg,IDC_COMBO1,CB_GETCURSEL,0,0);
					sprintf(temp,"%d",res);
					WritePrivateProfileString("Display","Resolution",temp,mwalkerIniPath);
					if(res==0){ Width=640; Height=480;}
					else if(res==1) { Width=800; Height=600;}
					else if(res==2) { Width=1024; Height=768;}
					else if(res==3) { Width=1280; Height=720;}
					else if(res==4) { Width=1280; Height=800;}
					else if(res==5) { Width=1280; Height=1024;}
					else if(res==6) { Width=1600; Height=1200;}
					else if(res==7) { Width=1680; Height=1050;}
					else if(res==8) { Width=1920; Height=1080;}
					else if(res==9) { Width=1920; Height=1200;}
					else if(res==10) { Width=3840; Height=1024;}
					else if (res == 11) { Width = 3840; Height = 2160; }
					else {
						Width = GetPrivateProfileInt("Display", "CustomWidth", 5760, mwalkerIniPath);
						Height = GetPrivateProfileInt("Display", "CustomHeight", 1080, mwalkerIniPath);
					}

					res = SendDlgItemMessage(hDlg,IDC_COMBO2,CB_GETCURSEL,0,0);
					sprintf(temp,"%d",res);
					WritePrivateProfileString("Display","Bits",temp,mwalkerIniPath);
					if(res==0) bitz=8;
					else if(res==1) bitz=16;
					else if(res==2) bitz=24;
					else bitz=32;
					res = SendDlgItemMessage(hDlg,IDC_CHECK1,BM_GETCHECK,0,0);
					sprintf(temp,"%d",res);
					WritePrivateProfileString("Display","Fullscreen",temp,mwalkerIniPath);
					if(res == BST_CHECKED)
					{ 
						fullscreen=true; 
						if(bitz==8)
						{ 
							if(MessageBox(hDlg,"Color dept should be at least 16 bits in fullscreen mode\n\nDo you want to change it to 16 bits?","MazeWalker",MB_YESNO)==IDYES) 
								bitz=16;
							else 
								fullscreen=false;
						} 
					}
					else fullscreen=false;

					res = SendDlgItemMessage(hDlg,IDC_CHECK8,BM_GETCHECK,0,0);
					if(res==BST_CHECKED) 
					{
						lights=true;
						WritePrivateProfileString("Display","Lights","1",mwalkerIniPath);
					}
					else
					{
						lights=false;
						WritePrivateProfileString("Display","Lights","0",mwalkerIniPath);
					}

					res = SendDlgItemMessage(hDlg,IDC_CHECK2,BM_GETCHECK,0,0);
					if(res==BST_CHECKED)
					{
						bShowCrosshair=true;
						WritePrivateProfileString("Display","ShowCursor","1",mwalkerIniPath);
					}
					else
					{
						bShowCrosshair=false;
						WritePrivateProfileString("Display","ShowCrosshair","0",mwalkerIniPath);
					}

					res = SendDlgItemMessage(hDlg,IDC_CHECK9,BM_GETCHECK,0,0);
					if(res==BST_CHECKED)
					{
						bShowBar=true;
						WritePrivateProfileString("Display","ShowBar","1",mwalkerIniPath);
					}
					else
					{
						bShowBar=false;
						WritePrivateProfileString("Display","ShowBar","0",mwalkerIniPath);
					}
					
					res = SendDlgItemMessage(hDlg,IDC_CHECK10,BM_GETCHECK,0,0);
					if(res==BST_CHECKED)
					{
						shader=true;
						WritePrivateProfileString("Display","UseShaders","1",mwalkerIniPath);
					}
					else
					{
						shader=false;
						WritePrivateProfileString("Display","UseShaders","0",mwalkerIniPath);
					}

					res = SendDlgItemMessage(hDlg,IDC_CHECK7,BM_GETCHECK,0,0);
					if(res == BST_CHECKED) videoPlayBack = true;
					else videoPlayBack = false;

					if(videoPlayBack)
					{
						GetDlgItemText(hDlg,IDC_FILENAME3,temp,TXTBUFFERLIMIT);
						clog = fopen(temp,"r");
						//int x;
						if(clog==NULL )
							MessageBox(hDlg,"Can not open maze log file","Error",MB_ICONERROR);
						int indexCount=-1;

						while(indexCount!=0&&fgets(temp,TXTBUFFERLIMIT,clog)!=NULL)
						{
							if(temp[0]=='M'&&temp[1]=='a')
								indexCount++;
						}

						//check maze file with current!!!!!!!!!!!!!!!!!!!
						//get size...
						while(temp[res]!=NULL)
							res++;
						temp[res-1]=NULL;
						while(temp[res]!='\\')
							res--;
						int i=0;
						while(filename[i]!=NULL)
							i++;
						while(filename[i]!='\\')
							i--;
						if(strcasecmp( &(temp[res+1]),&(filename[i+1]) ) != 0)   //Now makes case insensitive comparison of the two strings
							MessageBox(hDlg,"Log file belongs to another maze. Either change the current maze or input log file!","Incompatible maze and log file",MB_OK);

						if(NULL == fgets(temp,TXTBUFFERLIMIT,clog))
							break;		//time
						if(NULL == fgets(temp,TXTBUFFERLIMIT,clog))
							break;		//
						if(NULL == fgets(temp,TXTBUFFERLIMIT,clog))
							break;		//
						if(NULL == fgets(temp,TXTBUFFERLIMIT,clog))
							break;		//headers

						if(NULL == fgets(temp,TXTBUFFERLIMIT,clog))
							break;		//headers

						/*
						if(NULL == fgets(temp,TXTBUFFERLIMIT,clog))
							break;		//first recorded line
							*/
						float inp2=0;
						char* temp2 = "";
						temp2 = strtok(temp, "\t\n");
						int numInp = 0;
						bool isEvent = false;
						sscanf(temp2, "%f", &inp2);
						while (temp2 != NULL)
						{	
							isEvent = isEvent || (strcmp(temp2, "Event") == 0);
							if(numInp==1)
								sscanf(temp2, "%i", &clogItemTime);
							numInp++;
							temp2=strtok(NULL, "\t\n");
							
						}

						if(numInp == 7&&!isEvent)
						{
							clogItemTime=(int)inp2;
							newclogType=false;
						}

						clogStartTime = clogItemTime;  ///???
						////startTime = GetQPC(); //already initialized!!!
						//mazeStart = GetQPC();
					}

					res = SendDlgItemMessage(hDlg,IDC_AUTOLOGCHECK,BM_GETCHECK,0,0);
					if(res == BST_CHECKED)
					{
						autoLog = true;
						WritePrivateProfileString("options","autoLog","1",mwalkerIniPath);
					}
					else
					{
						autoLog=false;
						WritePrivateProfileString("options","autoLog","0",mwalkerIniPath);
					}

					if(videoPlayBack&&!interactivePlayback)
					{
						autoLog=false;
						logfile[0]=0;
					}



					if(autoLog&&strlen(logfile)<2)
					{
						GetDlgItemText(hDlg,IDC_EDIT1,walker,100);
						
						SHGetFolderPath(hWnd,CSIDL_PERSONAL,NULL,0,logfile);
						time_t rawtime;
						time ( &rawtime );
						struct tm * timeinfo;
						timeinfo = localtime ( &rawtime );
						sprintf(logfile,"%s\\MazeSuite\\LogFiles",logfile);
						CreateDirectory(logfile,NULL);
						sprintf(logfile,"%s\\autoLog_%i_%i_%i_%i_%i_%i_%s.txt",logfile,timeinfo->tm_year-100,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,walker);
						if((strlen(logfile)>2)&&!InitLog(logfile))
						{
							MessageBox(0,"Can not create automatic log file!. Make sure the folder is accessible and you read/write permissions.","Error",MB_ICONERROR);
						}
					}

					if(!autoLog&&logfile[0]!=0)
					{
						GetDlgItemText(hDlg,IDC_EDIT1,walker,100);
					}
					

					if(LogStatus())
					{
						char msg[800];
						//sprintf(msg,"%d\tLoading\t%s\n",GetQPC()-startTime,filename);
						sprintf(msg,"%d\tLoading\t%s\n",0,filename);
						AddToLog(msg);
					}

					res = SendDlgItemMessage(hDlg,IDC_CHECK3,BM_GETCHECK,0,0);
					if(res==BST_CHECKED){ bMouseInput=true;WritePrivateProfileString("OtherSettings","MouseInput","1",mwalkerIniPath);}
					else {bMouseInput=false;WritePrivateProfileString("OtherSettings","MouseInput","0",mwalkerIniPath);}
					
					res = SendDlgItemMessage(hDlg,IDC_CHECK5,BM_GETCHECK,0,0);
					if(res==BST_CHECKED){ bJoyStickInput=true;WritePrivateProfileString("OtherSettings","JoyStickInput","1",mwalkerIniPath);}
					else {bJoyStickInput=false;
					WritePrivateProfileString("OtherSettings","JoyStickInput","0",mwalkerIniPath);}
					

					//ZeroMemory((void*)joyInfoEx, sizeof(joyInfoEx));
					if(joystickNum>=0)
					{
						joyInfoEx.dwSize = sizeof(joyInfoEx);
						JoyPresent = (joyGetPosEx(JOYSTICKID1+joystickNum, &joyInfoEx) == JOYERR_NOERROR);
						if (bJoyStickInput && joystickNum>=0 && JoyPresent == FALSE)
						{
							MessageBox(hDlg,"Joystick is not present!","MazeWalker",0);
							bJoyStickInput=false;
						}
					}

					res = SendDlgItemMessage(hDlg,IDC_CHECK4,BM_GETCHECK,0,0);
					if(res==BST_CHECKED) bShowTimer=true;
					else bShowTimer=false;
					WritePrivateProfileString("Options","Timer",bShowTimer?"1":"0",mwalkerIniPath);
					


					res = SendDlgItemMessage(hDlg,IDC_SKIPWarningCHECK,BM_GETCHECK,0,0);
					if(res==BST_CHECKED) bSkipWarnings=true;
					else bSkipWarnings=false;

					EndDialog(hDlg, LOWORD(wParam));
				}
				else
				{
					if(deviceEnabled[0]&&!device[0].connected)
					{
						char msg[150];
						int devID=0;

						struct in_addr addr;
						addr.s_addr = htonl((long)deviceIP[devID]);
						
						char* strAddr=new char[50];
						if(deviceLocal[devID])
							strAddr="127.0.0.1";
						else
							strAddr= inet_ntoa(addr);

						sprintf(msg,"Could not connect to Device %i on\n%s:%i",devID+1,strAddr,devicePort[devID]);
						MessageBox(hDlg,msg,"Connectivity Error",0);
					}
					if(deviceEnabled[1]&&!device[1].connected)
					{
						char msg[150];
						int devID=1;

						struct in_addr addr;
						addr.s_addr = htonl((long)deviceIP[devID]);
						
						char* strAddr=new char[50];
						if(deviceLocal[devID])
							strAddr="127.0.0.1";
						else
							strAddr= inet_ntoa(addr);

						sprintf(msg,"Could not connect to Device %i on\n%s:%i",devID+1,strAddr,devicePort[devID]);
						MessageBox(hDlg,msg,"Connectivity Error",0);
					}
					if(deviceEnabled[2]&&!device[2].connected)
					{
						char msg[150];
						int devID=2;

						struct in_addr addr;
						addr.s_addr = htonl((long)deviceIP[devID]);
						
						char* strAddr=new char[50];
						if(deviceLocal[devID])
							strAddr="127.0.0.1";
						else
							strAddr= inet_ntoa(addr);

						sprintf(msg,"Could not connect to Device %i on\n%s:%i",devID+1,strAddr,devicePort[devID]);
						MessageBox(hDlg,msg,"Connectivity Error",0);
					}
					device[0].Disconnect();
					device[1].Disconnect();
					device[2].Disconnect();
				}
				break;
			case IDC_BUTTON4:
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = temp;

				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(temp);
				ofn.lpstrFilter = "Maze Log File (Text)\0*.txt\0Maze Log File (Excel)\0*.xls\0";
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				//GetCurrentDirectory(TXTBUFFERLIMIT,iDir); //default behavior is opening last folder...
				//ofn.lpstrInitialDir = iDir;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetOpenFileName(&ofn)==TRUE)
				{
					SetDlgItemText(hDlg,IDC_FILENAME3,ofn.lpstrFile);
					EnableWindow(GetDlgItem(hDlg,IDC_CHECK3),false);
					EnableWindow(GetDlgItem(hDlg,IDC_CHECK5),false);
					SendDlgItemMessage(hDlg,IDC_CHECK7,BM_SETCHECK,1,0);
				}
				


				break;
			case IDC_CHECK7:
				res = SendDlgItemMessage(hDlg,IDC_CHECK7,BM_GETCHECK,0,0);
				if(res == BST_CHECKED)
				{
					EnableWindow(GetDlgItem(hDlg,IDC_CHECK3),false);
					EnableWindow(GetDlgItem(hDlg,IDC_CHECK5),false);
				}
				else
				{
					EnableWindow(GetDlgItem(hDlg,IDC_CHECK3),true);
					EnableWindow(GetDlgItem(hDlg,IDC_CHECK5),true);
				}

				break;
			
			case ID_FILE_EXIT:
			case IDCANCEL:
				device[0].Disconnect();
				device[1].Disconnect();
				device[2].Disconnect();
				EndDialog(hDlg, IDCANCEL);
				break;
		}
		break;
	}
	return FALSE;
}

BOOL GetFixedInfo(VS_FIXEDFILEINFO& vsffi,LPBYTE m_lpVersionData)
{
    // Must call Open() first
    //ASSERT(m_lpVersionData != NULL);
    if ( m_lpVersionData == NULL )
        return FALSE;

    UINT nQuerySize;
	VS_FIXEDFILEINFO* pVsffi;
    if ( ::VerQueryValue((void **)m_lpVersionData, ("\\"),
                         (void**)&pVsffi, &nQuerySize) )
    {
        vsffi = *pVsffi;
        return TRUE;
    }

    return FALSE;
}




LRESULT CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char txt[100];
	DWORD   dwVerInfoSize;    // Size of version information block
    char    szFullPath[256];
    //char    szGetName[256];
	//char    szResult[256] = {0};
    DWORD   dwVerHnd=0;        // An 'ignored' parameter, always '0'
	LPBYTE  m_lpVersionData; 
	int y1=0,y2=0,y3=0,y4=0;  //local ver nums
	char* ct = "Disclaimer and Copyright Notices\r\n\r\n\
THIS SOFTWARE AND UTILIZED LIBRARIES ARE PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" \
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE \
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE \
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE \
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR \
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF \
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS \
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN \
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) \
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF \
THE POSSIBILITY OF SUCH DAMAGE. \r\n\r\n\
The OpenGL Extension Wrangler Library\r\n\
Copyright (C) 2002-2008, Milan Ikits\
Copyright (C) 2002-2008, Marcelo E. Magallon\
Copyright (C) 2002, Lev Povalahev\
All rights reserved.\
\r\n\r\n\
Mesa 3-D graphics library \r\n\
Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.\
Copyright (c) 2007 The Khronos Group Inc.\
\r\n\r\n\
Zlib \r\n\
Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler\
\r\n\r\n\
Libpng\r\n\
Copyright (c) 1998-2009 Glenn Randers-Pehrson\r\n\r\n\
FMOD Sound System\r\n\
Copyright (c) Firelight Technologies Pty, Ltd., 1994-2011\r\n\r\n\
See installation folder more information.";
	switch (message)
	{
		case WM_INITDIALOG:
				GetModuleFileName (NULL, szFullPath, sizeof(szFullPath));
				dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd);
				if (dwVerInfoSize) 
				{
					m_lpVersionData = new BYTE[dwVerInfoSize]; 
					GetFileVersionInfo(szFullPath, 0, dwVerInfoSize,(void**) m_lpVersionData);
					VS_FIXEDFILEINFO vsffi;
					if( GetFixedInfo(vsffi,m_lpVersionData) )
					{
						y1=HIWORD(vsffi.dwFileVersionMS);
						y2=LOWORD(vsffi.dwFileVersionMS);
						y3=HIWORD(vsffi.dwFileVersionLS);
						y4=LOWORD(vsffi.dwFileVersionLS);
					}
					sprintf(txt,"MazeWalker version %d.%d.%d.%d",y1,y2,y3,y4);
					SetDlgItemText(hDlg,IDC_VERSIONS,txt);
				}

				SetDlgItemText(hDlg,IDC_EDIT1,ct);
				break;
		case WM_CLOSE:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		case WM_COMMAND:
				switch(LOWORD(wParam))
				{
					case IDOK:
						EndDialog(hDlg, LOWORD(wParam));
						return TRUE;
				}
				break;
	}
	return FALSE;
}

LRESULT CALLBACK SensorProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	bool res;
	switch (message)
	{
	case WM_INITDIALOG:
		{
			SendDlgItemMessage(hDlg,IDC_IPADDRESS1,IPM_SETADDRESS,0,deviceIP[0]);
			SendDlgItemMessage(hDlg,IDC_IPADDRESS2,IPM_SETADDRESS,0,deviceIP[1]);
			SendDlgItemMessage(hDlg,IDC_IPADDRESS3,IPM_SETADDRESS,0,deviceIP[2]);
			if (bWaitForAPIdevices)
				SendDlgItemMessage(hDlg, IDC_WAIT_FOR_CONNECTION_CHECK, BM_SETCHECK, BST_CHECKED, 0);
			if(deviceEnabled[0])
				SendDlgItemMessage(hDlg,IDC_Sensor_CHECK1,BM_SETCHECK,BST_CHECKED,0);
			if(deviceEnabled[1])
				SendDlgItemMessage(hDlg,IDC_SENSOR_CHECK2,BM_SETCHECK,BST_CHECKED,0);
			if(deviceEnabled[2])
				SendDlgItemMessage(hDlg,IDC_SENSOR_CHECK3,BM_SETCHECK,BST_CHECKED,0);
			if(deviceLocal[0])
			{
				SendDlgItemMessage(hDlg,IDC_LOCALCHECK1,BM_SETCHECK,BST_CHECKED,0);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS1,IPM_SETADDRESS,0,2130706433);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS1,WM_ENABLE,FALSE,0);
			}
			if(deviceLocal[1])
			{
				SendDlgItemMessage(hDlg,IDC_LOCALCHECK2,BM_SETCHECK,BST_CHECKED,0);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS2,IPM_SETADDRESS,0,2130706433);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS2,WM_ENABLE,FALSE,0);
			}
			if(deviceLocal[2])
			{
				SendDlgItemMessage(hDlg,IDC_LOCALCHECK3,BM_SETCHECK,BST_CHECKED,0);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS3,IPM_SETADDRESS,0,2130706433);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS3,WM_ENABLE,FALSE,0);
			}
			HWND editHandle =GetDlgItem(hDlg,IDC_SENSORPORTEDIT1);
			char temp[10];
			sprintf(temp,"%i", devicePort[0]);
			SetWindowText(editHandle,temp);
			editHandle =GetDlgItem(hDlg,IDC_SENSORPORTEDIT2);
			sprintf(temp,"%i", devicePort[1]);
			SetWindowText(editHandle,temp);
			editHandle =GetDlgItem(hDlg,IDC_SENSORPORTEDIT3);
			sprintf(temp,"%i", devicePort[2]);
			SetWindowText(editHandle,temp);
		
		break;
		}
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_LOCALCHECK1:
			res = SendDlgItemMessage(hDlg,IDC_LOCALCHECK1,BM_GETCHECK,0,0);
			if(res==true)
			{
				SendDlgItemMessage(hDlg,IDC_IPADDRESS1,IPM_SETADDRESS,0,2130706433);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS1,WM_ENABLE,FALSE,0);
			}
			else
			{
				SendDlgItemMessage(hDlg,IDC_IPADDRESS1,IPM_SETADDRESS,0,deviceIP[0]);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS1,WM_ENABLE,TRUE,0);
			}
			break;
		case IDC_LOCALCHECK2:
			res = SendDlgItemMessage(hDlg,IDC_LOCALCHECK2,BM_GETCHECK,0,0);
			if(res==true)
			{
				SendDlgItemMessage(hDlg,IDC_IPADDRESS2,IPM_SETADDRESS,0,2130706433);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS2,WM_ENABLE,FALSE,0);
			}
			else
			{
				SendDlgItemMessage(hDlg,IDC_IPADDRESS2,IPM_SETADDRESS,0,deviceIP[1]);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS2,WM_ENABLE,TRUE,0);
			}
			break;
		case IDC_LOCALCHECK3:
			res = SendDlgItemMessage(hDlg,IDC_LOCALCHECK3,BM_GETCHECK,0,0);
			if(res==true)
			{
				SendDlgItemMessage(hDlg,IDC_IPADDRESS3,IPM_SETADDRESS,0,2130706433);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS3,WM_ENABLE,FALSE,0);
			}
			else
			{
				SendDlgItemMessage(hDlg,IDC_IPADDRESS3,IPM_SETADDRESS,0,deviceIP[2]);
				SendDlgItemMessage(hDlg,IDC_IPADDRESS3,WM_ENABLE,TRUE,0);
			}
			break;

		case IDOK:
			{
				char temp[10];
				int res=SendDlgItemMessage(hDlg,IDC_Sensor_CHECK1,BM_GETCHECK,BST_CHECKED,0);
				deviceEnabled[0]=res;
				sprintf(temp,"%i", res);
				WritePrivateProfileString("SensorSettings","Device1Enabled",temp,mwalkerIniPath);
				res=SendDlgItemMessage(hDlg,IDC_SENSOR_CHECK2,BM_GETCHECK,BST_CHECKED,0);
				deviceEnabled[1]=res;
				sprintf(temp,"%i", res);
				WritePrivateProfileString("SensorSettings","Device2Enabled",temp,mwalkerIniPath);
				res=SendDlgItemMessage(hDlg,IDC_SENSOR_CHECK3,BM_GETCHECK,BST_CHECKED,0);
				deviceEnabled[2]=res;
				sprintf(temp,"%i", res);
				WritePrivateProfileString("SensorSettings","Device3Enabled",temp,mwalkerIniPath);
				res = SendDlgItemMessage(hDlg, IDC_WAIT_FOR_CONNECTION_CHECK, BM_GETCHECK, BST_CHECKED, 1);
				bWaitForAPIdevices = res;
				sprintf(temp, "%i", res);
				WritePrivateProfileString("SensorSettings", "WaitForAPIdevices", temp, mwalkerIniPath);

				
//				char lpString4[8];
				//SendDlgItemMessage(hDlg,IDC_SENSORPORTEDIT1,EM_GETLINE,0,(LPARAM)lpString4);
				//devicePort[0]=atoi(lpString4);
				devicePort[0] = GetDlgItemInt(hDlg,IDC_SENSORPORTEDIT1,NULL,FALSE);

				sprintf(temp,"%i", devicePort[0]);

				WritePrivateProfileString("SensorSettings","Device1TCPPort",temp,mwalkerIniPath);
//				char lpString2[8];
				//SendDlgItemMessage(hDlg,IDC_SENSORPORTEDIT2,EM_GETLINE,0,(LPARAM)lpString2);
				//devicePort[1]=atoi(lpString2);
				devicePort[1] =GetDlgItemInt(hDlg,IDC_SENSORPORTEDIT2,NULL,FALSE);
				
				if(devicePort[1]==devicePort[0])
				{
					devicePort[1]=1;
					deviceEnabled[1]=false;
				}
				sprintf(temp,"%i", devicePort[1]);
				WritePrivateProfileString("SensorSettings","Device2TCPPort",temp,mwalkerIniPath);
//				char lpString3[8];
				//SendDlgItemMessage(hDlg,IDC_SENSORPORTEDIT3,EM_GETLINE,0,(LPARAM)lpString3);
				//devicePort[2]=atoi(lpString3);
				devicePort[2]= GetDlgItemInt(hDlg,IDC_SENSORPORTEDIT3,NULL,FALSE);

				if(devicePort[2]==devicePort[1]||devicePort[2]==devicePort[0])
				{
					devicePort[2]=2;
					deviceEnabled[2]=false;
				}
				sprintf(temp,"%i", devicePort[2]);
				WritePrivateProfileString("SensorSettings","Device3TCPPort",temp,mwalkerIniPath);
				res = SendDlgItemMessage(hDlg,IDC_LOCALCHECK1,BM_GETCHECK,0,0);
				deviceLocal[0]=res;
				sprintf(temp,"%i", res);
				WritePrivateProfileString("SensorSettings","Device1Local",temp,mwalkerIniPath);
				res = SendDlgItemMessage(hDlg,IDC_LOCALCHECK2,BM_GETCHECK,0,0);
				deviceLocal[1]=res;
				sprintf(temp,"%i", res);
				WritePrivateProfileString("SensorSettings","Device2Local",temp,mwalkerIniPath);
				res = SendDlgItemMessage(hDlg,IDC_LOCALCHECK3,BM_GETCHECK,0,0);
				deviceLocal[2]=res;
				sprintf(temp,"%i", res);
				WritePrivateProfileString("SensorSettings","Device3Local",temp,mwalkerIniPath);
				if(!deviceLocal[0])
				{
					SendDlgItemMessage(hDlg,IDC_IPADDRESS1,IPM_GETADDRESS,0,(LPARAM)&deviceIP[0]);
					sprintf(temp,"%.0f",(double) deviceIP[0]);
					WritePrivateProfileString("SensorSettings","Device1IP",temp,mwalkerIniPath);
				}
				if(!deviceLocal[1])
				{
					SendDlgItemMessage(hDlg,IDC_IPADDRESS2,IPM_GETADDRESS,0,(LPARAM)&deviceIP[1]);
					sprintf(temp,"%.0f",(double) deviceIP[1]);
					WritePrivateProfileString("SensorSettings","Device2IP",temp,mwalkerIniPath);
				}
				if(!deviceLocal[2])
				{
					SendDlgItemMessage(hDlg,IDC_IPADDRESS3,IPM_GETADDRESS,0,(LPARAM)&deviceIP[2]);
					sprintf(temp,"%.0f",(double) deviceIP[2]);
					WritePrivateProfileString("SensorSettings","Device3IP",temp,mwalkerIniPath);
				}
				


				for(int count=0;count<3;count++)
				{
					if(devicePort[count]==0)
						deviceEnabled[count]=false;
				}

				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK ShaderProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
	case WM_INITDIALOG:
		char temp2[10];
		int xWidth;
		xWidth = GetPrivateProfileInt("Display", "CustomWidth", 5760, mwalkerIniPath);
		int xHeight;
		xHeight = GetPrivateProfileInt("Display", "CustomHeight", 1080, mwalkerIniPath);

		sprintf(temp2, "%d", xWidth);
		SetDlgItemText(hDlg, IDC_EDIT_CustomWidth, temp2);
		sprintf(temp2, "%d", xHeight);
		SetDlgItemText(hDlg, IDC_EDIT_CustomHeight, temp2);

		borderlessWindowed = GetPrivateProfileInt("Display", "BorderlessWindowed", 0, mwalkerIniPath);
		if (borderlessWindowed)
			SendDlgItemMessage(hDlg, IDC_BORDERLESS_CHECK, BM_SETCHECK, BST_CHECKED, 0);

		SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_ADDSTRING,0,(LPARAM)"1");
		SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_ADDSTRING,0,(LPARAM)"2");
		SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_ADDSTRING,0,(LPARAM)"3");
		SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_ADDSTRING,0,(LPARAM)"4");
		SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_ADDSTRING,0,(LPARAM)"5");
		SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_ADDSTRING,0,(LPARAM)"6");
		SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_ADDSTRING,0,(LPARAM)"7");
		SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_ADDSTRING,0,(LPARAM)"8");

		if(ambientShader!=0)
			ambientShader=1;

		SendDlgItemMessage(hDlg,IDC_AMBIENT_COMBO,CB_ADDSTRING,0,(LPARAM)"No Ambient Lighting");
		SendDlgItemMessage(hDlg,IDC_AMBIENT_COMBO,CB_ADDSTRING,0,(LPARAM)"Global Ambient Lighting");

		SendDlgItemMessage(hDlg,IDC_AMBIENT_COMBO,CB_SETCURSEL,ambientShader,0);
		SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_SETCURSEL,(maxlights-1),0);

		if(diffuseShader)
			SendDlgItemMessage(hDlg,IDC_SHADER_DIFFUSE_CHECK,BM_SETCHECK,BST_CHECKED,0);
		if(specularShader)
			SendDlgItemMessage(hDlg,IDC_SHADER_SPECULAR_CHECK,BM_SETCHECK,BST_CHECKED,0);
		if(texturesShader)
			SendDlgItemMessage(hDlg,IDC_SHADER_TEXTURES_CHECK,BM_SETCHECK,BST_CHECKED,0);
		if(objectsShader)
			SendDlgItemMessage(hDlg,IDC_SHADER_OBJECTS_CHECK,BM_SETCHECK,BST_CHECKED,0);

		if(useMSAA)
			SendDlgItemMessage(hDlg,IDC_CHECK_MSAA,BM_SETCHECK,BST_CHECKED,0);
		if(useAniso)
			SendDlgItemMessage(hDlg,IDC_CHECK_ANISO,BM_SETCHECK,BST_CHECKED,0);
		if(useVSYNC)
			SendDlgItemMessage(hDlg,IDC_CHECK_VSYNC,BM_SETCHECK,BST_CHECKED,0);
		
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case 2:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_SHADER_OK:
			{
			EndDialog(hDlg, LOWORD(wParam));


			int res = SendDlgItemMessage(hDlg,IDC_MAXLIGHTS_COMBO,CB_GETCURSEL,0,0);
			maxlights=res+1;
			res=SendDlgItemMessage(hDlg,IDC_AMBIENT_COMBO,CB_GETCURSEL,0,0);
			ambientShader=res;
			res = SendDlgItemMessage(hDlg,IDC_SHADER_DIFFUSE_CHECK,BM_GETCHECK,0,0);
			diffuseShader=res;
			res = SendDlgItemMessage(hDlg,IDC_SHADER_SPECULAR_CHECK,BM_GETCHECK,0,0);
			specularShader=res;
			res = SendDlgItemMessage(hDlg,IDC_SHADER_TEXTURES_CHECK,BM_GETCHECK,0,0);
			texturesShader=res;
			res = SendDlgItemMessage(hDlg,IDC_SHADER_OBJECTS_CHECK,BM_GETCHECK,0,0);
			objectsShader=res;

			res=SendDlgItemMessage(hDlg,IDC_CHECK_MSAA,BM_GETCHECK,0,0);
			useMSAA=res;
			res=SendDlgItemMessage(hDlg,IDC_CHECK_ANISO,BM_GETCHECK,0,0);
			useAniso=res;
			res=SendDlgItemMessage(hDlg,IDC_CHECK_VSYNC,BM_GETCHECK,0,0);
			useVSYNC=res;

			res = SendDlgItemMessage(hDlg, IDC_BORDERLESS_CHECK, BM_GETCHECK, 0, 0);
			borderlessWindowed = res;

			char temp[10];
			sprintf(temp,"%i", diffuseShader);
			WritePrivateProfileString("ShaderSettings","DiffuseEnabled",temp,mwalkerIniPath);
			sprintf(temp,"%i",ambientShader);
			WritePrivateProfileString("ShaderSettings","AmbientMode",temp,mwalkerIniPath);
			sprintf(temp,"%i",specularShader);
			WritePrivateProfileString("ShaderSettings","Specular_Enabled",temp,mwalkerIniPath);
			sprintf(temp,"%i",texturesShader);
			WritePrivateProfileString("ShaderSettings","RenderTextures",temp,mwalkerIniPath);
			sprintf(temp,"%i",objectsShader);
			WritePrivateProfileString("ShaderSettings","ObjectsWithShaders",temp,mwalkerIniPath);
			sprintf(temp,"%i",maxlights);
			WritePrivateProfileString("ShaderSettings","MaxLights",temp,mwalkerIniPath);

			sprintf(temp,"%i",useMSAA);
			WritePrivateProfileString("GraphicsSettings","UseMSAA",temp,mwalkerIniPath);
			sprintf(temp,"%i",useAniso);
			WritePrivateProfileString("GraphicsSettings","UseAnisotropic",temp,mwalkerIniPath);
			sprintf(temp,"%i",useVSYNC);
			WritePrivateProfileString("GraphicsSettings","UseVSYNC",temp,mwalkerIniPath);
			sprintf(temp, "%i", borderlessWindowed);
			WritePrivateProfileString("Display", "BorderlessWindowed", temp, mwalkerIniPath);

			
			GetDlgItemText(hDlg, IDC_EDIT_CustomWidth, temp,10);
			int xWidth=1920;
			xWidth = atoi(temp);
			GetDlgItemText(hDlg, IDC_EDIT_CustomHeight, temp,10);
			int xHeight=1080;
			xHeight = atoi(temp);

			sprintf(temp, "%d", xWidth);
			WritePrivateProfileString("Display", "CustomWidth", temp, mwalkerIniPath);
			sprintf(temp, "%d", xHeight);
			WritePrivateProfileString("Display", "CustomHeight", temp, mwalkerIniPath);

//			char lpString2[8];

			return TRUE;
			}
		case IDC_SHADER_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

bool ignoreEscape=false;
LRESULT CALLBACK ControlProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hButton;
	LPCSTR keyDict[254];
	for(int count=0;count<254;count++)
			keyDict[count]="Other Key";
	keyDict[A]="A";
	keyDict[B]="B";
	keyDict[C]="C";
	keyDict[D]="D";
	keyDict[E]="E";
	keyDict[F]="F";
	keyDict[G]="G";
	keyDict[H]="H";
	keyDict[I]="I";
	keyDict[J]="J";
	keyDict[K]="K";
	keyDict[L]="L";
	keyDict[M]="M";
	keyDict[N]="N";
	keyDict[O]="O";
	keyDict[P]="P";
	keyDict[Q]="Q";
	keyDict[R]="R";
	keyDict[S]="S";
	keyDict[T]="T";
	keyDict[U]="U";
	keyDict[V]="V";
	keyDict[W]="W";
	keyDict[X]="X";
	keyDict[Y]="Y";
	keyDict[Z]="Z";
	keyDict[UP]="Up";
	keyDict[DOWN]="Down";
	keyDict[LEFT]="Left";
	keyDict[RIGHT]="Right";
	keyDict[LSHIFT]="L Shift";
	keyDict[RSHIFT]="R Shift";
	keyDict[SHIFT]="Shift";
	keyDict[LCONTROL]="L Control";
	keyDict[CONTROL]="Control";
	keyDict[RCONTROL]="R Control";
	keyDict[SPACE]="Space";
	keyDict[ADD]="NUM +";
	keyDict[SUBTRACT]="NUM -";
	keyDict[TAB]="Tab";
	keyDict[0x01]="Left Click";
	keyDict[0x02]="Right Click";
	keyDict[VK_ESCAPE]="Not Assigned";
	keyDict[VK_CANCEL]="Not Assigned";
	keyDict[49]="1";
	keyDict[50]="2";
	keyDict[51]="3";
	keyDict[52]="4";
	keyDict[53]="5";
	keyDict[54]="6";
	keyDict[55]="7";
	keyDict[56]="8";
	keyDict[57]="9";
	keyDict[48]="0";
	keyDict[F1]="F1";
	keyDict[F2]="F2";
	keyDict[F3]="F3";
	keyDict[F4]="F4";
	keyDict[F5]="F5";
	keyDict[F6]="F6";
	keyDict[F7]="F7";
	keyDict[F8]="F8";
	keyDict[F9]="F9";
	keyDict[F10]="F10";
	keyDict[F11]="F11";
	keyDict[F12]="F12";
	keyDict[DELETEKEY]="Delete";
	keyDict[VK_RETURN]="Enter";
	keyDict[0xA4]="Left Alt";
	keyDict[0xA5]="Right Alt";
	keyDict[VK_PRIOR]="Page Up";
	keyDict[VK_NEXT]="Page Down";
	keyDict[VK_INSERT]="Insert";
	keyDict[VK_OEM_PLUS]="=";
	keyDict[VK_OEM_MINUS]="-";
	keyDict[VK_END]="End";
	keyDict[NUMPAD0]="NUM 0";
	keyDict[NUMPAD1]="NUM 1";
	keyDict[NUMPAD2]="NUM 2";
	keyDict[NUMPAD3]="NUM 3";
	keyDict[NUMPAD4]="NUM 4";
	keyDict[NUMPAD5]="NUM 5";
	keyDict[NUMPAD6]="NUM 6";
	keyDict[NUMPAD7]="NUM 7";
	keyDict[NUMPAD8]="NUM 8";
	keyDict[NUMPAD9]="NUM 9";
	keyDict[VK_OEM_3]="~ `";
	keyDict[VK_OEM_1]=";";
	keyDict[VK_OEM_2]="/";
	keyDict[VK_OEM_4]="[";
	keyDict[VK_OEM_5]="\\";
	keyDict[VK_OEM_6]="]";
	


	


	switch (message)
	{

	case WM_INITDIALOG:
		if(ControlLibrary[0]!=NULL){
			hButton =GetDlgItem(hDlg,IDC_BUTTON1);
			SetWindowText(hButton,keyDict[ControlLibrary[Forward]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON2);
			SetWindowText(hButton,keyDict[ControlLibrary[Forward+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON3);
			SetWindowText(hButton,keyDict[ControlLibrary[Backward]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON4);
			SetWindowText(hButton,keyDict[ControlLibrary[Backward+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON5);
			SetWindowText(hButton,keyDict[ControlLibrary[StrafeLeft]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON6);
			SetWindowText(hButton,keyDict[ControlLibrary[StrafeLeft+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON7);
			SetWindowText(hButton,keyDict[ControlLibrary[StrafeRight]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON8);
			SetWindowText(hButton,keyDict[ControlLibrary[StrafeRight+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON9);
			SetWindowText(hButton,keyDict[ControlLibrary[LookLeft]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON10);
			SetWindowText(hButton,keyDict[ControlLibrary[LookLeft+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON11);
			SetWindowText(hButton,keyDict[ControlLibrary[LookRight]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON12);
			SetWindowText(hButton,keyDict[ControlLibrary[LookRight+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON13);
			SetWindowText(hButton,keyDict[ControlLibrary[Run]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON14);
			SetWindowText(hButton,keyDict[ControlLibrary[Crouch]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON15);
			SetWindowText(hButton,keyDict[ControlLibrary[SpeedUp]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON16);
			SetWindowText(hButton,keyDict[ControlLibrary[SpeedDown]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON17);
			SetWindowText(hButton,keyDict[ControlLibrary[LightToggle]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON18);
			SetWindowText(hButton,keyDict[ControlLibrary[Jump]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON19);
			SetWindowText(hButton,keyDict[ControlLibrary[CrosshairToggle]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON21);
			SetWindowText(hButton,keyDict[ControlLibrary[TimerToggle]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON20);
			SetWindowText(hButton,keyDict[ControlLibrary[BarToggle]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON23);
			SetWindowText(hButton,keyDict[ControlLibrary[LookUp]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON24);
			SetWindowText(hButton,keyDict[ControlLibrary[LookDown]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON_INTERACT);
			SetWindowText(hButton,keyDict[ControlLibrary[Interact]]);
			hButton = GetDlgItem(hDlg, IDC_BUTTON_RESTART);
			SetWindowText(hButton, keyDict[ControlLibrary[RestartMaze]]);
			hButton = GetDlgItem(hDlg, IDC_BUTTON_NEXT);
			SetWindowText(hButton, keyDict[ControlLibrary[NextMaze]]);
		}
		


		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON1:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON1);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
					
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}
					
				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[Forward]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[Forward]]);
			}
			break;
		case IDC_BUTTON2:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON2);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[Forward+Double]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[Forward+Double]]);
			}
			break;
		case IDC_BUTTON3:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON3);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[Backward]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[Backward]]);
			}
			break;
		case IDC_BUTTON4:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON4);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[Backward+Double]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[Backward+Double]]);
			}
			break;
		case IDC_BUTTON5:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON5);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[StrafeLeft]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[StrafeLeft]]);
			}
			break;
		case IDC_BUTTON6:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON6);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[StrafeLeft+Double]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[StrafeLeft+Double]]);
			}
			break;
		case IDC_BUTTON7:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON7);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[StrafeRight]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[StrafeRight]]);
			}
			break;
		case IDC_BUTTON8:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON8);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[StrafeRight+Double]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[StrafeRight+Double]]);
			}
			break;
		case IDC_BUTTON9:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON9);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}

				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[LookLeft]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[LookLeft]]);
			}
			break;
		case IDC_BUTTON10:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON10);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[LookLeft+Double]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[LookLeft+Double]]);
			}
			break;
		case IDC_BUTTON11:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON11);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[LookRight]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[LookRight]]);
			}
			break;
		case IDC_BUTTON12:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON12);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[LookRight+Double]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[LookRight+Double]]);
			}
			break;
		case IDC_BUTTON13:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON13);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[Run]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[Run]]);
			}
			break;
		case IDC_BUTTON14:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON14);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[Crouch]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[Crouch]]);
			}
			break;
		case IDC_BUTTON15:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON15);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[SpeedUp]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[SpeedUp]]);
			}
			break;
		case IDC_BUTTON16:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON16);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				ControlLibrary[SpeedDown]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[SpeedDown]]);
			}
			break;
		case IDC_BUTTON17:
			{
				ignoreEscape=true;
				hButton =GetDlgItem(hDlg,IDC_BUTTON17);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[LightToggle]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[LightToggle]]);
			}
			break;
		case IDC_BUTTON18:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON18);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[Jump]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[Jump]]);
			}
			break;
		case IDC_BUTTON19:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON19);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[CrosshairToggle]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[CrosshairToggle]]);
			}
			break;
		case IDC_BUTTON20:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON20);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}

				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[BarToggle]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[BarToggle]]);
			}
			break;
		case IDC_BUTTON21:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON21);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}
				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[TimerToggle]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[TimerToggle]]);
			}
			break;
		case IDC_BUTTON23:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON23);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}
				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[LookUp]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[LookUp]]);
			}
			break;
		case IDC_BUTTON24:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON24);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}
				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[LookDown]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[LookDown]]);
			}
			break;
		case IDC_BUTTON_INTERACT:
			{
				hButton =GetDlgItem(hDlg,IDC_BUTTON_INTERACT);
				SetWindowText(hButton,"Press a key");
				bool foundKey=false;
				SHORT key=0;
				int count;
				for(count=1;count<254;count++)
				{
					key=GetAsyncKeyState(count);
				}
				while(key==0)
				{
					for(count=1;count<254;count++)
					{
						key=GetAsyncKeyState(count);
						if(key!=0)
							break;
					}
				}
				if(count==VK_ESCAPE)
				{
					count=VK_CANCEL;
					ignoreEscape=true;
				}
				else
					ignoreEscape=false;
				ControlLibrary[Interact]=count;
				SetWindowText(hButton,keyDict[ControlLibrary[Interact]]);
			}
			break;
		case IDC_BUTTON_RESTART:
		{
			hButton = GetDlgItem(hDlg, IDC_BUTTON_RESTART);
			SetWindowText(hButton, "Press a key");
			bool foundKey = false;
			SHORT key = 0;
			int count;
			for (count = 1; count<254; count++)
			{
				key = GetAsyncKeyState(count);
			}
			while (key == 0)
			{
				for (count = 1; count<254; count++)
				{
					key = GetAsyncKeyState(count);
					if (key != 0)
						break;
				}
			}
			if (count == VK_ESCAPE)
			{
				count = VK_CANCEL;
				ignoreEscape = true;
			}
			else
				ignoreEscape = false;
			ControlLibrary[RestartMaze] = count;
			SetWindowText(hButton, keyDict[ControlLibrary[RestartMaze]]);
		}
			break;
		case IDC_BUTTON_NEXT:
		{
			hButton = GetDlgItem(hDlg, IDC_BUTTON_NEXT);
			SetWindowText(hButton, "Press a key");
			bool foundKey = false;
			SHORT key = 0;
			int count;
			for (count = 1; count<254; count++)
			{
				key = GetAsyncKeyState(count);
			}
			while (key == 0)
			{
				for (count = 1; count<254; count++)
				{
					key = GetAsyncKeyState(count);
					if (key != 0)
						break;
				}
			}
			if (count == VK_ESCAPE)
			{
				count = VK_CANCEL;
				ignoreEscape = true;
			}
			else
				ignoreEscape = false;
			ControlLibrary[NextMaze] = count;
			SetWindowText(hButton, keyDict[ControlLibrary[NextMaze]]);
		}
			break;
		case IDC_RestoreDefault:
			ControlLibrary[Forward]=W;
			ControlLibrary[Forward+Double]=UP;
			ControlLibrary[Backward]=S;
			ControlLibrary[Backward+Double]=DOWN;
			ControlLibrary[StrafeLeft]=A;
			ControlLibrary[StrafeLeft+Double]=VK_ESCAPE;
			ControlLibrary[StrafeRight]=D;
			ControlLibrary[StrafeRight+Double]=VK_ESCAPE;
			ControlLibrary[Run]=LSHIFT;
			ControlLibrary[Crouch]=LCONTROL;
			ControlLibrary[LightToggle]=L;
			ControlLibrary[CrosshairToggle]=C;
			ControlLibrary[BarToggle]=B;
			ControlLibrary[TimerToggle]=T;
			ControlLibrary[Jump]=SPACE;
			ControlLibrary[LookLeft]=LEFT;
			ControlLibrary[LookLeft+Double]=VK_ESCAPE;
			ControlLibrary[LookRight]=RIGHT;
			ControlLibrary[LookRight+Double]=VK_ESCAPE;
			ControlLibrary[SpeedUp]=ADD;
			ControlLibrary[SpeedDown]=SUBTRACT;
			ControlLibrary[LookUp]=VK_PRIOR;
			ControlLibrary[LookDown]=VK_NEXT;
			ControlLibrary[Interact]=RETURN;
			ControlLibrary[RestartMaze] = F9;
			ControlLibrary[NextMaze] = F10;

			hButton=NULL;
			hButton =GetDlgItem(hDlg,IDC_BUTTON1);
			SetWindowText(hButton,keyDict[ControlLibrary[Forward]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON2);
			SetWindowText(hButton,keyDict[ControlLibrary[Forward+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON3);
			SetWindowText(hButton,keyDict[ControlLibrary[Backward]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON4);
			SetWindowText(hButton,keyDict[ControlLibrary[Backward+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON5);
			SetWindowText(hButton,keyDict[ControlLibrary[StrafeLeft]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON6);
			SetWindowText(hButton,keyDict[ControlLibrary[StrafeLeft+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON7);
			SetWindowText(hButton,keyDict[ControlLibrary[StrafeRight]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON8);
			SetWindowText(hButton,keyDict[ControlLibrary[StrafeRight+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON9);
			SetWindowText(hButton,keyDict[ControlLibrary[LookLeft]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON10);
			SetWindowText(hButton,keyDict[ControlLibrary[LookLeft+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON11);
			SetWindowText(hButton,keyDict[ControlLibrary[LookRight]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON12);
			SetWindowText(hButton,keyDict[ControlLibrary[LookRight+Double]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON13);
			SetWindowText(hButton,keyDict[ControlLibrary[Run]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON14);
			SetWindowText(hButton,keyDict[ControlLibrary[Crouch]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON15);
			SetWindowText(hButton,keyDict[ControlLibrary[SpeedUp]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON16);
			SetWindowText(hButton,keyDict[ControlLibrary[SpeedDown]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON17);
			SetWindowText(hButton,keyDict[ControlLibrary[LightToggle]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON18);
			SetWindowText(hButton,keyDict[ControlLibrary[Jump]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON19);
			SetWindowText(hButton,keyDict[ControlLibrary[CrosshairToggle]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON21);
			SetWindowText(hButton,keyDict[ControlLibrary[TimerToggle]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON20);
			SetWindowText(hButton,keyDict[ControlLibrary[BarToggle]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON23);
			SetWindowText(hButton,keyDict[ControlLibrary[LookUp]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON24);
			SetWindowText(hButton,keyDict[ControlLibrary[LookDown]]);
			hButton =GetDlgItem(hDlg,IDC_BUTTON_INTERACT);
			SetWindowText(hButton,keyDict[ControlLibrary[Interact]]);
			hButton = GetDlgItem(hDlg, IDC_BUTTON_RESTART);
			SetWindowText(hButton, keyDict[ControlLibrary[RestartMaze]]);
			hButton = GetDlgItem(hDlg, IDC_BUTTON_NEXT);
			SetWindowText(hButton, keyDict[ControlLibrary[NextMaze]]);
			return TRUE;


		case 2:
			if(!ignoreEscape)
			{
				EndDialog(hDlg, LOWORD(wParam));
				ControlLibrary[Forward]=GetPrivateProfileInt("Controls","Forward",W,mwalkerIniPath);
				ControlLibrary[Forward+Double]=GetPrivateProfileInt("Controls","Forward2",UP,mwalkerIniPath);
				ControlLibrary[Backward]=GetPrivateProfileInt("Controls","Backward",S,mwalkerIniPath);
				ControlLibrary[Backward+Double]=GetPrivateProfileInt("Controls","Backward2",DOWN,mwalkerIniPath);
				ControlLibrary[StrafeLeft]=GetPrivateProfileInt("Controls","StrafeLeft",A,mwalkerIniPath);
				ControlLibrary[StrafeLeft+Double]=GetPrivateProfileInt("Controls","StrafeLeft2",VK_ESCAPE,mwalkerIniPath);
				ControlLibrary[StrafeRight]=GetPrivateProfileInt("Controls","StrafeRight",D,mwalkerIniPath);
				ControlLibrary[StrafeRight+Double]=GetPrivateProfileInt("Controls","StrafeRight2",VK_ESCAPE,mwalkerIniPath);
				ControlLibrary[Run]=GetPrivateProfileInt("Controls","Run",LSHIFT,mwalkerIniPath);
				ControlLibrary[Crouch]=GetPrivateProfileInt("Controls","Crouch",CONTROL,mwalkerIniPath);
				ControlLibrary[LightToggle]=GetPrivateProfileInt("Controls","LightToggle",L,mwalkerIniPath);
				ControlLibrary[CrosshairToggle]=GetPrivateProfileInt("Controls","CrosshairToggle",C,mwalkerIniPath);
				ControlLibrary[BarToggle]=GetPrivateProfileInt("Controls","BarToggle",B,mwalkerIniPath);
				ControlLibrary[TimerToggle]=GetPrivateProfileInt("Controls","TimerToggle",T,mwalkerIniPath);
				ControlLibrary[Jump]=GetPrivateProfileInt("Controls","Jump",SPACE,mwalkerIniPath);
				ControlLibrary[LookLeft]=GetPrivateProfileInt("Controls","LookLeft",LEFT,mwalkerIniPath);
				ControlLibrary[LookLeft+Double]=GetPrivateProfileInt("Controls","LookLeft2",VK_ESCAPE,mwalkerIniPath);
				ControlLibrary[LookRight]=GetPrivateProfileInt("Controls","LookRight",RIGHT,mwalkerIniPath);
				ControlLibrary[LookRight+Double]=GetPrivateProfileInt("Controls","LookRight2",VK_ESCAPE,mwalkerIniPath);
				ControlLibrary[SpeedUp]=GetPrivateProfileInt("Controls","SpeedUp",ADD,mwalkerIniPath);
				ControlLibrary[SpeedDown]=GetPrivateProfileInt("Controls","SpeedDown",SUBTRACT,mwalkerIniPath);
				ControlLibrary[LookDown]=GetPrivateProfileInt("Controls","LookDown",VK_NEXT,mwalkerIniPath);
				ControlLibrary[LookUp]=GetPrivateProfileInt("Controls","LookUp",VK_PRIOR,mwalkerIniPath);
				ControlLibrary[Interact]=GetPrivateProfileInt("Controls","Interact",RETURN,mwalkerIniPath);
				ControlLibrary[RestartMaze] = GetPrivateProfileInt("Controls", "RestartMaze", F9, mwalkerIniPath);
				ControlLibrary[NextMaze] = GetPrivateProfileInt("Controls", "NextMaze", F10, mwalkerIniPath);
				return TRUE;
			}
			else
				ignoreEscape=false;
			break;
		case IDC_KEYBOARD_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			ControlLibrary[Forward]=GetPrivateProfileInt("Controls","Forward",W,mwalkerIniPath);
			ControlLibrary[Forward+Double]=GetPrivateProfileInt("Controls","Forward2",UP,mwalkerIniPath);
			ControlLibrary[Backward]=GetPrivateProfileInt("Controls","Backward",S,mwalkerIniPath);
			ControlLibrary[Backward+Double]=GetPrivateProfileInt("Controls","Backward2",DOWN,mwalkerIniPath);
			ControlLibrary[StrafeLeft]=GetPrivateProfileInt("Controls","StrafeLeft",A,mwalkerIniPath);
			ControlLibrary[StrafeLeft+Double]=GetPrivateProfileInt("Controls","StrafeLeft2",VK_ESCAPE,mwalkerIniPath);
			ControlLibrary[StrafeRight]=GetPrivateProfileInt("Controls","StrafeRight",D,mwalkerIniPath);
			ControlLibrary[StrafeRight+Double]=GetPrivateProfileInt("Controls","StrafeRight2",VK_ESCAPE,mwalkerIniPath);
			ControlLibrary[Run]=GetPrivateProfileInt("Controls","Run",LSHIFT,mwalkerIniPath);
			ControlLibrary[Crouch]=GetPrivateProfileInt("Controls","Crouch",CONTROL,mwalkerIniPath);
			ControlLibrary[LightToggle]=GetPrivateProfileInt("Controls","LightToggle",L,mwalkerIniPath);
			ControlLibrary[CrosshairToggle]=GetPrivateProfileInt("Controls","CrosshairToggle",C,mwalkerIniPath);
			ControlLibrary[BarToggle]=GetPrivateProfileInt("Controls","BarToggle",B,mwalkerIniPath);
			ControlLibrary[TimerToggle]=GetPrivateProfileInt("Controls","TimerToggle",T,mwalkerIniPath);
			ControlLibrary[Jump]=GetPrivateProfileInt("Controls","Jump",SPACE,mwalkerIniPath);
			ControlLibrary[LookLeft]=GetPrivateProfileInt("Controls","LookLeft",LEFT,mwalkerIniPath);
			ControlLibrary[LookLeft+Double]=GetPrivateProfileInt("Controls","LookLeft2",VK_ESCAPE,mwalkerIniPath);
			ControlLibrary[LookRight]=GetPrivateProfileInt("Controls","LookRight",RIGHT,mwalkerIniPath);
			ControlLibrary[LookRight+Double]=GetPrivateProfileInt("Controls","LookRight2",VK_ESCAPE,mwalkerIniPath);
			ControlLibrary[SpeedUp]=GetPrivateProfileInt("Controls","SpeedUp",ADD,mwalkerIniPath);
			ControlLibrary[SpeedDown]=GetPrivateProfileInt("Controls","SpeedDown",SUBTRACT,mwalkerIniPath);
			ControlLibrary[LookDown]=GetPrivateProfileInt("Controls","LookDown",VK_NEXT,mwalkerIniPath);
			ControlLibrary[LookUp]=GetPrivateProfileInt("Controls","LookUp",VK_PRIOR,mwalkerIniPath);
			ControlLibrary[Interact]=GetPrivateProfileInt("Controls","Interact",RETURN,mwalkerIniPath);
			ControlLibrary[RestartMaze] = GetPrivateProfileInt("Controls", "RestartMaze", F9, mwalkerIniPath);
			ControlLibrary[NextMaze] = GetPrivateProfileInt("Controls", "NextMaze", F10, mwalkerIniPath);
			return TRUE;
		
		case IDC_CONT:
			{
			EndDialog(hDlg, LOWORD(wParam));
			char temp[10];
			sprintf(temp,"%d",ControlLibrary[Forward]);
			WritePrivateProfileString("Controls","Forward",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[Forward+Double]);
			WritePrivateProfileString("Controls","Forward2",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[Backward]);
			WritePrivateProfileString("Controls","Backward",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[Backward+Double]);
			WritePrivateProfileString("Controls","Backward2",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[StrafeLeft]);
			WritePrivateProfileString("Controls","StrafeLeft",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[StrafeLeft+Double]);
			WritePrivateProfileString("Controls","StrafeLeft2",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[StrafeRight]);
			WritePrivateProfileString("Controls","StrafeRight",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[StrafeRight+Double]);
			WritePrivateProfileString("Controls","StrafeRight2",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[LookLeft]);
			WritePrivateProfileString("Controls","LookLeft",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[LookLeft+Double]);
			WritePrivateProfileString("Controls","LookLeft2",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[LookRight]);
			WritePrivateProfileString("Controls","LookRight",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[LookRight+Double]);
			WritePrivateProfileString("Controls","LookRight2",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[Run]);
			WritePrivateProfileString("Controls","Run",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[Crouch]);
			WritePrivateProfileString("Controls","Crouch",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[Jump]);
			WritePrivateProfileString("Controls","Jump",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[LightToggle]);
			WritePrivateProfileString("Controls","LightToggle",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[BarToggle]);
			WritePrivateProfileString("Controls","BarToggle",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[TimerToggle]);
			WritePrivateProfileString("Controls","TimerToggle",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[CrosshairToggle]);
			WritePrivateProfileString("Controls","CrosshairToggle",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[SpeedUp]);
			WritePrivateProfileString("Controls","SpeedUp",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[SpeedDown]);
			WritePrivateProfileString("Controls","SpeedDown",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[LookDown]);
			WritePrivateProfileString("Controls","LookDown",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[LookUp]);
			WritePrivateProfileString("Controls","LookUp",temp,mwalkerIniPath);
			sprintf(temp,"%d",ControlLibrary[Interact]);
			WritePrivateProfileString("Controls","Interact",temp,mwalkerIniPath);
			sprintf(temp, "%d", ControlLibrary[RestartMaze]);
			WritePrivateProfileString("Controls", "RestartMaze", temp, mwalkerIniPath);
			sprintf(temp, "%d", ControlLibrary[NextMaze]);
			WritePrivateProfileString("Controls", "NextMaze", temp, mwalkerIniPath);

			return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK TxtDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT currentRC;
	RECT parentRC;
	int width1,width2;
	int height1,height2;
	char txt[TXTBUFFERLIMIT];
	switch (message)
	{
	case WM_INITDIALOG:
		int i,j;
		for(i =0,j=0;filename[i]!=NULL&&i<TXTBUFFERLIMIT;j++,i++)
		{
			if(filename[i]=='\\')
			{
				i++;
				if(filename[i]=='n')
					txt[j]= '\n';
				else if(filename[i]=='t')
					txt[j]= '\t';
				else if(filename[i]=='b')
					txt[j]= '\b';
			}
			else
			{
				txt[j]=filename[i];
			}
		}
		txt[j]=NULL;

		SetDlgItemText(hDlg,IDC_MES,txt);
		GetWindowRect(hWnd,&parentRC);
		GetWindowRect(hDlg,&currentRC);
		width1 = parentRC.right - parentRC.left;
		width2 = currentRC.right - currentRC.left;

		height1 = parentRC.bottom - parentRC.top;
		height2 = currentRC.bottom - currentRC.top;
		MoveWindow(hDlg,parentRC.left + (width1 - width2)/2,parentRC.top + (height1-height2)/2,width2,height2,true);

		if(lifeTime>0)
		{
			SetTimer(hDlg,1,lifeTime,NULL);
			ShowWindow(GetDlgItem(hDlg,IDC_CONT),SW_HIDE);
		}

		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_CONT:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	case WM_TIMER:
		KillTimer(hDlg,1);
		EndDialog(hDlg,LOWORD(wParam));
		break;
	}
	return FALSE;
}

LRESULT CALLBACK OculusProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_INITDIALOG:
	{
		if (oculusEnabled)
			SendDlgItemMessage(hDlg, IDC_OCULUS_CHECK, BM_SETCHECK, BST_CHECKED, 0);
		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{

		case IDOK:
		{
			char temp[10];
			int res = SendDlgItemMessage(hDlg, IDC_OCULUS_CHECK, BM_GETCHECK, BST_CHECKED, 0);
			//oculusEnabled = res;
			sprintf(temp, "%i", res);
			WritePrivateProfileString("OculusSettings", "OculusEnabled", temp, mwalkerIniPath);

			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		case 2:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_Default:

			SendDlgItemMessage(hDlg, IDC_OCULUS_CHECK, BM_SETCHECK, BST_UNCHECKED, 0);

		}
		break;
	}
	return FALSE;
}


LRESULT CALLBACK OtherProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_INITDIALOG:
		{
			if(developerKeys)
				SendDlgItemMessage(hDlg,IDC_DEVKEYSCHECK,BM_SETCHECK,BST_CHECKED,0);
			if(interactivePlayback)
				SendDlgItemMessage(hDlg,IDC_INTERACTIVECHECK,BM_SETCHECK,BST_CHECKED,0);
			if(devFPS)
				SendDlgItemMessage(hDlg,IDC_FPSCHECK,BM_SETCHECK,BST_CHECKED,0);
			if (hideRenderedMaze)
				SendDlgItemMessage(hDlg, IDC_HIDE_RENDERED_MAZE_CHECK, BM_SETCHECK, BST_CHECKED, 0);
			if (bDisableGravity)
				SendDlgItemMessage(hDlg, IDC_DISABLE_GRAVITY_CHECK, BM_SETCHECK, BST_CHECKED, 0);

			SendDlgItemMessage(hDlg, IDC_SCOREBAR_COMBO, CB_ADDSTRING, 0, (LPARAM)"API Score");
			SendDlgItemMessage(hDlg, IDC_SCOREBAR_COMBO, CB_ADDSTRING, 0, (LPARAM)"Maze Points");
			

			SendDlgItemMessage(hDlg, IDC_SCOREBAR_COMBO, CB_SETCURSEL, iScoreBarMode, 0);

			break;
		}
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		
		case IDOK:
			{
				char temp[10];
				int res=SendDlgItemMessage(hDlg,IDC_DEVKEYSCHECK,BM_GETCHECK,BST_CHECKED,0);
				developerKeys=res;
				sprintf(temp,"%i", res);
				WritePrivateProfileString("OtherSettings","DevKeysEnabled",temp,mwalkerIniPath);

				res=SendDlgItemMessage(hDlg,IDC_INTERACTIVECHECK,BM_GETCHECK,BST_CHECKED,0);
				interactivePlayback=res;
				sprintf(temp,"%i", res);
				WritePrivateProfileString("OtherSettings","InteractivePlayback",temp,mwalkerIniPath);

				res=SendDlgItemMessage(hDlg,IDC_FPSCHECK,BM_GETCHECK,BST_CHECKED,0);
				devFPS=res==1;
				sprintf(temp,"%i", res);
				WritePrivateProfileString("OtherSettings","DevFPSEnabled",temp,mwalkerIniPath);
				res=SendDlgItemMessage(hDlg,IDC_INTERACTIVECHECK,BM_GETCHECK,BST_CHECKED,0);

				res = SendDlgItemMessage(hDlg, IDC_HIDE_RENDERED_MAZE_CHECK, BM_GETCHECK, BST_CHECKED, 0);
				hideRenderedMaze=res;
				sprintf(temp, "%i", res);
				WritePrivateProfileString("OtherSettings", "HideRenderedMaze", temp, mwalkerIniPath);

				res = SendDlgItemMessage(hDlg, IDC_DISABLE_GRAVITY_CHECK, BM_GETCHECK, BST_CHECKED, 0);
				bDisableGravity = res;
				sprintf(temp, "%i", res);
				WritePrivateProfileString("OtherSettings", "DisableGravity", temp, mwalkerIniPath);

				iScoreBarMode =  SendDlgItemMessage(hDlg, IDC_SCOREBAR_COMBO, CB_GETCURSEL, 0, 0);
				sprintf(temp, "%i", iScoreBarMode);
				WritePrivateProfileString("OtherSettings", "ScoreBarMode", temp, mwalkerIniPath);
				
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
		case 2:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_Default:

			SendDlgItemMessage(hDlg,IDC_DEVKEYSCHECK,BM_SETCHECK,BST_UNCHECKED,0);
	
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK ViewProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		SendDlgItemMessage(hDlg, IDC_PRESEPECTIVE_COMBO, CB_ADDSTRING, 0, (LPARAM)"First-Person");
		SendDlgItemMessage(hDlg, IDC_PRESEPECTIVE_COMBO, CB_ADDSTRING, 0, (LPARAM)"Top-Down");
		SendDlgItemMessage(hDlg, IDC_PRESEPECTIVE_COMBO, CB_ADDSTRING, 0, (LPARAM)"3/4-View or Fixed");

		char temp[10];
		sprintf(temp, "%.0f", fieldOfView);
		SetWindowText(GetDlgItem(hDlg, IDC_FOV_EDIT), temp);

		int dSetting;
		dSetting = GetPrivateProfileInt("ViewSettings", "PerspectiveView", 0, mwalkerIniPath);
		SendDlgItemMessage(hDlg, IDC_PRESEPECTIVE_COMBO, CB_SETCURSEL, dSetting, 0);
		if (dSetting == 1)
		{
			bTopDown = true;
			b34View = false;
		}
		else if (dSetting==2)
		{
			bTopDown = false;
			b34View = true;
		}
		else
		{
			bTopDown = false;
			b34View = false;
		}

		SendDlgItemMessage(hDlg, IDC_ORIENTATION_COMBO, CB_ADDSTRING, 0, (LPARAM)"Free");
		SendDlgItemMessage(hDlg, IDC_ORIENTATION_COMBO, CB_ADDSTRING, 0, (LPARAM)"North");
		SendDlgItemMessage(hDlg, IDC_ORIENTATION_COMBO, CB_ADDSTRING, 0, (LPARAM)"South");
		SendDlgItemMessage(hDlg, IDC_ORIENTATION_COMBO, CB_ADDSTRING, 0, (LPARAM)"East");
		SendDlgItemMessage(hDlg, IDC_ORIENTATION_COMBO, CB_ADDSTRING, 0, (LPARAM)"West");

		dSetting = GetPrivateProfileInt("ViewSettings", "TopDownOrientation", 0, mwalkerIniPath);
		SendDlgItemMessage(hDlg, IDC_ORIENTATION_COMBO, CB_SETCURSEL, dSetting, 0);
		iTopDownOrientation = dSetting;

		int res = 0;
		res = GetPrivateProfileInt("ViewSettings", "TopDownMouseOrient", 0, mwalkerIniPath);
		bTopDownMouseAngleMode = res;
		if (bTopDownMouseAngleMode)
			SendDlgItemMessage(hDlg, IDC_TOPDOWN_MOUSE_CHECK, BM_SETCHECK, BST_CHECKED, 0);
		res = GetPrivateProfileInt("ViewSettings", "TopDownLineRender", 0, mwalkerIniPath);
		bTopDownSimpleRender = res;
		if (bTopDownSimpleRender)
			SendDlgItemMessage(hDlg, IDC_SIMPLE_RENDERCHECK, BM_SETCHECK, BST_CHECKED, 0);

		iTopDownHeight = GetPrivateProfileInt("ViewSettings", "CameraHeight", 15, mwalkerIniPath);
		SetDlgItemInt(hDlg, IDC_EDIT_HEIGHT, iTopDownHeight, false);

		res = GetPrivateProfileInt("ViewSettings", "bFixedCameraX", 0, mwalkerIniPath);
		bFixedCameraLocationX = res;
		if (bFixedCameraLocationX)
			SendDlgItemMessage(hDlg, IDC_FIXED_CAMERA_X_CHECK, BM_SETCHECK, BST_CHECKED, 0);

		res = GetPrivateProfileInt("ViewSettings", "bFixedCameraZ", 0, mwalkerIniPath);
		bFixedCameraLocationZ = res;
		if (bFixedCameraLocationZ)
			SendDlgItemMessage(hDlg, IDC_FIXED_CAMERA_Z_CHECK, BM_SETCHECK, BST_CHECKED, 0);

		fixedCameraLocationX = GetPrivateProfileInt("ViewSettings", "FixedCameraX", 10, mwalkerIniPath);
		SetDlgItemInt(hDlg, IDC_EDIT_FIXX, fixedCameraLocationX, false);

		fixedCameraLocationZ = GetPrivateProfileInt("ViewSettings", "FixedCameraZ", 10, mwalkerIniPath);
		SetDlgItemInt(hDlg, IDC_EDIT_FIXZ, fixedCameraLocationZ, false);


		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			char temp[10];
			int res = SendDlgItemMessage(hDlg, IDC_SIMPLE_RENDERCHECK, BM_GETCHECK, BST_CHECKED, 0);
			bTopDownSimpleRender = res;
			sprintf(temp, "%i", res);
			WritePrivateProfileString("ViewSettings", "TopDownLineRender", temp, mwalkerIniPath);

			res = SendDlgItemMessage(hDlg, IDC_TOPDOWN_MOUSE_CHECK, BM_GETCHECK, BST_CHECKED, 0);
			bTopDownMouseAngleMode = res == 1;
			sprintf(temp, "%i", res);
			WritePrivateProfileString("ViewSettings", "TopDownMouseOrient", temp, mwalkerIniPath);
			res = SendDlgItemMessage(hDlg, IDC_INTERACTIVECHECK, BM_GETCHECK, BST_CHECKED, 0);


			bTopDown = 1 == SendDlgItemMessage(hDlg, IDC_PRESEPECTIVE_COMBO, CB_GETCURSEL, 0, 0);
			b34View = 2 == SendDlgItemMessage(hDlg, IDC_PRESEPECTIVE_COMBO, CB_GETCURSEL, 0, 0);
			if (bTopDown)
				sprintf(temp, "%i", 1);
			else if (b34View)
				sprintf(temp, "%i", 2);
			else
				sprintf(temp, "%i", 0);
			WritePrivateProfileString("ViewSettings", "PerspectiveView", temp, mwalkerIniPath);

			iTopDownOrientation = SendDlgItemMessage(hDlg, IDC_ORIENTATION_COMBO, CB_GETCURSEL, 0, 0);
			sprintf(temp, "%i", iTopDownOrientation);
			WritePrivateProfileString("ViewSettings", "TopDownOrientation", temp, mwalkerIniPath);

			iTopDownHeight = GetDlgItemInt(hDlg, IDC_EDIT_HEIGHT, 0, FALSE);

			if (iTopDownHeight <= 1)
				iTopDownHeight = 1;
			else if (iTopDownHeight > 100)
				iTopDownHeight = 100;

			sprintf(temp, "%f", iTopDownHeight);
			WritePrivateProfileString("ViewSettings", "CameraHeight", temp, mwalkerIniPath);

			bFixedCameraLocationX = SendDlgItemMessage(hDlg, IDC_FIXED_CAMERA_X_CHECK, BM_GETCHECK, BST_CHECKED, 0);
			sprintf(temp, "%i", bFixedCameraLocationX);
			WritePrivateProfileString("ViewSettings", "bFixedCameraX", temp, mwalkerIniPath);

			bFixedCameraLocationZ = SendDlgItemMessage(hDlg, IDC_FIXED_CAMERA_Z_CHECK, BM_GETCHECK, BST_CHECKED, 0);
			sprintf(temp, "%i", bFixedCameraLocationZ);
			WritePrivateProfileString("ViewSettings", "bFixedCameraZ", temp, mwalkerIniPath);

			fixedCameraLocationX = GetDlgItemInt(hDlg, IDC_EDIT_FIXX, 0, FALSE);
			sprintf(temp, "%f", fixedCameraLocationX);
			WritePrivateProfileString("ViewSettings", "FixedCameraX", temp, mwalkerIniPath);

			fixedCameraLocationZ = GetDlgItemInt(hDlg, IDC_EDIT_FIXZ, 0, FALSE);
			sprintf(temp, "%f", fixedCameraLocationZ);
			WritePrivateProfileString("ViewSettings", "FixedCameraZ", temp, mwalkerIniPath);


			fieldOfView = GetDlgItemInt(hDlg, IDC_FOV_EDIT, NULL, FALSE);
			sprintf(temp, "%.0f", fieldOfView);
			WritePrivateProfileString("ViewSettings", "FieldOfView", temp, mwalkerIniPath);

			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		case 2:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_Default:
			SendDlgItemMessage(hDlg, IDC_DEVKEYSCHECK, BM_SETCHECK, BST_UNCHECKED, 0);

		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK SyncProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_INITDIALOG:
		{
			if(cueSkipButton)
				SendDlgItemMessage(hDlg,IDC_SKIPBUTTONCHECK,BM_SETCHECK,BST_CHECKED,0);

			SendDlgItemMessage(hDlg,IDC_CUECOMBO,CB_ADDSTRING,0,(LPARAM)"Cue Disabled");
			SendDlgItemMessage(hDlg,IDC_CUECOMBO,CB_ADDSTRING,0,(LPARAM)"Serial Char Cue");
			SendDlgItemMessage(hDlg,IDC_CUECOMBO,CB_ADDSTRING,0,(LPARAM)"Network (API) Cue");
			SendDlgItemMessage(hDlg,IDC_CUECOMBO,CB_ADDSTRING,0,(LPARAM)"Keyboard Cue");
			SendDlgItemMessage(hDlg,IDC_CUECOMBO,CB_ADDSTRING,0,(LPARAM)"Joystick Cue");

			SendDlgItemMessage(hDlg,IDC_CUECOMBO,CB_SETCURSEL,cueType,0);

			SendDlgItemMessage(hDlg,IDC_JOYCUECOMBO,CB_ADDSTRING,0,(LPARAM)"Button 1");
			SendDlgItemMessage(hDlg,IDC_JOYCUECOMBO,CB_ADDSTRING,0,(LPARAM)"Button 2");
			SendDlgItemMessage(hDlg,IDC_JOYCUECOMBO,CB_ADDSTRING,0,(LPARAM)"Button 3");
			SendDlgItemMessage(hDlg,IDC_JOYCUECOMBO,CB_ADDSTRING,0,(LPARAM)"Button 4");

			SendDlgItemMessage(hDlg,IDC_JOYCUECOMBO,CB_SETCURSEL,joyCueButton-1,0);

			SetDlgItemInt(hDlg,IDC_EDIT_TRIGGERCHAR,triggerChar,false);

			break;
		}
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{

		case IDOK:
			{
				char temp[10];
//				int res;

				cueType = SendDlgItemMessage(hDlg,IDC_CUECOMBO,CB_GETCURSEL,0,0);
				sprintf(temp,"%i", cueType);
				WritePrivateProfileString("OtherSettings","CueType",temp,mwalkerIniPath);

				joyCueButton = SendDlgItemMessage(hDlg,IDC_JOYCUECOMBO,CB_GETCURSEL,0,0)+1;
				sprintf(temp,"%i", joyCueButton);
				WritePrivateProfileString("OtherSettings","joyCueButton",temp,mwalkerIniPath);

				BOOL flag1=FALSE;
				triggerChar = GetDlgItemInt(hDlg, IDC_EDIT_TRIGGERCHAR,&flag1,FALSE);
				sprintf(temp,"%d",triggerChar);
				WritePrivateProfileString("Com","triggerChar",temp,mwalkerIniPath);

				cueSkipButton = SendDlgItemMessage(hDlg,IDC_SKIPBUTTONCHECK,BM_GETCHECK,0,0);
				if(cueSkipButton) 
				{
					WritePrivateProfileString("OtherSettings","cueSkipButton","1",mwalkerIniPath);
				}
				else
				{
					WritePrivateProfileString("OtherSettings","cueSkipButton","0",mwalkerIniPath);
				}

				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
		case 2:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_Default:
			SendDlgItemMessage(hDlg,IDC_SKIPBUTTONCHECK,BM_SETCHECK,BST_CHECKED,0);
			SetDlgItemInt(hDlg,IDC_EDIT_TRIGGERCHAR,116,false);
			SendDlgItemMessage(hDlg,IDC_JOYCUECOMBO,CB_SETCURSEL,0,0);
			SendDlgItemMessage(hDlg,IDC_CUECOMBO,CB_SETCURSEL,0,0);

		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK CommProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int res=0;
//	char str[10];
	switch (message)
	{
	case WM_INITDIALOG:
		{
			SendDlgItemMessage(hDlg,IDC_SerialCheck,BM_SETCHECK,bSendSync,0);
			SendDlgItemMessage(hDlg,IDC_LISTENCHECK,BM_SETCHECK,bListen,0);
			SendDlgItemMessage(hDlg, IDC_LPT_CHECK, BM_SETCHECK, bUseLPT, 0);
			char* newLPTstring = new char[256];
			sprintf(newLPTstring, "Send Maze Start/End over LPT:%i", iLptPortNum);
			SendDlgItemMessage(hDlg, IDC_LPT_CHECK, WM_SETTEXT,0, (LPARAM)newLPTstring);
			sprintf(newLPTstring, "%i", iLptPortNum);
			SetDlgItemText(hDlg, IDC_EDITLPT, newLPTstring);

			sprintf(newLPTstring, "%i", iLptDelay);
			SetDlgItemText(hDlg, IDC_EDITLPT_DELAY, newLPTstring);

			SendDlgItemMessage(hDlg,IDC_BAUDRATECOMBO,CB_ADDSTRING,0,(LPARAM)"1200");
			SendDlgItemMessage(hDlg,IDC_BAUDRATECOMBO,CB_ADDSTRING,0,(LPARAM)"2400");
			SendDlgItemMessage(hDlg,IDC_BAUDRATECOMBO,CB_ADDSTRING,0,(LPARAM)"4800");
			SendDlgItemMessage(hDlg,IDC_BAUDRATECOMBO,CB_ADDSTRING,0,(LPARAM)"9600");
			SendDlgItemMessage(hDlg,IDC_BAUDRATECOMBO,CB_ADDSTRING,0,(LPARAM)"14400");
			SendDlgItemMessage(hDlg,IDC_BAUDRATECOMBO,CB_ADDSTRING,0,(LPARAM)"19200");

			SendDlgItemMessage(hDlg,IDC_PARITYCOMBO,CB_ADDSTRING,0,(LPARAM)"None");
			SendDlgItemMessage(hDlg,IDC_PARITYCOMBO,CB_ADDSTRING,0,(LPARAM)"Odd");
			//SendDlgItemMessage(hDlg, IDC_PARITYCOMBO, CB_ADDSTRING, 0, (LPARAM)"Even");

			SendDlgItemMessage(hDlg,IDC_STOPBITSCOMBO,CB_ADDSTRING,0,(LPARAM)"1 Stopbit");
			SendDlgItemMessage(hDlg,IDC_STOPBITSCOMBO,CB_ADDSTRING,0,(LPARAM)"1.5 Stopbits");
			SendDlgItemMessage(hDlg, IDC_STOPBITSCOMBO, CB_ADDSTRING, 0, (LPARAM)"2 Stopbits");

			TCHAR lpTargetPath[5000]; // buffer to store the path of the COMPORTS
			int numPorts = 0;
			char str[100];
			for (int i = 0; i<20; i++) // checking ports from COM0 to COM20
			{
				
				sprintf(str, "COM%i",i);

				// Test the return value and error if any
				if (QueryDosDevice(str, (LPSTR)lpTargetPath, 5000) != 0) //QueryDosDevice returns zero if it didn't find an object
				{
					SendDlgItemMessage(hDlg, IDC_PORTCOMBO, CB_ADDSTRING, 0, (LPARAM)str);
					
					if(comPort==i)
						SendDlgItemMessage(hDlg, IDC_PORTCOMBO, CB_SETCURSEL, numPorts, 0);
					numPorts++;
				}


			}

			if (numPorts == 0)
			{
				SendDlgItemMessage(hDlg, IDC_PORTCOMBO, CB_ADDSTRING, 0, (LPARAM)"No Ports Detected");
				SendDlgItemMessage(hDlg, IDC_PORTCOMBO, CB_SETCURSEL, numPorts, 0);
			}
			sprintf(str, "COM%i", comPort);
			SetDlgItemText(hDlg, IDC_STATICCURRENTPORT, (LPCSTR)str);

			
			SendDlgItemMessage(hDlg,IDC_PARITYCOMBO,CB_SETCURSEL,parity,0);
			SendDlgItemMessage(hDlg,IDC_STOPBITSCOMBO,CB_SETCURSEL,stopBits,0);
			SendDlgItemMessage(hDlg,IDC_BAUDRATECOMBO,CB_SETCURSEL,baudRate,0);

			//sprintf_s(str,10, "%d" , triggerChar);
			//SetDlgItemText(hDlg, IDC_EDIT1, str);

			

			break;
		}


	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_SETPORT:
		{
			char str[20];
			char temp[10];
			res = SendDlgItemMessage(hDlg, IDC_PORTCOMBO, CB_GETCURSEL, 0, 0);

			GetDlgItemText(hDlg, IDC_PORTCOMBO, str, sizeof(str));
			int count = sscanf(str, "COM%i", &comPort);
			//comPort = res+1;
			if (count > 0)
			{
				sprintf(temp, "%i", comPort);
				WritePrivateProfileString("Com", "port", temp, mwalkerIniPath);

				SetDlgItemText(hDlg, IDC_STATICCURRENTPORT, (LPCSTR)str);
			}


			break;
		}
		case IDC_SerialCheck:
		{
			res = SendDlgItemMessage(hDlg, IDC_SerialCheck, BM_GETCHECK, 0, 0);

			if (BST_UNCHECKED == res)
			{
				SendDlgItemMessage(hDlg, IDC_LISTENCHECK, BM_SETCHECK, BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_WaitCheck, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			break;
		}
		case  IDC_LISTENCHECK:
		{
			res = SendDlgItemMessage(hDlg, IDC_LISTENCHECK, BM_GETCHECK, 0, 0);
			if (BST_CHECKED == res)
			{
				SendDlgItemMessage(hDlg, IDC_SerialCheck, BM_SETCHECK, BST_CHECKED, 0);
			}
			if (BST_UNCHECKED == res)
			{
				SendDlgItemMessage(hDlg, IDC_WaitCheck, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			break;
		}
		case IDC_SETLPTPORT:
		{
			char* lptStr = new char[TXTBUFFERLIMIT];
			GetDlgItemText(hDlg, IDC_EDITLPT, lptStr, TXTBUFFERLIMIT);
			int newLptPort = atoi(lptStr);

			if (iLptPortNum <= 0)
			{
				iLptPortNum = 1;
			}
			

			if (newLptPort > 0)
			{
				iLptPortNum = newLptPort;
				
			}

			sprintf(lptStr, "%i", iLptPortNum);

			SetDlgItemText(hDlg, IDC_EDITLPT, lptStr);

			char* newLPTstring = new char[256];
			sprintf(newLPTstring, "Send Maze Start/End over LPT:%i", iLptPortNum);
			SendDlgItemMessage(hDlg, IDC_LPT_CHECK, WM_SETTEXT, 0, (LPARAM)newLPTstring);
			break;

		}
		case IDC_SETLPT_DELAY:
		{
			char* lptStr = new char[TXTBUFFERLIMIT];
			GetDlgItemText(hDlg, IDC_EDITLPT_DELAY, lptStr, TXTBUFFERLIMIT);
			int newLPTdelay = atoi(lptStr);

			if (iLptDelay < 0)
			{
				iLptDelay = 10;
			}
			

			if (newLPTdelay > 0)
			{
				iLptDelay = newLPTdelay;

			}

			sprintf(lptStr, "%i", iLptDelay);

			SetDlgItemText(hDlg, IDC_EDITLPT_DELAY, lptStr);
			break;
		}

		case IDOK:
			{
				char temp[10];
				res = SendDlgItemMessage(hDlg,IDC_SerialCheck,BM_GETCHECK,0,0);
				if(res==BST_CHECKED) 
				{
					bSendSync=true;
					WritePrivateProfileString("Com","send","1",mwalkerIniPath);
				}
				else
				{
					bSendSync=false;
					WritePrivateProfileString("Com","send","0",mwalkerIniPath);
				}

				res = SendDlgItemMessage(hDlg,IDC_LISTENCHECK,BM_GETCHECK,0,0);

				if(res==BST_CHECKED) 
				{
					bListen=true;
					WritePrivateProfileString("Com","Listen","1",mwalkerIniPath);
				}
				else
				{
					bListen=false;
					WritePrivateProfileString("Com","Listen","0",mwalkerIniPath);
				}

				res = SendDlgItemMessage(hDlg, IDC_LPT_CHECK, BM_GETCHECK, 0, 0);

				if (res == BST_CHECKED)
				{
					bUseLPT = true;
					WritePrivateProfileString("LPT", "Enable", "1", mwalkerIniPath);

				}
				else
				{
					bUseLPT = false;
					WritePrivateProfileString("LPT", "Enable", "0", mwalkerIniPath);
				}

				
				char* lptStr = new char[TXTBUFFERLIMIT];
				sprintf(lptStr, "%i", iLptPortNum);
				WritePrivateProfileString("LPT", "Port", lptStr, mwalkerIniPath);

				sprintf(lptStr, "%i", iLptDelay);
				WritePrivateProfileString("LPT", "Delay", lptStr, mwalkerIniPath);

				res = SendDlgItemMessage(hDlg,IDC_BAUDRATECOMBO,CB_GETCURSEL,0,0);
				baudRate = res;
				sprintf(temp,"%d",baudRate);
				WritePrivateProfileString("Com","baudRate",temp,mwalkerIniPath);

				res = SendDlgItemMessage(hDlg,IDC_PARITYCOMBO,CB_GETCURSEL,0,0);
				parity = res;
				sprintf(temp,"%d",parity);
				WritePrivateProfileString("Com","parity",temp,mwalkerIniPath);

				res = SendDlgItemMessage(hDlg,IDC_STOPBITSCOMBO,CB_GETCURSEL,0,0);
				stopBits = res;
				sprintf(temp,"%d",stopBits);
				WritePrivateProfileString("Com","stopBits",temp,mwalkerIniPath);

				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
		case 2:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_Default:
		
		break;
		}
	}
	return FALSE;
}


LRESULT CALLBACK JoystickProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_INITDIALOG:
		{
			int dSetting;
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Disabled");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"X-Axis");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Y-Axis");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Z-Axis");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"X-Rotation");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Y-Rotation");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Left-Right D-Pad");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Up-Down D-Pad");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Network X Axis");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Network Y Axis");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Network Z Axis");
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_ADDSTRING,0,(LPARAM)"Network R Axis");
			dSetting=GetPrivateProfileInt("Joystick","jLeftRightAxis",1,mwalkerIniPath);
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_SETCURSEL,dSetting,0);

			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Disabled");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"X-Axis");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Y-Axis");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Z-Axis");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"X-Rotation");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Y-Rotation");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Left-Right D-Pad");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Up-Down D-Pad");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Network X Axis");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Network Y Axis");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Network Z Axis");
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_ADDSTRING,0,(LPARAM)"Network R Axis");
			dSetting=GetPrivateProfileInt("Joystick","jUpDownAxis",0,mwalkerIniPath);
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_SETCURSEL,dSetting,0);

			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Disabled");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"X-Axis");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Y-Axis");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Z-Axis");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"X-Rotation");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Y-Rotation");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Left-Right D-Pad");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Up-Down D-Pad");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Network X Axis");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Network Y Axis");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Network Z Axis");
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_ADDSTRING,0,(LPARAM)"Network R Axis");
			dSetting=GetPrivateProfileInt("Joystick","jMoveAxis",2,mwalkerIniPath);
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_SETCURSEL,dSetting,0);

			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Disabled");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"X-Axis");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Y-Axis");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Z-Axis");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"X-Rotation");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Y-Rotation");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Left-Right D-Pad");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Up-Down D-Pad");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Network X Axis");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Network Y Axis");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Network Z Axis");
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_ADDSTRING,0,(LPARAM)"Network R Axis");
			dSetting=GetPrivateProfileInt("Joystick","jStrafeAxis",0,mwalkerIniPath);
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_SETCURSEL,dSetting,0);

			char temp[10];
			dSetting=GetPrivateProfileInt("Joystick","moveSensitivity",10,mwalkerIniPath);
			sprintf(temp,"%d",dSetting);
			SendDlgItemMessage(hDlg,IDC_SENSEMOVE,WM_SETTEXT,0,(LPARAM)temp);
			dSetting=GetPrivateProfileInt("Joystick","lookSensitivity",10,mwalkerIniPath);
			sprintf(temp,"%d",dSetting);
			SendDlgItemMessage(hDlg,IDC_SENSELR,WM_SETTEXT,0,(LPARAM)temp);
			dSetting=GetPrivateProfileInt("Joystick","upDownSensitivity",10,mwalkerIniPath);
			sprintf(temp,"%d",dSetting);
			SendDlgItemMessage(hDlg,IDC_SENSEUD,WM_SETTEXT,0,(LPARAM)temp);
			dSetting=GetPrivateProfileInt("Joystick","strafeSensitivity",10,mwalkerIniPath);
			sprintf(temp,"%d",dSetting);
			SendDlgItemMessage(hDlg,IDC_SENSESTRAFE,WM_SETTEXT,0,(LPARAM)temp);


			
			JOYCAPS   joyCaps;
			joyGetDevCaps(JOYSTICKID1, &joyCaps, sizeof(joyCaps));
			
			SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_ADDSTRING,0,(LPARAM)"Network Joystick Only (API)");

			if(strlen(joyCaps.szPname)>0)
				SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_ADDSTRING,0,(LPARAM)joyCaps.szPname);
			else
				SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_ADDSTRING,0,(LPARAM)"No Valid Physical Joysticks");

			

			joyGetDevCaps(JOYSTICKID1+1, &joyCaps, sizeof(joyCaps));
			if(strlen(joyCaps.szPname)>0)
				SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_ADDSTRING,0,(LPARAM)joyCaps.szPname);

			joyGetDevCaps(JOYSTICKID1+2, &joyCaps, sizeof(joyCaps));
			if(strlen(joyCaps.szPname)>0)
				SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_ADDSTRING,0,(LPARAM)joyCaps.szPname);

			joyGetDevCaps(JOYSTICKID1+3, &joyCaps, sizeof(joyCaps));
			if(strlen(joyCaps.szPname)>0)
				SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_ADDSTRING,0,(LPARAM)joyCaps.szPname);

			

			SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_SETCURSEL,joystickNum+1,0);

			joyGetDevCaps(JOYSTICKID1, &joyCaps, sizeof(joyCaps));
			char * buttonName=new char[256];

			SendDlgItemMessage(hDlg,IDC_jJumpButton,CB_ADDSTRING,0,(LPARAM)"Disabled");
			int numButtons=joyCaps.wNumButtons;
			for (int i=1;i<=numButtons;i++)
			{
				sprintf(buttonName,"Button %i",i);
				SendDlgItemMessage(hDlg,IDC_jJumpButton,CB_ADDSTRING,0,(LPARAM)buttonName);
			}
			dSetting=GetPrivateProfileInt("Joystick","jumpButton",1,mwalkerIniPath);
			objCamera.j.jumpButton=dSetting;
			SendDlgItemMessage(hDlg,IDC_jJumpButton,CB_SETCURSEL,objCamera.j.jumpButton,0);
			
			SendDlgItemMessage(hDlg,IDC_jInteractButton,CB_ADDSTRING,0,(LPARAM)"Disabled");
			for (int i=1;i<=numButtons;i++)
			{
				sprintf(buttonName,"Button %i",i);
				SendDlgItemMessage(hDlg,IDC_jInteractButton,CB_ADDSTRING,0,(LPARAM)buttonName);
			}
			dSetting=GetPrivateProfileInt("Joystick","interactButton",2,mwalkerIniPath);
			objCamera.j.interactButton=dSetting;
			SendDlgItemMessage(hDlg,IDC_jInteractButton,CB_SETCURSEL,objCamera.j.interactButton,0);

			dSetting=GetPrivateProfileInt("Joystick","invertMoveAxis",0,mwalkerIniPath);
			if(dSetting) SendDlgItemMessage(hDlg,IDC_iMoveAxis,BM_SETCHECK,BST_CHECKED,0);
			dSetting=GetPrivateProfileInt("Joystick","invertLookAxis",0,mwalkerIniPath);
			if(dSetting) SendDlgItemMessage(hDlg,IDC_iLeftRightAxis,BM_SETCHECK,BST_CHECKED,0);
			dSetting=GetPrivateProfileInt("Joystick","invertUpDownAxis",0,mwalkerIniPath);
			if(dSetting) SendDlgItemMessage(hDlg,IDC_iUpDownAxis,BM_SETCHECK,BST_CHECKED,0);
			dSetting=GetPrivateProfileInt("Joystick","invertStrafeAxis",0,mwalkerIniPath);
			if(dSetting) SendDlgItemMessage(hDlg,IDC_iStrafeAxis,BM_SETCHECK,BST_CHECKED,0);
		}
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{

		case IDOK:
			{
				char temp[10];
				int res;
				res = SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_GETCURSEL,0,0);
				sprintf(temp,"%d",res);
				objCamera.j.jLeftRightAxis=res;
				WritePrivateProfileString("Joystick","jLeftRightAxis",temp,mwalkerIniPath);
				res = SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_GETCURSEL,0,0);
				sprintf(temp,"%d",res);
				objCamera.j.jUpDownAxis=res;
				WritePrivateProfileString("Joystick","jUpDownAxis",temp,mwalkerIniPath);
				res = SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_GETCURSEL,0,0);
				sprintf(temp,"%d",res);
				objCamera.j.jStrafeAxis=res;
				WritePrivateProfileString("Joystick","jStrafeAxis",temp,mwalkerIniPath);
				res = SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_GETCURSEL,0,0);
				sprintf(temp,"%d",res);
				objCamera.j.jMoveAxis=res;
				WritePrivateProfileString("Joystick","jMoveAxis",temp,mwalkerIniPath);

				res = SendDlgItemMessage(hDlg,IDC_iMoveAxis,BM_GETCHECK,0,0);
				sprintf(temp,"%d",res);
				WritePrivateProfileString("Joystick","invertMoveAxis",temp,mwalkerIniPath);
				objCamera.j.iMoveAxis=res;
				res = SendDlgItemMessage(hDlg,IDC_iLeftRightAxis,BM_GETCHECK,0,0);
				sprintf(temp,"%d",res);
				WritePrivateProfileString("Joystick","invertLookAxis",temp,mwalkerIniPath);
				objCamera.j.iLookAxis=res;
				res = SendDlgItemMessage(hDlg,IDC_iUpDownAxis,BM_GETCHECK,0,0);
				sprintf(temp,"%d",res);
				WritePrivateProfileString("Joystick","invertUpDownAxis",temp,mwalkerIniPath);
				objCamera.j.iUpDownAxis=res;
				res = SendDlgItemMessage(hDlg,IDC_iStrafeAxis,BM_GETCHECK,0,0);
				sprintf(temp,"%d",res);
				WritePrivateProfileString("Joystick","invertStrafeAxis",temp,mwalkerIniPath);
				objCamera.j.iStrafeAxis=res;

				SendDlgItemMessage(hDlg,IDC_SENSEMOVE,WM_GETTEXT,3,reinterpret_cast<long>(temp));
				WritePrivateProfileString("Joystick","moveSensitivity",temp,mwalkerIniPath);
				objCamera.j.moveSensitivity=atoi(temp);
				SendDlgItemMessage(hDlg,IDC_SENSELR,WM_GETTEXT,3,reinterpret_cast<long>(temp));
				WritePrivateProfileString("Joystick","lookSensitivity",temp,mwalkerIniPath);
				objCamera.j.lookSensitivity=atoi(temp);
				SendDlgItemMessage(hDlg,IDC_SENSEUD,WM_GETTEXT,3,reinterpret_cast<long>(temp));
				WritePrivateProfileString("Joystick","upDownSensitivity",temp,mwalkerIniPath);
				objCamera.j.upDownSensitivity=atoi(temp);
				SendDlgItemMessage(hDlg,IDC_SENSESTRAFE,WM_GETTEXT,3,reinterpret_cast<long>(temp));
				WritePrivateProfileString("Joystick","strafeSensitivity",temp,mwalkerIniPath);
				objCamera.j.strafeSensitivity=atoi(temp);

				joystickNum = SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_GETCURSEL,0,0)-1;
				sprintf(temp,"%d",joystickNum);
				WritePrivateProfileString("Joystick","Number",temp,mwalkerIniPath);

				objCamera.j.jumpButton = SendDlgItemMessage(hDlg,IDC_jJumpButton,CB_GETCURSEL,0,0);
				sprintf(temp,"%d",objCamera.j.jumpButton);
				WritePrivateProfileString("Joystick","jumpButton",temp,mwalkerIniPath);

				objCamera.j.interactButton = SendDlgItemMessage(hDlg,IDC_jInteractButton,CB_GETCURSEL,0,0);
				sprintf(temp,"%d",objCamera.j.interactButton);
				WritePrivateProfileString("Joystick","interactButton",temp,mwalkerIniPath);

				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
		case 2:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case ID_CANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_JOYSTICK_DEFAULT1:
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_SETCURSEL,2,0);
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_SETCURSEL,1,0);
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_SETCURSEL,0,0);
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_SETCURSEL,0,0);
			SendDlgItemMessage(hDlg,IDC_iMoveAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_iStrafeAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_iLeftRightAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_iUpDownAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_SENSELR,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_SENSEMOVE,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_SENSEUD,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_SENSESTRAFE,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_SETCURSEL,1,0);
			SendDlgItemMessage(hDlg,IDC_jJumpButton,CB_SETCURSEL,1,0);
			SendDlgItemMessage(hDlg,IDC_jInteractButton,CB_SETCURSEL,2,0);
			break;
		case IDC_JOYSTICK_DEFAULT2:
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_SETCURSEL,2,0);
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_SETCURSEL,4,0);
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_SETCURSEL,1,0);
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_SETCURSEL,5,0);
			SendDlgItemMessage(hDlg,IDC_iMoveAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_iStrafeAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_iLeftRightAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_iUpDownAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_SENSELR,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_SENSEMOVE,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_SENSEUD,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_SENSESTRAFE,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_SETCURSEL,1,0);
			SendDlgItemMessage(hDlg,IDC_jJumpButton,CB_SETCURSEL,1,0);
			SendDlgItemMessage(hDlg,IDC_jInteractButton,CB_SETCURSEL,2,0);
			break;
		case IDC_JOYSTICK_API:
			SendDlgItemMessage(hDlg,IDC_jMoveAxis,CB_SETCURSEL,9,0);
			SendDlgItemMessage(hDlg,IDC_jLeftRightAxis,CB_SETCURSEL,10,0);
			SendDlgItemMessage(hDlg,IDC_jStrafeAxis,CB_SETCURSEL,8,0);
			SendDlgItemMessage(hDlg,IDC_jUpDownAxis,CB_SETCURSEL,11,0);
			SendDlgItemMessage(hDlg,IDC_iMoveAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_iStrafeAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_iLeftRightAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_iUpDownAxis,BM_SETCHECK,BST_UNCHECKED,0);
			SendDlgItemMessage(hDlg,IDC_SENSELR,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_SENSEMOVE,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_SENSEUD,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_SENSESTRAFE,WM_SETTEXT,0,(LPARAM)"10");
			SendDlgItemMessage(hDlg,IDC_JOYNUMCOMBO,CB_SETCURSEL,0,0);
			SendDlgItemMessage(hDlg,IDC_jJumpButton,CB_SETCURSEL,0,0);
			SendDlgItemMessage(hDlg,IDC_jInteractButton,CB_SETCURSEL,0,0);
			break;
		}
		break;
	}
	return FALSE;
}

void Tcp_Input()
{

	Player.lookingDown.Move();
	Player.lookingLeft.Move();
	Player.lookingRight.Move();
	Player.lookingUp.Move();
	Player.movingBackwards.Move();
	Player.movingForward.Move();
	Player.strafingLeft.Move();
	Player.strafingRight.Move();
	Player.running.Move();
	Player.crouching.Move();
}
int retCheck(float distance,float angle)
{

	tVector3 initPos=objCamera.mPos;
	tVector3 initView=objCamera.mView;
	int ret=0;
	int xmax=distance/0.35*10+1;
	objCamera.Rotate_View(angle);
	for(int x=0;x<(int)xmax;x++)
	{

		ret=objCamera.Move_Test(distance*(float) x/(float)xmax,0); //check movement at xmax locations before moving.
		if(ret>0)
		{
			break;
		}
	}
	objCamera.Rotate_View(-angle);
	objCamera.mPos=initPos;
	objCamera.mView=initView;
	return ret;
}

bool boundedMovement=false;//block motion that is illegal
void Tcp_Instant()
{

	float turnstep=2.5f;
	float movestep= objCamera.cameraSpeedMove*10;


	if(Player.justJumped.moveCount>0)
	{
		if(Player.jumpReleased)
		{
			Player.justJumped.state=true;
			Player.crouching.state=false;
			Player.jumpReleased=false;
			Player.justJumped.moveCount--;
		}

	}
	if(Player.lookingLeft.instantMove>0)
	{
		if(Player.lookingLeft.instantMove<turnstep)
		{
			objCamera.Rotate_View(-Player.lookingLeft.instantMove*PI/180);
			Player.lookingLeft.instantMove=0;
		}
		else
		{
			objCamera.Rotate_View(-turnstep*PI/180);
			Player.lookingLeft.instantMove-=turnstep;
		}
	}
	if(Player.lookingRight.instantMove>0)
	{
		if(Player.lookingRight.instantMove<turnstep)
		{
			objCamera.Rotate_View(Player.lookingRight.instantMove*PI/180);
			Player.lookingRight.instantMove=0;
		}
		else
		{
			objCamera.Rotate_View(turnstep*PI/180);
			Player.lookingRight.instantMove-=turnstep;
		}
	}
	if(Player.movingForward.instantMove>0)
	{
		if(boundedMovement&&retCheck(Player.movingForward.instantMove,0)>0) //check max movment if bounded movement
			Player.movingForward.instantMove=0;
		else
		{
			float move;
			if(Player.movingForward.instantMove<movestep)
			{
				move=Player.movingForward.instantMove;
				Player.movingForward.instantMove=0;
			}
			else
			{
				move=movestep;
				Player.movingForward.instantMove-=movestep;
			}
			int ret=objCamera.Move_Test(move,0); //check movement at xmax locations before moving.
			if(!boundedMovement||ret<1)
			{
				Player.collisionEnabled=false;
				objCamera.Move_Camera(move);
				AlignCamera();
				Player.collisionEnabled=true;
			}
		}
	}
	if(Player.movingBackwards.instantMove>0)
	{

		if(boundedMovement&&retCheck(Player.movingBackwards.instantMove,PI)>0) //check max movment if bounded movement
			Player.movingBackwards.instantMove=0;
		else
		{

			float move;
			if(Player.movingBackwards.instantMove<movestep)
			{
				move=Player.movingBackwards.instantMove;
				Player.movingBackwards.instantMove=0;
			}
			else
			{
				move=movestep;
				Player.movingBackwards.instantMove-=movestep;
			}
			objCamera.Rotate_View(PI);
			int ret=objCamera.Move_Test(move,0); //check movement at xmax locations before moving.
			if(!boundedMovement||ret<1)
			{
				Player.collisionEnabled=false;
				objCamera.Move_Camera(move);
				AlignCamera();
				Player.collisionEnabled=true;
			}
			objCamera.Rotate_View(PI);
		}
	}
	if(Player.strafingLeft.instantMove>0)
	{
		if(boundedMovement&&retCheck(Player.strafingLeft.instantMove,-PI/2.0f)>1)
			Player.strafingLeft.instantMove=0;
		else
		{

			float move;
			if(Player.strafingLeft.instantMove<movestep)
			{
				move=Player.strafingLeft.instantMove;
				Player.strafingLeft.instantMove=0;
			}
			else
			{
				move=movestep;
				Player.strafingLeft.instantMove-=movestep;
			}
			objCamera.Rotate_View(-PI/2.0f);
			int ret=objCamera.Move_Test(move,0); //check movement at xmax locations before moving.
			if(!boundedMovement||ret<1)
			{
				Player.collisionEnabled=false;
				objCamera.Move_Camera(move);
				AlignCamera();
				Player.collisionEnabled=true;
			}
			objCamera.Rotate_View(PI/2.0f);
		}
	}
	if(Player.strafingRight.instantMove>0)
	{
		if(boundedMovement&&retCheck(Player.strafingRight.instantMove,PI/2.0f)>1)
			Player.strafingRight.instantMove=0;
		else
		{

			float move;
			if(Player.strafingRight.instantMove<movestep)
			{
				move=Player.strafingRight.instantMove;
				Player.strafingRight.instantMove=0;
			}
			else
			{
				move=movestep;
				Player.strafingRight.instantMove-=movestep;
			}
			objCamera.Rotate_View(PI/2.0f);
			int ret=objCamera.Move_Test(move,0); //check movement at xmax locations before moving.
			if(!boundedMovement||ret<1)
			{
				Player.collisionEnabled=false;
				objCamera.Move_Camera(move);
				AlignCamera();
				Player.collisionEnabled=true;
			}
			objCamera.Rotate_View(-PI/2.0f);
		}
	}

}
int Keyboard_Input()
{
	if(!videoPlayBack&&!pausePlayback)
	{
		int ret=0;
		int timeSince;
		timeSince=GetQPC()-updateTimeKeys;

		Player.halfSpeed=1;



		if(!Player.gravityEnabled)
			Player.jumpSpeed=0;

		if(GetKeyState(ControlLibrary[Forward])&0x80 ||GetKeyState(ControlLibrary[Forward+Double])&0x80||joyUp)
		{
			Player.movingForward.state=true;
		}
		else
			Player.movingForward.state=false;
		if(GetKeyState(ControlLibrary[Backward])&0x80 ||GetKeyState(ControlLibrary[Backward+Double])&0x80||joyDown)
		{
			Player.movingBackwards.state=true;
		}
		else
			Player.movingBackwards.state=false;


		if(GetKeyState(ControlLibrary[StrafeLeft])&0x80 ||GetKeyState(ControlLibrary[StrafeLeft+Double])&0x80)
		{
			Player.strafingLeft.state=true;
		}
		else
			Player.strafingLeft.state=false;

		if(GetKeyState(ControlLibrary[StrafeRight])&0x80 ||GetKeyState(ControlLibrary[StrafeRight+Double])&0x80)
		{
			Player.strafingRight.state=true;
		}
		else
			Player.strafingRight.state=false;


		if(GetKeyState(ControlLibrary[LookLeft])&0x80 ||GetKeyState(ControlLibrary[LookLeft+Double])&0x80||joyLeft)
		{
			Player.lookingLeft.state=true;
		}
		else
			Player.lookingLeft.state=false;

		if(GetKeyState(ControlLibrary[LookRight])&0x80 ||GetKeyState(ControlLibrary[LookRight+Double])&0x80||joyRight)
		{
			Player.lookingRight.state=true;
		}
		else
			Player.lookingRight.state=false;
		if(GetKeyState(ControlLibrary[LookUp])&0x80)
		{
			Player.lookingUp.state=true;
		}
		else
			Player.lookingUp.state=false;

		if(GetKeyState(ControlLibrary[LookDown])&0x80)
		{
			Player.lookingDown.state=true;
		}
		else
			Player.lookingDown.state=false;

		if(GetKeyState(ControlLibrary[Run])&0x80 ||GetKeyState(ControlLibrary[Run])&0x80)
		{
			Player.running.state=true;
			OculusOffset -= 0.001;
		}


		if(GetKeyState(ControlLibrary[Crouch])&0x80)
		{
			Player.crouching.state=true;
			OculusOffset += 0.001;
		}
		else
			Player.crouching.state=false;

		if(GetKeyState(ControlLibrary[Interact])&0x80)
		{
			Player.interact=true;
			
		}

		if (GetKeyState(ControlLibrary[RestartMaze]) & 0x80)
		{
			if (GetQPC() - toggleKeyTime > 1000)
			{
				toggleKeyTime = GetQPC();
				bRestartMaze = true;
				bMazeLoop = false;
			}

		}

		if (GetKeyState(ControlLibrary[NextMaze]) & 0x80)
		{
			if (GetQPC() - toggleKeyTime > 1000)
			{
				toggleKeyTime = GetQPC();
				bMazeLoop = false;
			}

		}


	
		
		if (GetKeyState('U') & 0x80 && developerKeys)
		{
			if (GetQPC() - toggleKeyTime > 200)
			{
				devViewMode++;
				toggleKeyTime = GetQPC();
				if (devViewMode > 2)
					devViewMode = 0;
				if (devViewMode == 0)
				{
					bTopDown = false;
					b34View = false;
					setDebugMessage("First-Person Persepective");
				}
				else if (devViewMode == 1)
				{
					bTopDown = true;
					b34View = false;
					setDebugMessage("Top-Down Persepective");
				}
				else if (devViewMode == 2)
				{
					bTopDown = false;
					b34View = true;
					setDebugMessage("3/4 Persepective");
				}
			}
		}
			
		if (GetKeyState('P') & 0x80 && developerKeys)
		{

			if(GetQPC()-toggleKeyTime>1000)
			{
				if(!bDisableGravity)
					Player.gravityEnabled=!Player.gravityEnabled;
				objCamera.Camera3D=!objCamera.Camera3D;
				Player.collisionEnabled=!Player.collisionEnabled;
				Player.jumping=false;
				Player.jumpSpeed=0;
				toggleKeyTime=GetQPC();
				
				if(!objCamera.Camera3D)//reset controller position
				{
							btTransform mTransform;
							mTransform.setIdentity();
							mTransform.setOrigin (btVector3(objCamera.mPos.x,objCamera.mPos.y,objCamera.mPos.z));
							ghostObject->setWorldTransform(mTransform);
							setDebugMessage("Ghost Mode Disabled");
				}
				else
					setDebugMessage("Ghost Mode Enabled");
			}
		}

		if(GetKeyState('O')&0x80&&developerKeys)
		{
			controller->setFallSpeed(-0.5);
			setDebugMessage("Debug Flight Mode");
		}
		else if(!controller->canJump())
		{
			controller->setFallSpeed(abs(controller->getFallSpeed()*1.1));
		}

		

		Tcp_Input(); //moves accoding to movement counters

		if(Player.movingBackwards.state&&Player.movingForward.state)
		{
			Player.movingBackwards.state=false;
			Player.movingForward.state=false;
		}
		if(Player.strafingLeft.state&&Player.strafingRight.state)
		{
			Player.strafingLeft.state=false;
			Player.strafingRight.state=false;
		}

		if(GetKeyState(ControlLibrary[Jump])&0x80)
		{
			if(Player.jumpReleased==true)
			{
				Player.jumpReleased=false;
				Player.justJumped.state=true;
 				Player.crouching.state=false;
			}
		}
		else
		{
			Player.justJumped.state=false;
			Player.jumpReleased=true;
		}
		if(Player.running.state)
		{
			Player.halfSpeed/=1.3;
			Player.running.state=false;
		}
		if(Player.crouching.state)
		{
			
				Player.halfSpeed*=2;

				if (Player.gravityEnabled)
				{
					if (Player.offset >= Player.defaultCameraOffset / 3.0f)
						Player.offset -= 0.01;
					else
						Player.offset = Player.defaultCameraOffset / 3.0f;
				}
		}
		else
		{
			if (Player.offset < Player.defaultCameraOffset)
				Player.offset+=0.01;
			else
				Player.offset = Player.defaultCameraOffset;
		}


		

		if(Player.justJumped.state)
		{
			if(Player.gravityEnabled)
			{
				if(Player.jumping)
					Player.justJumped.state=false;

			}
			else

			{
				Player.jumping=true;
				Player.goingUp=false;
			}
		}
		else if(!Player.gravityEnabled)
		{
			Player.goingUp=false;
		}

		
		if ((GetKeyState('8') & 0x80) && developerKeys)
		{
			targetBarScore = 50;
			setDebugMessage("Debug: Inactivate");
		}

		if((GetKeyState('9') & 0x80)&&developerKeys)
		{	
			targetBarScore =20;
			setDebugMessage("Debug: Inactivate");
		}
		if((GetKeyState('0') & 0x80)&&developerKeys)
		{	
			targetBarScore=100;
			setDebugMessage("Debug: Activate");
		}


		if((GetKeyState(ControlLibrary[SpeedUp]) & 0x80))
		{
			objCamera.MoveSpeedIncrement();
			char messageTxt[200]="";
			sprintf(messageTxt, "Debug: SpeedUp %0.1fMz/s", objCamera.cameraSpeedMove*60);
			setDebugMessage(messageTxt);
		}
		if((GetKeyState(ControlLibrary[SpeedDown]) & 0x80))
		{
			objCamera.MoveSpeedDecrement();
			char messageTxt[200] = "";
			sprintf(messageTxt, "Debug: SpeedDown %0.1fMz/s", objCamera.cameraSpeedMove * 60);
			setDebugMessage(messageTxt);
		}



		//if( GetKeyState('L') & 0x80)
		//{
		//	lights = !lights;
		//	if(lights)
		//		glEnable(GL_LIGHTING);
		//	else
		//		glDisable(GL_LIGHTING);
		//}



		updateTimeKeys=GetQPC();
		return ret;
	}
	else
	{
		
		if(GetKeyState(ControlLibrary[Jump]) & 0x80&&!interactivePlayback)
		{
			if((GetQPC()-updateTimeKeys)>300)
			{
				updateTimeKeys=GetQPC();
				pausePlayback=!pausePlayback;
				if(!pausePlayback)
				{
					mazeStart=GetQPC()-pauseTime;
					pauseTime=0;
				}
				else
				{
					pauseTime=GetQPC()-startTime;
				}
			}
		}
		/*if((GetKeyState('9') & 0x80)&&developerKeys)
		{	
			targetScore=0;
		}
		if((GetKeyState('0') & 0x80)&&developerKeys)
		{	
			targetScore=100;
		}
*/
		if((interactivePlayback&&pausePlayback&&askForResponse&&Player.testPass && iUserStatus==USER_STATUS_REPORT)||devMsgStatus==2)
		{
			if(GetKeyState('1')&0x80)
				userResponse=1;
			if(GetKeyState('2')&0x80)
				userResponse=2;
			if(GetKeyState('3')&0x80)
				userResponse=3;
			if(GetKeyState('4')&0x80)
				userResponse=4;
			if(GetKeyState('5')&0x80)
				userResponse=5;
			if(GetKeyState('6')&0x80)
				userResponse=6;
			if(GetKeyState('7')&0x80)
				userResponse=7;
			if(GetKeyState('8')&0x80)
				userResponse=8;
			if(GetKeyState('9')&0x80)
				userResponse=9;
			if(GetKeyState('0')&0x80)
				userResponse=10;
			if(GetKeyState(VK_RETURN)&0x80||GetKeyState(ControlLibrary[Jump])&0x80)
			{
				numHighlighted=0;
				Player.testPass=false;
				pausePlayback=false;
				int userRInt=userResponse;
				EventLog(5,0,userRInt);
				iUserStatus = USER_STATUS_NONE;
			}
			if((GetQPC()-updateTimeKeys)>25)
			{
				updateTimeKeys=GetQPC();
				if(GetKeyState(ControlLibrary[LookLeft])&0x80||GetKeyState(ControlLibrary[StrafeLeft+Double])&0x80||GetKeyState(ControlLibrary[StrafeLeft])&0x80||GetKeyState(ControlLibrary[LookLeft+Double])&0x80||GetKeyState(ControlLibrary[Backward])&0x80||GetKeyState(ControlLibrary[Backward+Double])&0x80)
					if(userResponse>1)
						userResponse=userResponse-0.25;
					else
						userResponse=1;
				if(GetKeyState(ControlLibrary[LookRight])&0x80||GetKeyState(ControlLibrary[StrafeRight+Double])&0x80||GetKeyState(ControlLibrary[StrafeRight])&0x80||GetKeyState(ControlLibrary[LookRight+Double])&0x80||GetKeyState(ControlLibrary[Forward])&0x80||GetKeyState(ControlLibrary[Forward+Double])&0x80)
					if(userResponse<1)
						userResponse=1.1;
					else if(userResponse<10)
						userResponse=userResponse+0.25;
					else
						userResponse=10;
			}
		}
		
		return 0;
	}
}



BOOL CALLBACK MonitorEnumProc(
  _In_  HMONITOR hMonitor,
  _In_  HDC hdcMonitor,
  _In_  LPRECT lprcMonitor,
  _In_  LPARAM dwData
){
	
	if(lprcMonitor)
	{
		hMonitor=MonitorFromRect(lprcMonitor,MONITOR_DEFAULTTONEAREST);
		MONITORINFO monInfo;
		monInfo.cbSize=sizeof(monInfo);
		GetMonitorInfo(hMonitor,&monInfo);
		desktopRC=monInfo.rcMonitor;
		return true;
	}
	else
		return false;
}

BOOL	done=FALSE;								// Bool Variable To Exit Loop



int WINAPI WinMain(	HINSTANCE	hInstance1,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)		// Window Show State
{




	MSG		msg;									// Windows Message Structure
	
	struct tm *newtime;
    time_t aclock;
	filename[0]=NULL;
	done=false;
	InitializeQPC();
	GetQPC();
	
	

	//lpCmdLine="\"C:\\Arena2.maz\" -f +r 600 800 +LG \"C:\\Arena2_log.txt\"";  //This is my command line test thing so i don't have to retype it everytime
	sprintf(cmdOptions,"%s",lpCmdLine);
	
	hInstance = hInstance1;

	HWND parent=GetDesktopWindow();
	GetWindowRect(parent, &desktopRC);

	int mazecount=0;
	bool bTimeLimitExceed=false;
	DWORD buf;	
	float x,z,y,vx,vz,vy=0;



	SHGetFolderPath(hWnd,CSIDL_APPDATA,NULL,0,mwalkerIniPath); //retreives users appdata path
	SHGetFolderPath(hWnd,CSIDL_PERSONAL,NULL,0,userLibraryDir);
	strcat(userLibraryDir,"\\MazeSuite");
	strcat(userLibraryDir,"\\Library");
	strcat(mwalkerIniPath,"\\MazeWalker");
	CreateDirectory(mwalkerIniPath,NULL);
	strcat(mwalkerIniPath,"\\MazeWalker.ini");
	if(ControlLibrary[0]==NULL&&ControlLibrary[1]==NULL)
	{ 
		loadIniSettings();
		sprintf(exePath,"%s",GetCommandLine());
		if(exePath[0]==34)
		{
			for(int i=1;exePath[i]!=NULL;i++)			
			{
				if(exePath[i]==34)
					{
						exePath[i]=NULL;
						break;
					}
			}
		}

		//exePath[strlen(exePath)-15]= NULL;

		for(int i = strlen(exePath);i>0 ; i--)
		{
			if(exePath[i]=='\\')
			{
				exePath[i]=NULL;
				break;
			}
		}
		if(exePath[0]==34)
			strcpy_s(libraryDir,&exePath[1]);
		else
			strcpy_s(libraryDir,exePath);
		strcat_s(libraryDir,"\\library");
		GetPrivateProfileString("LibraryPath","LibraryPath", userLibraryDir, userLibraryDir,MAX_PATH,mwalkerIniPath);
		char* libraryPathPart;
		char oldUserLibPath[800];
		strcpy_s(oldUserLibPath, userLibraryDir);
		libraryPathPart=strtok(oldUserLibPath,"\\");
		strcpy_s(userLibraryDir,libraryPathPart);
		while(libraryPathPart!=NULL)
		{
			libraryPathPart=strtok(NULL,"\\");
			if(libraryPathPart==NULL)
				break;
			strcat_s(userLibraryDir,"\\");
			strcat_s(userLibraryDir,libraryPathPart);
			CreateDirectory(userLibraryDir,NULL);
		}
		WritePrivateProfileString("LibraryPath","LibraryPath", userLibraryDir,mwalkerIniPath);
	}

	updateUserLibraryDir(userLibraryDir);
	updateLibraryDir(libraryDir);

	for(int count=0;count<3;count++)
	{
		if(devicePort[count]==0)
			deviceEnabled[count]=false;
	}

	int res=GetPrivateProfileInt("Display","Resolution",1,mwalkerIniPath);

	if(res==0){ Width=640; Height=480;}
	else if(res==1) { Width=800; Height=600;}
	else if(res==2) { Width=1024; Height=768;}
	else if(res==3) { Width=1280; Height=720;}
	else if(res==4) { Width=1280; Height=800;}
	else if(res==5) { Width=1280; Height=1024;}
	else if(res==6) { Width=1600; Height=1200;}
	else if(res==8) { Width=1920; Height=1080;}
	else if(res==9) { Width=1920; Height=1200;}
	else if(res==10) { Width=3840; Height=1024;}
	else if (res == 11) { Width = 3840; Height = 2160; }

	res=GetPrivateProfileInt("Display","Bits",3,mwalkerIniPath);	

	if(res==0) bitz=8;
	else if(res==1) bitz=16;
	else if(res==2) bitz=24;
	else bitz=32;

	lights=GetPrivateProfileInt("Display","Lights",1,mwalkerIniPath);
	fullscreen=GetPrivateProfileInt("Display","Fullscreen",0,mwalkerIniPath);
	bShowCrosshair=GetPrivateProfileInt("Display","ShowCrosshair",0,mwalkerIniPath);
	shader=GetPrivateProfileInt("Display","UseShaders",1,mwalkerIniPath);
	bShowBar=GetPrivateProfileInt("Display","ShowBar",0,mwalkerIniPath);
	bShowTimer=GetPrivateProfileInt("options","timer",1,mwalkerIniPath);
	autoLog=GetPrivateProfileInt("options","autoLog",0,mwalkerIniPath);
	

	bool justInitializeSettings=false;



	char* cmdOption;
	char cmdOptionChar[800];
	//DWORD actualTime; ////?????

	//cmdOption=strtok(cmdOptions," ");
	cmdOption=strtok(cmdOptions,"\"");
	//cmdOption=strtok(NULL,"\"");

	BOOL loadedMazeFile=FALSE;

	if(cmdOption!=NULL)
		interactivePlayback=false;
	while(cmdOption!=NULL)
	{
		sprintf(cmdOptionChar,"%s",cmdOption);
		if(cmdOptionChar[0]=='\"')
		{
			bool done2=false;
			for(int x=0;x<strlen(cmdOption);x++)
			{
				if(cmdOptionChar[x+1]!='\"')
					filename[x]=cmdOptionChar[x+1];
				if(cmdOptionChar[x+1]=='\"')
					done2=true;
			}
			while(cmdOption!=NULL&&!done2)
			{
				cmdOption=strtok(NULL," ");
				if(cmdOption==NULL)
					break;
				sprintf(cmdOptionChar,"%s",cmdOption);
				int currentLength;

				currentLength=strlen(filename)+1;
				filename[currentLength-1]=' ';
				for(int x=currentLength;x<strlen(cmdOption)+currentLength;x++)
				{
					if(cmdOptionChar[x-currentLength]!='\"')
						filename[x]=cmdOptionChar[x-currentLength];
					if(cmdOptionChar[x-currentLength]=='\"')
						done2=true;
				}
			}
			loadedMazeFile = TRUE;	
		}
		else if(cmdOptionChar[1]==':')
		{
			if(loadedMazeFile == FALSE)  //this is maze file
			{
				strcpy_s(filename,TXTBUFFERLIMIT,cmdOptionChar);
			}
			else //this is log file
			{

			}
		}
		else if(cmdOptionChar[1]=='w'||cmdOptionChar[1]=='W')
		{
			if(cmdOptionChar[0]=='-')
				bSkipWarnings=false;
			if(cmdOptionChar[0]=='+')
				bSkipWarnings=true;
		}
		else if(cmdOptionChar[1]=='f'||cmdOptionChar[1]=='F')
		{
			if(cmdOptionChar[0]=='-')
				fullscreen=false;
			if(cmdOptionChar[0]=='+')
				fullscreen=true;
		}
		else if (cmdOptionChar[1] == 't' || cmdOptionChar[1] == 'T')
		{
			if (cmdOptionChar[0] == '-')
				bTopDown = false;
			if (cmdOptionChar[0] == '+')
				bTopDown = true;
		}
		else if((cmdOptionChar[1]=='i'||cmdOptionChar[1]=='I')&&(cmdOptionChar[2]=='P'||cmdOptionChar[2]=='p'))
		{
			if(cmdOptionChar[0]=='-')
				interactivePlayback=false;
			if(cmdOptionChar[0]=='+')
				interactivePlayback=true;
		}
		else if ((cmdOptionChar[1] == '3' ) && (cmdOptionChar[2] == '4'))
		{
			if (cmdOptionChar[0] == '-')
				b34View = false;
			if (cmdOptionChar[0] == '+')
			{
				b34View = true;
				bTopDown = false;
			}
		}
		else if ((cmdOptionChar[1] == 'f' || cmdOptionChar[1] == 'F') && (cmdOptionChar[2] == 'P' || cmdOptionChar[2] == 'p'))
		{
			if (cmdOptionChar[0] == '-')
			{
				b34View = false;
				bTopDown = false;
			}
			if (cmdOptionChar[0] == '+')
			{
				b34View = false;
				bTopDown = false;
			}
		}
		//else if(cmdOptionChar[0]==":")
		//{
		//	clogIndex=atoi(&cmdOptionChar[x+3]);
		//}
		else if((cmdOptionChar[1]=='l'||cmdOptionChar[1]=='L')&&(cmdOptionChar[2]=='g'||cmdOptionChar[2]=='G'))
		{
			cmdOption=strtok(NULL,"\"");
			//cmdOption=strtok(NULL,"\"");
			char logAddress[800];

			sprintf(cmdOptionChar,"%s",cmdOption);
			if(cmdOptionChar[0]=='\"')
			{
				bool done2=false;
				for(int x=0;x<strlen(cmdOption);x++)
				{
					if(cmdOptionChar[x+1]!='\"')
						logAddress[x]=cmdOptionChar[x+1];
					if(cmdOptionChar[x+1]=='\"')
					{
						logAddress[x]=NULL;
						done2=true;
					}
				}				
			}
			else
			{
				strcpy(logAddress,cmdOption);
				if(logAddress[strlen(logAddress)-1] == '\\')
				{
					logAddress[strlen(logAddress)-1] = 0;
				}

			}


			if((strlen(logAddress)>2)&&!InitLog(logAddress))
			{
				MessageBox(0,"Can not create log file!. Make sure the folder is accessible and you read/write permissions.","Error",MB_ICONERROR);
			}
			else
			{
				sprintf(logfile,"%s",logAddress);
			}
		}
		else if(cmdOptionChar[1]=='v'||cmdOptionChar[1]=='V')
		{
			cmdOption=strtok(NULL,"\"");
			//cmdOption=strtok(NULL,"\"");
			char clogAddress[800];
			int clogIndex=0;
			sprintf(cmdOptionChar,"%s",cmdOption);
			if(cmdOptionChar[0]=='\"')
			{
				bool done2=false;
				for(int x=0;x<strlen(cmdOption);x++)
				{
					if(cmdOptionChar[x+1]!='\"')
						clogAddress[x]=cmdOptionChar[x+1];
					if(cmdOptionChar[x+1]=='\"')
					{
						clogAddress[x]=NULL;
						if(cmdOptionChar[x+2]==':')
						{
							clogIndex=atoi(&cmdOptionChar[x+3]);
						}
						done2=true;
					}
				}				
			}
			else
			{
				strcpy(clogAddress,cmdOption);
				if(clogAddress[strlen(clogAddress)-1] == '\\')
				{
					clogAddress[strlen(clogAddress)-1] = 0;
				}

			}

			cmdOption=strtok(NULL,"\""); //read if there's any indexing...
			if(cmdOption[0]==':')
			{
				clogIndex=atoi(&cmdOption[1]);
			}


			if(CheckFileExists(clogAddress))
			{
				videoPlayBack=true;
				
				bShowTimer=true;
				clog=fopen(clogAddress,"r");
				int indexCount=-1;
				char clogBuffer[TXTBUFFERLIMIT];

				while(indexCount!=clogIndex&&fgets(clogBuffer,TXTBUFFERLIMIT,clog)!=NULL)
				{
					if(clogBuffer[0]=='M'&&clogBuffer[1]=='a')
						indexCount++;
				}


				//check maze file with current!!!!!!!!!!!!!!!!!!!
				//get size...
				int res=0;
				while(clogBuffer[res]!=NULL)
					res++;
				clogBuffer[res-1]=NULL;
				while(clogBuffer[res]!='\\')
					res--;
				int i=0;
				while(filename[i]!=NULL)
					i++;
				while(filename[i]!='\\')
					i--;
				if(strcmp( &(clogBuffer[res+1]),&(filename[i+1]) ) != 0)
					MessageBox(0,"Log file belongs to another maze. Either change the current maze or input log file!","Incompatible maze and log file",MB_OK);

				if(NULL == fgets(clogBuffer,TXTBUFFERLIMIT,clog))
					break;		//time
				if(NULL == fgets(clogBuffer,TXTBUFFERLIMIT,clog))
					break;		//
				if(NULL == fgets(clogBuffer,TXTBUFFERLIMIT,clog))
					break;		//
				if(NULL == fgets(clogBuffer,TXTBUFFERLIMIT,clog))
					break;		//headers
				if(NULL == fgets(clogBuffer,TXTBUFFERLIMIT,clog))
					break;		//firstline
				float inp2=0,inp1=0;
				res=sscanf_s(clogBuffer,"%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f",&clogItemTime,&inp2,&inp1,&inp1,&inp1,&inp1,&inp1,&inp1);
				if(res==8)
				{
					clogItemTime=(int)inp2;
					newclogType=true;
				}

				clogStartTime = clogItemTime;
				startTime = GetQPC();
				mazeStart = GetQPC();
			}
		}
		else if(cmdOptionChar[1]=='l'||cmdOptionChar[1]=='L')
		{
			if(cmdOptionChar[0]=='-')
				lights=false;
			if(cmdOptionChar[0]=='+')
				lights=true;
		}
		else if(cmdOptionChar[1]=='s'||cmdOptionChar[1]=='S')
		{
			if(cmdOptionChar[0]=='-')
				shader=false;
			if(cmdOptionChar[0]=='+')
			{
				shader=true;
				cmdOption=strtok(NULL," ");
				sprintf(cmdOptionChar,"%s",cmdOption);
				shaderOptionCode=atoi(cmdOption);
			}
		}
		else if((cmdOptionChar[1]=='i'||cmdOptionChar[1]=='i')&&strlen(cmdOptionChar)>2)
		{
			if(cmdOptionChar[2]=='V'||cmdOptionChar[2]=='v')
			{
				cmdOption=strtok(NULL," ");
				sprintf(cmdOptionChar,"%s",cmdOption);
				mazeInputValue=atoi(cmdOption);

			}
		}
		else if(cmdOptionChar[1]=='b'||cmdOptionChar[1]=='B')
		{
			if(cmdOptionChar[0]=='+'||cmdOptionChar[0]=='-')
			{
				cmdOption=strtok(NULL," ");
				sprintf(cmdOptionChar,"%s",cmdOption);
				bitz=atoi(cmdOption);
				if(bitz!=32&&bitz!=24&&bitz!=16&&bitz!=8)
					bitz=32;
			}
		}
		else if(cmdOptionChar[1]=='r'||cmdOptionChar[1]=='R')
		{
			if(cmdOptionChar[0]=='+'||cmdOptionChar[0]=='-')
			{
				cmdOption=strtok(NULL," ");
				sprintf(cmdOptionChar,"%s",cmdOption);
				Height=atoi(cmdOption);
				cmdOption=strtok(NULL," ");
				sprintf(cmdOptionChar,"%s",cmdOption);
				Width=atoi(cmdOption);
			}
		}
		else if(cmdOptionChar[1]=='i'||cmdOptionChar[1]=='I')
		{
			if(cmdOptionChar[0]=='+'||cmdOptionChar[0]=='-')
			{
				justInitializeSettings=true;
			}
		}
		else if(cmdOptionChar[1]=='d'||cmdOptionChar[1]=='D')
		{
			if(cmdOptionChar[0]=='+'||cmdOptionChar[0]=='-')
			{
				developerKeys=true;
			}
		}
		else if(cmdOptionChar[1]=='t'||cmdOptionChar[1]=='T')
		{
			if(cmdOptionChar[0]=='-')
				bShowTimer=false;
			if(cmdOptionChar[0]=='+')
				bShowTimer=true;
		}
		cmdOption=strtok(NULL,"\" ");

	}
	if(justInitializeSettings)
		return 0;
	FILE *fp=fopen(filename,"r");
	if(fp==NULL)
	{
		
		if(DialogBox(hInstance, (LPCTSTR)IDD_DIALOG1, parent, (DLGPROC)DlgProc)==IDCANCEL)
			return 0;
	}
	else
	{
		deviceEnabled[0]=false;
		deviceEnabled[1]=false;
		deviceEnabled[2]=false;

		if (bJoyStickInput)
		{
			JOYINFOEX joyInfoEx;
			joyInfoEx.dwSize = sizeof(joyInfoEx);
			JoyPresent = (joyGetPosEx(JOYSTICKID1 + joystickNum, &joyInfoEx) == JOYERR_NOERROR);
			if (bJoyStickInput && JoyPresent == FALSE)
			{
				MessageBox(0, "Joystick enabled but not present!", "MazeWalker", 0);
				bJoyStickInput = false;
			}
		}

		bListen=false;
		bSendSync=false;
		//curList.AddMaze(filetemname);
		res=curList.ReadFile(filename);
		if(res==-1)
		{
			MessageBox(NULL,"Can not open maze file","Error",MB_ICONERROR);
		}
		else if(res==-2)
		{
			MessageBox(NULL,"Incorrect file format","Error",MB_ICONERROR);
		}
		if(LogStatus())
		{
			char msg[800];
			//sprintf(msg,"%d\tLoading\t%s\n",GetQPC()-startTime,filename);
			sprintf(msg,"%d\tLoading\t%s\n",0,filename);
			AddToLog(msg);
		}
	}
	if(!fullscreen)
	{	
		objCamera.x_offset = (desktopRC.right - Width)/2;
		objCamera.y_offset = (desktopRC.bottom - Height)/2;
	}


	WritePrivateProfileString("File","Map File",filename,mwalkerIniPath);

	//SetProcessDPIAware();
	
	// Create Our OpenGL Window
	if (!CreateWnd(".:: MAZE WALKER ::.",Width,Height,bitz,0))
	{
		return 0;									// Quit If Window Was Not Created
	}

	switch(cueType)
	{
		case 1:
			{
			if(!bSendSync||!bListen)
				cueType=0;
			break;
			}
		case 2:
			{if(!deviceEnabled[0]&&!deviceEnabled[1]&&!deviceEnabled[2])
				cueType=0;
			break;}
		case 3:
				cueType=3;
				cueSkipButton=true;
				break;
		case 4:{
			if(!bJoyStickInput)
			{
				cueType=0;
				GUIMessageBox("Joystick Input Not Enabled\nCue is being skipped",1000,TEXTBOXSTYLE_WARNING);
			}
			else if(joystickNum<0)
			{
				cueType=0;
				GUIMessageBox("API Joystick Input Not Supported for Cue\nCue is being skipped",1000,TEXTBOXSTYLE_WARNING);
			}
			else
			{
				switch(joyCueButton)
				{
				case 1:
					joyCueButton=JOY_BUTTON1;
					break;
				case 2:
					joyCueButton=JOY_BUTTON2;
					break;
				case 3:
					joyCueButton=JOY_BUTTON3;
					break;
				case 4:
					joyCueButton=JOY_BUTTON4;
					break;
				default:
					joyCueButton=JOY_BUTTON2;
					break;
				}
			}
			break;}
		default:
			{
				cueType=0;
			}
	}

  	if(bSendSync)
	{
		int baud;
		//open the channel and send start sync
		if(baudRate==0)
		{
			baud= 1200;
		}
		else if(baudRate==1)
		{
baud = 2400;
		}
		else if (baudRate == 2)
		{
			baud = 4800;
		}
		else if (baudRate == 3)
		{
			baud = 9600;
		}
		else if (baudRate == 4)
		{
			baud = 14400;
		}
		else if (baudRate == 5)
		{
			baud = 19200;
		}
		if (!ComConnect(comPort, baud, parity, stopBits))
		{
			bSendSync = false;
			bListen = false;
		}
	}


	if (bUseLPT&&iLptPortNum>0)
	{
		WORD wLptPort;
		switch (iLptPortNum)
		{
		case 0:
			wLptPort = 0xE010;
		case 1:
			wLptPort = 0x378;
			break;
		case 2:
			wLptPort = 0x3bc;
			break;
		case 3:
			wLptPort = 0x278;
			break;
		default:
			wLptPort = (WORD)iLptPortNum;
		}

		


		res = LPT_Connect(wLptPort);
		if (res == -1)
			GUIMessageBox("Error: Unable to Open InpOut32 Driver", 0, TEXTBOXSTYLE_FATAL_ERROR);
	}
	sendToAll(1, 50); //MazeWalker Opened
	objCamera.Initialize();
	char rootDIR[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, rootDIR);


	curMazeListItem = curList.GetFirst();
	//curTextureDict.Clear();


	while (!done && curMazeListItem)									// Loop That Runs While done=FALSE
	{
		if (curMazeListItem && curMazeListItem->type == Maze)
		{
			bStatusMazeRunning = false;
			bStatusMazeLoading = true;

			SetCurrentDirectory(rootDIR);

			char* out;
			if (curMazeListItem->isMazX)
			{
				out = ExtractToTempDIR(curMazeListItem->value);
				usingMazX = true;
			}

			//if (out != NULL)
			//	strcpy_s(temp->value,sizeof(char)*300, out);

			mazeStart = GetQPC();
			mazeStartedAndRunning = 1; //for log timer settings
			bMazeEndReached = false;
			bMazeLoop = true;
			bTimeLimitExceed = false;
			bRestartMaze = false;
			objMap = new Map();


			EventLog(0, 58, 0, "Maze Loading");



			if (ReadMap(curMazeListItem->value) == 0)
			{
				bMazeLoop = false;
				done = true;
				continue;
			}


			objMap->Count();

			//GUIMessageBox(shaderResult,0,1,0);

			//glLoadIdentity();
			//glNewList(ROOM, GL_COMPILE);			
			//objMap->Draw();			
			//glEndList();

			//glCallList(ROOM); //gets rid of color issue


			bStatusMazeLoading = false;


			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (bWaitForAPIdevices)
			{
				bStatusWaitingForAPI = true;
				EventLog(0, 59, 0, "Waiting for API devices");
				if (shadersCompiled)
					glUseProgram(shaderProgNoLights);
				else
					glDisable(GL_LIGHTING);



				int loopCount = 0;

				int numAPIDevices = deviceEnabled[0] + deviceEnabled[1] + deviceEnabled[2];

				int numAPIConnected=0;

				while (!((deviceEnabled[0] && device[0].connected) || !deviceEnabled[0]) && ((deviceEnabled[1] && device[1].connected) || !deviceEnabled[1]) && ((deviceEnabled[2] && device[2].connected) || !deviceEnabled[2]))
				{
					////////////////////

					numAPIConnected = device[0].connected + device[1].connected + device[2].connected;
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glLoadIdentity();
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();

					gluOrtho2D(0, Width, 0, Height);
					// Clear The Screen And The Depth Buffer
					glColor3f(1, 1, 1);
					glRasterPos2f(Width / 2, Height / 2);
					glPrint(true, "%s", "Waiting for API Devices to Connect");
					glRasterPos2f(Width / 2, Height / 2-30);
					glPrint(true, "%i of %i Connected", numAPIConnected, numAPIDevices);
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
					SwapBuffers(hDC); //display cue text



					

					//	Sleep(50);
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Check for message in the queue
					{
						if (msg.message == WM_QUIT)				// Check Quit Message
						{
							done = TRUE;							// If So done=TRUE
							bMazeLoop = TRUE;
							break;
						}
						else									// If Not, Deal With Window Messages
						{
							if (msg.message == WM_KILLFOCUS)
								keys[VK_ESCAPE] = true;

							TranslateMessage(&msg);				// Translate The Message
							DispatchMessage(&msg);				// Dispatch The Message
						}
					}
					if (keys[VK_ESCAPE])
					{
						GUIMessageBox("Wait For API Skipped: Escape Key Pressed", 0, TEXTBOXSTYLE_ONDIALOG_CLEAR_BK);
						break;
					}

					loopCount++;

					if (loopCount > 1000000 && false)//loop timeout disabled for now
					{
						GUIMessageBox("API wait skipped: Time Out", 0, TEXTBOXSTYLE_ONDIALOG_CLEAR_BK);
						break;
					}
				}
			}

			bStatusWaitingForAPI = false;


			if (cueType != 0)
			{
				bStatusWaitingForCue = true;
				EventLog(1, 60, 0, "Cue Period Started");
				if (shadersCompiled)
					glUseProgram(shaderProgNoLights);
				else
					glDisable(GL_LIGHTING);



				int loopCount = 0;

				while (!cue)
				{
					////////////////////


					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					glLoadIdentity();
					glMatrixMode(GL_PROJECTION);
					glPushMatrix();
					glLoadIdentity();

					gluOrtho2D(0, Width, 0, Height);
					// Clear The Screen And The Depth Buffer
					glColor3f(1, 1, 1);
					glRasterPos2f(Width / 2, Height / 2);
					glPrint(true,"%s", "Waiting for Cue");
					glPopMatrix();
					glMatrixMode(GL_MODELVIEW);
					SwapBuffers(hDC); //display cue text



					////////////////
					if (joystickNum >= 0)
					{
						JOYINFOEX joyInfoEx;
						joyInfoEx.dwFlags = JOY_RETURNBUTTONS;
						joyGetPosEx(JOYSTICKID1 + joystickNum, &joyInfoEx);

						if (joyInfoEx.dwButtons & joyCueButton&&cueType == 4)
						{
							cue = true;
							EventLog(2, 40+cueType, joyCueButton,"Joystick Cue");
							cueType = 0;
						}
					}



					//	Sleep(50);
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Check for message in the queue
					{
						if (msg.message == WM_QUIT)				// Check Quit Message
						{
							done = TRUE;							// If So done=TRUE
							bMazeLoop = TRUE;
							break;
						}
						else									// If Not, Deal With Window Messages
						{
							if (msg.message == WM_KILLFOCUS)
								keys[VK_ESCAPE] = true;

							TranslateMessage(&msg);				// Translate The Message
							DispatchMessage(&msg);				// Dispatch The Message
						}
					}
					if (keys[VK_ESCAPE])
					{
						GUIMessageBox("Trigger Skipped: Escape Key Pressed", 0, TEXTBOXSTYLE_ONDIALOG_CLEAR_BK);
						cueType = 0;
						EventLog(2, 40, 0,"Cue Skipped");
						break;
					}
					if (keys[ControlLibrary[Jump]] && (cueType == 3 || cueSkipButton))
					{
						if (cueType != 3)
						{
							GUIMessageBox("Trigger Skipped: Jump/Pause Pressed   ", 1000, TEXTBOXSTYLE_ONDIALOG_CLEAR_BK);
							EventLog(2, 40, 0,"Cue Skipped");
						}
						else
							EventLog(2, 40+cueType, 1,"Keyboard Cue");
						cueType = 0;
						keys[ControlLibrary[Jump]] = false;
						break;
					}
					loopCount++;

					if (loopCount > 1000000 && false)//loop timeout disabled for now
					{
						GUIMessageBox("Trigger Skipped: Time Out", 0, TEXTBOXSTYLE_ONDIALOG_CLEAR_BK);
						EventLog(2, 45, 2,"Cue Timeout");
						cueType = 0;
						break;
					}
				}
			}

			bStatusWaitingForCue = false;

			bStatusMazeRunning = true;

			if (LogStatus())
			{
				char msg[800];
				time(&aclock);   // Get time in seconds
				newtime = localtime(&aclock);   // Convert time to struct tm form
				sprintf(msg, "\nWalker\t:\t%s\nMaze\t:\t%s\nTime\t:\t%s\n\nTime(ms)\tMazeTime(ms)\tPos X\tPos Z\tPosY\tView X\tView Z\tView Y\n", walker, curMazeListItem->value, asctime(newtime));
				AddToLog(msg);
			}

			if (strlen(objMap->startMessage) > 0 && objMap->startMessageEnabled)
			{
				Player.gravityEnabled = false;
				GUIMessageBox(objMap->startMessage, 0, TEXTBOXSTYLE_ONDIALOG_CLEAR_BK);
				
			}
			
			
			mazeStart = GetQPC();
			updateTime = mazeStart;
			updateTimeKeys = mazeStart;
			EventLog(1, 61, 0, "MazeStart");   //MazeInitialized
			

			AlignCamera();
			char temp[TXTBUFFERLIMIT];

			if (bMouseInput)
			{
				if (fullscreen) SetCursorPos(Width >> 1, Height >> 1);
				else SetCursorPos(xLoc + Width / 2, yLoc + Height / 2);
			}

			while(bMazeLoop)
			{
#ifdef _MAZE_PROFILER
				LARGE_INTEGER start,now,freq;
				float drawStepTime;
				QueryPerformanceFrequency(&freq);
				QueryPerformanceCounter(&start);
#endif

//				LARGE_INTEGER start, now, freq;
//				float drawStepTime;
	//			QueryPerformanceFrequency(&freq);
		//		QueryPerformanceCounter(&start);

				if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Check for message in the queue
				{
					if (msg.message==WM_QUIT)				// Check Quit Message
					{
						done=TRUE;							// If So done=TRUE
						bMazeLoop=TRUE;
						break;
					}
					else									// If Not, Deal With Window Messages
					{
						if(msg.message==WM_KILLFOCUS)
							keys[VK_ESCAPE]=true;

						TranslateMessage(&msg);				// Translate The Message
						DispatchMessage(&msg);				// Dispatch The Message
					}
				}
				else										// If There Are No Messages
				{
					if(videoPlayBack==false)
					{					
						buf = Keyboard_Input();
						if(bMouseInput)
							buf += objCamera.Mouse_Move();
						if(bJoyStickInput)
						{
							float* ret;
							ret = new float[5];
							ret[0] = 0; ret[1] = 0; ret[3] = 0; ret[4] = 0;
							ret=objCamera.JoyStick_Move();
							if (ret[0] != -95432)
							{
								if (ret[1] < 0)
								{
									Player.strafingLeft.state = true;
								}
								else if (ret[1] > 0)
								{
									Player.strafingRight.state = true;
								}

								if (ret[0] > 0)
									Player.movingForward.state = true;
								else if (ret[0] < 0)
									Player.movingBackwards.state = true;

								if (ret[2] > 0)
									Player.running.state = true;
								else
									Player.running.state = false;

								if (ret[3] > 0 && Player.jumpReleased)
								{
									Player.justJumped.state = true;
									Player.jumpReleased = false;
								}
								else
									Player.jumpReleased = true;

								if (ret[4] > 0)
									Player.interact = true;
							}
							
						}
						if(buf>0||true)//objCamera.mPos.y!=0)   //if there's any change in input, record it into log..
						{
							if(firstLog==false)
							{
								Player.gravityEnabled= !bDisableGravity;
								Player.collisionEnabled=true;
								fallSpeed=0;
								jumpSpeed=0;
								Player.jumpSpeed=0;
								Player.goingUp=false;
								Player.halfSpeed=1;
								firstLog=true;
							}
							objCamera.DoLog();
						}

						
				if(objMap)
						{
							if(objMap->maxMazeTime)
							{
								buf=GetQPC() - mazeStart;
								if(objMap->maxMazeTime < buf )
								{
									bMazeLoop=false;
									bTimeLimitExceed=true;
									bMazeEndReached=true;
								}
							}
							if (curMazePoints >= objMap->pointExitThreshold&&objMap->pointExitThreshold>0)
							{
								mazeReturnValue = curMazePoints;
								printf(objMap->successMessage, objMap->pointMessage);
								bMazeLoop = false;
								
								bTimeLimitExceed = true;
								bMazeEndReached = true;

							}
						}
					}
					else if(videoPlayBack)
					{
						if(GetQPC()-mazeStart >= clogItemTime-clogStartTime)
						{
							Player.gravityEnabled=false;
							Keyboard_Input();

							if(!pausePlayback)
								{
								//read controls from log
//									char actionChar[500];
								int type,dID,state;
								

								if (NULL == fgets(temp, TXTBUFFERLIMIT, clog))
								{
									//end...
									bMazeLoop = false;
									bMazeEndReached = true;
									done = true;
								}
								else if(strlen(temp)>1)
								{
									char* pch;
									pch = strtok(temp, "\t\n");

									if (newclogType) //Includes overall time in newer log files
									{
										sscanf_s(pch, "%d\t", &actualTime);
										pch = strtok(NULL, "\t\n");
									}

									sscanf_s(pch, "%d\t", &clogItemTime);
									pch = strtok(NULL, "\t\n");

									//int ret = fscanf_s(clog, "Event\t%i\t%i\t%i", &type, &dID, &state);
									if (strcmp(pch, "Event")==0)
									{
										pch = strtok(NULL, "\t\n");
										sscanf_s(pch, "%i\t", &type);
										pch = strtok(NULL, "\t\n");
										sscanf_s(pch, "%i\t", &dID);
										pch = strtok(NULL, "\t\n");
										sscanf_s(pch, "%i\t", &state);
										pch = strtok(NULL, "\t\n");

										char nextChar;
										char comment[256];
										if(pch!=NULL && strlen(pch)>0)
											sprintf(comment, "%s", pch);

										if (type == 3) //object types of events
										{
											MapModel *temp2;
											temp2 = MapModelHead;
											while (temp2 != NULL&&temp2->dID != dID)
											{
												temp2 = temp2->next;
											}
											if (temp2 != NULL)
											{
												if (state == 1)
													temp2->Highlight(false);
												if (state == 2)
												{
													temp2->Activate();
												}
											}
											if (barScore == 0 && !Player.testPass&&interactivePlayback&&state == 2)
											{
												pausePlayback = true;
												iScoreBarMode = 1;
												bShowBar = true;
											}
										}
									}
									else if (strcmp(pch, "tcpValue") == 0)//fscanf_s(clog, "tcpValue\t%i\t%i\t%i\n", &type, &dID, &state))
									{
										pch = strtok(NULL, "\t\n");
										sscanf_s(pch, "%i\t", &type);
										pch = strtok(NULL, "\t\n");
										sscanf_s(pch, "%i\t", &dID);
										pch = strtok(NULL, "\t\n");
										sscanf_s(pch, "%i\t", &state);
										pch = strtok(NULL, "\t\n");
									}
									else// (fscanf_s(clog, "%f\t%f\t%f\t%f\t%f\t%f\n", &x, &z, &y, &vx, &vz, &vy) == 6)
									{
										bool ret=true;
										//pch = strtok(NULL, "\t\n");
										ret = sscanf_s(pch, "%f\t", &x)>0&&ret;
										pch = strtok(NULL, "\t\n");
										ret = sscanf_s(pch, "%f\t", &z)>0 && ret;
										pch = strtok(NULL, "\t\n");
										ret = sscanf_s(pch, "%f\t", &y)>0 && ret;
										pch = strtok(NULL, "\t\n");
										ret = sscanf_s(pch, "%f\t", &vx)>0 && ret;
										pch = strtok(NULL, "\t\n");
										ret = sscanf_s(pch, "%f\t", &vz)>0 && ret;
										pch = strtok(NULL, "\t\n");
										ret = sscanf_s(pch, "%f\t", &vy)>0 && ret;
										pch = strtok(NULL, "\t\n");

										if (!ret)
										{
											//end...
											bMazeLoop = false;
											bMazeEndReached = true;
											done = true;
											continue;
										}
										else
										{

											objCamera.mPos.x = x;
											objCamera.mPos.z = z;
											objCamera.mView.x = vx;
											objCamera.mView.z = vz;
											objCamera.mPos.y = y;
											objCamera.mView.y = vy;
											objCamera.DoLog();

											barScore = 0;
											targetBarScore = 0;
											Player.testPass = false;


											AlignCamera();

											if (fullscreen) SetCursorPos(Width >> 1, Height >> 1);
										}
									}
								}
								
							}
							else
							{
								//AlignCamera();
								
							}
						}

					}
					// Draw The Scene.  Watch For ESC Key And Quit Messages From Draw()
					if ((active && !Draw()))	
					{
						bMazeLoop=false;
						break;
						//done=TRUE;						// Exit Loop
					}
					else									// Update Screen
					{
#ifdef _MAZE_PROFILER
						QueryPerformanceCounter(&now);
						drawStepTime = ((float)(now.QuadPart - start.QuadPart)*1000.0)/freq.QuadPart;
						start = now;
#endif					
						//QueryPerformanceCounter(&now);
						//drawStepTime = ((float)(now.QuadPart - start.QuadPart)*1000.0) / freq.QuadPart;
						//start = now;
						SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
						
					}
#ifdef _MAZE_PROFILER
					QueryPerformanceCounter(&now);
					DWORD bufferSwapTime = GetQPC();//((float)(now.QuadPart - start.QuadPart)*1000.0)/freq.QuadPart;
					start = now;
#endif
					//QueryPerformanceCounter(&now);
					//float bufferSwapTime = ((float)(now.QuadPart - start.QuadPart)*1000.0) / freq.QuadPart;
					//start = now;
					//specialVar = drawStepTime;
					//specialVar2 = bufferSwapTime;
					//START DRAWING MESSAGEBOX
					if (keys[VK_ESCAPE])
					{
						int res=0;
						bool showSkip=false;
						
						bool mouseSelect=false;
						keys[VK_ESCAPE]=false;

						

						POINT initMouse;						// Stores The X And Y Coords For The Current Mouse Position
						initMouse=GetOGLPos();

						POINT prevMousePos = GetOGLPos();
						POINT mouse = GetOGLPos();						// Gets The Current Cursor Coordinates (Mouse Coordinates)

						

						if(curMazeListItem->next!=NULL)
							showSkip=true;
						if(!pausePlayback)
						{
							pauseTime=GetQPC()-mazeStart;
							EventLog(2,10,10,"Paused");//paused
						}
						bool once = true;

						GLubyte* screenCap;	// Captures current screen and sets as background
						std::list<OptionBox> boxList;
						int selectedIndex = 0;
						bool selectNext = true;
						bool selectPrev = false;

						glUseProgram(0);
						glDisable(GL_LIGHTING);
						glDisable(GL_DEPTH_TEST);


						glEnable(GL_TEXTURE_2D);

						screenCap = new GLubyte[4 * Width*Height];
						glReadPixels(0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, screenCap);
						glGenTextures(1, &screenCapID);
						glBindTexture(GL_TEXTURE_2D, screenCapID);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, screenCap);
						free(screenCap);
						glMatrixMode(GL_PROJECTION);
						glPushMatrix();
						glLoadIdentity();
						gluOrtho2D(0, Width, 0, Height);

						while (res == 0)
						{
							if (once || resizeFLAG)
							{
								once = false;
								if (resizeFLAG)
									resizeFLAG = false;
								
								

								boxList.clear();
								if (!showSkip)
								{
									int w = 110;
									int spc = 40;
									boxList.push_back(OptionBox("\nDo you want to quit MazeWalker?\n", -1, -230, -100, 460, 180));

									boxList.push_back(OptionBox("Yes", 2, -220 + spc, -60, w, 40));
									boxList.push_back(OptionBox("No", 1, -w / 2, -60, w, 40));
									boxList.push_back(OptionBox("Restart", 3, 220 - w - spc, -60, w, 40));
								}
								else
								{
									int w = 100;
									int spc = 20;
									boxList.push_back(OptionBox("\nDo you want to quit MazeWalker?\n", -1, -240, -100, 480, 180));

									boxList.push_back(OptionBox("Yes", 2, -240 + spc / 2, -60, w, 40));
									boxList.push_back(OptionBox("No", 1, -w - spc / 2, -60, w, 40));
									boxList.push_back(OptionBox("Restart", 3, +spc / 2, -60, w, 40));
									boxList.push_back(OptionBox("Next", 4, 240 - w - spc / 2, -60, w, 40));
								}
								selectedIndex = 0;
								selectNext = true;
								selectPrev = false;

								glLoadIdentity();
								gluOrtho2D(0, Width, 0, Height);
								mouse = GetOGLPos();
								prevMousePos = mouse;

							}
							
							bool clicked = false;

							if (keys[VK_ESCAPE])
							{
								res=1;
								keys[VK_ESCAPE]=false;
							}
							
							if (keys[Y])
							{
								res=2;
								keys[Y]=false;
							}
							if (keys[N])
							{
								res=1;
								keys[N]=false;
							}
							if (keys[VK_LEFT])
							{
								selectPrev = true;
								mouseSelect=false;
								keys[VK_LEFT]=false;
							}
							if (keys[VK_RIGHT])
							{
								selectPrev = false;
								selectNext = true;
								mouseSelect=false;
								keys[VK_RIGHT]=false;
							}
							if (keys[VK_RETURN])
							{
								clicked = true;
								mouseSelect=false;
								keys[VK_RETURN]=false;
							}
							if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Check for message in the queue
							{
								if (msg.message==WM_QUIT)				// Check Quit Message
								{
									done =true;// If So done=TRUE
									bMazeLoop=TRUE;
									res=2;
								}
								else if(msg.message==WM_LBUTTONDOWN)
								{
									clicked = true;
								}
								else									// If Not, Deal With Window Messages
								{
									TranslateMessage(&msg);				// Translate The Message
									DispatchMessage(&msg);				// Dispatch The Message
								}
							}
							
							/*
							if (shadersCompiled)
							{
								glUseProgram(shaderProgBasic);
							}
							*/
							

							glEnable(GL_TEXTURE_2D);
							
							//if(oculusEnabled) //Use Oculus menu
								//glBindTexture(GL_TEXTURE_2D, screenCapID);
							//else
								glBindTexture(GL_TEXTURE_2D, screenCapID);

							glColor3f(1,1,1);							
							glBegin(GL_QUADS); //BG Fill Box
								glTexCoord2d(0,0); glVertex2f(0, 0);
								glTexCoord2d(1,0); glVertex2f(Width, 0);
								glTexCoord2d(1,1); glVertex2f(Width, Height);
								glTexCoord2d(0,1); glVertex2f(0, Height);
							glEnd();							
							glDisable(GL_TEXTURE_2D);
							glBindTexture(GL_TEXTURE_2D,0);


							prevMousePos = mouse;
						
							

							mouse=GetOGLPos();						// Gets The Current Cursor Coordinates (Mouse Coordinates)s
							
							if (mouse.x != prevMousePos.x || mouse.y != prevMousePos.y) //using mouse reverts to mouse mode
								mouseSelect = true;

						    
							int boxIndex = -1;
							bool selectCheck = false;

							for (std::list<OptionBox>::const_iterator iterator = boxList.begin(), end = boxList.end(); iterator != end; ++iterator)
							{
								boxIndex++;
								OptionBox box = (OptionBox)*iterator;

								if (selectedIndex == boxIndex)
								{
									if (box.GetRes() == -1)
									{
										selectNext = true;
										if (selectCheck)
										{
											selectNext = false;
											selectPrev = true;
											selectCheck = false;
										}
									}
									if (selectNext)
									{
										selectedIndex++;
										selectNext = false;
									}
									if (selectPrev)
									{
										selectedIndex--;
										selectCheck = true;
										selectPrev = false;
									}

									if (selectedIndex >= boxList.size())
										selectedIndex = boxList.size() - 1;
									if (selectedIndex < 0)
									{
										selectPrev = false;
										selectCheck = false;
										selectedIndex = 0;
									}
								}

								if (mouseSelect)
								{
									box.Select(box.IsMouseOver(mouse.x, mouse.y));
								}
								else
								{
									if (boxIndex == selectedIndex)
									{
										box.Select(true);
									}
									else
										box.Select(false);
								}

								if (box.Selected())
								{
									selectedIndex = boxIndex;
									selectCheck = false;
									selectNext = false;
									selectPrev = false;
								}

								if (box.Selected() && clicked)
								{
									res = box.GetRes();
								}
								box.Draw();
							}

						


							DrawMouse();


							SwapBuffers(hDC); //Escape menu
							glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer

						}

						if(res!=0)
						{
							
							if(bMouseInput)
								SetCursorPos(xLoc+Width/2,yLoc+Height/2);
							
							
							if(!pausePlayback)
							{	
								mazeStart=GetQPC()-pauseTime;
								EventLog(2,11,11,"Unpaused");//unpaused
								pauseTime=0;
								updateTime=GetQPC();
								updateTime2=GetQPC();
							}
						}
						for (int z=UpdateTimeBufferCount;z>0;z--)
						{
							updateTimeFrame[z]=0;
						}
						if(res==2||res==3||res==4) //if Quit, repeat, or skip
						{
							bMazeLoop=false; //leave loop
							if (res == 3) //restart maze
							{
								EventLog(1, 71, 0, "Maze Restarted");
								bRestartMaze = true;
							}
							else if(res==2)
								done=true;
							else if (res==4) //NextMaze
								EventLog(1, 72, 0, "Maze Skipped");
						}	
						boxList.clear();
						glPopMatrix();
						glMatrixMode(GL_MODELVIEW);
						glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
						glEnable(GL_TEXTURE_2D);
						
						if(shadersCompiled)
							glUseProgram(shaderProg);

						glDeleteTextures(1, &screenCapID);
						screenCapID=0;

					}

#ifdef _MAZE_PROFILER 
					if(debugFile!=NULL)
					{
						fprintf(debugFile,"%d\n",bufferSwapTime);
					}
#endif
					if(keys['I'])
					{
						if(shadersCompiled)
							shader = !shader;
						keys['I'] = false;
						if(shader)
							setDebugMessage("Advanced Shader On");
						else
							setDebugMessage("Advanced Shader Off");
					}
					if(keys[ControlLibrary[LightToggle]])
					{
						lights = !lights;
						objMap->lights=lights;
						
						keys[ControlLibrary[LightToggle]] = false;
						if(lights)
							setDebugMessage("Lights On");
						else
							setDebugMessage("Lights Off");
					}
					if(keys[ControlLibrary[TimerToggle]])
					{
						bShowTimer = !bShowTimer;
						keys[ControlLibrary[TimerToggle]] = false;
						if(bShowTimer)
							setDebugMessage("Timer On");
						else
							setDebugMessage("Timer Off");
					}
					if(keys[ControlLibrary[CrosshairToggle]])
					{
						bShowCrosshair = !bShowCrosshair;	
						keys[ControlLibrary[CrosshairToggle]] = false;
						if(bShowCrosshair)
							setDebugMessage("Crosshair On");
						else
							setDebugMessage("Crosshair Off");
					}
					if(keys[ControlLibrary[BarToggle]])
					{
						bShowBar = !bShowBar;
						keys[ControlLibrary[BarToggle]] = false;
						if(bShowBar)
							setDebugMessage("API//MazePoints Bar On");
						else
							setDebugMessage("API//MazePoints Bar Off");
					}

				}

			}
			

			if(bMazeEndReached==true&&!videoPlayBack)
			{
				if(bTimeLimitExceed==false)
				{
					if(objMap->successMessage[0]!=NULL)
						GUIMessageBox(objMap->successMessage,0,TEXTBOXSTYLE_ONDIALOG_WITH_BK);
		
				}
				else if (objMap->pointExitThreshold>0&&curMazePoints>=objMap->pointExitThreshold)
				{
					if (objMap->pointMessageEnabled&&objMap->pointMessage[0] != NULL)
						GUIMessageBox(objMap->pointMessage, 0, TEXTBOXSTYLE_ONDIALOG_WITH_BK);
				}
				else
				{
					if (objMap->timeoutMessage[0] != NULL)
						GUIMessageBox(objMap->timeoutMessage, 0, TEXTBOXSTYLE_ONDIALOG_WITH_BK);
				}


		
			}
			else if(bMazeEndReached&&videoPlayBack)
				GUIMessageBox("Log file Ended",0,TEXTBOXSTYLE_ONDIALOG_WITH_BK);

			glDeleteLists(ROOM,1);
			delete objMap;
			objMap=NULL;

			mazeStartedAndRunning = 2;
			bStatusMazeRunning = false;
			EventLog(1,70, 0,"Maze Ended");
		}
		else if(curMazeListItem && curMazeListItem->type==Text)
		{
			sprintf(curMazeFilename, "MazeList Text");

			bStatusMazeMessage = true;
			//Show Text Message...
			strcpy_s(filename, curMazeListItem->value);
			lifeTime = curMazeListItem->lifeTime;
			//DialogBox(hInstance, (LPCTSTR)IDD_DIALOG5, hWnd, (DLGPROC)TxtDlgProc);
			//SetCursorPos(Width,Height);

			EventLog(1,80,  0, "TextMessage Start");
			if(strlen(curList.mListDIR)>1)
				SetCurrentDirectoryA(curList.mListDIR);
			
			//curTextureDict.ClearUnused();
			
			curTextureDict.SetWhite(whiteID);
			GLuint texID = 0;
			if (strlen(curMazeListItem->BGfname) > 9)
			{
				LoadTexture(curMazeListItem->BGfname, 33000);
				texID = curTextureDict.Get_glKey(33000);
				//LoadTexture(temp->BGfname, 300);
			}
			else 
					texID = 0;
			
			//LoadTexture("texture\\123.jpg", 300);
			

			

			GUIMessageBox(curMazeListItem->value, lifeTime, curMazeListItem->showStyle, texID);
			SetFocus(hWnd);
			EventLog(1, 81, 0, "TextMessage End");
			bStatusMazeMessage = false;
			//MessageBox(parent,temp->value,"Message",0);
		}

		if (bRestartMaze==false) //repeat maze prevents loading next maze
		{
			curMazeListItem = curMazeListItem->next;
			
		}
		else // Maze Restarted
		{
			bRestartMaze = false;
			EventLog(1, 71, 0, "Maze Restarted");
		}
	}

	EventLog(1, 51,0,"MazeWalker Closing");   //MazeWalker Closing

	if(bSendSync)
	{
		if(bListen)
			bListen=false;
		else
			ComClose();
		bSendSync=false;
	}

	if (bUseLPT)
		LPT_Free();
	



	device[0].Disconnect();
	device[1].Disconnect();
	device[2].Disconnect();
	deviceEnabled[0] = false;
	deviceEnabled[1] = false;
	deviceEnabled[2] = false;

	
	curAudioDict.Stop(0);
	
	curAudioDict.Clear();

	if(joystickNum>0)
		joyReleaseCapture(JOYSTICKID1+joystickNum);	
	curTextureDict.Clear();

	ModelDictHead=NULL;
	ExitBulletPhysics();
	_CrtDumpMemoryLeaks();


	// Shutdown
	Kill(); // Kill The Window
	Sleep(100);
	return 0;								// Exit The Program
}


DWORD timeSincePhysics = 0;

int CheckCollision(float* x,float dx,float *vx, float* z,float dz,float *vz,float* y, float dy, float *vy)
{

	int changes=0;

	if(Player.collisionEnabled)
		objMap->CheckCollision(x,y,z);

	

	if(Player.collisionEnabled&&!videoPlayBack)
	{
			

			if(_isnan(dx)||_isnan(dz)||_isnan(dy))
			{
				dx=0;
				dz=0;
				dy=0;
			}


			btVector3 walkDirection = btVector3(dx, dy, dz);
			
			controller->setWalkDirection(walkDirection);
			//btTransform characterWorldTrans = ghostObject->getWorldTransform();
			//btVector3 origin = characterWorldTrans.getOrigin();

			DWORD diffTimeSincePhysics = GetQPC() - timeSincePhysics;

			if (diffTimeSincePhysics > 1000)
				diffTimeSincePhysics = 0;

			DWORD minTimeStep = (1.0f / 120.0f * 1000.0f);

			//while (diffTimeSincePhysics>minTimeStep)
			//{
			//	 diffTimeSincePhysics -= minTimeStep;
			//	 StepBulletPhysics(minTimeStep/1000.0f); // Movement happens in here
			//}

			timeSincePhysics = GetQPC();
			StepBulletPhysics(diffTimeSincePhysics / (double)1000.0f); // Movement happens in here
			

			
			
			//This section is outdated
	//		dx = newOrigin.getX() - origin.getX();
		//	dy = newOrigin.getY()- origin.getY();
			//dz = newOrigin.getZ() - origin.getZ();


			//changes = changes + abs(dx) > 0 + abs(dy) > 0 + abs(dz) > 0;

			
			//if (changes>0)
			//{
			//	//*x=origin.getX();
			//	//*y=origin.getY();
			//	//*z=origin.getZ();

			//	*vx += dx;
			//	*vy += dy;
			//	*vz += dz;
			//}
			//
			
		}
	else
	{
		*x+=dx;
		*y+=dy;
		*z+=dz;
		*vx += dx;
		*vz += dz;
		*vy += dy;
	}
	


	return changes;
}
 void sendToTCP(tcpMessage t);


 MapModel* getDynModelByID(int ID)
 {
	 MapModel *temp2;
	 temp2 = MapModelHead;
	 bool modelFound = false;

	 while (temp2 != NULL)
	 {
		 if (temp2->dynamic&&temp2->dID == ID)
		 {
			 modelFound = true;
			 break;
		 }
		 temp2 = temp2->next;
	 }
	 if (!modelFound)
		 return NULL;
	 else
		 return temp2;
 }

 MapModel* getDynModelByLabel(char* label)
 {
	MapModel *temp2;
	temp2 = MapModelHead;
	bool modelFound = false;

	while (temp2 != NULL)
	{
		if (temp2->dynamic&&strcmp(label, temp2->name) == 0)
		{
			modelFound = true;
			break;
		}
		temp2 = temp2->next;
	}
	if (!modelFound)
		return NULL;
	else
		return temp2;
 }

 void sendTCPerror(int command,char* errormessage)
 {
	 tcpMessage posPacket;
	 posPacket.command = command;
	posPacket.iArg = -999;
	posPacket.dArgs[0] = -999;
	posPacket.dArgs[1] = -999;
	posPacket.dArgs[2] = -999;
	posPacket.dArgs[3] = -999;

	posPacket.storeStringInArr(errormessage);
	 sendToTCP(posPacket);
 }

 void sendTCPstring(int command, char* tcpString)
 {

	 int msgLen = strlen(tcpString);
	 int numPackets = (msgLen-(msgLen%32))/32+1;

	 tcpMessage posPacket;
	 posPacket.command = command;

	 for (int i = 0; i < numPackets; i++)
	 {
		 
		 posPacket.iArg = i;
		 posPacket.dArgs[0] = -999;
		 posPacket.dArgs[1] = -999;
		 posPacket.dArgs[2] = -999;
		 posPacket.dArgs[3] = -999;

		 posPacket.storeStringInArr(&tcpString[i*32]);
		 sendToTCP(posPacket);
	 }

	 
	 
 }



 DWORD WINAPI listenThread(LPVOID lpParam)
 {
	 int devID = int(lpParam);
	 
	 DWORD ipAddr;
	 if (deviceLocal[devID])
		 ipAddr = 2130706433;
	 else
		 ipAddr = deviceIP[devID];
	 bool connected = false;
	 while (!done&&deviceEnabled[devID])
	 {
		 device[devID].connecting = true;
		 device[devID] = ServerSocket(devicePort[devID], ipAddr);
		 device[devID].connecting = false;
		 
		 device[devID].unused = false;

		 Sleep(100);
		 
		 while (deviceEnabled[devID]&&device[devID].Receive() > 0)
		 {
			 if (connected == false)
			 {
				 char msg[256];
				 sprintf(msg, "Device %i Connected", devID + 1);
				 setDebugMessage(msg);
				 connected = true;

			 }
			 
			 tcpMessage m = device[devID].message;

			 if (m.command != -1000)
				 EventLog(4, m.command, m.iArg,"API");

			 if (m.command == 10 && !interactivePlayback)
			 {
				 pausePlayback = !pausePlayback;
				 if (!pausePlayback)
				 {
					 mazeStart = GetQPC() - pauseTime;
					 pauseTime = 0;
				 }
				 else
				 {
					 pauseTime = GetQPC() - startTime;
				 }

			 }
			 else if (m.command == 20 && cueType == 2)
			 {
				 cue = true;
				 EventLog(2, 40+cueType, devID,"Cue Received");
			 }
			 else if (m.command == 13) //jump
			 {
				 Player.justJumped.moveCount = 1;
			 }
			 else if (m.command == 14) //move forward for iArg franes
			 {
				 Player.movingForward.state = true;
				 Player.movingForward.moveCount = m.iArg;
			 }
			 else if (m.command == 15) //move backwards for iArg frames
			 {
				 Player.movingBackwards.state = true;
				 Player.movingBackwards.moveCount = m.iArg;
			 }
			 else if (m.command == 16)// strafe left for iArg frames
			 {
				 Player.strafingLeft.state = true;
				 Player.strafingLeft.moveCount = m.iArg;
			 }
			 else if (m.command == 17) //strafe right for iArg frames
			 {
				 Player.strafingRight.state = true;
				 Player.strafingRight.moveCount = m.iArg;
			 }
			 else if (m.command == 18)//look left for iArg frames
			 {
				 Player.lookingLeft.state = true;
				 Player.lookingLeft.moveCount = m.iArg;
			 }
			 else if (m.command == 19)//look right for iArg frames
			 {
				 Player.lookingRight.state = true;
				 Player.lookingRight.moveCount = m.iArg;
			 }
			 else if (m.command == 24) //moves forward instantly by iArg/100 units
			 {
				 Player.movingForward.instantMove += (float)m.iArg / 100.0f;
			 }
			 else if (m.command == 25) //moves backward instantly by iArg/100 units
			 {
				 Player.movingBackwards.instantMove += (float)m.iArg / 100.0f;
			 }
			 else if (m.command == 26) //strafes left instantly by iArg/100 units
			 {
				 Player.strafingLeft.instantMove += (float)m.iArg / 100.0f;
			 }
			 else if (m.command == 27) //strafes right instantly by iArg/100 units
			 {
				 Player.strafingRight.instantMove += (float)m.iArg / 100.0f;
			 }
			 else if (m.command == 28) //turns left instantly by iArg/10 degrees
			 {
				 Player.lookingLeft.instantMove += (float)m.iArg / 10.0f;
			 }
			 else if (m.command == 29) //turns right instantly by iArg/10 degrees
			 {
				 Player.lookingRight.instantMove += (float)m.iArg / 10.0f;
			 }
			 else if (m.command == 30) //set default walking speed
			 {
				 if (m.iArg == 1) //reset speed
					 objCamera.cameraSpeedMove = objCamera.cameraSpeedMoveDefault;
				 else
					objCamera.cameraSpeedMove = (float)m.dArgs[0]/60;  //Expects default value to be 3Mz/s  ... 0.05*60
			 }
			 else if (m.command == -100)  //Logg message from tcp
			 {
				 if (LogStatus())
				 {
					 char* tcpMsg = new char[256];
					 tcpMsg = m.getStringFromArr();
					 char msg[800];
					 sprintf(msg, "%d\t%d\ttcpMessage\t%d\t%d\t%s\n", GetQPC() - startTime, GetQPC() - mazeStart, m.command, m.iArg, tcpMsg);
					 AddToLog(msg);
				 }
			 }
			 else if (m.command == -101)  //Log message and show to user
			 {
				 char* tcpMsg = new char[256];
				 tcpMsg = m.getStringFromArr();
				 char msg[200];
				 sprintf(devMsg, "%s", tcpMsg);
				 sprintf(msg, "%d\t%d\ttcpAlert\t%d\t%d\t%s\n", GetQPC() - startTime, GetQPC() - mazeStart, m.command, m.iArg, tcpMsg);
				 devMsgStatus = 1;
				 devMsgTime = m.iArg;
				 if (LogStatus())
					 AddToLog(msg);
			 }
			 else if (m.command == -102 && !interactivePlayback)
			 {
				 char* tcpMsg = new char[256];
				 tcpMsg = m.getStringFromArr();
				 char msg[200];
				 sprintf(devMsg, "%s", tcpMsg);
				 sprintf(msg, "%d\t%d\tUserResponse\t%d\t%d\t%s\n", GetQPC() - startTime, GetQPC() - mazeStart, m.command, m.iArg, tcpMsg);
				 devMsgStatus = 2;
				 devMsgTime = m.iArg;
				 if (LogStatus())
					 AddToLog(msg);

				 pausePlayback = true;
				 pauseTime = GetQPC() - startTime;

			 }

			 else if (m.command == -90)
			 {	 
				 tVector3 vVector = objCamera.mView - objCamera.mPos;	// Get the view vector
				 newAPIpos = objCamera.mPos;
				 
				 if (!isnan(m.dArgs[0]))
					 newAPIpos.x = m.dArgs[0];
				 if (!isnan(m.dArgs[1]))
					 newAPIpos.y = m.dArgs[1];
				 if (!isnan(m.dArgs[2]))
					 newAPIpos.z = m.dArgs[2];

				 newAPIview = newAPIpos + vVector;

				 if (!isnan(m.dArgs[3]))
				 {
					newAPIview = newAPIpos;
					newAPIview.x += 1;
					apiRotateViewAngleTo = m.dArgs[3];

				 }
				 alignCameraFlag = true;

			 }
			 else if (m.command == -91) //set X pos
			 {
				 tVector3 vVector = objCamera.mView - objCamera.mPos;	// Get the view vector
				 newAPIpos = objCamera.mPos;
				 newAPIpos.x = m.dArgs[0];
				 newAPIview = newAPIpos + vVector;
				 //objCamera.Position_Camera(objCamera.mPos.x, objCamera.mPos.y, objCamera.mPos.z, 0, 0, 0, 0, 1, 0);
				 //objCamera.InitialRotate(objCamera.GetCurAngle(), xLoc + Width / 2, yLoc + Height / 2, 0);
				 alignCameraFlag = true;
			 }
			 else if (m.command == -92) //set Y pos
			 {
				 tVector3 vVector = objCamera.mView - objCamera.mPos;	// Get the view vector
				 newAPIpos = objCamera.mPos;
				 newAPIpos.y = m.dArgs[1];
				 newAPIview = newAPIpos + vVector;
				 alignCameraFlag = true;
			 }
			 else if (m.command == -93) //set Z pos
			 {

				 tVector3 vVector = objCamera.mView - objCamera.mPos;	// Get the view vector
				 newAPIpos = objCamera.mPos;
				 newAPIpos.z = m.dArgs[2];
				 newAPIview = newAPIpos + vVector;
				 alignCameraFlag = true;
			 }
			 else if (m.command == -94) //set XYZ pos
			 {
				 newAPIpos = objCamera.mPos;
				 newAPIview = newAPIpos;
				 newAPIview.x += 1;

				 apiRotateViewAngleTo = m.dArgs[3];
				 alignCameraFlag = true;
			 }
			 else if (m.command == -297)//set dynamic model position by ID
			 {
				 MapModel *temp2;
				 bool modelFound = false;

				 if (m.iArg != -1) //search for model by ID
				 {
					 temp2 = getDynModelByID(m.iArg);
					 if (temp2 != NULL)
						 modelFound = true;
				 }


				 if (modelFound)
				 {
					 if (!isnan(m.dArgs[0]))
						temp2->pos.x = m.dArgs[0];
					 if (!isnan(m.dArgs[1]))
						temp2->pos.y = m.dArgs[1];
					 if (!isnan(m.dArgs[2]))
						temp2->pos.z = m.dArgs[2];
				 }
				 else  //not found error
				 {
					 sendTCPerror(-297, "Model Not Found");
				 }
			 }
			 else if (m.command == -298)//set dynamic model rotation and scale by ID
			 {
				 MapModel *temp2;
				 bool modelFound = false;

				 if (m.iArg != -1) //search for model by ID
				 {
					 temp2 = getDynModelByID(m.iArg);
					 if (temp2 != NULL)
						 modelFound = true;
				 }
				 if (modelFound)
				 {
					 temp2->moving = false;

					 if(!isnan(m.dArgs[0]))
						temp2->rot.x = m.dArgs[0];
					 if (!isnan(m.dArgs[1]))
						temp2->rot.y = m.dArgs[1];
					 if (!isnan(m.dArgs[2]))
						temp2->rot.z = m.dArgs[2];
					 if(!isnan(m.dArgs[3])&&m.dArgs[3]>=0)
						temp2->scale = m.dArgs[3];
				 }
				 else  //not found error
				 {
					 sendTCPerror(-298, "Model Not Found");
				 }
			 }
			 else if (m.command == -299)//set current model to new modelID
			 {
				 MapModel *temp2;
				 bool modelFound = false;

				 if (m.iArg != -1) //search for model by ID
				 {
					 temp2 = getDynModelByID(m.iArg);
					 if (temp2 != NULL)
						 modelFound = true;
				 }
				 int targetModelID = (int)m.dArgs[0];
				 bool targetModelFound = false;
				 if (modelFound&&temp2->modelID!= targetModelID)
				 {
					 

					 if (targetModelID != temp2->modelID) //switch model
					 {
						 GLMmodelNode *temp;
						 temp = (GLMmodelNode*)malloc(sizeof(GLMmodelNode));
						 temp = ModelDictHead;
						 while (temp != NULL&&temp->modelID != targetModelID)
						 {
							 temp = temp->next;
						 }
						 if (temp != NULL)
						 {
							 temp2->modelData = temp->modelData;
							 temp2->MinCoords = temp->MinCoords;
							 temp2->MaxCoords = temp->MaxCoords;
							 temp2->modelID = temp->modelID;
							 targetModelFound = true;
						 }
						 
					 }
				 }
				 if(modelFound&&temp2->modelID== targetModelID&&!targetModelFound)
				 {
					 sendTCPerror(-299, "Model and new model are identical");
				 }
				 else if (modelFound&&temp2->modelID != targetModelID)
				 {
					 sendTCPerror(-299, "Target Model Not Found");
				 }
				 else  //not found error
				 {
					 sendTCPerror(-299, "Model Not Found");
				 }
			 }
			 else if (m.command == -500)//skip or end current level
			 {
				 bMazeLoop = false;
			 }

			 else if (m.command == 1) //set Score (buffered)
			 {
				 targetBarScore = m.iArg;
				 if (LogStatus())
				 {
					 char msg[800];
					 sprintf(msg, "%d\t%d\tEvent\t%d\t%d\t%f\n", GetQPC() - startTime, GetQPC() - mazeStart, 20, 0, targetBarScore);
					 AddToLog(msg);
				 }
			 }
			 else if (m.command == 2) //instantaneous Set Score
			 {
				 barScore = m.iArg;
				 targetBarScore = barScore;
				 if (LogStatus())
				 {
					 char msg[800];
					 sprintf(msg, "%d\t%d\tEvent\t%d\t%d\t%f\n", GetQPC() - startTime, GetQPC() - mazeStart, 20, 0, barScore);
					 AddToLog(msg);
				 }
			 }
			 else if (m.command == 3) //interact with objects (formerly temporary instantaneous set score)
			 {
				 Player.interact = true;
				 if (LogStatus())
				 {
					 char msg[800];
					 sprintf(msg, "%d\t%d\tEvent\t%d\t%d\t%d\n", GetQPC() - startTime, GetQPC() - mazeStart, 20, 0, 101);
					 AddToLog(msg);
				 }
			 }
			 else if (m.command == -10)
			 {
				 int val = m.iArg;
				 if (LogStatus())
				 {
					 char msg[800];
					 sprintf(msg, "%d\t%d\ttcpValue\t%d\t%d\t%d\n", GetQPC() - startTime, GetQPC() - mazeStart, 80, 0, val);
					 AddToLog(msg);
				 }
			 }
			 else if (m.command == 5) //Get X Pos
				 sendToTCP(5, (int)(objCamera.mPos.x * 10));
			 else if (m.command == 6) //Get Y Pos
				 sendToTCP(6, (int)(objCamera.mPos.y * 10));
			 else if (m.command == 7) //Get Z Pos
				 sendToTCP(7, (int)(objCamera.mPos.z * 10));
			 else if (m.command == 12) //Get Current Score
				 sendToTCP(12, (int)barScore);
			 else if (m.command == 101) //Get MazeWalker Status Code
			 {
				 if (bStatusMazeLoading)
					 sendToTCP(101, 0);
				 else if (bStatusWaitingForAPI)
					 sendToTCP(101, 1);
				 else if (bStatusWaitingForCue)
					 sendToTCP(101, 2);
				 else if (bStatusMazeMessage)
					 sendToTCP(101, 3);
				 else if (bStatusMazeRunning && !pausePlayback)
					 sendToTCP(101, 4);
				 else if (bStatusMazeRunning && pausePlayback)
					 sendToTCP(101, 5);
				 else
					 sendToTCP(101, 999);
				
		     }

			 else if (m.command == 102) //get elapsed time (in maze)
			 {
					sendToTCP(102, GetQPC() - startTime);
			 }

			 else if (m.command == 103) //get current maze name
			 {
					sendTCPstring(103, curMazeFilename);
			 }

			 else if (m.command == 97)//get position
			 {
				 tcpMessage posPacket;
				 posPacket.command = 97;
				 posPacket.iArg = GetQPC() - startTime;
				 posPacket.dArgs[0] = objCamera.mPos.x;
				 posPacket.dArgs[1] = objCamera.mPos.y;
				 posPacket.dArgs[2] = objCamera.mPos.z;
				 float angle = (float)(atan2((objCamera.mView.x - objCamera.mPos.x), (objCamera.mView.z - objCamera.mPos.z))* 180.0f) / (float)M_PI;
				 angle -= 90;

				 posPacket.dArgs[3] = angle;
				 sendToTCP(posPacket);
			 }
			 else if (m.command == 290) //get dynamic model ID by label
			 {
				 MapModel *temp2;
				 bool modelFound = false;


				char* labelstr = m.getStringFromArr();
				temp2 = getDynModelByLabel(labelstr);
				if (temp2 != NULL)
					modelFound = true;

				 if (modelFound)
				 {
					 tcpMessage posPacket;
					 posPacket.command = 290;
					 posPacket.iArg = temp2->dID;
					 sendToTCP(posPacket);
				 }
				 else  //not found error
				 {
					 sendTCPerror(290, "Model Not Found");
				 }
			 }
			 else if (m.command == 291) //get dynamic model status by ID
			 {
				 MapModel* temp2;
				 bool modelFound = false;

				 if (m.iArg != -1) //search for model by ID
				 {
					 temp2 = getDynModelByID(m.iArg);
					 if (temp2 != NULL)
						 modelFound = true;
				 }
				 else //search by label
				 {
					 char* labelstr = m.getStringFromArr();
					 temp2 = getDynModelByLabel(labelstr);
					 if (temp2 != NULL)
						 modelFound = true;
				 }
				 if (modelFound)
				 {
					 int status = 0;

					 if (temp2->highlighted)
						 status = 1;
					 else if (temp2->activated&&temp2->moving)
						 status = 2;
					 else if (temp2->activated)
						 status = 3;
					 else if (temp2->destroyed)
						 status = 4;

					sendToTCP(291, status);
				 }
				 else  //not found error
				 {
					 sendTCPerror(291, "Model Not Found");
				 }
			 }
			 else if (m.command == 297)//get dynamic model position & ModelID
			 {
				 MapModel *temp2;
				 bool modelFound=false;

				 if (m.iArg != -1) //search for model by ID
				 {
					 temp2 = getDynModelByID(m.iArg);
					 if (temp2 != NULL)
						 modelFound = true;
				 }
				 else //search by label
				 {
					 char* labelstr = m.getStringFromArr();
					 temp2 = getDynModelByLabel(labelstr);
					 if (temp2 != NULL)
						 modelFound = true;
				 }
				 if (modelFound)
				 {
					 tcpMessage posPacket;
					 posPacket.command = 297;
					 posPacket.iArg = GetQPC() - startTime;
					 posPacket.dArgs[0] = temp2->pos.x;
					 posPacket.dArgs[1] = temp2->pos.y;
					 posPacket.dArgs[2] = temp2->pos.z;
					 posPacket.dArgs[3] = temp2->modelID;
					 sendToTCP(posPacket);
				 }
				 else  //not found error
				 {
					 sendTCPerror(297, "Model Not Found");
				 }
			 }
			 else if (m.command == 298)//get dynamic model rotation & scale
			 {
				 MapModel *temp2;
				 bool modelFound = false;

				 if (m.iArg != -1) //search for model by ID
				 {
					 temp2 = getDynModelByID(m.iArg);
					 if (temp2 != NULL)
						 modelFound = true;
				 }
				 else //search by label
				 {
					 char* labelstr = m.getStringFromArr();
					 temp2 = getDynModelByLabel(labelstr);
					 if (temp2 != NULL)
						 modelFound = true;
				 }


				 if (modelFound)
				 {
					 tcpMessage posPacket;
					 posPacket.command = 298;
					 posPacket.iArg = GetQPC() - startTime;
					 posPacket.dArgs[0] = temp2->rot.x;
					 posPacket.dArgs[1] = temp2->rot.y;
					 posPacket.dArgs[2] = temp2->rot.z;
					 posPacket.dArgs[3] = temp2->scale;
					 sendToTCP(posPacket);
				 }
				 else  //not found error
				 {
					 sendTCPerror(298, "Model Not Found");
				 }
			 }
			 else if (m.command == 98)  // enable/disable boundedmovement , prevents API movement when movement isn't possible due to collision
			 {
				 boundedMovement = (m.iArg != 0);
			 }
			 else if (m.command == 99)  // enable/disable gravity , prevents API movement when movement isn't possible due to collision
			 {
				 bDisableGravity = (m.iArg == 0);

			 }
			 else if (m.command == 400)  // joystick movement
			 {
				 if (bJoyStickInput)
				 {
					 if (m.iArg == 0) //all axes
					 {
						 if (!isnan(m.dArgs[0]))
							objCamera.j.networkX = m.dArgs[0];
						 if (!isnan(m.dArgs[1]))
							objCamera.j.networkY = m.dArgs[1];
						 if (!isnan(m.dArgs[2]))
							objCamera.j.networkZ = m.dArgs[2];
						 if (!isnan(m.dArgs[3]))
							objCamera.j.networkR = m.dArgs[3];
					 }
					 else if (m.iArg == 1)
					 {
						 objCamera.j.networkX = m.dArgs[m.iArg - 1];
					 }
					 else if (m.iArg == 2)
					 {
						 objCamera.j.networkY = m.dArgs[m.iArg - 1];
					 }
					 else if (m.iArg == 3)
					 {
						 objCamera.j.networkZ = m.dArgs[m.iArg - 1];
					 }
					 else if (m.iArg == 4)
					 {
						 objCamera.j.networkR = m.dArgs[m.iArg - 1];
					 }
					 else if (m.iArg == -1) //clear
					 {
						 objCamera.j.networkX = -1;
						 objCamera.j.networkY = -1;
						 objCamera.j.networkZ = -1;
						 objCamera.j.networkR = -1;
					 }

					 if (objCamera.j.networkX > 65536)
						 objCamera.j.networkX = 65536;
					 else if (objCamera.j.networkX < 0)
						 objCamera.j.networkX = -1;

					 if (objCamera.j.networkY>65536)
						 objCamera.j.networkY = 65536;
					 else if (objCamera.j.networkY < 0)
						 objCamera.j.networkY = -1;

					 if (objCamera.j.networkZ>65536)
						 objCamera.j.networkZ = 65536;
					 else if (objCamera.j.networkZ < 0)
						 objCamera.j.networkZ = -1;

					 if (objCamera.j.networkR>65536)
						 objCamera.j.networkR = 65536;
					 else if (objCamera.j.networkR < 0)
						 objCamera.j.networkR = -1;
				 }
			 }
			 //device[devID].Send(targetScore);
		 }

		 if (!device[devID].unused&&connected)
		 {
			 char msg[256];
			 sprintf(msg, "Device %i Disconnected", devID + 1);
			 setDebugMessage(msg);
			 connected = false;
		 }
		 //EventLog(-1, -1, devID); //disconnect
		 device[devID].Disconnect();

	 }
	return 0;
}

//DWORD WINAPI comListenThread( LPVOID lpParam)
//{
//	/*int comValue=0;
//	while(bListen)
//	{		
//		comValue=bufferVar;
//		bufferVar=0;
//		if(comValue!=0 && plog!=NULL)
//			fprintf(plog,"%d\tEvent\t%d\t%d\t%d\r\n",GetQPC()-startTime,30,0,comValue);
//		returned=comValue;
//		if(waitForCue&&comValue==10)
//			mark=true;
//		Sleep(10);		
//	}
//	ComClose();*/
//	return 0;
//}






void sendToAll(int code,int val) //Sends code to COM port, sends val to devices
{
	if(code<=3&&code>0)
		ComSend(val);

	if(code>0)
	{
		sendToTCP(code, val);
	}

	if (code == 1) //LPT only gets system events
	{
		if (bUseLPT)
		{
			LPT_Send(val);
		}
	}
}

void sendToTCP(int code, int val)
{
	for (int x = 0; x<3; x++)
	{
		device[x].Send(code, val);
	}
}


void sendToTCP(tcpMessage t)
{
	for (int x=0;x<3;x++)
	{
		device[x].Send(t);
	}
}





 void loadIniSettings()
 {
		ControlLibrary[Forward]=GetPrivateProfileInt("Controls","Forward",W,mwalkerIniPath);
		ControlLibrary[Forward+Double]=GetPrivateProfileInt("Controls","Forward2",UP,mwalkerIniPath);
		ControlLibrary[Backward]=GetPrivateProfileInt("Controls","Backward",S,mwalkerIniPath);
		ControlLibrary[Backward+Double]=GetPrivateProfileInt("Controls","Backward2",DOWN,mwalkerIniPath);
		ControlLibrary[StrafeLeft]=GetPrivateProfileInt("Controls","StrafeLeft",A,mwalkerIniPath);
		ControlLibrary[StrafeLeft+Double]=GetPrivateProfileInt("Controls","StrafeLeft2",VK_ESCAPE,mwalkerIniPath);
		ControlLibrary[StrafeRight]=GetPrivateProfileInt("Controls","StrafeRight",D,mwalkerIniPath);
		ControlLibrary[StrafeRight+Double]=GetPrivateProfileInt("Controls","StrafeRight2",VK_ESCAPE,mwalkerIniPath);
		ControlLibrary[Run]=GetPrivateProfileInt("Controls","Run",LSHIFT,mwalkerIniPath);
		ControlLibrary[Crouch]=GetPrivateProfileInt("Controls","Crouch",CONTROL,mwalkerIniPath);
		ControlLibrary[LightToggle]=GetPrivateProfileInt("Controls","LightToggle",L,mwalkerIniPath);
		ControlLibrary[CrosshairToggle]=GetPrivateProfileInt("Controls","CrosshairToggle",C,mwalkerIniPath);
		ControlLibrary[BarToggle]=GetPrivateProfileInt("Controls","BarToggle",B,mwalkerIniPath);
		ControlLibrary[TimerToggle]=GetPrivateProfileInt("Controls","TimerToggle",T,mwalkerIniPath);
		ControlLibrary[Jump]=GetPrivateProfileInt("Controls","Jump",SPACE,mwalkerIniPath);
		ControlLibrary[LookLeft]=GetPrivateProfileInt("Controls","LookLeft",LEFT,mwalkerIniPath);
		ControlLibrary[LookLeft+Double]=GetPrivateProfileInt("Controls","LookLeft2",VK_ESCAPE,mwalkerIniPath);
		ControlLibrary[LookRight]=GetPrivateProfileInt("Controls","LookRight",RIGHT,mwalkerIniPath);
		ControlLibrary[LookRight+Double]=GetPrivateProfileInt("Controls","LookRight2",VK_ESCAPE,mwalkerIniPath);
		ControlLibrary[SpeedUp]=GetPrivateProfileInt("Controls","SpeedUp",ADD,mwalkerIniPath);
		ControlLibrary[SpeedDown]=GetPrivateProfileInt("Controls","SpeedDown",SUBTRACT,mwalkerIniPath);
		ControlLibrary[LookUp]=GetPrivateProfileInt("Controls","LookUp",VK_PRIOR,mwalkerIniPath);
		ControlLibrary[LookDown]=GetPrivateProfileInt("Controls","LookDown",VK_NEXT,mwalkerIniPath);
		ControlLibrary[Interact]=GetPrivateProfileInt("Controls","Interact",RETURN,mwalkerIniPath);
		ControlLibrary[RestartMaze] = GetPrivateProfileInt("Controls", "RestartMaze", F9, mwalkerIniPath);
		ControlLibrary[NextMaze] = GetPrivateProfileInt("Controls", "NextMaze", F10, mwalkerIniPath);

		bJoyStickInput=GetPrivateProfileInt("OtherSettings","JoyStickInput",0,mwalkerIniPath);
		bMouseInput=GetPrivateProfileInt("OtherSettings","MouseInput",1,mwalkerIniPath);
		joyCueButton=GetPrivateProfileInt("OtherSettings","joyCueButton",2,mwalkerIniPath);
		cueSkipButton=GetPrivateProfileInt("OtherSettings","cueSkipButton",1,mwalkerIniPath);
		cueType=GetPrivateProfileInt("OtherSettings","CueType",0,mwalkerIniPath);
		maxlights=GetPrivateProfileInt("ShaderSettings","MaxLights",8,mwalkerIniPath);
		ambientShader=GetPrivateProfileInt("ShaderSettings","AmbientMode",1,mwalkerIniPath);
		specularShader=GetPrivateProfileInt("ShaderSettings","Specular_Enabled",0,mwalkerIniPath);
		diffuseShader=GetPrivateProfileInt("ShaderSettings","DiffuseEnabled",1,mwalkerIniPath);
		texturesShader=GetPrivateProfileInt("ShaderSettings","RenderTextures",1,mwalkerIniPath);
		objectsShader=GetPrivateProfileInt("ShaderSettings","ObjectsWithShaders",1,mwalkerIniPath);
		useMSAA=GetPrivateProfileInt("GraphicsSettings","UseMSAA",1,mwalkerIniPath);
		useAniso=GetPrivateProfileInt("GraphicsSettings","UseAnisotropic",1,mwalkerIniPath);
		useVSYNC=GetPrivateProfileInt("GraphicsSettings","UseVSYNC",0,mwalkerIniPath);
		fieldOfView = GetPrivateProfileInt("ViewSettings", "FieldOfView", 45, mwalkerIniPath);

		objCamera.j.jMoveAxis=GetPrivateProfileInt("Joystick","jMoveAxis",2,mwalkerIniPath);
		objCamera.j.jStrafeAxis=GetPrivateProfileInt("Joystick","jStrafeAxis",0,mwalkerIniPath);
		objCamera.j.jLeftRightAxis=GetPrivateProfileInt("Joystick","jLeftRightAxis",1,mwalkerIniPath);
		objCamera.j.jUpDownAxis=GetPrivateProfileInt("Joystick","jUpDownAxis",0,mwalkerIniPath);

		objCamera.j.iMoveAxis=GetPrivateProfileInt("Joystick","invertMoveAxis",0,mwalkerIniPath);
		objCamera.j.iLookAxis=GetPrivateProfileInt("Joystick","invertLookAxis",0,mwalkerIniPath);
		objCamera.j.iUpDownAxis=GetPrivateProfileInt("Joystick","invertUpDownAxis",0,mwalkerIniPath);
		objCamera.j.iStrafeAxis=GetPrivateProfileInt("Joystick","invertStrafeAxis",0,mwalkerIniPath);

		objCamera.j.jumpButton=GetPrivateProfileInt("Joystick","jumpButton",1,mwalkerIniPath);
		objCamera.j.interactButton=GetPrivateProfileInt("Joystick","interactButton",2,mwalkerIniPath);

		objCamera.j.moveSensitivity=GetPrivateProfileInt("Joystick","moveSensitivity",10,mwalkerIniPath);
		objCamera.j.lookSensitivity=GetPrivateProfileInt("Joystick","lookSensitivity",10,mwalkerIniPath);
		objCamera.j.upDownSensitivity=GetPrivateProfileInt("Joystick","upDownSensitivity",10,mwalkerIniPath);
		objCamera.j.strafeSensitivity=GetPrivateProfileInt("Joystick","strafeSensitivity",10,mwalkerIniPath);
		joystickNum=GetPrivateProfileInt("Joystick","Number",0,mwalkerIniPath);
		deviceEnabled[0]=GetPrivateProfileInt("SensorSettings","Device1Enabled",0,mwalkerIniPath);
		deviceEnabled[1]=GetPrivateProfileInt("SensorSettings","Device2Enabled",0,mwalkerIniPath);
		deviceEnabled[2]=GetPrivateProfileInt("SensorSettings","Device3Enabled",0,mwalkerIniPath);
		bWaitForAPIdevices = GetPrivateProfileInt("SensorSettings", "WaitForAPIdevices", 1, mwalkerIniPath);
		deviceLocal[0]=GetPrivateProfileInt("SensorSettings","Device1Local",1,mwalkerIniPath);
		deviceLocal[1]=GetPrivateProfileInt("SensorSettings","Device2Local",1,mwalkerIniPath);
		deviceLocal[2]=GetPrivateProfileInt("SensorSettings","Device3Local",1,mwalkerIniPath);
		devicePort[0]=GetPrivateProfileInt("SensorSettings","Device1TCPPort",6350,mwalkerIniPath);
		devicePort[1]=GetPrivateProfileInt("SensorSettings","Device2TCPPort",6351,mwalkerIniPath);
		devicePort[2]=GetPrivateProfileInt("SensorSettings","Device3TCPPort",6352,mwalkerIniPath);
		deviceIP[0]=GetPrivateProfileInt("SensorSettings","Device1IP",2130706433,mwalkerIniPath);
		deviceIP[1]=GetPrivateProfileInt("SensorSettings","Device2IP",2130706433,mwalkerIniPath);
		deviceIP[2]=GetPrivateProfileInt("SensorSettings","Device3IP",2130706433,mwalkerIniPath);
		developerKeys=GetPrivateProfileInt("OtherSettings","DevKeysEnabled",0,mwalkerIniPath);
		devFPS=GetPrivateProfileInt("OtherSettings","DevFPSEnabled",0,mwalkerIniPath);
		hideRenderedMaze=GetPrivateProfileInt("OtherSettings", "HideRenderedMaze", 0, mwalkerIniPath);
		bDisableGravity = GetPrivateProfileInt("OtherSettings", "DisableGravity", 0, mwalkerIniPath);
		interactivePlayback=GetPrivateProfileInt("OtherSettings","InteractivePlayback",0,mwalkerIniPath);
		
		iScoreBarMode = GetPrivateProfileInt("OtherSettings", "ScoreBarMode", 0, mwalkerIniPath);
	//	oculusEnabled = GetPrivateProfileInt("OculusSettings", "OculusEnabled", 0, mwalkerIniPath);



		char* temp = new char[156];;
		sprintf(temp, "arialbd.ttf");
		GetPrivateProfileString("OtherSettings", "Font", temp, fontname, TXTBUFFERLIMIT, mwalkerIniPath);

		WritePrivateProfileString("OtherSettings", "Font", fontname, mwalkerIniPath);

		int dSetting = GetPrivateProfileInt("ViewSettings", "PerspectiveView", 0, mwalkerIniPath);
		if (dSetting == 1)
		{
			bTopDown = true;
			b34View = false;
		}
		else if (dSetting == 2)
		{
			bTopDown = false;
			b34View = true;
		}
		else
		{
			bTopDown = false;
			b34View = false;
		}

		iTopDownOrientation = GetPrivateProfileInt("ViewSettings", "TopDownOrientation", 0, mwalkerIniPath);
		bTopDownSimpleRender = GetPrivateProfileInt("ViewSettings", "TopDownLineRender", 0, mwalkerIniPath);
		bTopDownMouseAngleMode = GetPrivateProfileInt("ViewSettings", "TopDownMouseOrient", 0, mwalkerIniPath);
		iTopDownHeight = GetPrivateProfileInt("ViewSettings", "CameraHeight", 15, mwalkerIniPath);
		f34Angle = GetPrivateProfileInt("ViewSettings", "34ViewAngle", 45, mwalkerIniPath);
		bFixedCameraLocationX = GetPrivateProfileInt("ViewSettings", "bFixedCameraX", 0, mwalkerIniPath);
		bFixedCameraLocationZ = GetPrivateProfileInt("ViewSettings", "bFixedCameraZ", 0, mwalkerIniPath);
		fixedCameraLocationX = GetPrivateProfileInt("ViewSettings", "FixedCameraX", 10, mwalkerIniPath);
		fixedCameraLocationZ = GetPrivateProfileInt("ViewSettings", "FixedCameraZ", 10, mwalkerIniPath);
 }


 POINT GetOGLPos()
{
	POINT initMouse=POINT();						// Stores The X And Y Coords For The Current Mouse Position
	initMouse.x = 0;
	initMouse.y = 0;
	GetCursorPos(&initMouse);
	initMouse.x-=xLoc;
	initMouse.y-=yLoc;

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX=0, winY=0, winZ=0;
	GLdouble posX=0, posY=0, posZ=0;

	glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
	glGetDoublev( GL_PROJECTION_MATRIX, projection );
	glGetIntegerv( GL_VIEWPORT, viewport );

	winX = (float)initMouse.x;
	winY = (float)viewport[3] - (float)initMouse.y;

	
//	if (FLAG == true)
	//{ 
		//FLAG = false;

	//}
	//glReadPixels( initMouse.x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
	GLenum i = glGetError();
	gluUnProject( winX, winY, 0, modelview, projection, viewport, &posX, &posY, &posZ);
	initMouse.x=posX;
	initMouse.y=posY;

	return initMouse;
}

 /////BULLETPHYSICS

  DWORD lastUpdate;
  DWORD lastFallUpdate;
 double prevYpos=0;
 double prevDy=0;
 double prevProtoDy = 0;
 int dYindex = 0;
 //float dYs[50];
 bool dyInit = true;


 
 void StepBulletPhysics(double stepSize)
{
	
	DWORD dt=0;
	DWORD dt2=0;
	

	
	//dt=GetQPC()-lastUpdate;


	//if(dt<4)
	//{
	//	Sleep(2); //Cap framerate at 250
	//	dt=GetQPC()-lastUpdate;
	//}

	//dt2=GetQPC()-lastFallUpdate;
	
	//lastUpdate=GetQPC();
	//if(dt>5000)
	//{
	//	return;
	//}
		
	if (m_dynamicsWorld)//step the simulation
	{
		//float t = (float)dt / 1000.0f;

		tVector3 initPos = objCamera.mPos;

		btTransform characterWorldTrans = ghostObject->getWorldTransform();
		btVector3 initOrigin = characterWorldTrans.getOrigin();
		m_dynamicsWorld->stepSimulation(stepSize, 100,(double)1.0f/600.0f);
#ifdef MAZE_PROFILER
		lastStepSize = stepSize;
#endif

		characterWorldTrans = ghostObject->getWorldTransform();
		btVector3 origin = characterWorldTrans.getOrigin();

		float dx = objCamera.mPos.x - origin.getX();
		float dy = objCamera.mPos.y - origin.getY();
		float dz = objCamera.mPos.z - origin.getZ();

		//dYs[dYindex] = dy;
		//dYindex = dYindex + 1 - (dYindex >= 50) * 50;
		//float avgDy = 0;
		//for (int i = 0; i < 50; i++)	{ if (dyInit){ dYs[i] = 0; } avgDy = avgDy + dYs[i]; }
		//dyInit = false;

		//if (abs(dy) < 0.001)
		//{
		//	prevProtoDy = dy;
		//	dy = 0;
		//	origin.setY(objCamera.mPos.y);
		//	characterWorldTrans.setOrigin(origin);
		//	//specialVar2=0;
		//}
		//else if (abs(avgDy/50)<0.005)
		//{
		//	
		//	dy=0;
		//	origin.setY(objCamera.mPos.y);
		//	characterWorldTrans.setOrigin(origin);
		//	
		//}
		//else
		//{
		//	prevProtoDy = dy;
		//	falling = true;
		//	specialVar2 = specialVar2 + 1;
		//}
		//
		//if (sqrt(dx*dx + dy*dy + dz*dz) < 0.001)
		//{
		//	dx = 0; dy = 0; dz = 0;
		//	characterWorldTrans.setOrigin(initOrigin);
		//}


		objCamera.mPos.x-=dx;
		objCamera.mPos.y-=dy;
		objCamera.mPos.z-=dz;

		tVector3 changePos=tVector3(dx,dy,dz);
		objCamera.mView=objCamera.mView-changePos;


		dy=prevYpos-origin.getY(); //change since last frame

		////Sleep(10);
		//bool skip=false;
		//if(dt2>50)
		//{
		//	dy=dy/dt2;
		//	for(int i=dt2;i>=16;i-=16)
		//	{
		//		if(!skip&&Player.jumping&&Player.jumpSpeed!=0)
		//		{
		//			jumpSpeed=2/100.0f;
		//			if(Player.jumpSpeed>0)
		//				Player.jumpSpeed-=0.15;
		//			else
		//				Player.jumpSpeed=0;

		//			if(Player.jumpSpeed<=0.1)
		//			{
		//				fallSpeed=jumpSpeed+fallSpeed;
		//				Player.jumpSpeed=0;
		//			
		//			}
		//		}
		//		if(!skip&&dy>0.001||Player.jumping&&jumpSpeed+fallSpeed>0)
		//		{
		//			lastFallUpdate=GetQPC();
		//			if(fallSpeed>-1)
		//				fallSpeed-=0.0005; //seems to work , may be framerate dependent
		//			else
		//				fallSpeed=-1;
		//			falling=true;
		//		}
		//		else if(!skip&&falling&&(prevDy>0.001||dy>0.001))
		//		{
 	//				
		//			lastFallUpdate=GetQPC();
		//			if(fallSpeed>-1)
		//				fallSpeed-=0.0005; //seems to work , may be framerate dependent
		//			else
		//				fallSpeed=-1;
		//		}
		//		else
		//		{	
		//			falling=false;
		//			lastFallUpdate=GetQPC();
		//			fallSpeed=-0.01f;
		//			//if (controller->onGround())
		//			
		//			//prevDy=dy;

		//			skip=true;
		//			dyInit = true;
		//		}
		//		if(i<32)
		//		{
		//			prevDy=dy;
		//			prevYpos=objCamera.mPos.y;
		//			lastFallUpdate-=i-16;
		//		}
		//	
		//	}
		//}

		//
		//
	}
}



void InitBulletPhysics()
{

	m_collisionConfiguration = new btDefaultCollisionConfiguration(); //collision configuration contains default setup for memory, collision setup
	m_dispatcher			 = new btCollisionDispatcher(m_collisionConfiguration); //use the default collision dispatcher 
	m_broadphase		  	 = new btDbvtBroadphase();
	m_solver			 	 = new btSequentialImpulseConstraintSolver; //the default constraint solver
	m_dynamicsWorld			 = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0,-10,0));
	lastUpdate=GetQPC();

	//Build Triangle Mesh for World
	if(objMap->colMesh)
	{
		btBvhTriangleMeshShape* shape=new btBvhTriangleMeshShape(objMap->colMesh,false,true);
		
		btScalar mass = 0; //rigidbody is static if mass is zero, otherwise dynamic
		btVector3 localInertia(0,0,0);
		m_collisionShapes.push_back(shape);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0,0,0));
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform); //motionstate provides interpolation capabilities, and only synchronizes 'active' objects
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		body->setFriction(btScalar(10));
		body->setRestitution(btScalar(0));
		shape->setMargin(0.1);


		m_dynamicsWorld->addRigidBody(body); //add the body to the dynamics world

		//Add objects
		MapModel *temp2;
		temp2=MapModelHead;
		while(temp2!=NULL)
		{
			if(!temp2->collide)
			{
				temp2=temp2->next;
				continue;
			}
			//btBvhTriangleMeshShape* shape=new btBvhTriangleMeshShape(temp2->colMesh,true,true);
			float objScale=2.0f;
			btBoxShape* shape = new btBoxShape(btVector3((temp2->MaxCoords.x-temp2->MinCoords.x)*temp2->scale/objScale,(temp2->MaxCoords.y-temp2->MinCoords.y)*temp2->scale/objScale,(temp2->MaxCoords.z-temp2->MinCoords.z)*temp2->scale/objScale));

			btScalar mass = temp2->mass;//temp2->roughRadius*temp2->roughRadius*4+4; //rigidbody is static if mass is zero, otherwise dynamic
			
			btVector3 localInertia(0,0,0);

			shape->calculateLocalInertia(mass,localInertia);
			m_collisionShapes.push_back(shape);
			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setRotation(btQuaternion(PI/180*temp2->rot.y,PI/180*temp2->rot.x,PI/180*temp2->rot.z));
			groundTransform.setOrigin(btVector3(temp2->pos.x,temp2->pos.y,temp2->pos.z));
			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform); //motionstate provides interpolation capabilities, and only synchronizes 'active' objects
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,shape,localInertia);
			
			temp2->body = new btRigidBody(rbInfo);
			temp2->body->setCenterOfMassTransform(groundTransform);
			temp2->body->setFriction(btScalar(10));
			temp2->body->setRestitution(btScalar(0));
			temp2->body->setDamping(0.3,0.3);
			temp2->body->setCcdMotionThreshold(1);

			temp2->body->setSleepingThresholds(0.1,0.1);

			

			m_dynamicsWorld->addRigidBody(temp2->body); //add the body to the dynamics world
			temp2=temp2->next;
		}
		


	
		//Build Player
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin (btVector3(objCamera.startPos.x,objCamera.startPos.y,objCamera.startPos.z));

		
		//btCylinderShape* playerShape= new btCylinderShape(btVector3(0.35,0.5,0.35));
		btCapsuleShape* playerShape = new btCapsuleShape(0.35, 0.5);
		
		ghostObject= new btPairCachingGhostObject();
		m_dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		ghostObject->setCollisionShape(playerShape);
		ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
		ghostObject->setWorldTransform(startTransform);
		
		controller = new btKinematicCharacterController(ghostObject,playerShape,0.3);

		m_dynamicsWorld->addCollisionObject(ghostObject,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);
		m_dynamicsWorld->addAction(controller);
		const btVector3 grav = btVector3(0, -10, 0);
		controller->setGravity(grav);
	}

}

void ExitBulletPhysics()
{
	//cleanup in the reverse order of creation/initialization
	if(m_dynamicsWorld)
	{
		for (int i=m_dynamicsWorld->getNumCollisionObjects()-1; i>=0; i--) //remove the rigidbodies from the dynamics world and delete them
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			if(body && body->getMotionState())
				delete body->getMotionState();
		
			m_dynamicsWorld->removeCollisionObject(obj);
			delete obj;
		}

	
		for (int j=0; j<m_collisionShapes.size(); j++) //delete collision shapes
		{
			btCollisionShape* shape = m_collisionShapes[j];
			delete shape;
		}
	
		m_collisionShapes.clear();
		delete m_dynamicsWorld;
		delete m_solver;
		delete m_broadphase;
		delete m_dispatcher;
		delete m_collisionConfiguration;
		delete controller;
		
	}
}

void AlignCamera()
{
	if(m_dynamicsWorld)
	{
		btTransform startTransform;
		startTransform.setIdentity ();
		startTransform.setOrigin (btVector3(objCamera.mPos.x,objCamera.mPos.y,objCamera.mPos.z));
		ghostObject->setWorldTransform(startTransform);
	}
}


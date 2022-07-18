
#pragma once


#define MO_TEXTURE		-1
#define MO_INITIALPOS	-2
#define MO_ENDPOS		-3
#define MO_TIMELIMIT	-4
#define MO_DESIGNER		-5
#define MO_COMMENTS		-6
#define MO_SCALE		-7
#define MO_SPEED		-8
#define MO_LIGHT		-9
#define MO_MODEL_LIST	-10
#define MO_AUDIO_LIST	-11
#define MO_AMBIENT		-20
#define MO_STARTMESSAGE		-21
#define MO_SKYBOX		-22

#define MO_PLANE		0
#define MO_TRI			1
#define MO_SPHERE		2
#define MO_OBJECT		3
#define MO_MODEL		10
#define MO_DYNAMICMODEL	11

#define NONE		-1
#define X_WISE		0
#define Z_WISE		1

#define XZ_WISE		2
#include <list>

#include <btBulletDynamicsCommon.h>

struct SColor{
	float r,g,b;
};
struct SVertex{
	float x,y,z;
	float tex_x,tex_y;
};
struct SRect{
	float xmin,xmax,ymin,ymax;
};


struct SPlane{
	int pID; //planeID
	int type;
	int texture;
	struct SColor col;
	struct SVertex vertex[4];
	float normal[3];
	struct SPlane *next;
	int direction;
	//float max,min;
	//for collision detection
	float coef; //sqrt(A^2+B^2+C^2+D^2)
	float A,B,C,D;
	float zmin,zmax,xmin,xmax;
	float m; //slope in xz plane...
	float distToPlayer;
	int transparent;
	int visible;
	int polyOffset;
	//GLfloat* vertices;
};

struct SPlanveVBO:SPlane {
	//GLfloat* vertices;
};

enum thresholdOperator
{
	mGreaterThan = 2,
	mGreaterThanEqual = 1,
	mEqual = 0,
	mLessThanEqual = -1,
	mLessThan = -2,
	mNotEqual = 100
};


struct EndRegion{
	float zmin,zmax,xmin,xmax,height,offset;
	char label[200];
	int returnValue;
	EndRegion *next;
	int mode;
	float distToPlayer;
	char messageText[500];
	bool last;
	int index; //identity of end region
	int pointThreshold; //pointsRequired to Activete
	thresholdOperator pointThresholdOperator = mGreaterThanEqual;
	int moveToPos;//index of StartPosition to move to
};

struct ActiveRegion: EndRegion{
	ActiveRegion *next;
	char messageText[500];
	int p1PointThreshold,p2PointThreshold; //pointsRequired to Highlight & Activate

	thresholdOperator p1PointOperator = mGreaterThanEqual;
	thresholdOperator p2PointOperator = mGreaterThanEqual;

	int moveToPos;//index of StartPosition to move to

	DWORD highlightTime;

	bool activated;
	bool highlighted;
	bool p2InteractRequired;

	bool activationRepeatable = false;

	int triggerAction;	//Trigger action type stored as int  : 0=No action 1=Move/Scale/Rotate 2=Change Model 3=Destroy Model

	int pointsGranted =0;
	bool pointsGrantedSetPoints = false; // if not set points, add points

	int triggerAudioID;
	int highlightAudioID;

	float p1TriggerTime; //time until automatically highlighted
	float p2TriggerTime; //time until automatically triggered from P1

	thresholdOperator p1TimeOperator = mGreaterThanEqual;
	thresholdOperator p2TimeOperator = mGreaterThanEqual;
	
	int triggerAudioLoop; //triggered audio is looped until end of maze
	int triggerAudioBehavior; //not defined
	int highlightAudioLoop; //highlight audio is looped
	int highlightAudioBehavior; //what happens after you leave highlighted proximity   0=stop audio, 1=pause audio, 2=continue playing

	int activatedObjectID;

	

	void Activate();
	void CalcDistance(float x, float y, float z);
	void Highlight(bool highlight);
};

struct StartPosition{
	float x,y,z;
	int inputValue;
	float angle;
	float vertAngle;
	StartPosition *next;
	bool isDeafultStartPos;
	bool last;
	int index;//index for start position (not used)
};


class Map{

	struct SPlane *root;
	

public:
	Map();
	~Map();
	int Add(int type,int texture,struct SColor color,struct SVertex v[4],int curVisiblity);
	void Draw();
	void DrawSimple();
	void DrawQuery();
	void SortWalls();
	void Count();
	void WaitForReady();
	int CheckCollision(float* x,float* z,float* y);
	int CalcDistance(struct SPlane *inp, float x,float z,float y,float *outDistance);
	float CalcDistanceToPoint(float fromx, float fromz, float tox, float toz);
	float CalcDistanceToPoint(float fromx, float fromy, float fromz, float tox, float toy, float toz);
	int SetDirectionInfo(struct SPlane* inp);
	int CalcCoefs(struct SPlane *inp);

	int numObjs;
	int wallCount;
	int modelCount;
	int dModelCount;
	int modelsDrawn;
	int wallsDrawn;

	bool bEndPos;
	bool bStartPos;
	EndRegion* endRegionHead=NULL;
	ActiveRegion* activeRegionHead = NULL;
	StartPosition* startPosHead=NULL;
	StartPosition* startPosDefault = NULL;
	//struct SRect end;
	DWORD maxMazeTime;
	bool lights;
	bool shaders;
	bool shadersCompiled;
	int pointExitThreshold;
	thresholdOperator pointExitThresholdOperator;
	bool pointMessageEnabled;
	char pointMessage[500];
	char timeoutMessage[500];
	char startMessage[500];
	bool startMessageEnabled;
	char successMessage[500];
	bool IsGettingCloser(float distNew, float distOriginal);
	int countCollidingPlane(SPlane* temp);
	btTriangleMesh* colMesh;
	btCollisionShape* staticBVHtree;
	char *fname;

};


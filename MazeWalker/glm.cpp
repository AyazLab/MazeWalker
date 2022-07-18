/*    
      glm.c
      Nate Robins, 1997, 2000
      nate@pobox.com, http://www.pobox.com/~nate
 
      Wavefront OBJ model file format reader/writer/manipulator.

      Includes routines for generating smooth normals with
      preservation of edges, welding redundant vertices & texture
      coordinate generation (spheremap and planar projections) + more.
	
      Improved version of GLM - 08.05.2008 Tudor Carean
	  Added support for textures and loading callbacks
*/

//#include <stdafx.h>
#include <stdio.h>
#include <tchar.h>
#include <math.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <assert.h>
#include "Log.h"
#include "GL/glew.h"
#include "glm.h"
#include "main.h"
extern volatile int curMazePoints;
#define TEXTBOXSTYLE_WARNING 3

extern GLuint* whiteID;
extern AudioDictionary curAudioDict; //defined in main.cpp

void MapModel::Activate()
{
	if(!activated&&dynamic) //prevent activation of static or already activated objects
	{
		activated=true;
		Highlight(false);	//unhighlight
	
		curAudioDict.Play(triggerAudioID,triggerAudioLoop);  //play assigned audio

		EventLog(3,202,instanceID,name); //log action (COM as well)

		switch(triggerAction)	//depending on trigger action
		{
			case 1:
				moving=true;
				break;
			case 2:
				switchModel=true;
				break;
			case 3:
				destroy=true;
				break;
		}

        if (pointsGrantedSetPoints) {
            curMazePoints = pointsGranted;
        }
        else {
            curMazePoints += pointsGranted;
        }

		triggerCriteria=0; //prevents double activation and checks
		triggerAction=0;
	}

}

void MapModel::Highlight(bool highlight)
{
	if(dynamic&&(highlight!=highlighted||activated)) //to prevent double logging/actions
	{
		highlighted=highlight;
		if(highlight) //highlight
		{
			highlightTime = GetQPC(); //get-time...
			EventLog(3,201,instanceID,name);
			curAudioDict.Play(highlightAudioID,highlightAudioLoop);
		}
		else if(!activated) //unhighlight
		{
			if(this->highlightAudioBehavior==0) // stop audio immediately
				curAudioDict.Stop(highlightAudioID);
			else if(highlightAudioBehavior==1) //pause audio
				curAudioDict.Pause(highlightAudioID,true);
			else if(highlightAudioBehavior==2) // let it finish
			{

			}
			else
				curAudioDict.Stop(highlightAudioID);
			EventLog(3, 200, instanceID, name);
		}
		else //if activated 
		{
			curAudioDict.Stop(highlightAudioID);
			highlighted=false;
		}
	}
}


#define _MODEL_PROFILER

#ifdef _MODEL_PROFILER

FILE *modelDebugFile;

#endif


//#define DebugVisibleSurfaces

#define total_textures 5

#define T(x) (model->triangles[(x)])
GLuint glmLoadTexture(char *filename, GLboolean alpha, GLboolean repeat, GLboolean filtering, GLboolean mipmaps, GLfloat *texcoordwidth, GLfloat *texcoordheight);

/* _GLMnode: general purpose node */
typedef struct _GLMnode {
    GLuint         index;
    GLboolean      averaged;
    struct _GLMnode* next;
} GLMnode;


/* glmMax: returns the maximum of two floats */
static GLfloat
glmMax(GLfloat a, GLfloat b) 
{
    if (b > a)
        return b;
    return a;
}

/* glmAbs: returns the absolute value of a float */
static GLfloat
glmAbs(GLfloat f)
{
    if (f < 0)
        return -f;
    return f;
}

/* glmDot: compute the dot product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 */
GLfloat glmDot(GLfloat* u, GLfloat* v)
{
    assert(u); assert(v);
    
    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

GLvoid
glmTranslate(GLMmodel* model)
{
    GLuint i;
    
    for (i = 1; i <= model->numvertices; i++) {
        model->vertices[3 * i + 0] += model->pos.x;
        model->vertices[3 * i + 1] += model->pos.y;
        model->vertices[3 * i + 2] += model->pos.z;
    }
}

/* glmCross: compute the cross product of two vectors
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3])
 * n - array of 3 GLfloats (GLfloat n[3]) to return the cross product in
 */
static GLvoid
glmCross(GLfloat* u, GLfloat* v, GLfloat* n)
{
    assert(u); assert(v); assert(n);
    
    n[0] = u[1]*v[2] - u[2]*v[1];
    n[1] = u[2]*v[0] - u[0]*v[2];
    n[2] = u[0]*v[1] - u[1]*v[0];
}

/* glmNormalize: normalize a vector
 *
 * v - array of 3 GLfloats (GLfloat v[3]) to be normalized
 */
static GLvoid
glmNormalize(GLfloat* v)
{
    GLfloat l;
    
    assert(v);
    
    l = (GLfloat)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= l;
    v[1] /= l;
    v[2] /= l;
}

/* glmEqual: compares two vectors and returns GL_TRUE if they are
 * equal (within a certain threshold) or GL_FALSE if not. An epsilon
 * that works fairly well is 0.000001.
 *
 * u - array of 3 GLfloats (GLfloat u[3])
 * v - array of 3 GLfloats (GLfloat v[3]) 
 */
static GLboolean
glmEqual(GLfloat* u, GLfloat* v, GLfloat epsilon)
{
    if (glmAbs(u[0] - v[0]) < epsilon &&
        glmAbs(u[1] - v[1]) < epsilon &&
        glmAbs(u[2] - v[2]) < epsilon) 
    {
        return GL_TRUE;
    }
    return GL_FALSE;
}

/* glmWeldVectors: eliminate (weld) vectors that are within an
 * epsilon of each other.
 *
 * vectors     - array of GLfloat[3]'s to be welded
 * numvectors - number of GLfloat[3]'s in vectors
 * epsilon     - maximum difference between vectors 
 *
 */
GLfloat*
glmWeldVectors(GLfloat* vectors, GLuint* numvectors, GLfloat epsilon)
{
    GLfloat* copies;
    GLuint   copied;
    GLuint   i, j;
    
    copies = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (*numvectors + 1));
    memcpy(copies, vectors, (sizeof(GLfloat) * 3 * (*numvectors + 1)));
    
    copied = 1;
    for (i = 1; i <= *numvectors; i++) {
        for (j = 1; j <= copied; j++) {
            if (glmEqual(&vectors[3 * i], &copies[3 * j], epsilon)) {
                goto duplicate;
            }
        }
        
        /* must not be any duplicates -- add to the copies array */
        copies[3 * copied + 0] = vectors[3 * i + 0];
        copies[3 * copied + 1] = vectors[3 * i + 1];
        copies[3 * copied + 2] = vectors[3 * i + 2];
        j = copied;             /* pass this along for below */
        copied++;
        
duplicate:
/* set the first component of this vector to point at the correct
        index into the new copies array */
        vectors[3 * i + 0] = (GLfloat)j;
    }
    
    *numvectors = copied-1;
    return copies;
}

/* glmFindGroup: Find a group in the model */
GLMgroup*
glmFindGroup(GLMmodel* model, char* name)
{
    GLMgroup* group;
    
    assert(model);
    
    group = model->groups;
    while(group) {
        if (!strcmp(name, group->name))			
            break;
        group = group->next;
    }
    
    return group;
}

/* glmAddGroup: Add a group to the model */
GLMgroup*
glmAddGroup(GLMmodel* model, char* name)
{
    GLMgroup* group;
    
    group = glmFindGroup(model, name);
    if (!group) {
        group = (GLMgroup*)malloc(sizeof(GLMgroup));
        group->name = _strdup(name);
        group->material = 0;
        group->numtriangles = 0;
        group->triangles = NULL;
        group->next = model->groups;
        model->groups = group;
        model->numgroups++;
    }
    
    return group;
}

/* glmFindGroup: Find a material in the model */
GLuint
glmFindMaterial(GLMmodel* model, char* name)
{
    GLuint i;
	char modelCh[256] = "";
	char nameCh[256] = "";

	sprintf(nameCh,"%s", name);
	
    /* XXX doing a linear search on a string key'd list is pretty lame,
    but it works and is fast enough for now. */
    for (i = 0; i < model->nummaterials; i++) {
		sprintf(modelCh, "%s", model->materials[i].name);
        if (strcmp(nameCh, modelCh)==0)
            return i;
    }
    
    /* didn't find the name, so print a warning and return the default
    material (0). */
    printf("glmFindMaterial():  can't find material \"%s\".\n", name);
    i = 0;
    

    return i;
}
/* glmDirName: return the directory given a path
 *
 * path - filesystem path
 *
 * NOTE: the return value should be free'd.
 */
static char*
glmDirName(char* path)
{
    char* dir;
    char* s;
    
    dir = _strdup(path);
    
    s = strrchr(dir, '/');
    if (s)
        s[1] = '\0';
    else
        dir[0] = '\0';
    
    return dir;
}

int glmFindOrAddTexture(GLMmodel* model, char* name,mycallback *call)
{

    GLuint i;
    char *dir, *filename;
    float width, height;

    char *numefis = name;
    while (*numefis==' ') numefis++;

    for (i = 0; i < model->numtextures; i++) {
        if (!strcmp(model->textures[i].name, numefis))
            return i;
    }
	char afis[80];
	sprintf(afis,"Loading Textures (%s )...",name);
	if (call){
	int procent = ((float)(((float)model->numtextures)*30/total_textures)/100)*(call->end-call->start)+call->start;
	 call->loadcallback(procent,afis); // textures represent 30% from the model (just saying :))
	}
	if (strstr(name,":\\"))
	{
		filename = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(filename,name);		
	}
	else
	{
		dir = glmDirName(model->pathname);
		filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(numefis) + 1));
	
		strcpy(filename, dir);
		//strcpy(filename, numefis);
		strcat(filename, numefis);
		free(dir);
	}
	int lung = (int) strlen(filename);
	if (filename[lung-1]<32) filename[lung-1]=0;
	if (filename[lung-2]<32) filename[lung-2]=0;    

    model->numtextures++;
    model->textures = (GLMtexture*)realloc(model->textures, sizeof(GLMtexture)*model->numtextures);
    model->textures[model->numtextures-1].name = _strdup(numefis);
    model->textures[model->numtextures-1].id = glmLoadTexture(filename, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, &width, &height);

	if(model->textures[model->numtextures-1].id==-1) // if fails to load use white texture instead
	{
		model->textures[model->numtextures-1].id=whiteID[0];
		model->textures[model->numtextures-1].width = 1;
		model->textures[model->numtextures-1].height = 1;
	}

    model->textures[model->numtextures-1].width = width;
    model->textures[model->numtextures-1].height = height;
    

    free(filename);

    return model->numtextures-1;
}




/* glmReadMTL: read a wavefront material library file
 *
 * model - properly initialized GLMmodel structure
 * name  - name of the material library
 */
static GLvoid
glmReadMTL(GLMmodel* model, char* name, mycallback *call)
{
    FILE* MTLfile;
    char* dir;
    char* filename;
	char* Texture;
    char    buf[128];
    GLuint nummaterials, i;

	
    dir = glmDirName(model->pathname);
    filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(name) + 1));
    strcpy(filename, dir);
    strcat(filename, name);
    free(dir);
    
	MTLfile = fopen(filename, "r");
	if (!MTLfile) {
		char message[500];
		sprintf(message,"Material file not found:\n%s",filename);
		GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
		return;
		//exit(1);
    }
    free(filename);
    
    /* count the number of materials in the file */
    nummaterials = 1;
	while (fscanf(MTLfile, "%s", buf) != EOF) {
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
			fgets(buf, sizeof(buf), MTLfile);
            break;
        case 'n':               /* newmtl */
			fgets(buf, sizeof(buf), MTLfile);
            nummaterials++;
            sscanf(buf, "%s %s", buf, buf);
            break;
        default:
            /* eat up rest of line */
			fgets(buf, sizeof(buf), MTLfile);
            break;
        }
    }
    
	rewind(MTLfile);
    
    model->materials = (GLMmaterial*)malloc(sizeof(GLMmaterial) * nummaterials);
    model->nummaterials = nummaterials;
    
    /* set the default material */
    for (i = 0; i < nummaterials; i++) {
        model->materials[i].name = NULL;
        model->materials[i].shininess = 65.0;
        model->materials[i].diffuse[0] = 0.8;
        model->materials[i].diffuse[1] = 0.8;
        model->materials[i].diffuse[2] = 0.8;
        model->materials[i].diffuse[3] = 1.0;
        model->materials[i].ambient[0] = 0.2;
        model->materials[i].ambient[1] = 0.2;
        model->materials[i].ambient[2] = 0.2;
        model->materials[i].ambient[3] = 1.0;
        model->materials[i].specular[0] = 0.0;
        model->materials[i].specular[1] = 0.0;
        model->materials[i].specular[2] = 0.0;
        model->materials[i].specular[3] = 1.0;
		model->materials[i].emmissive[0] = 0.0;
        model->materials[i].emmissive[1] = 0.0;
        model->materials[i].emmissive[2] = 0.0;
        model->materials[i].emmissive[3] = 0.5;
		model->materials[i].IDTexture = -1;
		model->materials[i].opacity=1;
		model->transparent=false;
    }
    model->materials[0].name = _strdup("default");
   
    /* now, read in the data */
    nummaterials = 0;
	while (fscanf(MTLfile, "%s", buf) != EOF) {
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
			fgets(buf, sizeof(buf), MTLfile);
            break;
        case 'n':               /* newmtl */
			
			fgets(buf, sizeof(buf), MTLfile);
            sscanf(buf, "%s %s", buf, buf);
            nummaterials++;
			model->materials[nummaterials].IDTexture = -1;
            model->materials[nummaterials].name = _strdup(buf);
            break;
        case 'N':
            if (buf[1]!='s') break; // 3DS pune 'i' aici pentru indici de refractie si se incurca
			fscanf(MTLfile, "%f", &model->materials[nummaterials].shininess);
            /* wavefront shininess is from [0, 1000], so scale for OpenGL */
            model->materials[nummaterials].shininess /= 1000.0;
            model->materials[nummaterials].shininess *= 128.0;
            break;
		case 'd':
			fscanf(MTLfile, "%f", &model->materials[nummaterials].opacity);
            if (model->materials[nummaterials].opacity>1)
				model->materials[nummaterials].opacity=1;
			if (model->materials[nummaterials].opacity<0)
				model->materials[nummaterials].opacity=0;
			if(model->materials[nummaterials].opacity<1)
				model->transparent=true;
            break;
        case 'K':
            switch(buf[1]) {
            case 'd':
				fscanf(MTLfile, "%f %f %f",
                    &model->materials[nummaterials].diffuse[0],
                    &model->materials[nummaterials].diffuse[1],
                    &model->materials[nummaterials].diffuse[2]);
                break;
            case 's':
				fscanf(MTLfile, "%f %f %f",
                    &model->materials[nummaterials].specular[0],
                    &model->materials[nummaterials].specular[1],
                    &model->materials[nummaterials].specular[2]);
                break;
            case 'a':
				fscanf(MTLfile, "%f %f %f",
                    &model->materials[nummaterials].ambient[0],
                    &model->materials[nummaterials].ambient[1],
                    &model->materials[nummaterials].ambient[2]);
                break;
            default:
                /* eat up rest of line */
				fgets(buf, sizeof(buf), MTLfile);
                break;
            }
            break;
             case 'm':
            
            filename = (char *)malloc(FILENAME_MAX);
			fgets(filename, FILENAME_MAX, MTLfile);
            Texture = _strdup(filename);
            free(filename);
            if(strncmp(buf, "map_Kd", 6) == 0) 
			{
				char afis[80];
				sprintf(afis,"Loading Textures (%s)...",Texture);				
                model->materials[nummaterials].IDTexture = glmFindOrAddTexture(model, Texture,call);
                free(Texture);
            } else {
                //printf("map %s %s ignored",buf,t_filename);
                free(Texture);
                //fgets(buf, sizeof(buf), file);
            }
            break;
            default:
                /* eat up rest of line */
				fgets(buf, sizeof(buf), MTLfile);
                break;
        }
    }
}

/* glmWriteMTL: write a wavefront material library file
 *
 * model   - properly initialized GLMmodel structure
 * modelpath  - pathname of the model being written
 * mtllibname - name of the material library to be written
 */
static GLvoid
glmWriteMTL(GLMmodel* model, char* modelpath, char* mtllibname)
{
    FILE* file;
    char* dir;
    char* filename;
    GLMmaterial* material;
    GLuint i;
    
    dir = glmDirName(modelpath);
    filename = (char*)malloc(sizeof(char) * (strlen(dir)+strlen(mtllibname)));
    strcpy(filename, dir);
    strcat(filename, mtllibname);
    free(dir);
    
    /* open the file */
    file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "glmWriteMTL() failed: can't open file \"%s\".\n",
            filename);
        exit(1);
    }
    free(filename);
    
    /* spit out a header */
    fprintf(file, "#  \n");
    fprintf(file, "#  Wavefront MTL generated by GLM library\n");
    fprintf(file, "#  \n");
    fprintf(file, "#  GLM library\n");
    fprintf(file, "#  Nate Robins\n");
    fprintf(file, "#  ndr@pobox.com\n");
    fprintf(file, "#  http://www.pobox.com/~ndr\n");
    fprintf(file, "#  \n\n");
    
    for (i = 0; i < model->nummaterials; i++) {
        material = &model->materials[i];
        fprintf(file, "newmtl %s\n", material->name);
        fprintf(file, "Ka %f %f %f\n", 
            material->ambient[0], material->ambient[1], material->ambient[2]);
        fprintf(file, "Kd %f %f %f\n", 
            material->diffuse[0], material->diffuse[1], material->diffuse[2]);
        fprintf(file, "Ks %f %f %f\n", 
            material->specular[0],material->specular[1],material->specular[2]);
        fprintf(file, "Ns %f\n", material->shininess / 128.0 * 1000.0);
        fprintf(file, "\n");
    }
}


/* glmFirstPass: first pass at a Wavefront OBJ file that gets all the
 * statistics of the model (such as #vertices, #normals, etc)
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor 
 */
static GLvoid glmFirstPass(GLMmodel* model, FILE* file, mycallback *call) 
{
    GLuint  numvertices;        /* number of vertices in model */
    GLuint  numnormals;         /* number of normals in model */
    GLuint  numtexcoords;       /* number of texcoords in model */
    GLuint  numtriangles;       /* number of triangles in model */
    GLMgroup* group;            /* current group */
    unsigned    v, n, t;
    char        buf[128];
    
    /* make a default group */
    group = glmAddGroup(model, "default");
    bool newGroup=true;
	int autoGroup=0;
    numvertices = numnormals = numtexcoords = numtriangles = 0;
	DWORD x3;
	DWORD x4;
	x3=GetQPC();
    while(fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        case 'v':               /* v, vn, vt */
            switch(buf[1]) {
            case '\0':          /* vertex */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                numvertices++;
                break;
            case 'n':           /* normal */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                numnormals++;
                break;
            case 't':           /* texcoord */
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                numtexcoords++;
                break;
            default:
				char message[500];
				sprintf(message, "Model Error\nglmFirstPass(): Unknown token \"%s\".\n", buf);
				GUIMessageBox(message, 0, TEXTBOXSTYLE_WARNING);
				return;
                break;
            }
            break;
            case 'm': //mtllib
                fgets(buf, sizeof(buf), file);
				model->mtllibname = _strdup(buf);
				trim(buf);
				glmReadMTL(model, buf, call);
				break;
            case 'u': //usemtl
                /* eat up rest of line */
				
				if (newGroup)
				{
					
					autoGroup++;
					char name[50];
					sprintf(name,"AutoGroup%d",autoGroup);
					group = glmAddGroup(model, name);
				}
				else
					newGroup=true;
                fgets(buf, sizeof(buf), file);
                break;
            case 'g':               /* group */
                /* eat up rest of line */
				newGroup=false;
                fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
                sscanf_s(buf, "%s", buf);
#else
                buf[strlen(buf)-1] = '\0';  /* nuke '\n' */
#endif
                group = glmAddGroup(model, buf);
                break;
            case 'f':               /* face */
                v = n = t = 0;
                fscanf(file, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d//%d", &v, &n) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                } else if (sscanf_s(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                } else if (sscanf_s(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */
                    fscanf(file, "%d/%d", &v, &t);
                    fscanf(file, "%d/%d", &v, &t);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d/%d", &v, &t) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                } else {
                    /* v */
                    fscanf(file, "%d", &v);
                    fscanf(file, "%d", &v);
                    numtriangles++;
                    group->numtriangles++;
                    while(fscanf(file, "%d", &v) > 0) {
                        numtriangles++;
                        group->numtriangles++;
                    }
                }
                break;
                
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
        }
		
  }
  x4=GetQPC()-x3;
  /* set the stats in the model structure */
  model->numvertices  = numvertices;
  model->numnormals   = numnormals;
  model->numtexcoords = numtexcoords;
  model->numtriangles = numtriangles;
  
  /* allocate memory for the triangles in each group */
  group = model->groups;
  while(group) {
      group->triangles = (GLuint*)malloc(sizeof(GLuint) * group->numtriangles);
      group->numtriangles = 0;
      group = group->next;
  }
}

/* glmSecondPass: second pass at a Wavefront OBJ file that gets all
 * the data.
 *
 * model - properly initialized GLMmodel structure
 * file  - (fopen'd) file descriptor 
 */
static GLvoid glmSecondPass(GLMmodel* model, FILE* file, mycallback *call) 
{
    GLuint  numvertices;        /* number of vertices in model */
    GLuint  numnormals;         /* number of normals in model */
    GLuint  numtexcoords;       /* number of texcoords in model */
    GLuint  numtriangles;       /* number of triangles in model */
    GLfloat*    vertices;           /* array of vertices  */
    GLfloat*    normals;            /* array of normals */
    GLfloat*    texcoords;          /* array of texture coordinates */
    GLMgroup* group;            /* current group pointer */
    GLuint  material;           /* current material */
    GLuint  v, n, t;
    char        buf[128];
	char afis[80];
    
    /* set the pointer shortcuts */
    vertices       = model->vertices;
    normals    = model->normals;
    texcoords    = model->texcoords;
    group      = model->groups;

    /* on the second pass through the file, read all the data into the
    allocated arrays */
    numvertices = numnormals = numtexcoords = 1;
    numtriangles = 0;
    material = 0;
	int autoGroup=0;
	bool newGroup=true;
	int retValue=0;

    while(fscanf(file, "%s", buf) != EOF) {
		
        switch(buf[0]) {
        case '#':               /* comment */
            /* eat up rest of line */
            fgets(buf, sizeof(buf), file);
            break;
        case 'v':               /* v, vn, vt */
            switch(buf[1]) {
            case '\0':          /* vertex */
                fscanf(file, "%f %f %f", 
                    &vertices[3 * numvertices + 0], 
                    &vertices[3 * numvertices + 1], 
                    &vertices[3 * numvertices + 2]);
                numvertices++;
				if (numvertices%200==0)
					if (call)
					{
						sprintf(afis,"%s (%s )... ",call->text, group->name);
						int procent = ((float)((float)numvertices*70/model->numvertices+30)/100)*(call->end-call->start)+call->start;
						call->loadcallback(procent,afis);
					}
                break;
            case 'n':           /* normal */
                fscanf(file, "%f %f %f", 
                    &normals[3 * numnormals + 0],
                    &normals[3 * numnormals + 1], 
                    &normals[3 * numnormals + 2]);
                numnormals++;
                break;
            case 't':           /* texcoord */
                fscanf(file, "%f %f", 
                    &texcoords[2 * numtexcoords + 0],
                    &texcoords[2 * numtexcoords + 1]);
                numtexcoords++;
                break;
            }
            break;
            case 'u':
				if(newGroup)
				{
					autoGroup++;
					char name[50];
					sprintf(name,"AutoGroup%d",autoGroup);
					group=glmFindGroup(model,name);
				}
                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                material = glmFindMaterial(model, buf);
				group->material=material;
				group->usetexture=true;
				newGroup=true;
                break;
            case 'g':               /* group */
                /* eat up rest of line */
				newGroup=false;
                fgets(buf, sizeof(buf), file);
#if SINGLE_STRING_GROUP_NAMES
                sscanf_s(buf, "%s", buf);
#else
                buf[strlen(buf)-1] = '\0';  /* nuke '\n' */
#endif
                group = glmFindGroup(model, buf);
                group->material = material;
				group->usetexture=true;
                break;
            case 'f':               /* face */
                v = n = t = 0;
				T(numtriangles).findex = -1; // ???
				T(numtriangles).vecini[0]=-1;
				T(numtriangles).vecini[1]=-1;
				T(numtriangles).vecini[2]=-1;
                fscanf(file, "%s", buf);
                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//")) {
                    /* v//n */
					group->usetexture=false;
                    retValue = sscanf_s(buf, "%d//%d", &v, &n);
                    assert(retValue==2);
					T(numtriangles).vindices[0] = v;
					if (n== 181228)
					{
						printf("");
					}
                    T(numtriangles).nindices[0] = n;
                    fscanf(file, "%d//%d", &v, &n);
                    T(numtriangles).vindices[1] = v;
                    T(numtriangles).nindices[1] = n;
                    fscanf(file, "%d//%d", &v, &n);
                    T(numtriangles).vindices[2] = v;
                    T(numtriangles).nindices[2] = n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d//%d", &v, &n) > 0) {						
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
                        T(numtriangles).vindices[2] = v;
                        T(numtriangles).nindices[2] = n;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                } else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
                    /* v/t/n */
					
                    T(numtriangles).vindices[0] = v; 
                    T(numtriangles).tindices[0] = t;
                    T(numtriangles).nindices[0] = n;
                    retValue=fscanf(file, "%d/%d/%d", &v, &t, &n);
                    T(numtriangles).vindices[1] = v;
                    T(numtriangles).tindices[1] = t;
                    T(numtriangles).nindices[1] = n;
                    retValue=fscanf(file, "%d/%d/%d", &v, &t, &n);
                    T(numtriangles).vindices[2] = v;
                    T(numtriangles).tindices[2] = t;
                    T(numtriangles).nindices[2] = n;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
						//if (n== 181228)					
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
                        T(numtriangles).nindices[0] = T(numtriangles-1).nindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
                        T(numtriangles).nindices[1] = T(numtriangles-1).nindices[2];
                        T(numtriangles).vindices[2] = v;
                        T(numtriangles).tindices[2] = t;
                        T(numtriangles).nindices[2] = n;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                } else if (sscanf_s(buf, "%d/%d", &v, &t) == 2) {
                    /* v/t */					
                    T(numtriangles).vindices[0] = v;
                    T(numtriangles).tindices[0] = t;
                    fscanf(file, "%d/%d", &v, &t);
                    T(numtriangles).vindices[1] = v;
                    T(numtriangles).tindices[1] = t;
                    fscanf(file, "%d/%d", &v, &t);
                    T(numtriangles).vindices[2] = v;
                    T(numtriangles).tindices[2] = t;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d/%d", &v, &t) > 0) {						
					
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).tindices[0] = T(numtriangles-1).tindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).tindices[1] = T(numtriangles-1).tindices[2];
                        T(numtriangles).vindices[2] = v;
                        T(numtriangles).tindices[2] = t;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                } else {
                    /* v */
					//if (n== 181228)				
                    sscanf(buf, "%d", &v);
                    T(numtriangles).vindices[0] = v;
                    fscanf(file, "%d", &v);
                    T(numtriangles).vindices[1] = v;
                    fscanf(file, "%d", &v);
                    T(numtriangles).vindices[2] = v;
                    group->triangles[group->numtriangles++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%d", &v) > 0) {
                        T(numtriangles).vindices[0] = T(numtriangles-1).vindices[0];
                        T(numtriangles).vindices[1] = T(numtriangles-1).vindices[2];
                        T(numtriangles).vindices[2] = v;
                        group->triangles[group->numtriangles++] = numtriangles;
                        numtriangles++;
                    }
                }
                break;
                
            default:
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
			
    }
	
  }
  
#if 0
  /* announce the memory requirements */
  printf(" Memory: %d bytes\n",
      numvertices  * 3*sizeof(GLfloat) +
      numnormals   * 3*sizeof(GLfloat) * (numnormals ? 1 : 0) +
      numtexcoords * 3*sizeof(GLfloat) * (numtexcoords ? 1 : 0) +
      numtriangles * sizeof(GLMtriangle));
#endif
}


/* public functions */


/* glmUnitize: "unitize" a model by translating it to the origin and
 * scaling it to fit in a unit cube around the origin.   Returns the
 * scalefactor used.
 *
 * model - properly initialized GLMmodel structure 
 */
GLfloat
glmUnitize(GLMmodel* model)
{
    GLuint  i;
    GLfloat maxx, minx, maxy, miny, maxz, minz;
    GLfloat cx, cy, cz, w, h, d;
    GLfloat scale;
    
    assert(model);
    assert(model->vertices);
    
    /* get the max/mins */
    maxx = minx = model->vertices[3 + 0];
    maxy = miny = model->vertices[3 + 1];
    maxz = minz = model->vertices[3 + 2];
    for (i = 1; i <= model->numvertices; i++) {
        if (maxx < model->vertices[3 * i + 0])
            maxx = model->vertices[3 * i + 0];
        if (minx > model->vertices[3 * i + 0])
            minx = model->vertices[3 * i + 0];
        
        if (maxy < model->vertices[3 * i + 1])
            maxy = model->vertices[3 * i + 1];
        if (miny > model->vertices[3 * i + 1])
            miny = model->vertices[3 * i + 1];
        
        if (maxz < model->vertices[3 * i + 2])
            maxz = model->vertices[3 * i + 2];
        if (minz > model->vertices[3 * i + 2])
            minz = model->vertices[3 * i + 2];
    }
    
    /* calculate model width, height, and depth */
    w = glmAbs(maxx) + glmAbs(minx);
    h = glmAbs(maxy) + glmAbs(miny);
    d = glmAbs(maxz) + glmAbs(minz);
    
    /* calculate center of the model */
    cx = (maxx + minx) / 2.0;
    cy = (maxy + miny) / 2.0;
    cz = (maxz + minz) / 2.0;
    
    /* calculate unitizing scale factor */
    scale = 2.0 / glmMax(glmMax(w, h), d);
    
    /* translate around center then scale */
    for (i = 1; i <= model->numvertices; i++) {
        model->vertices[3 * i + 0] -= cx;
        model->vertices[3 * i + 1] -= cy;
        model->vertices[3 * i + 2] -= cz;
        model->vertices[3 * i + 0] *= scale;
        model->vertices[3 * i + 1] *= scale;
        model->vertices[3 * i + 2] *= scale;
    }
    
    return scale;
}


/* glmDimensions: Calculates the dimensions (width, height, depth) of
 * a model.
 *
 * model   - initialized GLMmodel structure
 * dimensions - array of 3 GLfloats (GLfloat dimensions[3])
 */
GLvoid
glmDimensions(GLMmodel* model, GLfloat* dimensions)
{
    GLuint i;
    GLfloat maxx, minx, maxy, miny, maxz, minz;
    
    assert(model);
    assert(model->vertices);
    assert(dimensions);
    
    /* get the max/mins */
    maxx = minx = model->vertices[3 + 0];
    maxy = miny = model->vertices[3 + 1];
    maxz = minz = model->vertices[3 + 2];
    for (i = 1; i <= model->numvertices; i++) {
        if (maxx < model->vertices[3 * i + 0])
            maxx = model->vertices[3 * i + 0];
        if (minx > model->vertices[3 * i + 0])
            minx = model->vertices[3 * i + 0];
        
        if (maxy < model->vertices[3 * i + 1])
            maxy = model->vertices[3 * i + 1];
        if (miny > model->vertices[3 * i + 1])
            miny = model->vertices[3 * i + 1];
        
        if (maxz < model->vertices[3 * i + 2])
            maxz = model->vertices[3 * i + 2];
        if (minz > model->vertices[3 * i + 2])
            minz = model->vertices[3 * i + 2];
    }
    
    /* calculate model width, height, and depth */
    dimensions[0] = glmAbs(maxx) + glmAbs(minx);
    dimensions[1] = glmAbs(maxy) + glmAbs(miny);
    dimensions[2] = glmAbs(maxz) + glmAbs(minz);
}

/* glmScale: Scales a model by a given amount.
 * 
 * model - properly initialized GLMmodel structure
 * scale - scalefactor (0.5 = half as large, 2.0 = twice as large)
 */
GLvoid
glmScale(GLMmodel* model, GLfloat scale)
{
    GLuint i;
    
    for (i = 1; i <= model->numvertices; i++) {
        model->vertices[3 * i + 0] *= scale;
        model->vertices[3 * i + 1] *= scale;
        model->vertices[3 * i + 2] *= scale;
    }
}

/* glmReverseWinding: Reverse the polygon winding for all polygons in
 * this model.   Default winding is counter-clockwise.  Also changes
 * the direction of the normals.
 * 
 * model - properly initialized GLMmodel structure 
 */
GLvoid
glmReverseWinding(GLMmodel* model)
{
    GLuint i, swap;
    
    assert(model);
    
    for (i = 0; i < model->numtriangles; i++) {
        swap = T(i).vindices[0];
        T(i).vindices[0] = T(i).vindices[2];
        T(i).vindices[2] = swap;
        
        if (model->numnormals) {
            swap = T(i).nindices[0];
            T(i).nindices[0] = T(i).nindices[2];
            T(i).nindices[2] = swap;
        }
        
        if (model->numtexcoords) {
            swap = T(i).tindices[0];
            T(i).tindices[0] = T(i).tindices[2];
            T(i).tindices[2] = swap;
        }
    }
    
    /* reverse facet normals */
    for (i = 1; i <= model->numfacetnorms; i++) {
        model->facetnorms[3 * i + 0] = -model->facetnorms[3 * i + 0];
        model->facetnorms[3 * i + 1] = -model->facetnorms[3 * i + 1];
        model->facetnorms[3 * i + 2] = -model->facetnorms[3 * i + 2];
    }
    
    /* reverse vertex normals */
    for (i = 1; i <= model->numnormals; i++) {
        model->normals[3 * i + 0] = -model->normals[3 * i + 0];
        model->normals[3 * i + 1] = -model->normals[3 * i + 1];
        model->normals[3 * i + 2] = -model->normals[3 * i + 2];
    }
}

/* glmFacetNormals: Generates facet normals for a model (by taking the
 * cross product of the two vectors derived from the sides of each
 * triangle).  Assumes a counter-clockwise winding.
 *
 * model - initialized GLMmodel structure
 */
GLvoid
glmFacetNormals(GLMmodel* model)
{
    GLuint  i;
    GLfloat u[3];
    GLfloat v[3];
    
    assert(model);
    assert(model->vertices);
    
    /* clobber any old facetnormals */
    if (model->facetnorms)
        free(model->facetnorms);
    
    /* allocate memory for the new facet normals */
    model->numfacetnorms = model->numtriangles;
    model->facetnorms = (GLfloat*)malloc(sizeof(GLfloat) *
                       3 * (model->numfacetnorms + 1));
    
    for (i = 0; i < model->numtriangles; i++) 
	{
        model->triangles[i].findex = i+1;		
        //assert(T(i).vindices[1]<4);
        u[0] = model->vertices[3 * T(i).vindices[1] + 0] -
            model->vertices[3 * T(i).vindices[0] + 0];
        u[1] = model->vertices[3 * T(i).vindices[1] + 1] -
            model->vertices[3 * T(i).vindices[0] + 1];
        u[2] = model->vertices[3 * T(i).vindices[1] + 2] -
            model->vertices[3 * T(i).vindices[0] + 2];
        
        v[0] = model->vertices[3 * T(i).vindices[2] + 0] -
            model->vertices[3 * T(i).vindices[0] + 0];
        v[1] = model->vertices[3 * T(i).vindices[2] + 1] -
            model->vertices[3 * T(i).vindices[0] + 1];
        v[2] = model->vertices[3 * T(i).vindices[2] + 2] -
            model->vertices[3 * T(i).vindices[0] + 2];
        
        glmCross(u, v, &model->facetnorms[3 * (i+1)]);
        glmNormalize(&model->facetnorms[3 * (i+1)]);
    }
}

/* glmVertexNormals: Generates smooth vertex normals for a model.
 * First builds a list of all the triangles each vertex is in.   Then
 * loops through each vertex in the the list averaging all the facet
 * normals of the triangles each vertex is in.   Finally, sets the
 * normal index in the triangle for the vertex to the generated smooth
 * normal.   If the dot product of a facet normal and the facet normal
 * associated with the first triangle in the list of triangles the
 * current vertex is in is greater than the cosine of the angle
 * parameter to the function, that facet normal is not added into the
 * average normal calculation and the corresponding vertex is given
 * the facet normal.  This tends to preserve hard edges.  The angle to
 * use depends on the model, but 90 degrees is usually a good start.
 *
 * model - initialized GLMmodel structure
 * angle - maximum angle (in degrees) to smooth across
 */
GLvoid
glmVertexNormals(GLMmodel* model, GLfloat angle)
{
    GLMnode*    node;
    GLMnode*    tail;
    GLMnode** members;
    GLfloat*    normals;
    GLuint  numnormals;
    GLfloat average[3];
    GLfloat dot, cos_angle;
    GLuint  i, avg;
    
    assert(model);
    assert(model->facetnorms);
    
    /* calculate the cosine of the angle (in degrees) */
    cos_angle = cos(angle * M_PI / 180.0);
    
    /* nuke any previous normals */
    if (model->normals)
        free(model->normals);
    
    /* allocate space for new normals */
    model->numnormals = model->numtriangles * 3; /* 3 normals per triangle */
    model->normals = (GLfloat*)malloc(sizeof(GLfloat)* 3* (model->numnormals+1));
    
    /* allocate a structure that will hold a linked list of triangle
    indices for each vertex */
    members = (GLMnode**)malloc(sizeof(GLMnode*) * (model->numvertices + 1));
    for (i = 1; i <= model->numvertices; i++)
        members[i] = NULL;
    
    /* for every triangle, create a node for each vertex in it */
    for (i = 0; i < model->numtriangles; i++) {
        node = (GLMnode*)malloc(sizeof(GLMnode));
        node->index = i;
        node->next  = members[T(i).vindices[0]];
        members[T(i).vindices[0]] = node;
        
        node = (GLMnode*)malloc(sizeof(GLMnode));
        node->index = i;
        node->next  = members[T(i).vindices[1]];
        members[T(i).vindices[1]] = node;
        
        node = (GLMnode*)malloc(sizeof(GLMnode));
        node->index = i;
        node->next  = members[T(i).vindices[2]];
        members[T(i).vindices[2]] = node;
    }
    
    /* calculate the average normal for each vertex */
    numnormals = 1;
    for (i = 1; i <= model->numvertices; i++) {
    /* calculate an average normal for this vertex by averaging the
        facet normal of every triangle this vertex is in */
        node = members[i];
        if (!node)
            fprintf(stderr, "glmVertexNormals(): vertex w/o a triangle\n");
        average[0] = 0.0; average[1] = 0.0; average[2] = 0.0;
        avg = 0;
        while (node) {
        /* only average if the dot product of the angle between the two
        facet normals is greater than the cosine of the threshold
        angle -- or, said another way, the angle between the two
            facet normals is less than (or equal to) the threshold angle */
            dot = glmDot(&model->facetnorms[3 * T(node->index).findex],
                &model->facetnorms[3 * T(members[i]->index).findex]);
            if (dot > cos_angle) {
                node->averaged = GL_TRUE;
                average[0] += model->facetnorms[3 * T(node->index).findex + 0];
                average[1] += model->facetnorms[3 * T(node->index).findex + 1];
                average[2] += model->facetnorms[3 * T(node->index).findex + 2];
                avg = 1;            /* we averaged at least one normal! */
            } else {
                node->averaged = GL_FALSE;
            }
            node = node->next;
        }
        
        if (avg) {
            /* normalize the averaged normal */
            glmNormalize(average);
            
            /* add the normal to the vertex normals list */
            model->normals[3 * numnormals + 0] = average[0];
            model->normals[3 * numnormals + 1] = average[1];
            model->normals[3 * numnormals + 2] = average[2];
            avg = numnormals;
            numnormals++;
        }
        
        /* set the normal of this vertex in each triangle it is in */
        node = members[i];
        while (node) 
		{
			if (node->index==204543)
				{
					printf("");
				}
            if (node->averaged) {
				
                /* if this node was averaged, use the average normal */
                if (T(node->index).vindices[0] == i)
                    T(node->index).nindices[0] = avg;
                else if (T(node->index).vindices[1] == i)
                    T(node->index).nindices[1] = avg;
                else if (T(node->index).vindices[2] == i)
                    T(node->index).nindices[2] = avg;
            } else {
				
                /* if this node wasn't averaged, use the facet normal */
                model->normals[3 * numnormals + 0] = 
                    model->facetnorms[3 * T(node->index).findex + 0];
                model->normals[3 * numnormals + 1] = 
                    model->facetnorms[3 * T(node->index).findex + 1];
                model->normals[3 * numnormals + 2] = 
                    model->facetnorms[3 * T(node->index).findex + 2];
                if (T(node->index).vindices[0] == i)
                    T(node->index).nindices[0] = numnormals;
                else if (T(node->index).vindices[1] == i)
                    T(node->index).nindices[1] = numnormals;
                else if (T(node->index).vindices[2] == i)
                    T(node->index).nindices[2] = numnormals;
                numnormals++;
            }
            node = node->next;
        }
    }
    
    model->numnormals = numnormals - 1;
    
    /* free the member information */
    for (i = 1; i <= model->numvertices; i++) {
        node = members[i];
		
        while (node) {
            tail = node;
            node = node->next;
            free(tail);
        }
    }
    free(members);
    
    /* pack the normals array (we previously allocated the maximum
    number of normals that could possibly be created (numtriangles *
    3), so get rid of some of them (usually alot unless none of the
    facet normals were averaged)) */
    normals = model->normals;
    model->normals = (GLfloat*)malloc(sizeof(GLfloat)* 3* (model->numnormals+1));
    for (i = 1; i <= model->numnormals; i++) {
        model->normals[3 * i + 0] = normals[3 * i + 0];
        model->normals[3 * i + 1] = normals[3 * i + 1];
        model->normals[3 * i + 2] = normals[3 * i + 2];
    }
    free(normals);
}

GLvoid
glmLinearTexture(GLMmodel* model)
{
    GLMgroup *group;
    GLfloat dimensions[3];
    GLfloat x, y, scalefactor;
    GLuint i;
    
    assert(model);
    
    if (model->texcoords)
        free(model->texcoords);
    model->numtexcoords = model->numvertices;
    model->texcoords=(GLfloat*)malloc(sizeof(GLfloat)*2*(model->numtexcoords+1));
    
    glmDimensions(model, dimensions);
    scalefactor = 2.0 / 
        glmAbs(glmMax(glmMax(dimensions[0], dimensions[1]), dimensions[2]));
    
    /* do the calculations */
    for(i = 1; i <= model->numvertices; i++) {
        x = model->vertices[3 * i + 0] * scalefactor;
        y = model->vertices[3 * i + 2] * scalefactor;
        model->texcoords[2 * i + 0] = (x + 1.0) / 2.0;
        model->texcoords[2 * i + 1] = (y + 1.0) / 2.0;
    }
    
    /* go through and put texture coordinate indices in all the triangles */
    group = model->groups;
    while(group) {
        for(i = 0; i < group->numtriangles; i++) {
            T(group->triangles[i]).tindices[0] = T(group->triangles[i]).vindices[0];
            T(group->triangles[i]).tindices[1] = T(group->triangles[i]).vindices[1];
            T(group->triangles[i]).tindices[2] = T(group->triangles[i]).vindices[2];
        }    
        group = group->next;
    }
    
#if 0
    printf("glmLinearTexture(): generated %d linear texture coordinates\n",
        model->numtexcoords);
#endif
}

/* glmSpheremapTexture: Generates texture coordinates according to a
 * spherical projection of the texture map.  Sometimes referred to as
 * spheremap, or reflection map texture coordinates.  It generates
 * these by using the normal to calculate where that vertex would map
 * onto a sphere.  Since it is impossible to map something flat
 * perfectly onto something spherical, there is distortion at the
 * poles.  This particular implementation causes the poles along the X
 * axis to be distorted.
 *
 * model - pointer to initialized GLMmodel structure
 */
GLvoid
glmSpheremapTexture(GLMmodel* model)
{
    GLMgroup* group;
    GLfloat theta, phi, rho, x, y, z, r;
    GLuint i;
    
    assert(model);
    assert(model->normals);
    
    if (model->texcoords)
        free(model->texcoords);
    model->numtexcoords = model->numnormals;
    model->texcoords=(GLfloat*)malloc(sizeof(GLfloat)*2*(model->numtexcoords+1));
    
    for (i = 1; i <= model->numnormals; i++) {
        z = model->normals[3 * i + 0];  /* re-arrange for pole distortion */
        y = model->normals[3 * i + 1];
        x = model->normals[3 * i + 2];
        r = sqrt((x * x) + (y * y));
        rho = sqrt((r * r) + (z * z));
        
        if(r == 0.0) {
            theta = 0.0;
            phi = 0.0;
        } else {
            if(z == 0.0)
                phi = 3.14159265 / 2.0;
            else
                phi = acos(z / rho);
            
            if(y == 0.0)
                theta = 3.141592365 / 2.0;
            else
                theta = asin(y / r) + (3.14159265 / 2.0);
        }
        
        model->texcoords[2 * i + 0] = theta / 3.14159265;
        model->texcoords[2 * i + 1] = phi / 3.14159265;
    }
    
    /* go through and put texcoord indices in all the triangles */
    group = model->groups;
    while(group) {
        for (i = 0; i < group->numtriangles; i++) {
            T(group->triangles[i]).tindices[0] = T(group->triangles[i]).nindices[0];
            T(group->triangles[i]).tindices[1] = T(group->triangles[i]).nindices[1];
            T(group->triangles[i]).tindices[2] = T(group->triangles[i]).nindices[2];
        }
        group = group->next;
    }
}

/* glmDelete: Deletes a GLMmodel structure.
 *
 * model - initialized GLMmodel structure
 */
GLvoid
glmDelete(GLMmodel* model)
{
    GLMgroup* group;
    GLuint i;
    
    assert(model);
    
    if (model->pathname)     free(model->pathname);
    if (model->mtllibname) free(model->mtllibname);
    if (model->vertices)     free(model->vertices);
    if (model->normals)  free(model->normals);
    if (model->texcoords)  free(model->texcoords);
    if (model->facetnorms) free(model->facetnorms);
    if (model->triangles)  free(model->triangles);
    if (model->materials) {
        for (i = 0; i < model->nummaterials; i++)
            free(model->materials[i].name);
        free(model->materials);
    }
    if (model->textures) {
        for (i = 0; i < model->numtextures; i++) {
            free(model->textures[i].name);
            glDeleteTextures(1,&model->textures[i].id);
        }
        free(model->textures);
    }

    while(model->groups) {
        group = model->groups;
        model->groups = model->groups->next;
        free(group->name);
        free(group->triangles);
        free(group);
    }
    
    free(model);
}

/* glmReadOBJ: Reads a model description from a Wavefront .OBJ file.
 * Returns a pointer to the created object which should be free'd with
 * glmDelete().
 *
 * filename - name of the file containing the Wavefront .OBJ format data.  
 */

GLMmodel* glmReadOBJ(char* filename)
{
	return glmReadOBJ(filename,0);
}
GLMmodel* glmReadOBJ(char* filename,mycallback *call)
{
    GLMmodel* model;
    FILE*   file;
	//if (call) call->loadcallback(0,"Loading Models...");
    /* open the file */
	
	char		szPath[MAX_PATH+1];

	GetCurrentDirectory(MAX_PATH, szPath);
	char		curDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, curDir);
	strcat(szPath, "\\");
	if (filename[1]!=':')
	{
		strcat(szPath, filename);
		filename=szPath;
	}
	int slashCount;
	for (slashCount=(int)strlen(filename);slashCount>0;slashCount=slashCount-1)
	{
		if (filename[slashCount]=='\\')
			break;
	}

	

    file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "glmReadOBJ() failed: can't open data file \"%s\".\n",
            filename);
        return NULL;
    }
	
    
    /* allocate a new model */
    model = (GLMmodel*)malloc(sizeof(GLMmodel));
    model->pathname    = _strdup(filename);
    model->mtllibname    = NULL;
    model->numvertices   = 0;
    model->vertices    = NULL;
    model->numnormals    = 0;
    model->normals     = NULL;
    model->numtexcoords  = 0;
    model->texcoords       = NULL;
    model->numfacetnorms = 0;
    model->facetnorms    = NULL;
    model->numtriangles  = 0;
    model->triangles       = NULL;
    model->nummaterials  = 0;
    model->materials       = NULL;
    model->numtextures  = 0;
    model->textures       = NULL;
    model->numgroups       = 0;
    model->groups      = NULL;
    model->pos.x   = 0.0;
    model->pos.y   = 0.0;
    model->pos.z   = 0.0;
    model->rot.x   = 0.0;
	model->rot.y   = 0.0;
	model->rot.z   = 0.0;
	model->scale   = 1.0;
	
	char modelDir[MAX_PATH+1]="";

	strncpy(modelDir,filename,slashCount);
	SetCurrentDirectory(modelDir);

    /* make a first pass through the file to get a count of the number
    of vertices, normals, texcoords & triangles */

    glmFirstPass(model, file, call);

    /* allocate memory */
    model->vertices = (GLfloat*)malloc(sizeof(GLfloat) *
        3 * (model->numvertices + 1));
    model->triangles = (GLMtriangle*)malloc(sizeof(GLMtriangle) *
        model->numtriangles);
    if (model->numnormals) {
        model->normals = (GLfloat*)malloc(sizeof(GLfloat) *
            3 * (model->numnormals + 1));
    }
    if (model->numtexcoords) {
        model->texcoords = (GLfloat*)malloc(sizeof(GLfloat) *
            2 * (model->numtexcoords + 1));
    }
    
    /* rewind to beginning of file and read in the data this pass */
    rewind(file);

    glmSecondPass(model, file,call);

    /* close the file */
    fclose(file);

    SetCurrentDirectory(curDir);

    return model;
}

/* glmWriteOBJ: Writes a model description in Wavefront .OBJ format to
 * a file.
 *
 * model - initialized GLMmodel structure
 * filename - name of the file to write the Wavefront .OBJ format data to
 * mode  - a bitwise or of values describing what is written to the file
 *             GLM_NONE     -  render with only vertices
 *             GLM_FLAT     -  render with facet normals
 *             GLM_SMOOTH   -  render with vertex normals
 *             GLM_TEXTURE  -  render with texture coords
 *             GLM_COLOR    -  render with colors (color material)
 *             GLM_MATERIAL -  render with materials
 *             GLM_COLOR and GLM_MATERIAL should not both be specified.  
 *             GLM_FLAT and GLM_SMOOTH should not both be specified.  
 */
GLvoid
glmWriteOBJ(GLMmodel* model, char* filename, GLuint mode)
{
    GLuint  i;
    FILE*   file;
    GLMgroup* group;  
    assert(model);
    
    /* do a bit of warning */
    if (mode & GLM_FLAT && !model->facetnorms) {
        printf("glmWriteOBJ() warning: flat normal output requested "
            "with no facet normals defined.\n");
        mode &= ~GLM_FLAT;
    }
    if (mode & GLM_SMOOTH && !model->normals) {
        printf("glmWriteOBJ() warning: smooth normal output requested "
            "with no normals defined.\n");
        mode &= ~GLM_SMOOTH;
    }
    if (mode & GLM_TEXTURE && !model->texcoords) {
        printf("glmWriteOBJ() warning: texture coordinate output requested "
            "with no texture coordinates defined.\n");
        mode &= ~GLM_TEXTURE;
    }
    if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
        printf("glmWriteOBJ() warning: flat normal output requested "
            "and smooth normal output requested (using smooth).\n");
        mode &= ~GLM_FLAT;
    }
    if (mode & GLM_COLOR && !model->materials) {
        printf("glmWriteOBJ() warning: color output requested "
            "with no colors (materials) defined.\n");
        mode &= ~GLM_COLOR;
    }
    if (mode & GLM_MATERIAL && !model->materials) {
        printf("glmWriteOBJ() warning: material output requested "
            "with no materials defined.\n");
        mode &= ~GLM_MATERIAL;
    }
    if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
        printf("glmWriteOBJ() warning: color and material output requested "
            "outputting only materials.\n");
        mode &= ~GLM_COLOR;
    }
    
    
    /* open the file */
    file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "glmWriteOBJ() failed: can't open file \"%s\" to write.\n",
            filename);
        exit(1);
    }
    
    /* spit out a header */
    fprintf(file, "#  \n");
    fprintf(file, "#  Wavefront OBJ generated by GLM library\n");
    fprintf(file, "#  \n");
    fprintf(file, "#  GLM library\n");
    fprintf(file, "#  Nate Robins\n");
    fprintf(file, "#  ndr@pobox.com\n");
    fprintf(file, "#  http://www.pobox.com/~ndr\n");
    fprintf(file, "#  \n");
    
    if (mode & GLM_MATERIAL && model->mtllibname) {
        fprintf(file, "\nmtllib %s\n\n", model->mtllibname);
        glmWriteMTL(model, filename, model->mtllibname);
    }
    
    /* spit out the vertices */
    fprintf(file, "\n");
    fprintf(file, "# %d vertices\n", model->numvertices);
    for (i = 1; i <= model->numvertices; i++) {
        fprintf(file, "v %f %f %f\n", 
            model->vertices[3 * i + 0],
            model->vertices[3 * i + 1],
            model->vertices[3 * i + 2]);
    }
    
    /* spit out the smooth/flat normals */
    if (mode & GLM_SMOOTH) {
        fprintf(file, "\n");
        fprintf(file, "# %d normals\n", model->numnormals);
        for (i = 1; i <= model->numnormals; i++) {
            fprintf(file, "vn %f %f %f\n", 
                model->normals[3 * i + 0],
                model->normals[3 * i + 1],
                model->normals[3 * i + 2]);
        }
    } else if (mode & GLM_FLAT) {
        fprintf(file, "\n");
        fprintf(file, "# %d normals\n", model->numfacetnorms);
        for (i = 1; i <= model->numnormals; i++) {
            fprintf(file, "vn %f %f %f\n", 
                model->facetnorms[3 * i + 0],
                model->facetnorms[3 * i + 1],
                model->facetnorms[3 * i + 2]);
        }
    }
    
    /* spit out the texture coordinates */
    if (mode & GLM_TEXTURE) {
        fprintf(file, "\n");
        fprintf(file, "# %d texcoords\n", model->texcoords);
        for (i = 1; i <= model->numtexcoords; i++) {
            fprintf(file, "vt %f %f\n", 
                model->texcoords[2 * i + 0],
                model->texcoords[2 * i + 1]);
        }
    }
    
    fprintf(file, "\n");
    fprintf(file, "# %d groups\n", model->numgroups);
    fprintf(file, "# %d faces (triangles)\n", model->numtriangles);
    fprintf(file, "\n");
    
    group = model->groups;
    while(group) {
        fprintf(file, "g %s\n", group->name);
        if (mode & GLM_MATERIAL)
            fprintf(file, "usemtl %s\n", model->materials[group->material].name);
        for (i = 0; i < group->numtriangles; i++) {
            if (mode & GLM_SMOOTH && mode & GLM_TEXTURE) {
                fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                    T(group->triangles[i]).vindices[0], 
                    T(group->triangles[i]).nindices[0], 
                    T(group->triangles[i]).tindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).nindices[1],
                    T(group->triangles[i]).tindices[1],
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).nindices[2],
                    T(group->triangles[i]).tindices[2]);
            } else if (mode & GLM_FLAT && mode & GLM_TEXTURE) {
                fprintf(file, "f %d/%d %d/%d %d/%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).findex);
            } else if (mode & GLM_TEXTURE) {
                fprintf(file, "f %d/%d %d/%d %d/%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).tindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).tindices[1],
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).tindices[2]);
            } else if (mode & GLM_SMOOTH) {
                fprintf(file, "f %d//%d %d//%d %d//%d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).nindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).nindices[1],
                    T(group->triangles[i]).vindices[2], 
                    T(group->triangles[i]).nindices[2]);
            } else if (mode & GLM_FLAT) {
                fprintf(file, "f %d//%d %d//%d %d//%d\n",
                    T(group->triangles[i]).vindices[0], 
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).findex,
                    T(group->triangles[i]).vindices[2],
                    T(group->triangles[i]).findex);
            } else {
                fprintf(file, "f %d %d %d\n",
                    T(group->triangles[i]).vindices[0],
                    T(group->triangles[i]).vindices[1],
                    T(group->triangles[i]).vindices[2]);
            }
        }
        fprintf(file, "\n");
        group = group->next;
    }
    
    fclose(file);
}

/* glmDraw: Renders the model to the current OpenGL context using the
 * mode specified.
 *
 * model - initialized GLMmodel structure
 * mode  - a bitwise OR of values describing what is to be rendered.
 *             GLM_NONE     -  render with only vertices
 *             GLM_FLAT     -  render with facet normals
 *             GLM_SMOOTH   -  render with vertex normals
 *             GLM_TEXTURE  -  render with texture coords
 *             GLM_COLOR    -  render with colors (color material)
 *             GLM_MATERIAL -  render with materials
 *             GLM_COLOR and GLM_MATERIAL should not both be specified.  
 *             GLM_FLAT and GLM_SMOOTH should not both be specified.  
 */

GLvoid glmMinDrawQuery(float bBox[][3], Vector pos,int show)
{

	if(!show)
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
	}
	else
	{
		glEnable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
		if(show==1)
			glColor4f(0.7,0,0.7,1);
		else
			glColor4f(0.7,0.7,0,1);
	}

	

	glBegin(GL_QUADS);
		glVertex3f(  bBox[5][0]+pos.x, bBox[5][1]+pos.y, bBox[5][2]+pos.z );
		glVertex3f( bBox[7][0]+pos.x, bBox[7][1]+pos.y, bBox[7][2]+pos.z);
		glVertex3f( bBox[3][0]+pos.x, bBox[3][1]+pos.y, bBox[3][2]+pos.z );
		glVertex3f(  bBox[1][0]+pos.x, bBox[1][1]+pos.y, bBox[1][2]+pos.z );
	glEnd();
 
	// Render the left quad
	glBegin(GL_QUADS);
		glVertex3f( bBox[4][0]+pos.x, bBox[4][1]+pos.y, bBox[4][2]+pos.z );
		glVertex3f(  bBox[5][0]+pos.x, bBox[5][1]+pos.y, bBox[5][2]+pos.z );
		glVertex3f(  bBox[1][0]+pos.x, bBox[1][1]+pos.y, bBox[1][2]+pos.z );
		glVertex3f( bBox[0][0]+pos.x,  bBox[0][1]+pos.y,  bBox[0][2]+pos.z );
	glEnd();
 
	// Render the back quad
	glBegin(GL_QUADS);
		glVertex3f( bBox[6][0]+pos.x, bBox[6][1]+pos.y, bBox[6][2]+pos.z );
		glVertex3f( bBox[4][0]+pos.x, bBox[4][1]+pos.y, bBox[4][2]+pos.z );
		glVertex3f( bBox[0][0]+pos.x,  bBox[0][1]+pos.y,  bBox[0][2]+pos.z );
		glVertex3f( bBox[2][0]+pos.x, bBox[2][1]+pos.y, bBox[2][2]+pos.z );
 
	glEnd();
 
	// Render the right quad
	glBegin(GL_QUADS);
		glVertex3f( bBox[7][0]+pos.x, bBox[7][1]+pos.y, bBox[7][2]+pos.z );
		glVertex3f( bBox[6][0]+pos.x, bBox[6][1]+pos.y, bBox[6][2]+pos.z );
		glVertex3f( bBox[2][0]+pos.x, bBox[2][1]+pos.y, bBox[2][2]+pos.z );
		glVertex3f( bBox[3][0]+pos.x, bBox[3][1]+pos.y, bBox[3][2]+pos.z );
	glEnd();
 
	// Render the top quad
	glBegin(GL_QUADS);
		glVertex3f( bBox[3][0]+pos.x, bBox[3][1]+pos.y, bBox[3][2]+pos.z );
		glVertex3f( bBox[2][0]+pos.x, bBox[2][1]+pos.y, bBox[2][2]+pos.z );
		glVertex3f( bBox[0][0]+pos.x,  bBox[0][1]+pos.y,  bBox[0][2]+pos.z );
		glVertex3f(  bBox[1][0]+pos.x, bBox[1][1]+pos.y, bBox[1][2]+pos.z );
	glEnd();
 
	// Render the bottom quad
	glBegin(GL_QUADS);
		glVertex3f( bBox[7][0]+pos.x, bBox[7][1]+pos.y, bBox[7][2]+pos.z );
		glVertex3f( bBox[6][0]+pos.x, bBox[6][1]+pos.y, bBox[6][2]+pos.z );
		glVertex3f( bBox[4][0]+pos.x, bBox[4][1]+pos.y, bBox[4][2]+pos.z );
		glVertex3f(  bBox[5][0]+pos.x, bBox[5][1]+pos.y, bBox[5][2]+pos.z );
	glEnd();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
	glColor3f(1,1,1);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
}

GLvoid glmReducedDraw(GLMmodel* model)
{
	glmReducedDraw(model,1,0,0);
}
GLvoid glmReducedDraw(GLMmodel* model,float scale)
{
	glmReducedDraw(model,scale,0,0);
}

GLvoid glmReducedDraw(GLMmodel* model,float scale,float glowFactor,int colorIndex)
{
	
	static GLuint i;
    static GLMgroup* group;
    static GLMtriangle* triangle;
    static GLMmaterial* material;
    GLuint IDTexture;
	char *drawonly=0;
	GLuint mode;
	mode=GLM_SMOOTH|GLM_MATERIAL|GLM_TEXTURE;

    assert(model);
    assert(model->vertices);

    glEnable(GL_COLOR_MATERIAL);

    IDTexture = -1;
	bool twice=false;
    group = model->groups;


    while (group&&group->numtriangles!=0) 
	{

		if (drawonly)
			if (strcmp(group->name,drawonly))
			{
				group=group->next;
				continue;
			}		
		
		material = &model->materials[group->material];
		if (material&&group->usetexture)
			IDTexture = material->IDTexture;
		else IDTexture=-1;
        
		if(twice&&(!material||material->opacity==1))
		{
			group=group->next;
			if(group==NULL||group->numtriangles==0)
			{	
				break;
			}
			continue;
		}
		if(material&&!twice&&material->opacity!=1)
		{
			model->transparent=true;
			group=group->next;
			if(group==NULL||group->numtriangles==0)
			{	
				twice=true;
				group=model->groups;
			}
			continue;
		}

		if(twice)
		{	
			
			glEnable(GL_BLEND);		// Turn Blending On
		}
		else
		{
			glDisable(GL_BLEND);
		}
		
		if(material)
		{
			if(glowFactor>0.001f)
			{
				material->ambient[colorIndex]+=glowFactor;
				material->diffuse[colorIndex]+=glowFactor;
				material->emmissive[colorIndex]+=glowFactor;
				material->specular[colorIndex]+=glowFactor;
			}
			
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
			glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,  material->emmissive);
			glColor4f(material->diffuse[0],material->diffuse[1],material->diffuse[2],material->opacity);
		}
		else

		{
			glColor4f(1, 1, 1, 1);
			glEnable(GL_COLOR_MATERIAL);
		}
		if(material&&glowFactor>0.001f)
		{
			material->ambient[colorIndex]-=glowFactor;
			material->diffuse[colorIndex]-=glowFactor;
			material->emmissive[colorIndex]-=glowFactor;
			material->specular[colorIndex]-=glowFactor;
		}

		if (mode & GLM_TEXTURE) 
		{				
			if(IDTexture == -1)
				glBindTexture(GL_TEXTURE_2D,whiteID[0]);
			else
			{

				glBindTexture(GL_TEXTURE_2D, model->textures[IDTexture].id);
			}		
		}
        
        glPushMatrix();
		glScalef(scale,scale,scale);
		glEnable(GL_RESCALE_NORMAL);
        glBegin(GL_TRIANGLES);
        for (i = 0; i < group->numtriangles; i++) {
            triangle = &T(group->triangles[i]);
#ifdef DebugVisibleSurfaces
			if (!triangle->visible) continue;
#endif

				
				glNormal3fv(&model->normals[3 * triangle->nindices[0]]);
                
				if(IDTexture!=-1)glTexCoord2fv(&model->texcoords[2 * triangle->tindices[0]]);
				glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
                
				glNormal3fv(&model->normals[3 * triangle->nindices[1]]);
                if(IDTexture!=-1)glTexCoord2fv(&model->texcoords[2 * triangle->tindices[1]]);
	            glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);

                glNormal3fv(&model->normals[3 * triangle->nindices[2]]);
                if(IDTexture!=-1)glTexCoord2fv(&model->texcoords[2 * triangle->tindices[2]]);
				glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
            
        }
        glEnd();
		glPopMatrix();

    group = group->next;
	if(group==NULL||group->numtriangles==0)
		{	
			if(twice)
				break;
			twice=true;
			group=model->groups;
		}
	}
	GLfloat	white[]    ={1.0,1.0,1.0,1.0}; //resets materials
	GLfloat	black[]    ={0.0,0.0,0.0,0.0}; //resets materials
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, black);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 1);
	glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
	
	glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_RESCALE_NORMAL);
	glEnable(GL_BLEND);

}

GLvoid glmDraw(GLMmodel* model, GLuint mode)
{
	glmDraw(model,mode,0);
}


GLvoid glmDraw(GLMmodel* model, GLuint mode,char *drawonly)
{
    static GLuint i;
    static GLMgroup* group;
    static GLMtriangle* triangle;
    static GLMmaterial* material;
    GLuint IDTexture;

    assert(model);
    assert(model->vertices);
    
    /* do a bit of warning */
    if (mode & GLM_FLAT && !model->facetnorms) {
        printf("glmDraw() warning: flat render mode requested "
            "with no facet normals defined.\n");
        mode &= ~GLM_FLAT;
    }
    if (mode & GLM_SMOOTH && !model->normals) {
        printf("glmDraw() warning: smooth render mode requested "
            "with no normals defined.\n");
        mode &= ~GLM_SMOOTH;
    }
    if (mode & GLM_TEXTURE && !model->texcoords) {
        printf("glmDraw() warning: texture render mode requested "
            "with no texture coordinates defined.\n");
        mode &= ~GLM_TEXTURE;
    }
    if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
        printf("glmDraw() warning: flat render mode requested "
            "and smooth render mode requested (using smooth).\n");
        mode &= ~GLM_FLAT;
    }
    if (mode & GLM_COLOR && !model->materials) {
        printf("glmDraw() warning: color render mode requested "
            "with no materials defined.\n");
        mode &= ~GLM_COLOR;
    }
    if (mode & GLM_MATERIAL && !model->materials) {
        printf("glmDraw() warning: material render mode requested "
            "with no materials defined.\n");
        mode &= ~GLM_MATERIAL;
    }
    if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
        printf("glmDraw() warning: color and material render mode requested "
            "using only material mode.\n");
        mode &= ~GLM_COLOR;
    }
    if (mode & GLM_COLOR)
        glEnable(GL_COLOR_MATERIAL);
    else if (mode & GLM_MATERIAL)
        glDisable(GL_COLOR_MATERIAL);
    if (mode & GLM_TEXTURE) {
        glEnable(GL_TEXTURE_2D);
				
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }
    /* perhaps this loop should be unrolled into material, color, flat,
       smooth, etc. loops?  since most cpu's have good branch prediction
       schemes (and these branches will always go one way), probably
       wouldn't gain too much?  */
    
    IDTexture = -1;
    group = model->groups;
    while (group&&group->numtriangles!=0) 
	{

		if (drawonly)
			if (strcmp(group->name,drawonly))
			{
				group=group->next;
				continue;
			}		
		
		material = &model->materials[group->material];
		if (material&&group->usetexture)
			IDTexture = material->IDTexture;
		else 
		{
			IDTexture=-1;
		}

		if (mode & GLM_MATERIAL) 
		{            
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material->ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material->diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material->specular);
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
			glMaterialfv(GL_FRONT,GL_EMISSION,material->emmissive);
        }

		if (mode & GLM_COLOR) {
            glColor3fv(material->diffuse);
        }
		
		if (mode & GLM_TEXTURE) 
		{				
			if(IDTexture == -1)
				glBindTexture(GL_TEXTURE_2D, 0);
			else
			{
				glBindTexture(GL_TEXTURE_2D, model->textures[IDTexture].id);
			}		
		}
        
        

        glBegin(GL_TRIANGLES);
        for (i = 0; i < group->numtriangles; i++) {
            triangle = &T(group->triangles[i]);
#ifdef DebugVisibleSurfaces
			if (!triangle->visible) continue;
#endif
            if (mode & GLM_FLAT)
                glNormal3fv(&model->facetnorms[3 * triangle->findex]);
            
            if (mode & GLM_SMOOTH)
                glNormal3fv(&model->normals[3 * triangle->nindices[0]]);
            if (mode & GLM_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[0]]);
            glVertex3fv(&model->vertices[3 * triangle->vindices[0]]);
            
            if (mode & GLM_SMOOTH)
                glNormal3fv(&model->normals[3 * triangle->nindices[1]]);
            if (mode & GLM_TEXTURE)
			{
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[1]]);
            }
            glVertex3fv(&model->vertices[3 * triangle->vindices[1]]);
            
            if (mode & GLM_SMOOTH)
                glNormal3fv(&model->normals[3 * triangle->nindices[2]]);
            if (mode & GLM_TEXTURE)
                glTexCoord2fv(&model->texcoords[2 * triangle->tindices[2]]);
            glVertex3fv(&model->vertices[3 * triangle->vindices[2]]);
            
        }
        glEnd();
        
        group = group->next;
    }
}

/* glmList: Generates and returns a display list for the model using
 * the mode specified.
 *
 * model - initialized GLMmodel structure
 * mode  - a bitwise OR of values describing what is to be rendered.
 *             GLM_NONE     -  render with only vertices
 *             GLM_FLAT     -  render with facet normals
 *             GLM_SMOOTH   -  render with vertex normals
 *             GLM_TEXTURE  -  render with texture coords
 *             GLM_COLOR    -  render with colors (color material)
 *             GLM_MATERIAL -  render with materials
 *             GLM_COLOR and GLM_MATERIAL should not both be specified.  
 * GLM_FLAT and GLM_SMOOTH should not both be specified.  
 */
GLuint
glmList(GLMmodel* model, GLuint mode)
{
    GLuint list;
    
    list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glmDraw(model, mode);
    glEndList();
    
    return list;
}

/* glmWeld: eliminate (weld) vectors that are within an epsilon of
 * each other.
 *
 * model   - initialized GLMmodel structure
 * epsilon     - maximum difference between vertices
 *               ( 0.00001 is a good start for a unitized model)
 *
 */
GLvoid
glmWeld(GLMmodel* model, GLfloat epsilon)
{
    GLfloat* vectors;
    GLfloat* copies;
    GLuint   numvectors;
    GLuint   i;
    
    /* vertices */
    numvectors = model->numvertices;
    vectors  = model->vertices;
    copies = glmWeldVectors(vectors, &numvectors, epsilon);
    
#if 0
    printf("glmWeld(): %d redundant vertices.\n", 
        model->numvertices - numvectors - 1);
#endif
    
    for (i = 0; i < model->numtriangles; i++) {
        T(i).vindices[0] = (GLuint)vectors[3 * T(i).vindices[0] + 0];
        T(i).vindices[1] = (GLuint)vectors[3 * T(i).vindices[1] + 0];
        T(i).vindices[2] = (GLuint)vectors[3 * T(i).vindices[2] + 0];
    }
    
    /* free space for old vertices */
    free(vectors);
    
    /* allocate space for the new vertices */
    model->numvertices = numvectors;
    model->vertices = (GLfloat*)malloc(sizeof(GLfloat) * 
        3 * (model->numvertices + 1));
    
    /* copy the optimized vertices into the actual vertex list */
    for (i = 1; i <= model->numvertices; i++) {
        model->vertices[3 * i + 0] = copies[3 * i + 0];
        model->vertices[3 * i + 1] = copies[3 * i + 1];
        model->vertices[3 * i + 2] = copies[3 * i + 2];
    }
    
    free(copies);
}

/* glmReadPPM: read a PPM raw (type P6) file.  The PPM file has a header
 * that should look something like:
 *
 *    P6
 *    # comment
 *    width height max_value
 *    rgbrgbrgb...
 *
 * where "P6" is the magic cookie which identifies the file type and
 * should be the only characters on the first line followed by a
 * carriage return.  Any line starting with a # mark will be treated
 * as a comment and discarded.   After the magic cookie, three integer
 * values are expected: width, height of the image and the maximum
 * value for a pixel (max_value must be < 256 for PPM raw files).  The
 * data section consists of width*height rgb triplets (one byte each)
 * in binary format (i.e., such as that written with fwrite() or
 * equivalent).
 *
 * The rgb data is returned as an array of unsigned chars (packed
 * rgb).  The malloc()'d memory should be free()'d by the caller.  If
 * an error occurs, an error message is sent to stderr and NULL is
 * returned.
 *
 * filename   - name of the .ppm file.
 * width      - will contain the width of the image on return.
 * height     - will contain the height of the image on return.
 *
 */
GLubyte* 
glmReadPPM(char* filename, int* width, int* height)
{
    FILE* fp;
    int i, w, h, d;
    unsigned char* image;
    char head[70];          /* max line <= 70 in PPM (per spec). */
    
    fp = fopen(filename, "rb");
    if (!fp) {
        perror(filename);
        return NULL;
    }
    
    /* grab first two chars of the file and make sure that it has the
       correct magic cookie for a raw PPM file. */
    fgets(head, 70, fp);
    if (strncmp(head, "P6", 2)) {
        fprintf(stderr, "%s: Not a raw PPM file\n", filename);
        return NULL;
    }
    
    /* grab the three elements in the header (width, height, maxval). */
    i = 0;
    while(i < 3) {
        fgets(head, 70, fp);
        if (head[0] == '#')     /* skip comments. */
            continue;
        if (i == 0)
            i += sscanf_s(head, "%d %d %d", &w, &h, &d);
        else if (i == 1)
            i += sscanf_s(head, "%d %d", &h, &d);
        else if (i == 2)
            i += sscanf_s(head, "%d", &d);
    }
    
    /* grab all the image data in one fell swoop. */
    image = (unsigned char*)malloc(sizeof(unsigned char)*w*h*3);
    fread(image, sizeof(unsigned char), w*h*3, fp);
    fclose(fp);
    
    *width = w;
    *height = h;
    return image;
}

extern btDynamicsWorld*						m_dynamicsWorld; //this is the most important class
extern btAlignedObjectArray<btCollisionShape*>	m_collisionShapes; //keep the collision shapes, for deletion/cleanup

GLvoid glmCalcBounds(MapModel* model)
{	
	int i,j,k;

	double XRotMatrix[3][3];
	double YRotMatrix[3][3];
	double ZRotMatrix[3][3];
	double RotationMatrix[3][3];
	double PrimaryRotationMatrix[3][3];

	for (i=0;i<3;i++)		//Assigns X Y and Z Rotation Matrices to Identity, and The calculated matrices to zeros
	{
		for (j=0;j<3;j++)
		{
			if (i==j)
			{
				XRotMatrix[i][j]=1;
				YRotMatrix[i][j]=1;
				ZRotMatrix[i][j]=1;
			}			
			else
			{
				XRotMatrix[i][j]=0;
				YRotMatrix[i][j]=0;
				ZRotMatrix[i][j]=0;
			}

			RotationMatrix[i][j]=0;
			PrimaryRotationMatrix[i][j]=0;
		}
	}
	
	XRotMatrix[1][1]=cos(model->rot.x/180*3.142);				//Setting up the specifics of the OpenGL rotation matrices
	XRotMatrix[1][2]=sin(model->rot.x/180*3.142);
	XRotMatrix[2][1]=-sin(model->rot.x/180*3.142);
	XRotMatrix[2][2]=cos(model->rot.x/180*3.142);
	
	YRotMatrix[0][0]=cos(model->rot.y/180*3.142);
	YRotMatrix[0][2]=-sin(model->rot.y/180*3.142);
	YRotMatrix[2][0]=sin(model->rot.y/180*3.142);
	YRotMatrix[2][2]=cos(model->rot.y/180*3.142);
	
	ZRotMatrix[0][0]=cos(model->rot.z/180*3.142);
	ZRotMatrix[0][1]=sin(model->rot.z/180*3.142);
	ZRotMatrix[1][0]=-sin(model->rot.z/180*3.142);
	ZRotMatrix[1][1]=cos(model->rot.z/180*3.142);
	
	for (i=0; i<3; i++)   //Applying X*Y
	{
		for (j=0; j<3; j++)
		{
			for (k=0; k<3; k++)
			{
				PrimaryRotationMatrix[i][j]+=XRotMatrix[i][k]*YRotMatrix[k][j];
			}
		}
	}

	
	for (i=0; i<3; i++)  //Applying (X*Y)*Z
	{
		for (j=0; j<3; j++)
		{
			for (k=0; k<3; k++)
			{
				RotationMatrix[i][j]+=PrimaryRotationMatrix[i][k]*ZRotMatrix[k][j];
			}
		}
	}
		
	model->BBoxCoords[0][0]=model->MaxCoords.x*model->scale;	//initial Bounding Box Coordinates
	model->BBoxCoords[0][1]=model->MaxCoords.y*model->scale;
	model->BBoxCoords[0][2]=model->MaxCoords.z*model->scale;
	
	model->BBoxCoords[1][0]=model->MaxCoords.x*model->scale;
	model->BBoxCoords[1][1]=model->MaxCoords.y*model->scale;
	model->BBoxCoords[1][2]=model->MinCoords.z*model->scale;

	model->BBoxCoords[2][0]=model->MinCoords.x*model->scale;
	model->BBoxCoords[2][1]=model->MaxCoords.y*model->scale;
	model->BBoxCoords[2][2]=model->MaxCoords.z*model->scale;

	model->BBoxCoords[3][0]=model->MinCoords.x*model->scale;
	model->BBoxCoords[3][1]=model->MaxCoords.y*model->scale;
	model->BBoxCoords[3][2]=model->MinCoords.z*model->scale;

	model->BBoxCoords[4][0]=model->MaxCoords.x*model->scale;
	model->BBoxCoords[4][1]=model->MinCoords.y*model->scale;
	model->BBoxCoords[4][2]=model->MaxCoords.z*model->scale;
	
	model->BBoxCoords[5][0]=model->MaxCoords.x*model->scale;
	model->BBoxCoords[5][1]=model->MinCoords.y*model->scale;
	model->BBoxCoords[5][2]=model->MinCoords.z*model->scale;

	model->BBoxCoords[6][0]=model->MinCoords.x*model->scale;
	model->BBoxCoords[6][1]=model->MinCoords.y*model->scale;
	model->BBoxCoords[6][2]=model->MaxCoords.z*model->scale;

	model->BBoxCoords[7][0]=model->MinCoords.x*model->scale;
	model->BBoxCoords[7][1]=model->MinCoords.y*model->scale;
	model->BBoxCoords[7][2]=model->MinCoords.z*model->scale;
	
	float RotatedCoord[3];

	for (int count=0; count<8;count++)		//Multiplies the corners of the bounding box by the rotation Matrix
	{
		RotatedCoord[0]=0;  //sets initial coordinate back to zero
		RotatedCoord[1]=0;
		RotatedCoord[2]=0;

		for (j=0; j<3; j++)
		{
			for (k=0; k<3; k++)
			{
				RotatedCoord[j]+=model->BBoxCoords[count][k]*RotationMatrix[j][k];  //multiplies the bounding box by rotation matrix
			}
		}

		model->BBoxCoords[count][0]=RotatedCoord[0];	//Assigns the Rotated Coordinate to the Bounding Box Corner
		model->BBoxCoords[count][1]=RotatedCoord[1];
		model->BBoxCoords[count][2]=RotatedCoord[2];
	}

/*
	model->colMesh = new btTriangleMesh();
	btVector3 v0= btVector3(model->BBoxCoords[0][0],model->BBoxCoords[0][1],model->BBoxCoords[0][2]);
	btVector3 v1= btVector3(model->BBoxCoords[1][0],model->BBoxCoords[1][1],model->BBoxCoords[1][2]);
	btVector3 v2= btVector3(model->BBoxCoords[2][0],model->BBoxCoords[2][1],model->BBoxCoords[2][2]);
	btVector3 v3= btVector3(model->BBoxCoords[3][0],model->BBoxCoords[3][1],model->BBoxCoords[3][2]);
	btVector3 v4= btVector3(model->BBoxCoords[4][0],model->BBoxCoords[4][1],model->BBoxCoords[4][2]);
	btVector3 v5= btVector3(model->BBoxCoords[5][0],model->BBoxCoords[5][1],model->BBoxCoords[5][2]);
	btVector3 v6= btVector3(model->BBoxCoords[6][0],model->BBoxCoords[6][1],model->BBoxCoords[6][2]);
	btVector3 v7= btVector3(model->BBoxCoords[7][0],model->BBoxCoords[7][1],model->BBoxCoords[7][2]);

	model->colMesh->addTriangle(v0,v1,v2,false);
	model->colMesh->addTriangle(v3,v1,v2,false);

	model->colMesh->addTriangle(v0,v2,v6,false);
	model->colMesh->addTriangle(v0,v4,v6,false);

	model->colMesh->addTriangle(v0,v4,v5,false);
	model->colMesh->addTriangle(v0,v1,v5,false);

	model->colMesh->addTriangle(v3,v7,v6,false);
	model->colMesh->addTriangle(v3,v2,v6,false);

	model->colMesh->addTriangle(v1,v3,v7,false);
	model->colMesh->addTriangle(v1,v5,v7,false);

	model->colMesh->addTriangle(v5,v6,v7,false);
	model->colMesh->addTriangle(v5,v4,v6,false);*/

	model->roughRadius=(float)sqrt(model->MaxCoords.x*model->MaxCoords.x*model->scale*model->scale+model->MaxCoords.z*model->MaxCoords.z*model->scale*model->scale);
	
#if 0
/* normals */
if (model->numnormals) {
    numvectors = model->numnormals;
    vectors  = model->normals;
    copies = glmOptimizeVectors(vectors, &numvectors);
    
    printf("glmOptimize(): %d redundant normals.\n", 
        model->numnormals - numvectors);
    
    for (i = 0; i < model->numtriangles; i++) {
        T(i).nindices[0] = (GLuint)vectors[3 * T(i).nindices[0] + 0];
        T(i).nindices[1] = (GLuint)vectors[3 * T(i).nindices[1] + 0];
        T(i).nindices[2] = (GLuint)vectors[3 * T(i).nindices[2] + 0];
    }
    
    /* free space for old normals */
    free(vectors);
    
    /* allocate space for the new normals */
    model->numnormals = numvectors;
    model->normals = (GLfloat*)malloc(sizeof(GLfloat) * 
        3 * (model->numnormals + 1));
    
    /* copy the optimized vertices into the actual vertex list */
    for (i = 1; i <= model->numnormals; i++) {
        model->normals[3 * i + 0] = copies[3 * i + 0];
        model->normals[3 * i + 1] = copies[3 * i + 1];
        model->normals[3 * i + 2] = copies[3 * i + 2];
    }
    
    free(copies);
}

/* texcoords */
if (model->numtexcoords) {
    numvectors = model->numtexcoords;
    vectors  = model->texcoords;
    copies = glmOptimizeVectors(vectors, &numvectors);
    
    printf("glmOptimize(): %d redundant texcoords.\n", 
        model->numtexcoords - numvectors);
    
    for (i = 0; i < model->numtriangles; i++) {
        for (j = 0; j < 3; j++) {
            T(i).tindices[j] = (GLuint)vectors[3 * T(i).tindices[j] + 0];
        }
    }
    
    /* free space for old texcoords */
    free(vectors);
    
    /* allocate space for the new texcoords */
    model->numtexcoords = numvectors;
    model->texcoords = (GLfloat*)malloc(sizeof(GLfloat) * 
        2 * (model->numtexcoords + 1));
    
    /* copy the optimized vertices into the actual vertex list */
    for (i = 1; i <= model->numtexcoords; i++) {
        model->texcoords[2 * i + 0] = copies[2 * i + 0];
        model->texcoords[2 * i + 1] = copies[2 * i + 1];
    }
    
    free(copies);
}
#endif

#if 0
/* look for unused vertices */
/* look for unused normals */
/* look for unused texcoords */
for (i = 1; i <= model->numvertices; i++) {
    for (j = 0; j < model->numtriangles; i++) {
        if (T(j).vindices[0] == i || 
            T(j).vindices[1] == i || 
            T(j).vindices[1] == i)
            break;
    }
}
#endif
}
GLvoid glmMovePercent(MapModel* model, double movePercent)
{

	if (movePercent>100)
		movePercent=100;
	if (movePercent<0)
		movePercent=0;

	model->currentPercentage = movePercent;
	movePercent /= 100.0f;

	model->rot.x=model->initRot.x+(model->endRot.x-model->initRot.x)*movePercent;
	model->rot.y=model->initRot.y+(model->endRot.y-model->initRot.y)*movePercent;
	model->rot.z=model->initRot.z+(model->endRot.z-model->initRot.z)*movePercent;

	model->pos.x=model->initPos.x+(model->endPos.x-model->initPos.x)*movePercent;
	model->pos.y=model->initPos.y+(model->endPos.y-model->initPos.y)*movePercent;
	model->pos.z=model->initPos.z+(model->endPos.z-model->initPos.z)*movePercent;

	model->scale=model->initScale+(model->endScale-model->initScale)*movePercent;
	
	if(model->collide)
		glmCalcBounds(model);
}
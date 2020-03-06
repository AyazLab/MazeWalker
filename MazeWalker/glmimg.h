#include <string.h>
#include <math.h>
#include "main.h"
#include "GL/glew.h"


#ifndef _GLMIMG

GLuint glmLoadTexture(char *filename);
GLuint glmLoadTexture(char *filename, GLboolean alpha, GLboolean repeat, GLboolean filtering, GLboolean mipmaps, GLfloat *texcoordwidth, GLfloat *texcoordheight);

#endif
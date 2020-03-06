//#include "stdafx.h"



#include "glmimg.h"
#include "texture.h"
#ifndef GL_BGR
#define GL_BGR GL_BGR_EXT
#endif

#ifndef GL_BGRA
#define GL_BGRA GL_BGRA_EXT
#endif
#define TEXTBOXSTYLE_WARNING 3
static GLint gl_max_texture_size;

GLuint glmLoadTexture(char *filename)
{
	return glmLoadTexture(filename, GL_TRUE, GL_TRUE,GL_TRUE,GL_TRUE,NULL,NULL);
}
GLuint glmLoadTexture(char *filename, GLboolean alpha, GLboolean repeat, GLboolean filtering, GLboolean mipmaps, GLfloat *texcoordwidth, GLfloat *texcoordheight)
{
    GLuint tex;
    int width, height,pixelsize;
    int type;
    int filter_min, filter_mag;
    GLubyte *data;
    int xSize2, ySize2;
	char err[800];

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);

    while (*filename==' ') filename++;
	Texture ttt;

	int x =(int)strlen(filename);
	char *extension = filename;
    if (extension[x-4]=='.')
		extension=extension+(x-3);
	else if (extension[x-5]=='.')
		extension=extension+(x-4);

		if (extension[0]=='J')
			extension[0]='j';
		if (extension[0]=='P')
			extension[0]='p';
		if (extension[1]=='N')
			extension[1]='n';
		if (extension[2]=='G')
			extension[2]='g';
		if (extension[0]=='B')
			extension[0]='b';
		if (extension[0]=='T')
			extension[0]='t';
		if (extension[1]=='M')
			extension[1]='m';
		if (extension[1]=='P')
			extension[1]='p';
		if (extension[1]=='G')
			extension[1]='g';
		if (extension[2]=='A')
			extension[2]='a';
		if (extension[0]=='P')
			extension[0]='p';
		if (extension[2]=='E'||extension[2]=='G'||extension[2]=='e')
			extension[2]='g';


		if (extension[0]=='j'&&extension[1]=='p'&&extension[2]=='g')
		{
			if (!LoadJPG(tex,filename))
			{			
				
				sprintf(err,"Could not Load Texture %s!\nInvalid file name or invalid JPG file.",filename);
				GUIMessageBox(err, 0, TEXTBOXSTYLE_WARNING);
				return -1;
			}
			else
				return tex;
		}
		else if (extension[0]=='p'&&extension[1]=='n'&&extension[2]=='g')
		{
			if (!LoadPNG(tex,filename))
			{			
				sprintf(err,"Could not Load Texture\n%s!\nInvalid file name or invalid PNG file.",filename);
				GUIMessageBox(err, 0, TEXTBOXSTYLE_WARNING);
				return -1;
			}
			else
				return tex;
		}
	
	else if (extension[0]=='b'&&extension[1]=='m'&&extension[2]=='p')
	{
			if (!LoadBMP(tex,filename))
				{			
					sprintf(err,"Could not Load Texture\n%s!\nInvalid file name or invalid BMP file.",filename);
					GUIMessageBox(err, 0, TEXTBOXSTYLE_WARNING);
					return -1;
				}
				else
					return tex;
	}

	else if (extension[0]=='t'&&extension[1]=='g'&&extension[2]=='a')
	{
		if(!(LoadTGA(&ttt,(char *)filename)))
		{
			sprintf(err,"Could not Load Texture\n%s!\nInvalid file name or invalid TGA file.",filename);
			GUIMessageBox(err, 0, TEXTBOXSTYLE_WARNING);
			return -1;
		}
	}
	else
	{
			sprintf(err,"Could not Load Texture\n%s!\n%s is not a valid file type.",filename,extension);
			GUIMessageBox(err, 0, TEXTBOXSTYLE_WARNING);
			return tex;
	}
	data = ttt.imageData;
	width = ttt.width;
	height = ttt.height;
	type = ttt.type;
	
    if(data == NULL) 
	{
		sprintf(err,"Could not Load Texture\n%s!",filename);
		GUIMessageBox(err, 0, TEXTBOXSTYLE_WARNING);
		return -1;
    }


    switch(type) {
    case GL_LUMINANCE:
	pixelsize = 1;
	break;
    case GL_RGB:
    case GL_BGR:
	pixelsize = 3;
	break;
    case GL_RGBA:
    case GL_BGRA:
	pixelsize = 4;
	break;
    default:
	printf( "glmLoadTexture(): unknown type 0x%x", type);
	pixelsize = 0;
	break;
    }

    if((pixelsize*width) % 4 == 0)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    else
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    xSize2 = width;
	ySize2 = height;


	

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);   
    
    if(filtering) 
	{
		filter_min = (mipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
		filter_mag = GL_LINEAR;
    }
    else 
	{
		filter_min = (mipmaps) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
		filter_mag = GL_NEAREST;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (repeat) ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (repeat) ? GL_REPEAT : GL_CLAMP);

		
	gluBuild2DMipmaps(GL_TEXTURE_2D, type, xSize2, ySize2, type, GL_UNSIGNED_BYTE, data);        

   
    
    free(data);

    
	/**texcoordwidth = 1.;		// texcoords are in [0,1]
	*texcoordheight = 1.;*/
    
    if(texcoordwidth!=NULL)
	{
		*texcoordwidth = xSize2;		// size of texture coords 
	}
	if(texcoordheight!=NULL)
	{ 
		*texcoordheight = ySize2;
	}
   
    return tex;
}

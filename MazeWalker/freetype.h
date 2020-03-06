#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>

//OpenGL Headers 
#include <windows.h>		//(the GL headers need it)
#include <GL/gl.h>
#include <GL/glu.h>
//#include "Libs/freetype/include/trigon.h"
//Some STL headers
#include <vector>
#include <string>

//Using the STL exception library increases the
//chances that someone else using our code will corretly
//catch any exceptions that we throw.
#include <stdexcept>


//MSVC will spit out all sorts of useless warnings if
//you create vectors of strings, this pragma gets rid of them.
#pragma warning(disable: 4786) 

///Wrap everything in a namespace, that we can use common
///function names like "print" without worrying about
///overlapping with anyone else's code.
namespace freetype {

	//Inside of this namespace, give ourselves the ability
	//to write just "vector" instead of "std::vector"
	using std::vector;

	//Ditto for string.
	using std::string;

	const int fontoffset = 30;

	//This holds all of the information related to any
	//freetype font that we want to create.  
	struct font_data {
		float h;			///< Holds the height of the font.
		GLuint * textures;	///< Holds the texture id's 
		GLuint list_base;	///< Holds the first display list id
		FT_Face face;
		FT_Library library;
		const int dictLength = 66000;
		
		bool* inLib;
		int* charLenLib;

		//The init function will create a font of
		//of the height h from the file fname.
		void init(const char * fname, unsigned int h);
		
		int extend(wchar_t* wstr);
		int getPixelLength(wchar_t* wstr);

		//Free all the resources assosiated with the font.
		void clean();
		void releaseLib();
	};

	void ascii2utf8(wchar_t* out, char* text, const int tsize);

	//The flagship function of the library - this thing will print
	//out text at window coordinates x,y, using the font ft_font.
	//The current modelview matrix will also be applied to the text. 
	
	void print(font_data &ft_font, float x, float y, int allign, wchar_t *fmt, ...);

}
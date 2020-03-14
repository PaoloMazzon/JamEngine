#include "JamError.h"
#include "Font.h"
#include <malloc.h>
#include <Font.h>
#include <ft2build.h>
#include <freetype/freetype.h>

// The freetype library
static FT_Library gFontLib;

// Weather or not freetype has been initialized
static bool gFontLibInitialized;

///////////////////////////////////////////////////////////
JamFont* jamFontCreate(const char* filename) {
	if (!gFontLibInitialized) {
		FT_Error err = FT_Init_FreeType(&gFontLib);
		if (err)
			jSetError(ERROR_FREETYPE_ERROR, "FreeType could not be initialized, error code=%i", err);
		else
			gFontLibInitialized = true;
	}

	JamFont* newFont = NULL;

	if (gFontLibInitialized) {
		newFont = (JamFont*)malloc(sizeof(JamFont));

		if (newFont != NULL) {
			FT_New_Face(gFontLib, filename, 0, newFont->fontFace);
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create font");
		}
	}

	return newFont;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamFontSetSize(JamFont* font, int size) {
	FT_Error err;
	if (font != NULL && gFontLibInitialized) {
		// For the sake of this function producing the same size font
		// across every system (such is what you want in a pixel-perfect
		// game), FT_Set_Char_Size is not given the actual system DPI.
		// This is a conscious choice that can be changed without too much
		// effort since the renderer can provide that information.
		err = FT_Set_Char_Size(font->fontFace, 0, size, 300, 300);

		if (err != 0)
			jSetError(ERROR_FREETYPE_ERROR, "An error (%i) occurred while setting font size", err);
	} else {
		if (font == NULL)
			jSetError(ERROR_NULL_POINTER, "Font does not exist");
		if (!gFontLibInitialized)
			jSetError(ERROR_FREETYPE_ERROR, "FreeType has not been initialized");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamFontRender(JamFont* font, int x, int y, const char* string) {
	FT_Error err;
	if (font != NULL && gFontLibInitialized) {
		// TODO: This
	} else {
		if (font == NULL)
			jSetError(ERROR_NULL_POINTER, "Font does not exist");
		if (!gFontLibInitialized)
			jSetError(ERROR_FREETYPE_ERROR, "FreeType has not been initialized");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamFontFree(JamFont* font) {
	if (font != NULL) {
		FT_Done_Face(font->fontFace);
		free(font);
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamFontQuit() {
	if (gFontLibInitialized) {
		FT_Done_FreeType(gFontLib);
		gFontLibInitialized = false;
	}
}
///////////////////////////////////////////////////////////
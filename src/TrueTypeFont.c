#include "JamError.h"
#include "Font.h"
#include <malloc.h>
#include <Font.h>

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
			FT_New_Face(gFontLib, filename, 0, &newFont->fontFace);
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create font");
		}
	}

	return newFont;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamFontFree(JamFont* font) {
	free(font);
}
///////////////////////////////////////////////////////////
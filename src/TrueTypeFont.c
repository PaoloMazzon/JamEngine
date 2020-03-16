#include "JamError.h"
#include "Font.h"
#include <malloc.h>
#include <Font.h>
#include <StringUtil.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <Texture.h>

// The freetype library
static FT_Library gFontLib;

// Weather or not freetype has been initialized
static bool gFontLibInitialized;

///////////// Functions that manage the "hidden" struct _JamFontRangeCache /////////////
// To spare confusion on the whole inclusive-range-to-array-index bit
static inline uint32 _rangeEndToPureEnd(uint32 rangeStart, uint32 rangeEnd) {
	return (rangeEnd - rangeStart + 1);
}

// This function creates the x amount of array slots but does not so much as NULL them
// so it is important you fill them right away.
static _JamFontRangeCache* _createFontRange(uint32 rangeStart, uint32 rangeEnd) {
	_JamFontRangeCache* range = (_JamFontRangeCache*)malloc(sizeof(_JamFontRangeCache));
	JamTexture** texArray = (JamTexture**)malloc(sizeof(JamTexture*) * _rangeEndToPureEnd(rangeStart, rangeEnd));

	if (range != NULL && texArray != NULL) {
		range->characters = texArray;
		range->rangeStart = rangeStart;
		range->rangeEnd = rangeEnd;
	} else {
		free(range);
		free(texArray);
		range = NULL;
	}

	return range;
}

static void _freeFontRange(_JamFontRangeCache* fontRange) {
	uint32 i;
	if (fontRange != NULL) {
		for (i = 0; i < _rangeEndToPureEnd(fontRange->rangeStart, fontRange->rangeEnd); i++)
			jamTextureFree(fontRange->characters[i]);
		free(fontRange);
	}
}
////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
JamFont* jamFontCreate(const char* filename, int size, bool preloadASCII) {
	FT_Error err;
	if (!gFontLibInitialized) {
		err = FT_Init_FreeType(&gFontLib);
		if (err)
			jSetError(ERROR_FREETYPE_ERROR, "FreeType could not be initialized, error code=%i", err);
		else
			gFontLibInitialized = true;
	}

	JamFont* newFont = NULL;

	if (gFontLibInitialized) {
		newFont = (JamFont*)malloc(sizeof(JamFont));

		if (newFont != NULL) {
			err = FT_New_Face(gFontLib, filename, 0, newFont->fontFace);

			if (!err) {
				// For the sake of this function producing the same size font
				// across every system (such is what you want in a pixel-perfect
				// game), FT_Set_Char_Size is not given the actual system DPI.
				// This is a conscious choice that can be changed without too much
				// effort since the renderer can provide that information.
				err = FT_Set_Char_Size(newFont->fontFace, 0, size, 300, 300);

				if (preloadASCII)
					jamFontPreloadRange(newFont, 32, 127);

				if (err) {
					jSetError(ERROR_FREETYPE_ERROR, "Failed to set character size, error code=%i", err);
				}
			} else {
				jSetError(ERROR_FREETYPE_ERROR, "Failed to create font face, error code=%i", err);
			}
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create font");
		}
	}

	return newFont;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamFontPreloadRange(JamFont* font, uint32 rangeStart, uint32 rangeEnd) {
	if (font != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_NULL_POINTER, "Font does not exist");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamFontRender(JamFont* font, int x, int y, const char* string) {
	FT_Error err;
	int pos = 0;
	uint32 c = jamStringNextUnicode(string, &pos);
	if (font != NULL && gFontLibInitialized) {
		while (err == 0 && c != 0) {
			FT_Load_Glyph(font->fontFace, FT_Get_Char_Index(font->fontFace, c), 0);

			// TODO: Render text using current glyph

			c = jamStringNextUnicode(string, &pos);
		}
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
#include "JamError.h"
#include "Font.h"
#include <malloc.h>
#include <Font.h>
#include <StringUtil.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Texture.h>
#include <SDL.h>
#include <Drawing.h>

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
			newFont->ranges = 0;

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
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	int shift = (req_format == STBI_rgb) ? 8 : 0;
  	rmask = 0xff000000 >> shift;
  	gmask = 0x00ff0000 >> shift;
  	bmask = 0x0000ff00 >> shift;
  	amask = 0x000000ff >> shift;
#else // little endian, like x86
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif


	uint32 i;
	FT_Bitmap bitmap;
	_JamFontRangeCache* range;
	SDL_Surface* surf;
	_JamFontRangeCache** newRanges;

	if (font != NULL) {
		range = _createFontRange(rangeStart, rangeEnd);
		newRanges = (_JamFontRangeCache**)realloc(font->ranges, sizeof(_JamFontRangeCache*) * (font->rangeCount + 1));

		if (range != NULL && newRanges != NULL) {
			range->rangeStart = rangeStart;
			range->rangeEnd = rangeEnd;
			font->ranges[font->rangeCount] = range;
			font->rangeCount++;

			for (i = rangeStart; i <= rangeEnd; i++) {
				// 1. Load glyph
				// 2. Create surface with glyph bitmap
				// 3. Create texture from surface and send it off to jamTextureCreateFromTex
				FT_Load_Glyph(font->fontFace, FT_Get_Char_Index(font->fontFace, i), 0);
				bitmap = ((FT_Face) font->fontFace)->glyph->bitmap;

				surf = SDL_CreateRGBSurfaceFrom(bitmap.buffer,
										 bitmap.width,
										 bitmap.rows,
										 3,
										 bitmap.pitch,
										 0,
										 0,
										 0,
										 0);

				if (surf != NULL) {
					range->characters[i - rangeStart] = jamTextureCreateFromTex(
							SDL_CreateTextureFromSurface(
									jamRendererGetInternalRenderer(),
									surf
							));

					SDL_FreeSurface(surf);
				} else {
					jSetError(ERROR_SDL_ERROR, "Failed to create surface from FreeType bitmap");
					range->characters[i - rangeStart] = NULL;
				}
			}
		} else {
			if (range == NULL)
				jSetError(ERROR_ALLOC_FAILED, "Failed to create font range");
			if (newRanges == NULL)
				jSetError(ERROR_REALLOC_FAILED, "Failed to resize range list");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Font does not exist");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamFontRender(JamFont* font, int x, int y, const char* string) {
	FT_Error err;
	int pos = 0;
	int i;
	int xx = x;
	uint32 c = jamStringNextUnicode(string, &pos);
	if (font != NULL && gFontLibInitialized) {
		while (err == 0 && c != 0) {
			for (i = 0; i < font->rangeCount; i++) {
				if (c >= font->ranges[i]->rangeStart && c <= font->ranges[i]->rangeEnd) {
					jamDrawTexture(font->ranges[i]->characters[c], xx, y);
					xx += font->ranges[i]->characters[c]->w;
				}
			}

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
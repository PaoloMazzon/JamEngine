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
			err = FT_New_Face(gFontLib, filename, 0, (FT_Face*)&newFont->fontFace);
			newFont->ranges = NULL;
			newFont->rangeCount = 0;

			if (!err) {
				// For the sake of this function producing the same size font
				// across every system (such is what you want in a pixel-perfect
				// game), FT_Set_Char_Size is not given the actual system DPI.
				// This is a conscious choice that can be changed without too much
				// effort since the renderer can provide that information.
				err = FT_Set_Char_Size(newFont->fontFace, size, size, 300, 300);

				if (preloadASCII)
					jamFontPreloadRange(newFont, 33, 126);

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
	SDL_Texture* tex;
	_JamFontRangeCache** newRanges;
	bool error = false;
	FT_Error err;

	if (font != NULL) {
		range = _createFontRange(rangeStart, rangeEnd);
		newRanges = (_JamFontRangeCache**)realloc(font->ranges, sizeof(_JamFontRangeCache*) * (font->rangeCount + 1));

		if (range != NULL && newRanges != NULL) {
			range->rangeStart = rangeStart;
			range->rangeEnd = rangeEnd;
			font->ranges = newRanges;
			font->ranges[font->rangeCount] = range;
			font->rangeCount++;

			for (i = rangeStart; i <= rangeEnd; i++) {
				// 1. Load glyph (and most likely render it)
				// 2. Create texture with necessary attributes
				// 3. Copy bitmap data to texture
				err = FT_Load_Char(font->fontFace, i, FT_LOAD_RENDER);
				bitmap = ((FT_Face) font->fontFace)->glyph->bitmap;

				tex = SDL_CreateTexture(jamRendererGetInternalRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, bitmap.width, bitmap.rows);

				// This code is from an example and is going to be cleaned up
				// Credit to this kind fellow https://github.com/wutipong
				void* buffer;
				int pitch;
				SDL_LockTexture(tex, NULL, &buffer, &pitch);

				unsigned char *src_pixels = bitmap.buffer;
				unsigned int *target_pixels = buffer;

				SDL_PixelFormat* pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

				for (int y = 0; y < bitmap.rows; y++)
				{
					for (int x = 0; x < bitmap.width; x++)
					{
						int index = (y * bitmap.width) + x;

						unsigned char alpha = src_pixels[index];
						unsigned int pixel_value =
								SDL_MapRGBA(pixel_format, 255, 255, 255, alpha);

						target_pixels[index] = pixel_value;
					}
				}

				SDL_FreeFormat(pixel_format);
				SDL_UnlockTexture(tex);

				if (tex != NULL) {
					range->characters[i - rangeStart] = jamTextureCreateFromTex(tex);
				} else {
					if (!error)
						jSetError(ERROR_SDL_ERROR, "Failed to create surface from FreeType bitmap FT Error=%i SDL Error=%s", err, SDL_GetError());
					error = true;
					range->characters[i - rangeStart] = NULL;
				}
			}

			// If there was an error we abandon all progress and return null
			if (error) {
				_freeFontRange(range);
				font->ranges = (_JamFontRangeCache**)realloc(font->ranges, sizeof(_JamFontRangeCache*) * (font->rangeCount - 1));
				font->rangeCount--;
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
	FT_Error err = 0;
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
	int i;
	if (font != NULL) {
		for (i = 0; i < font->rangeCount; i++)
			_freeFontRange(font->ranges[i]);
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
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

////////// Helper functions (These guys don't make sure font is not NULL) ///////////

// This returns how far to the right the cursor should move
static sint32 _jamDrawCharacter(JamFont* font, uint32 c, sint32 x, sint32 y) {
	int i;
	sint32 cursorMove = 0;
	SDL_Rect dest;
	_JamFontTexture* tex;
	if (c != 0) {
		for (i = 0; i < font->rangeCount; i++) {
			if (c == 32) {
				cursorMove = font->space;
			} else if (c >= font->ranges[i]->rangeStart && c <= font->ranges[i]->rangeEnd) {
				tex = font->ranges[i]->characters[c - font->ranges[i]->rangeStart];
				dest.x = x;
				dest.y = y + font->height - tex->yOffset;
				dest.w = tex->w;
				dest.h = tex->h;
				SDL_RenderCopy(jamRendererGetInternalRenderer(), tex->tex, NULL, &dest);
				cursorMove = tex->advance;
			}
		}
	}
	return cursorMove;
}

///////////// Functions that manage the "hidden" struct _JamFontTexture /////////////

static _JamFontTexture* _jamFontTextureCreate(void* texture, sint32 yOffset, sint32 advance) {
	_JamFontTexture* tex = (_JamFontTexture*)malloc(sizeof(_JamFontTexture));

	if (tex != NULL && texture != NULL) {
		tex->tex = texture;
		tex->yOffset = yOffset;
		tex->advance = advance;
		SDL_QueryTexture(texture, NULL, NULL, &tex->w, &tex->h);

	} else {
		if (tex == NULL)
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate JamTexture. SDL Error: %s\n", SDL_GetError());
		if (texture == NULL)
			jSetError(ERROR_NULL_POINTER, "Texture does not exist");
	}

	return tex;
}

static void _jamFontTextureFree(_JamFontTexture* tex) {
	if (tex != NULL) {
		SDL_DestroyTexture(tex->tex);
		free(tex);
	}
}

///////////// Functions that manage the "hidden" struct _JamFontRangeCache /////////////

// To spare confusion on the whole inclusive-range-to-array-index bit
static inline uint32 _rangeEndToPureEnd(uint32 rangeStart, uint32 rangeEnd) {
	return (rangeEnd - rangeStart + 1);
}

// This function creates the x amount of array slots but does not so much as NULL them
// so it is important you fill them right away.
static _JamFontRangeCache* _createFontRange(uint32 rangeStart, uint32 rangeEnd) {
	_JamFontRangeCache* range = (_JamFontRangeCache*)malloc(sizeof(_JamFontRangeCache));
	_JamFontTexture** texArray = (_JamFontTexture**)malloc(sizeof(_JamFontTexture*) * _rangeEndToPureEnd(rangeStart, rangeEnd));

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
			_jamFontTextureFree(fontRange->characters[i]);
		free(fontRange);
	}
}
////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
JamFont* jamFontCreate(const char* filename, uint32 size, bool preloadASCII) {
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
				err = FT_Set_Pixel_Sizes(newFont->fontFace, 0, size);

				// Various metrics
				FT_Load_Char(newFont->fontFace, 32, 0);
				newFont->space = (sint32)((FT_Face)newFont->fontFace)->glyph->linearHoriAdvance / 65536;
				newFont->height = (sint32)((FT_Face)newFont->fontFace)->size->metrics.height >> 6;

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
	uint32 i;
	FT_Bitmap bitmap;
	_JamFontRangeCache* range;
	SDL_Texture* tex;
	_JamFontRangeCache** newRanges;
	bool error = false;
	FT_Error err;
	unsigned char *srcPixels;
	unsigned int *targetPixels;
	int index, x, y;

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
				SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);

				// Credit to this kind fellow https://github.com/wutipong
				// For most of the following code before the if statement
				// (Which I modified for the purpose of making it fit the
				// code style of the engine)
				void* buffer;
				int pitch;
				SDL_LockTexture(tex, NULL, &buffer, &pitch);

				srcPixels = bitmap.buffer;
				targetPixels = (unsigned int*)buffer;

				SDL_PixelFormat* pixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

				for (y = 0; y < bitmap.rows; y++) {
					for (x = 0; x < bitmap.width; x++) {
						index = (y * bitmap.width) + x;
						targetPixels[index] = SDL_MapRGBA(pixelFormat, 255, 255, 255, srcPixels[index]);
					}
				}

				SDL_FreeFormat(pixelFormat);
				SDL_UnlockTexture(tex);

				if (tex != NULL) {
					range->characters[i - rangeStart] = _jamFontTextureCreate(
							tex,
							((FT_Face) font->fontFace)->glyph->bitmap_top,
							(sint32)((FT_Face) font->fontFace)->glyph->linearHoriAdvance / 65536);
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
	int xx = x;
	uint32 c = jamStringNextUnicode(string, &pos);
	if (font != NULL && gFontLibInitialized) {
		while (c != 0) {
			xx += _jamDrawCharacter(font, c, xx, y);
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
void jamFontRenderExt(JamFont* font, int x, int y, const char* string, int w) {
	// TODO: This
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
sint32 jamFontWidth(JamFont* font, const char* string) {
	int i;
	int pos = 0;
	uint32 c;
	sint32 w = 0;
	sint32 prevW = 0;
	sint32 prevAdvance = 0;
	if (font != NULL) {
		c = jamStringNextUnicode(string, &pos);
		while (c != 0) {
			if (c == 32) {
				w += font->space;
			} else {
				for (i = 0; i < font->rangeCount; i++) {
					if (c >= font->ranges[i]->rangeStart && c <= font->ranges[i]->rangeEnd) {
						prevAdvance = font->ranges[i]->characters[c - font->ranges[i]->rangeStart]->advance;
						w += prevAdvance;
						prevW = font->ranges[i]->characters[c - font->ranges[i]->rangeStart]->w;
					}
				}
			}
			c = jamStringNextUnicode(string, &pos);
		}

		// For the last character we don't need the character advance,
		// we need to instead know the width of the last character's
		// texture.
		w -= prevAdvance;
		w += prevW;
	}

	return w;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
sint32 jamFontHeight(JamFont* font) {
	if (font != NULL) {
		return font->height;
	} else {
		jSetError(ERROR_NULL_POINTER, "Font does not exist");
	}

	return 0;
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
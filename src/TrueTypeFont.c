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
#include <Sprite.h>

// The freetype library
static FT_Library gFontLib;

// Weather or not freetype has been initialized
static bool gFontLibInitialized;

////////// Helper functions (These guys don't make sure font is not NULL) ///////////

// To spare confusion on the whole inclusive-range-to-array-index bit
static inline uint32 _rangeEndToPureEnd(uint32 rangeStart, uint32 rangeEnd) {
	return (rangeEnd - rangeStart + 1);
}


// Finds the texture associated with a character (or NULL)
static _JamFontTexture* _jamGetCharacterTex(JamFont* font, uint32 c) {
	int i;
	_JamFontTexture* tex = NULL;
	if (c != 0) {
			for (i = 0; i < font->rangeCount; i++) {
				if (c >= font->ranges[i]->rangeStart && c <= font->ranges[i]->rangeEnd) {
					tex = font->ranges[i]->characters[c - font->ranges[i]->rangeStart];
			}
		}
	}

	return tex;
}

// Easily draws a character's texture
static void _jamRenderCharacter(JamFont* font, _JamFontTexture* tex, sint32 x, sint32 y, sint32 height, uint8 r, uint8 g, uint8 b) {
	SDL_Rect dest, src;

	if (!font->bitmap) {
		dest.x = x;
		dest.y = y - tex->yOffset + height;
		dest.w = tex->w;
		dest.h = tex->h;
		SDL_SetTextureColorMod(tex->tex, r, g, b);
		SDL_RenderCopy(jamRendererGetInternalRenderer(), tex->tex, NULL, &dest);
	} else {
		dest.x = x;
		dest.y = y;
		dest.w = font->width;
		dest.h = font->height;
		src.x = tex->xInTex;
		src.y = tex->yInTex;
		SDL_SetTextureColorMod(font->fontTex, r, g, b);
		SDL_RenderCopy(jamRendererGetInternalRenderer(), font->fontTex, &src, &dest);
	}
}

// Simply draws a character and returns how far forward to move the cursor
static sint32 _jamDrawCharacter(JamFont* font, uint32 c, sint32 x, sint32 y, uint8 r, uint8 g, uint8 b) {
	if (c != 32) {
		_JamFontTexture *tex = _jamGetCharacterTex(font, c);
		if (tex != NULL) {
			_jamRenderCharacter(font, tex, x, y, font->height, r, g, b);
			return !font->bitmap ? tex->advance : font->width;
		}
		return 0;
	} else {
		return font->space;
	}
}

// All of the above combined for one fancy function to draw characters
static sint32 _jamDrawCharacterComplete(JamFont* font, uint32 c, sint32* x, sint32* y, sint32 baseX, int w, uint8 r, uint8 g, uint8 b) {
	_JamFontTexture* tex;

	if (c != '\n') {
		// If we have a left limit, we check if the following character will go past before drawing
		if (w != 0) {
			if (c != 32) {
				tex = _jamGetCharacterTex(font, c);
				if (tex != NULL) {
					if (tex->w + *x > baseX + w) {
						*y += font->height;
						*x = baseX;
					}
					_jamRenderCharacter(font, tex, *x, *y, font->height, r, g, b);
					*x += !font->bitmap ? tex->advance : font->width;
				}
			} else {
				*x += font->space;
			}
		} else {
			*x += _jamDrawCharacter(font, c, *x, *y, r, g, b);
		}
	} else {
		*y += font->height;
		*x = baseX;
	}
}

// Finds the max height of ascii characters and returns that height
static sint32 _jamMaxASCIIHeight(JamFont* font) {
	int i;
	sint32 max = 0;
	if (font->rangeCount == 1)
		for (i = 0; i < _rangeEndToPureEnd(font->ranges[0]->rangeStart, font->ranges[0]->rangeEnd); i++)
			max = (font->ranges[0]->characters[i]->h > max) ? font->ranges[0]->characters[i]->h : max;
	return max;
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

static void _jamFontTextureFree(JamFont* font, _JamFontTexture* tex) {
	if (tex != NULL) {
		if (!font->bitmap)
			SDL_DestroyTexture(tex->tex);
		free(tex);
	}
}

///////////// Functions that manage the "hidden" struct _JamFontRangeCache /////////////

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

static void _freeFontRange(JamFont* font, _JamFontRangeCache* fontRange) {
	uint32 i;
	if (fontRange != NULL) {
		for (i = 0; i < _rangeEndToPureEnd(fontRange->rangeStart, fontRange->rangeEnd); i++)
			_jamFontTextureFree(font, fontRange->characters[i]);
		free(fontRange->characters);
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
				newFont->space = (sint32)round((double)((FT_Face)newFont->fontFace)->glyph->linearHoriAdvance / 65536.0f);
				newFont->height = (sint32)((FT_Face)newFont->fontFace)->size->metrics.height >> 6;
				newFont->bitmap = false;

				if (preloadASCII) {
					jamFontPreloadRange(newFont, 33, 126);
					newFont->height = _jamMaxASCIIHeight(newFont);
				}

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
JamFont* jamFontCreateFromBitmap(const char* filename, uint32 characterWidth, uint32 characterHeight) {
	JamFont* newFont = (JamFont*)malloc(sizeof(JamFont));

	if (newFont != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate font");
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

	if (font != NULL && font->bitmap == false) {
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
							(sint32)round((double)((FT_Face) font->fontFace)->glyph->linearHoriAdvance / 65536.0f));
				} else {
					if (!error)
						jSetError(ERROR_SDL_ERROR, "Failed to create surface from FreeType bitmap FT Error=%i SDL Error=%s", err, SDL_GetError());
					error = true;
					range->characters[i - rangeStart] = NULL;
				}
			}

			// If there was an error we abandon all progress and return null
			if (error) {
				_freeFontRange(font, range);
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
		if (font == NULL)
			jSetError(ERROR_NULL_POINTER, "Font does not exist");
		else
			jSetError(ERROR_NULL_POINTER, "Font is not a true type font");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
/*
 * There is actually only one font rendering function (this
 * one), all of the ones you would typically call are just
 * macros that simplify calls to this beast of a function.
 * You could technically call this on your own but this
 * function has a lot of side effects that if you don't account
 * for on your own you may get wacky results.
 */
void _jamFontRenderDetailed(JamFont* font, int x, int y, const char* string, int w, uint8 r, uint8 g, uint8 b, ...) {
	jamRendererCalculateForCamera(&x, &y);
	FT_Error err = 0;
	int pos = 0;
	int xx = x;
	int potentialAdvance;
	_JamFontTexture* tex;
	uint32 c = jamStringNextUnicode(string, &pos);

	// Inline strings and such insertion variables
	const char* buffer = "";
	bool deleteBuffer = false;
	int posInBuffer = -1;
	uint32 prevC = 0;
	va_list va;
	va_start(va, b);
	JamColour* col;
	JamSprite* spr;

	if (font != NULL && gFontLibInitialized) {
		while (c != 0) {
			// Process opcodes within the text
			if (posInBuffer != -1 || (prevC != '%' && c != '%') || (prevC == '%' && c == '%')) {
				_jamDrawCharacterComplete(font, c, &xx, &y, x, w, r, g, b);

				// So we don't double trigger
				if (prevC == '%') {
					prevC = 0;
					if (c == '%')
						c = 0;
				}
			} else if (prevC == '%') { // Insert a string or character or float
				if (c == 'f') { // Real number
					buffer = ftoa(va_arg(va, double));
					if (buffer != NULL) {
						posInBuffer = 0;
						deleteBuffer = true;
					} else {
						jSetError(ERROR_WARNING, "Failed to ftoa a number");
					}
				} else if (c == 'c') { // Character
					_jamDrawCharacterComplete(font, c, &xx, &y, x, w, r, g, b);
				} else if (c == 's') { // String
					posInBuffer = 0;
					buffer = va_arg(va, const char*);
				} else if (c == 'C') { // Change of colour
					col = va_arg(va, JamColour*);
					r = col->r;
					g = col->g;
					b = col->b;
				} else if (c == 'S') { // Inline sprite
					spr = va_arg(va, JamSprite*);
					if (spr != NULL) {
						if (w != 0 && xx + spr->width > x + w) {
							xx = x;
							y += font->height;
						}
						jamDrawSprite(spr, xx + (sint32)jamRendererGetCameraX(), y + (sint32)jamRendererGetCameraY(), 1, 1, 0, 255, true);
						xx += spr->width;
					}
				}
			}

			// Advance to the next character
			if (posInBuffer == -1) {
				prevC = c;
				c = jamStringNextUnicode(string, &pos);
			} else {
				// We pull instead from the buffer, and switch back if the well is dry
				c = jamStringNextUnicode(buffer, &posInBuffer);
				if (c == 0) {
					posInBuffer = -1;
					prevC = 0;
					c = jamStringNextUnicode(string, &pos);
					if (deleteBuffer) {
						deleteBuffer = false;
						free((void*)buffer);
					}
					buffer = NULL;
				}
			}
		}
	} else {
		if (font == NULL)
			jSetError(ERROR_NULL_POINTER, "Font does not exist");
		if (!gFontLibInitialized)
			jSetError(ERROR_FREETYPE_ERROR, "FreeType has not been initialized");
	}

	va_end(va);
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
sint32 jamFontRenderCharacter(JamFont* font, sint32 x, sint32 y, uint32 c, uint8 r, uint8 g, uint8 b) {
	_JamFontTexture* tex;
	sint32 advance = 0;

	if (font != NULL) {
		if (c != 32) {
			tex = _jamGetCharacterTex(font, c);

			if (tex != NULL) {
				advance = !font->bitmap ? tex->advance : font->width;
				_jamRenderCharacter(font, tex, x, y, font->height, r, g, b);
			}
		} else {
			advance = font->space;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Font does not exist");
	}

	return advance;
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
			_freeFontRange(font, font->ranges[i]);
		free(font->ranges);
		if (font->bitmap)
			SDL_DestroyTexture(font->fontTex);
		else
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
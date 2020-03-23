/// \file Font.h
/// \author lugi1
/// \brief Fonts are a must for most modern games, and as such we have this
///
/// Font.h is composed of two major parts: the fonts and the font renderers.
/// Each font is made up of a latin base texture since most texts will include
/// at least a couple latin/ascii characters as well as an optional unicode
/// texture for whatever language you want to render. The font renderer expects
/// all unicode text to be encoded in UTF-8, and will render any character it
/// has access to (The font struct defines what range of unicode characters
/// the extra font has access to).

#pragma once
#include "Renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////
/// \brief A font used for drawing text
///
/////////////////////////////////////////////////////
typedef struct {
	void* latin;           ///< The essential latin characters; expects U+0000 - U+00FF (0 - 255)
	void* font;            ///< The internal bitmap font
	uint32 uStart;         ///< Where the font begins in unicode points (Inclusive)
	uint32 uEnd;           ///< Where the font ends in unicode points (Inclusive)
	uint8 characterWidth;  ///< The width in pixels of a single character
	uint8 characterHeight; ///< The height in pixels of a single character
	uint16 latinWidth;     ///< The width in pixels of the latin page
	uint16 latinHeight;    ///< The height in pixels of the latin page
	uint16 fontWidth;      ///< The width in pixels of the font page
	uint16 fontHeight;     ///< The height in pixels of the font page
} JamBitmapFont;

typedef struct {
	sint32 w;       ///< Width of this glyph
	sint32 h;       ///< Height of this glyph
	sint32 yOffset; ///< How far down to render this glyph (q should be lower than P, for example)
	sint32 advance; ///< How far forward to move the next character after this one
	void* tex;      ///< Internal bitmap texture of the glyph
} _JamFontTexture;

typedef struct {
	uint32 rangeStart;            ///< Start of the unicode range of this cache
	uint32 rangeEnd;              ///< End of the unicode range of this cache
	_JamFontTexture** characters; ///< Array of textures representing the range of characters
} _JamFontRangeCache;

/// \brief A true type font for rendering .ttf/.otf type fonts
typedef struct {
	void* fontFace;              ///< The font itself
	_JamFontRangeCache** ranges; ///< Array of caches that will be loaded with textures of the font
	int rangeCount;              ///< Number of ranges in this font
	sint32 space;                ///< How many pixels a space is
	sint32 height;               ///< The maximum height of a line of font
} JamFont;

/// \brief Creates a new free type font
///
/// Calling this function with preloadASCII enabled will make this
/// function call jamFontPreloadRange with the range 33-126 (inclusive)
/// after the font is created. Generally, you want this enabled.
///
/// \throws ERROR_FREETYPE_ERROR
/// \throws ERROR_ALLOC_FAILED
JamFont* jamFontCreate(const char* filename, uint32 size, bool preloadASCII);

/// \brief Loads a range of characters into video memory for quick rendering
///
/// This function preloads a range of characters into video memory making font
/// rendering much, much quicker at the expense of some amount of memory. The
/// range is inclusive.
void jamFontPreloadRange(JamFont* font, uint32 rangeStart, uint32 rangeEnd);

/// \brief Renders a font
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
void jamFontRender(JamFont* font, int x, int y, const char* string);

/// \brief Renders a font, moving text past w to the next line
///
/// This function draws words until the next word would be past w,
/// at which point it will move down a line and continue drawing
/// from there.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
void jamFontRenderExt(JamFont* font, int x, int y, const char* string, int w);

/// \brief Gets the width of a string if it were to be drawn
///
/// There are some cases where this would be wrong, but
/// assuming you're just using some average kind of font
/// and not some ridiculous size parameters/characters this
/// will produce an exact width.
///
/// \throws ERROR_NULL_POINTER
sint32 jamFontWidth(JamFont* font, const char* string);

/// \brief Gets the height of a string if it were to be drawn
/// \throws ERROR_NULL_POINTER
sint32 jamFontHeight(JamFont* font);

/// \brief Frees a jam font from memory
void jamFontFree(JamFont* font);

/// \brief Shuts down the font system, call at the end of the program if you used jamFontCreate
///
/// jamFontCreate will automatically initialize the font system if it hasn't been initialized
/// yet. If you ever call jamFontCreate, you need to call this at the end of your program to
/// prevent memory leaks. It is also safe to call this if you never called jamFontCreate.
void jamFontQuit();

/////////////////////////////////////////////////////
/// \brief Creates a new font and returns it
///
/// \param latinFname The latin font filename
/// \param fontFname The bitmap font's filename
///
/// \return Returns a new font
///
/// \throws ERROR_OPEN_FAILED
/// \throws ERROR_ALLOC_FAILED
/////////////////////////////////////////////////////
JamBitmapFont* jamBitmapFontCreate(const char *latinFname, const char *fontFname);

/////////////////////////////////////////////////////
/// \brief Frees a font
///
/// \param font The font to free
/////////////////////////////////////////////////////
void jamBitmapFontFree(JamBitmapFont *font);

/////////////////////////////////////////////////////
/// \brief Renders text to a renderer
///
/// \param x The x position to draw at
/// \param y The y position to draw at
/// \param string The string to render
/// \param font The font to use
///
/// \throws ERROR_OUT_OF_BOUNDS
/////////////////////////////////////////////////////
void jamBitmapFontRender(int x, int y, const char *string, JamBitmapFont *font);

/////////////////////////////////////////////////////
/// \brief Renders text to a renderer with wrapping
/// 	   and support for % tags/newlines
///
/// \param x The x position to draw at
/// \param y The y position to draw at
/// \param w The width at which to wrap
/// \param string The string to render
/// \param font The font to use
/// \param w Width before a newline
///
/// \throws ERROR_OUT_OF_BOUNDS
/// \throws ERROR_NULL_POINTER
/////////////////////////////////////////////////////
void jamBitmapFontRenderExt(int x, int y, const char *string, JamBitmapFont *font, int w, ...);

/////////////////////////////////////////////////////
/// \brief Renders text to a renderer with wrapping
///
/// \param x The x position to draw at
/// \param y The y position to draw at
/// \param w The width at which to wrap
/// \param string The string to render
/// \param font The font to use
///
/// \throws ERROR_OUT_OF_BOUNDS
/////////////////////////////////////////////////////
void jamBitmapFontRenderWrap(int x, int y, uint16 w, const char *string, JamBitmapFont *font);

/////////////////////////////////////////////////////
/// \brief Gets the width of a given string
///
/// \param font The font to check
/// \param string The string to check
///
/// \return Returns the width of the string in pixels
/////////////////////////////////////////////////////
uint16 jamBitmapFontStringWidth(JamBitmapFont *font, const char *string);

/////////////////////////////////////////////////////
/// \brief Gets the height of a given string
///
/// \param font The font to check
/// \param string The string to check
///
/// \return Returns the height of the string in pixels
/////////////////////////////////////////////////////
uint16 jamBitmapFontStringHeight(JamBitmapFont *font, const char *string);

/////////////////////////////////////////////////////
/// \brief Gets the height of a given wrapped string
///
/// \param font The font to check
/// \param string The string to check
/// \param w The w at which to wrap
///
/// \return Returns the height of the string in pixels
/////////////////////////////////////////////////////
uint16 jamBitmapFontStringHeightWrap(JamBitmapFont *font, const char *string, uint16 w);

#ifdef __cplusplus
}
#endif

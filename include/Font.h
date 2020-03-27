/// \file Font.h
/// \author plo
/// \brief Fonts are a must for most modern games, and as such we have this
///
/// Font.h has tools for both TTF fonts and bitmap fonts. TTF fonts can look
/// really good in multiple different resolutions, while you get complete
/// control over how everything looks with bitmap fonts (but they take a lot
/// longer to make whereas you just load a .ttf file and JamFont takes care
/// of the rest).

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
	sint32 height;               ///< The max height of latin characters, this is set automatically by the preloadASCII option in jamFontCreate
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

/// \brief The actual font rendering function, use the macros instead
void _jamFontRenderDetailed(JamFont* font, int x, int y, const char* string, int w, uint8 r, uint8 g, uint8 b, ...);

/// \brief Renders a font
/// \param font JamFont* to render from
/// \param x X Position to start drawing from
/// \param y Y position to start drawing from
/// \param string Message to draw, may use % opcodes (read below) or newlines
/// \param ... Extra arguments to back up any potential opcodes
///
/// This function accepts string modifiers like printf would accept.
/// Specifically, it accepts %%s, %%c, %%C, and %%f (they are not 1-1
/// with the behaviour of printf).
///
///  + `%%s` Inserts a given string at that spot
///  + `%%c` Inserts a given character at that spot (can be UTF-8)
///  + `%%f` Inserts a given floating point number (double)
///  + `%%C` Switches the colour of the text from here on out (JamColour*)
///
/// For example, if you call `jamFontRender(font, x, y, "FPS: %f\n%CMy Game", (double)fps, &myColour);`,
/// it would print
///
///     FPS: 60
///     My Game
///
/// Where "My Game" would be rendered in the colour denoted by myColour.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
#define jamFontRender(font, x, y, string, ...) _jamFontRenderDetailed(font, x, y, string, 0, 255, 255, 255, ##__VA_ARGS__);

/// \brief Renders a font
/// \param font JamFont* to render from
/// \param x X Position to start drawing from
/// \param y Y position to start drawing from
/// \param string Message to draw, may use % opcodes (read below) or newlines
/// \param r Red component of the colour to render the font as (0-255)
/// \param g Green component of the colour to render the font as (0-255)
/// \param b Blue component of the colour to render the font as (0-255)
/// \param ... Extra arguments to back up any potential opcodes
///
/// This function accepts string modifiers like printf would accept.
/// Specifically, it accepts %%s, %%c, %%C, and %%f (they are not 1-1
/// with the behaviour of printf).
///
///  + `%%s` Inserts a given string at that spot
///  + `%%c` Inserts a given character at that spot (can be UTF-8)
///  + `%%f` Inserts a given floating point number (double)
///  + `%%C` Switches the colour of the text from here on out (JamColour*)
///
/// For example, if you call `jamFontRender(font, x, y, "FPS: %f\n%CMy Game", (double)fps, &myColour);`,
/// it would print
///
///     FPS: 60
///     My Game
///
/// Where "My Game" would be rendered in the colour denoted by myColour.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
#define jamFontRenderColour(font, x, y, string, r, g, b, ...) _jamFontRenderDetailed(font, x, y, string, 0, r, g, b, ##__VA_ARGS__);

/// \brief Renders a font
/// \param font JamFont* to render from
/// \param x X Position to start drawing from
/// \param y Y position to start drawing from
/// \param string Message to draw, may use % opcodes (read below) or newlines
/// \param w Width to draw to until moving down a line
/// \param ... Extra arguments to back up any potential opcodes
///
/// Width specifies how far to draw to until this function will automatically
/// start drawing down a line. This is useful if you're drawing in a text box
/// or something where you don't want text to overflow out of the text box or
/// whatever. It does this on a character-to-character basis, however, and as
/// such it will cut words off should they end up near the end of a line.
///
/// This function accepts string modifiers like printf would accept.
/// Specifically, it accepts %%s, %%c, %%C, and %%f (they are not 1-1
/// with the behaviour of printf).
///
///  + `%%s` Inserts a given string at that spot
///  + `%%c` Inserts a given character at that spot (can be UTF-8)
///  + `%%f` Inserts a given floating point number (double)
///  + `%%C` Switches the colour of the text from here on out (JamColour*)
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
#define jamFontRenderExt(font, x, y, string, w, ...) _jamFontRenderDetailed(font, x, y, string, w, 255, 255, 255, ##__VA_ARGS__);

/// \brief Renders a font
/// \param font JamFont* to render from
/// \param x X Position to start drawing from
/// \param y Y position to start drawing from
/// \param string Message to draw, may use % opcodes (read below) or newlines
/// \param r Red component of the colour to render the font as (0-255)
/// \param g Green component of the colour to render the font as (0-255)
/// \param b Blue component of the colour to render the font as (0-255)
/// \param w Width to draw to until moving down a line
/// \param ... Extra arguments to back up any potential opcodes
///
/// Width specifies how far to draw to until this function will automatically
/// start drawing down a line. This is useful if you're drawing in a text box
/// or something where you don't want text to overflow out of the text box or
/// whatever. It does this on a character-to-character basis, however, and as
/// such it will cut words off should they end up near the end of a line.
///
/// This function accepts string modifiers like printf would accept.
/// Specifically, it accepts %%s, %%c, %%C, and %%f (they are not 1-1
/// with the behaviour of printf).
///
///  + `%%s` Inserts a given string at that spot
///  + `%%c` Inserts a given character at that spot (can be UTF-8)
///  + `%%f` Inserts a given floating point number (double)
///  + `%%C` Switches the colour of the text from here on out (JamColour*)
///
/// For example, if you call `jamFontRender(font, x, y, "FPS: %f\n%CMy Game", (double)fps, &myColour);`,
/// it would print
///
///     FPS: 60
///     My Game
///
/// Where "My Game" would be rendered in the colour denoted by myColour.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
#define jamFontRenderColourExt(font, x, y, string, r, g, b, w, ...) _jamFontRenderDetailed(font, x, y, string, w, r, g, b, ##__VA_ARGS__);

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

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
#include "Frame.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief A specialized character texture that handles both bitmap and true type textures
typedef struct {
	union {
		///< For bitmap fonts
		struct {
			sint32 xInTex;   ///< X position in the font texture of this character
			sint32 yInTex;   ///< Y position in the font texture of this character
			JamFrame *frame; ///< Particular texture for this bitmap bit
		};

		///< If this is a true type font we need this data
		struct {
			sint32 w;        ///< Width of this glyph
			sint32 h;        ///< Height of this glyph
			sint32 yOffset;  ///< How far down to render this glyph (q should be lower than P, for example)
			sint32 advance;  ///< How far forward to move the next character after this one
			JamTexture *tex; ///< Internal bitmap texture of the glyph
		};
	};
} _JamFontTexture;

/// \brief A range of unicode character textures
typedef struct {
	uint32 rangeStart;            ///< Start of the unicode range of this cache
	uint32 rangeEnd;              ///< End of the unicode range of this cache
	_JamFontTexture** characters; ///< Array of textures representing the range of characters
} _JamFontRangeCache;

/// \brief A true type font for rendering .ttf/.otf type fonts
typedef struct {
	///< In bitmap fonts we need the source texture, otherwise we need the font face
	union {
		void *fontFace;      ///< TrueType font face
		JamTexture *fontTex; ///< Bitmap texture containing the font
	};
	_JamFontRangeCache** ranges; ///< Array of caches that will be loaded with textures of the font
	int rangeCount;              ///< Number of ranges in this font
	sint32 space;                ///< How many pixels a space is
	sint32 width;                ///< Width of a character (only applies to bitmap fonts)
	sint32 height;               ///< The max height of latin characters, this is set automatically by the preloadASCII option in jamFontCreate
	bool bitmap;                 ///< Weather or not this is a bitmap font
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

/// \brief Creates a font from a bitmap font
/// \param filename Filename of the bitmap image
/// \param characterWidth Width of each character in pixels
/// \param characterHeight Height of each character in pixels
///
/// Loads a font from a bitmap, expecting the characters 0-128 exclusive.
/// While it is possible to load a bitmap font with unicode characters, it
/// it not officially supported and if you create a font with this function
/// you will be unable to use jamFontPreloadRange to load unicode ranges and
/// you will have to instead load it manually yourself. This function also
/// loads the image as if it is a sprite sheet and as such you can have it
/// wrap around the image to the next line.
///
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_ALLOC_FAILED
JamFont* jamFontCreateFromBitmap(const char* filename, uint32 characterWidth, uint32 characterHeight);

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
/// Specifically, it accepts %%s, %%c, %%C, %%S, and %%f (they are not 1-1
/// with the behaviour of printf).
///
///  + `%%s` Inserts a given string at that spot
///  + `%%c` Inserts a given character at that spot (can be UTF-8)
///  + `%%f` Inserts a given floating point number (double)
///  + `%%C` Switches the colour of the text from here on out (JamColour*)
///  + `%%S` Renders a sprite at this spot (JamSprite*)
///
/// For example, if you call `jamFontRender(font, x, y, "FPS: %f\n%CMy Game", (double)fps, &myColour);`,
/// it would print
///
///     FPS: 60
///     My Game
///
/// Where "My Game" would be rendered in the colour denoted by myColour.
/// Please note that %%S will render a sprite at exactly that spot in
/// text, and for that reason will most likely look a little "janky".
/// This is because most characters have a vertical advance they are
/// pushed down by, and it will take a little playing around with before
/// you get perfect results.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
#define jamFontRender(font, x, y, string, ...) _jamFontRenderDetailed(font, x, y, string, 0, 255, 255, 255, ##__VA_ARGS__)

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
/// Specifically, it accepts %%s, %%c, %%C, %%S, and %%f (they are not 1-1
/// with the behaviour of printf).
///
///  + `%%s` Inserts a given string at that spot
///  + `%%c` Inserts a given character at that spot (can be UTF-8)
///  + `%%f` Inserts a given floating point number (double)
///  + `%%C` Switches the colour of the text from here on out (JamColour*)
///  + `%%S` Renders a sprite at this spot (JamSprite*)
///
/// For example, if you call `jamFontRender(font, x, y, "FPS: %f\n%CMy Game", (double)fps, &myColour);`,
/// it would print
///
///     FPS: 60
///     My Game
///
/// Where "My Game" would be rendered in the colour denoted by myColour.
/// Please note that %%S will render a sprite at exactly that spot in
/// text, and for that reason will most likely look a little "janky".
/// This is because most characters have a vertical advance they are
/// pushed down by, and it will take a little playing around with before
/// you get perfect results.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
#define jamFontRenderColour(font, x, y, string, r, g, b, ...) _jamFontRenderDetailed(font, x, y, string, 0, r, g, b, ##__VA_ARGS__)

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
/// Specifically, it accepts %%s, %%c, %%C, %%S, and %%f (they are not 1-1
/// with the behaviour of printf).
///
///  + `%%s` Inserts a given string at that spot
///  + `%%c` Inserts a given character at that spot (can be UTF-8)
///  + `%%f` Inserts a given floating point number (double)
///  + `%%C` Switches the colour of the text from here on out (JamColour*)
///  + `%%S` Renders a sprite at this spot (JamSprite*)
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
#define jamFontRenderExt(font, x, y, string, w, ...) _jamFontRenderDetailed(font, x, y, string, w, 255, 255, 255, ##__VA_ARGS__)

/// \brief Renders a font
/// \param font JamFont* to render from
/// \param x X Position to start drawing from
/// Please note that %%S will render a sprite at exactly that spot in
/// text, and for that reason will most likely look a little "janky".
/// This is because most characters have a vertical advance they are
/// pushed down by, and it will take a little playing around with before
/// you get perfect results.
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
/// Specifically, it accepts %%s, %%c, %%C, %%S, and %%f (they are not 1-1
/// with the behaviour of printf).
///
///  + `%%s` Inserts a given string at that spot
///  + `%%c` Inserts a given character at that spot (can be UTF-8)
///  + `%%f` Inserts a given floating point number (double)
///  + `%%C` Switches the colour of the text from here on out (JamColour*)
///  + `%%S` Renders a sprite at this spot (JamSprite*)
///
/// For example, if you call `jamFontRender(font, x, y, "FPS: %f\n%CMy Game", (double)fps, &myColour);`,
/// it would print
///
///     FPS: 60
///     My Game
///
/// Where "My Game" would be rendered in the colour denoted by myColour.
/// Please note that %%S will render a sprite at exactly that spot in
/// text, and for that reason will most likely look a little "janky".
/// This is because most characters have a vertical advance they are
/// pushed down by, and it will take a little playing around with before
/// you get perfect results.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_TRUETYPE_ERROR
#define jamFontRenderColourExt(font, x, y, string, r, g, b, w, ...) _jamFontRenderDetailed(font, x, y, string, w, r, g, b, ##__VA_ARGS__)

/// \brief Renders a specific character from a font returning the advance
/// \param font Font to find the character in
/// \param x X position of the character
/// \param y Y position of the character
/// \param c Unicode character to render (yes, ASCII codes are fine)
/// \param r Red component of the character's colour
/// \param g green component of the character's colour
/// \param b blue component of the character's colour
///
/// This function is to have greater control over your font rendering.
/// When called, it will render the character to the given x/y in the
/// selected colour and return the horizontal advance of that character.
/// For the uninitiated, the horizontal advance is how far forward the
/// next character should be rendered (and is not necessarily the width
/// of the character). In the case of a space, nothing will be rendered
/// and the recommended advance of the space in this font is returned.
/// If the character is not found or its something like newline, nothing
/// will be rendered, no errors will be set, and 0 will be returned..
///
/// \return The horizontal advance of the character (read desc.)
///
/// \throws ERROR_NULL_POINTER
sint32 jamFontRenderCharacter(JamFont* font, sint32 x, sint32 y, uint32 c, uint8 r, uint8 g, uint8 b);

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

#ifdef __cplusplus
}
#endif

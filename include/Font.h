//////////////////////////////////////////////////////////
// Declares the font things
//
// Copyright (c) Paolo Mazzon
// All rights reserved
//////////////////////////////////////////////////////////

#pragma once
#include "Renderer.h"

/////////////////////////////////////////////////////
/// \brief A font used for drawing text
///
/////////////////////////////////////////////////////
typedef struct {
	SDL_Texture* latin; ///< The essential latin characters; expects U+0000 - U+00FF (0 - 255)
	SDL_Texture* font; ///< The internal bitmap font
	uint32 uStart; ///< Where the font begins in unicode points (Inclusive)
	uint32 uEnd; ///< Where the font ends in unicode points (Inclusive)
	uint8 characterWidth; /// <The width in pixels of a single character
	uint8 characterHeight; /// <The height in pixels of a single character
	uint16 latinWidth; ///< The width in pixels of the latin page
	uint16 latinHeight; ///< The height in pixels of the latin page
	uint16 fontWidth; ///< The width in pixels of the font page
	uint16 fontHeight; ///< The height in pixels of the font page
} Font;

/////////////////////////////////////////////////////
/// \brief Creates a new font and returns it
///
/// \param renderer The renderer to load the texture
/// \param latinFname The latin font filename
/// \param fontFname The bitmap font's filename
///
/// \return Returns a new font
/////////////////////////////////////////////////////
Font* createFont(Renderer* renderer, const char* latinFname, const char* fontFname);

/////////////////////////////////////////////////////
/// \brief Frees a font
///
/// \param font The font to free
/////////////////////////////////////////////////////
void freeFont(Font* font);

/////////////////////////////////////////////////////
/// \brief Renders text to a renderer
///
/// \param x The x position to draw at
/// \param y The y position to draw at
/// \param string The string to render
/// \param font The font to use
/// \param renderer The renderer to draw to
/////////////////////////////////////////////////////
void renderFont(int x, int y, const char* string, Font* font, Renderer* renderer);

/////////////////////////////////////////////////////
/// \brief Renders text to a renderer with wrapping
///
/// \param x The x position to draw at
/// \param y The y position to draw at
/// \param w The width at which to wrap
/// \param string The string to render
/// \param font The font to use
/// \param renderer The renderer to draw to
/////////////////////////////////////////////////////
void renderFontWrap(int x, int y, uint16 w, const char* string, Font* font, Renderer* renderer);

/////////////////////////////////////////////////////
/// \brief Gets the width of a given string
///
/// \param font The font to check
/// \param string The string to check
///
/// \return Returns the width of the string in pixels
/////////////////////////////////////////////////////
uint16 getStringWidth(Font* font, const char* string);

/////////////////////////////////////////////////////
/// \brief Gets the height of a given string
///
/// \param font The font to check
/// \param string The string to check
///
/// \return Returns the height of the string in pixels
/////////////////////////////////////////////////////
uint16 getStringHeight(Font* font, const char* string);

/////////////////////////////////////////////////////
/// \brief Gets the height of a given wrapped string
///
/// \param font The font to check
/// \param string The string to check
/// \param w The w at which to wrap
///
/// \return Returns the height of the string in pixels
/////////////////////////////////////////////////////
uint16 getStringHeightWrap(Font* font, const char* string, uint16 w);

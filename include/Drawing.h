/// \file Drawing.h
/// \author lugi1
/// \brief Declares a lot of tools for drawing.

#pragma once
#include "Renderer.h"
#include "Texture.h"
#include "Constants.h"
#include "Sprite.h"
#include "TileMap.h"
#include "Vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Few functions need this, but its here nonetheless
///
/// At the time of writing this, the only functions that need
/// this are the font rendering "functions", since they can change
/// colour mid-text using this struct (as apposed to 3 arguments
/// in va_arg which would be confusing at best and segfault-galore
/// at worst).
typedef struct _JamColour {
	uint8 r; ///< The red component from 0-255
	uint8 g; ///< The green component from 0-255
	uint8 b; ///< The blue component from 0-255
} JamColour;

/// \brief Sets the current colour of the renderer
/// \throws ERROR_NULL_POINTER
void jamDrawSetColour(uint8 r, uint8 g, uint8 b, uint8 a);

/// \brief Grabs the current colour of the renderer
/// \throws ERROR_NULL_POINTER
void jamDrawGetColour(uint8 *r, uint8 *g, uint8 *b, uint8 *a);

/// \brief Fills the current target with a colour
/// \throws ERROR_NULL_POINTER
void jamDrawFillColour(uint8 r, uint8 g, uint8 b, uint8 a);

/// \brief Draws a rectangle with the current colour
/// \throws ERROR_NULL_POINTER
void jamDrawRectangle(int x, int y, int w, int h);

/// \brief Draws a filled rectangle with the current colour
/// \throws ERROR_NULL_POINTER
void jamDrawRectangleFilled(int x, int y, int w, int h);

/// \brief Draws a polygon
/// \throws ERROR_NULL_POINTER
void jamDrawPolygon(JamPolygon *poly, int x, int y);

/// \breif Draws a texture to the current target
/// \throws ERROR_NULL_POINTER
void jamDrawTexture(JamTexture *texture, sint32 x, sint32 y);

/// \brief Draws a tilemap using its map as it's sprite frames
///
/// This function draws a TileMap using the TileMap's sprite as the thing to draw
/// and the values in the grid itself as the frames of the sprite to draw. This
/// is typically used to draw things after drawSortedMap has already gone through
/// and prepped the map for this function, but you can still do this manually.
/// If you want the x/yInMapEnd to be the end of the map, just put 0.
///
/// \warning All values in the grid must the sprite's frame + 1 (for example,
/// frame 0 should be 1 in the grid). This is so zero is still considered empty
/// in the grid itself.
/// \throws ERROR_NULL_POINTER
void jamDrawTileMap(JamTileMap *tileMap, int x, int y, uint32 xInMapStart, uint32 yInMapStart,
					uint32 xInMapFinish, uint32 yInMapFinish);

/// \breif Draws a texture to the current target with a bunch of extra preferences
/// \param scaleX The x scale of the texture, 1 for normal
/// \param scaleY The y scale of the texture, 1 for normal
/// \param rot Rotation in radians
/// \param alpha The alpha from 0-1
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_NULL_POINTER
void jamDrawTextureExt(JamTexture *texture, sint32 x, sint32 y, sint32 originX, sint32 originY,
					   float scaleX, float scaleY, double rot, uint8 alpha);


/// \breif Draws part of a texture to the current target
void jamDrawTexturePart(JamTexture *texture, sint32 x, sint32 y, sint32 texX, sint32 texY,
						sint32 texW, sint32 texH);

/// \breif Draws a texture to the renderer with a bunch of extra preferences
/// \param scaleX The x scale of the texture, 1 for normal
/// \param scaleY The y scale of the texture, 1 for normal
/// \param rot Rotation in radians
/// \param alpha The alpha from 0-1
void jamDrawTexturePartExt(JamTexture *texture, sint32 x, sint32 y, sint32 originX,
						   sint32 originY, float scaleX, float scaleY, double rot, uint8 alpha, sint32 texX,
						   sint32 texY, sint32 texW, sint32 texH);

#ifdef __cplusplus
}
#endif
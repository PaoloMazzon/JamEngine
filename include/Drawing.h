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

/// \brief Sets the current colour of the renderer
/// \throws ERROR_NULL_POINTER
void drawSetColour(JamRenderer* renderer, uint8 r, uint8 g, uint8 b, uint8 a);

/// \brief Grabs the current colour of the renderer
/// \throws ERROR_NULL_POINTER
void drawGetColour(JamRenderer* renderer, uint8* r, uint8* g, uint8* b, uint8* a);

/// \brief Fills the current target with a colour
/// \throws ERROR_NULL_POINTER
void drawFillColour(JamRenderer* renderer, uint8 r, uint8 g, uint8 b, uint8 a);

/// \brief Draws a rectangle with the current colour
/// \throws ERROR_NULL_POINTER
void drawRectangle(JamRenderer* renderer, int x, int y, int w, int h);

/// \brief Draws a circle with the current colour
/// \throws ERROR_NULL_POINTER
void drawCircle(JamRenderer* renderer, int x, int y, int r);

/// \brief Draws a polygon
/// \throws ERROR_NULL_POINTER
void drawPolygon(JamRenderer* renderer, Polygon* poly, int x, int y);

/// \breif Draws a texture to the current target
/// \throws ERROR_NULL_POINTER
void drawTexture(JamRenderer* renderer, JamTexture* texture, sint32 x, sint32 y);

/// \brief Draws a tile map using an 48-frame sprite
///
/// This function serves double purpose in that will auto-tile and draw the
/// map, but it will also save the auto-tile information/sprite to the renderer
/// so you can just use drawTileMap in the future and it will not need to re-do
/// the auto-tiling, it just needs to draw the map. For this reason, you're
/// usually better off letting this function draw the whole tilemap even though
/// you may not need the whole thing yet just so you can have the auto-tile ready
/// for later. NOTE: This also has the side effect of making the collision range
/// 1-48 (all "true" values)
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_INCORRECT_FORMAT
void drawSortedMap(JamRenderer* renderer, Sprite* spr, TileMap* map, int x, int y, uint32 startingCellX, uint32 startingCellY);

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
void drawTileMap(JamRenderer* renderer, TileMap* tileMap, int x, int y, uint32 xInMapStart, uint32 yInMapStart, uint32 xInMapFinish, uint32 yInMapFinish);

/// \breif Draws a texture to the current target with a bunch of extra preferences
/// \param scaleX The x scale of the texture, 1 for normal
/// \param scaleY The y scale of the texture, 1 for normal
/// \param rot Rotation in radians
/// \param alpha The alpha from 0-1
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_NULL_POINTER
void drawTextureExt(JamRenderer* renderer, JamTexture* texture, sint32 x, sint32 y, sint32 originX, sint32 originY, float scaleX, float scaleY, double rot, Uint8 alpha);


/// \breif Draws part of a texture to the current target
void drawTexturePart(JamRenderer* renderer, JamTexture* texture, sint32 x, sint32 y, sint32 texX, sint32 texY, sint32 texW, sint32 texH);

/// \breif Draws a texture to the renderer with a bunch of extra preferences
/// \param scaleX The x scale of the texture, 1 for normal
/// \param scaleY The y scale of the texture, 1 for normal
/// \param rot Rotation in radians
/// \param alpha The alpha from 0-1
void drawTexturePartExt(JamRenderer* renderer, JamTexture* texture, sint32 x, sint32 y, sint32 originX, sint32 originY, float scaleX, float scaleY, double rot, Uint8 alpha, sint32 texX, sint32 texY, sint32 texW, sint32 texH);
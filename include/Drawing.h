//
// Created by lugi1 on 2018-07-13.
//

#pragma once
#include "Renderer.h"
#include "Texture.h"
#include "Constants.h"
#include "Sprite.h"
#include "TileMap.h"

/// \brief Sets the current colour of the renderer
void drawSetColour(Renderer* renderer, uint8 r, uint8 g, uint8 b, uint8 a);

/// \brief Grabs the current colour of the renderer
void drawGetColour(Renderer* renderer, uint8* r, uint8* g, uint8* b, uint8* a);

/// \brief Fills the current target with a colour
void drawFillColour(Renderer* renderer, uint8 r, uint8 g, uint8 b, uint8 a);

/// \brief Draws a rectangle with the current colour
void drawRectangle(Renderer* renderer, int x, int y, int w, int h);

/// \brief Draws a circle with the current colour
void drawCircle(Renderer* renderer, int x, int y, int r);

/// \breif Draws a texture to the current target
void drawTexture(Renderer* renderer, Texture* texture, sint32 x, sint32 y);

/// \brief Draws a tile map using an 48-frame sprite
void drawSortedMap(Renderer* renderer, Sprite* spr, TileMap* map, int x, int y, uint32 startingCellX, uint32 startingCellY);

/// \breif Draws a texture to the current target with a bunch of extra preferences
///
/// \param scaleX The x scale of the texture, 1 for normal
/// \param scaleY The y scale of the texture, 1 for normal
/// \param rot Rotation in radians
/// \param alpha The alpha from 0-1
void drawTextureExt(Renderer* renderer, Texture* texture, sint32 x, sint32 y, sint32 originX, sint32 originY, float scaleX, float scaleY, double rot, Uint8 alpha);


/// \breif Draws part of a texture to the current target
void drawTexturePart(Renderer* renderer, Texture* texture, sint32 x, sint32 y, sint32 texX, sint32 texY, sint32 texW, sint32 texH);

/// \breif Draws a texture to the renderer with a bunch of extra preferences
///
/// \param scaleX The x scale of the texture, 1 for normal
/// \param scaleY The y scale of the texture, 1 for normal
/// \param rot Rotation in radians
/// \param alpha The alpha from 0-1
void drawTexturePartExt(Renderer* renderer, Texture* texture, sint32 x, sint32 y, sint32 originX, sint32 originY, float scaleX, float scaleY, double rot, Uint8 alpha, sint32 texX, sint32 texY, sint32 texW, sint32 texH);
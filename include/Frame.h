//
// Created by lugi1 on 2018-07-14.
//

#pragma once
#include "Renderer.h"
#include "Texture.h"

/// \brief Contains information on a sprite frame
///
/// This struct contains a texture as well as where on the texture
/// this frame is. This means you can load a spritesheet onto a
/// texture and use that texture for many frames, since you only
/// need to tell the frame where on the spritesheet to gather its
/// info.
typedef struct {
	Texture* tex; ///< The texture this frame is using
	sint32 x; ///< The top left corner of the drawn area
	sint32 y; ///< The top left corner of the drawn area
	sint32 w; ///< The width of the drawn area
	sint32 h; ///< The height of the drawn area
} Frame;

/// \brief Creates a frame
///
/// There is no way to create an empty frame
/// for that is completely redundant
Frame* createFrame(Texture* tex, sint32 x, sint32 y, sint32 w, sint32 h);

/// \brief Draws a frame
///
/// The sprite struct doesn't use this, it's
/// just a convenience for users
void drawFrame(Frame* frame, Renderer* renderer, sint32 x, sint32 y);

/// \brief Empties a frame from memory
void freeFrame(Frame* frame, bool destroyTexture);
/// \file Frame.h
/// \author lugi1
/// \brief This is really just to make animations easier
///
/// Generally, you want to keep track of the textures frames
/// use outside of the frame struct and clean them up yourself,
/// ESPECIALLY if you use spritesheets. If each frame is just
/// a file, sure, whatever, just make the frame clean it up.
#pragma once
#include "Renderer.h"
#include "Texture.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Contains information on a sprite frame
///
/// This struct contains a texture as well as where on the texture
/// this frame is. This means you can load a spritesheet onto a
/// texture and use that texture for many frames, since you only
/// need to tell the frame where on the spritesheet to gather its
/// info.
typedef struct {
	JamTexture* tex; ///< The texture this frame is using
	sint32 x; ///< The top left corner of the drawn area
	sint32 y; ///< The top left corner of the drawn area
	sint32 w; ///< The width of the drawn area
	sint32 h; ///< The height of the drawn area
} JamFrame;

/// \brief Creates a frame
///
/// There is no way to create an empty frame
/// for that is completely redundant
///
/// \throws ERROR_ALLOC_FAILED
JamFrame* jamCreateFrame(JamTexture *tex, sint32 x, sint32 y, sint32 w, sint32 h);

/// \brief Draws a frame
///
/// The sprite struct doesn't use this, it's
/// just a convenience for users
///
/// \throws ERROR_NULL_POINTER
void jamDrawFrame(JamFrame *frame, sint32 x, sint32 y);

/// \brief Empties a frame from memory
void jamFreeFrame(JamFrame *frame, bool destroyTexture);

#ifdef __cplusplus
}
#endif
/// \file Sprite.h
/// \author lugi1
/// \brief An extremely vital tool of game development, the sprite
///
/// This is an extremely powerful tool for game development since
/// it can keep track of animations, size, rotation, and origins
/// by itself. The loadSpriteFromSheet function allows you to draw
/// your animations in any software then import any sort of wacky
/// sheet then import it here easily.

#pragma once
#include "Renderer.h"
#include "Frame.h"
#include "Constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Contains information surrounding jam sprites
///
/// A sprite's width and height are in most cases the width and
/// height of each frame in the animation. To be more specific,
/// it is the width and height of either the most recently added
/// frame (via jamSpriteAppendFrame) or the cellW/cellH passed
/// to jamLoadSpriteFromSheet. It can also, of course, be whatever
/// wacky values you wish to set it to. It is used by the engine
/// when calculating various things like collisions (and of course
/// its always there when you need it).
///
/// originX and originY are values the engine uses when drawing a
/// sprite basically anywhere, or calculating things on the entity
/// side of things. Technically speaking, the x/y values you pass
/// to a function which draws a sprite will have the origin on said
/// sprite subtracted from them. This is to say that sprites are
/// "drawn from" their origins. For example, a sprite of width/height
/// 16/16 with an origin of 8/8 would be drawn from the center; the
/// x/y values passed to jamDrawSprite would represent the center of
/// the sprite and not the top-left.
typedef struct {
	// Animation things
	JamFrame** frames;      ///< The list of frames in the sprite's animation
	uint32 animationLength; ///< The length (in frames) of the animation
	uint16 frameDelay;      ///< How many frames in between each animation frame
	uint32 currentFrame;    ///< The current frame of the animation
	uint16 frameTime;       ///< Amount of frames left till the next frame
	bool looping;           ///< Weather or not to loop the animation

	// Convenience
	sint32 width;  ///< The width of the sprite
	sint32 height; ///< The height of the sprite

	// Things for the sprite to draw properly
	sint32 originX; ///< The x origin of the sprite
	sint32 originY; ///< The y origin of the sprite
} JamSprite;

/// \brief Creates a sprite
///
/// It is recommended that you pass the animation length
/// from creation then upload the frames individually
/// after, but if you care not for performance, spriteAppendFrame
/// can be used and is a bit easier to use. If there is no animation
/// you can just set the latter two arguments to 0/false.
///
/// \throws ERROR_ALLOC_FAILED
JamSprite* jamSpriteCreate(uint32 animationLength, uint16 frameDelay, bool looping);

/// \brief Adds a frame to a sprite
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_NULL_POINTER
void jamSpriteAppendFrame(JamSprite *sprite, JamFrame *frame);

/// \brief Loads a sprite from a spritesheet
/// \param spriteSheet The parent texture with which to pull from
/// \param cellCount The number of frames we will be pulling from the texture
/// \param xInSheet Where the animation starts in the texture (x-coord)
/// \param yInSheet Where the animation starts in the texture (y-coord)
/// \param cellW The width of each frame
/// \param cellH The height of each frame
/// \param paddingW The width in pixels in between each frame
/// \param paddingH The height in pixels in between each frame
/// \param xAlign The alignment of the frames in the texture (in case they're not continuous)
/// \param frameDelay The delay in in-game-frames between each frame in the animation
/// \param looping Does the animation loop?
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_SDL_ERROR
JamSprite* jamSpriteLoadFromSheet(JamTexture *spriteSheet, uint32 cellCount, uint32 xInSheet, uint32 yInSheet,
								  uint32 cellW, uint32 cellH, uint32 paddingW, uint32 paddingH, uint32 xAlign,
								  uint16 frameDelay, bool looping);

/// \brief Updates a sprite's animation
///
/// Don't run this if updateOnDraw is on, also don't
/// run this multiple times a frame
///
/// \throws ERROR_NULL_POINTER
void jamSpriteUpdate(JamSprite *sprite);

/// \brief Draws a sprite on screen
/// \throws ERROR_NULL_POINTER
void jamDrawSprite(JamSprite *sprite, sint32 x, sint32 y, float scaleX, float scaleY, double rot,
				   uint8 alpha, bool updateOnDraw);

/// \brief Draws a specific frame of a sprite without screwing with the animation
/// \throws ERROR_OUT_OF_BOUNDS
/// \throws ERROR_NULL_POINTER
void jamDrawSpriteFrame(JamSprite *sprite, sint32 x, sint32 y, float scaleX, float scaleY,
						double rot, uint8 alpha, uint32 frame);

/// \brief Frees a sprite
void jamSpriteFree(JamSprite *sprite, bool freeFrames, bool freeTextures);

#ifdef __cplusplus
}
#endif
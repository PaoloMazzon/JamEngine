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

/// \brief Contains information surrounding the mystical sprite
typedef struct {
	// Animation things
	Frame** frames; ///< The list of frames in the sprite's animation
	uint32 animationLength; ///< The length (in frames) of the animation
	uint16 frameDelay; ///< How many frames in between each animation frame
	uint32 currentFrame; ///< The current frame of the animation
	uint16 frameTime; ///< Amount of frames left till the next frame
	bool looping; ///< Weather or not to loop the animation

	// Things for the sprite to draw properly
	sint32 originX; ///< The x origin of the sprite
	sint32 originY; ///< The y origin of the sprite
	float scaleX; ///< The x scale of the sprite
	float scaleY; ///< The y scale of the sprite
	double rot; ///< The rotation of the sprite
	uint8 alpha; ///< The alpha of the sprite
	bool updateOnDraw; ///< Weather or not the sprite will update on drawing
} Sprite;

/// \brief Creates a sprite
///
/// It is recommended that you pass the animation length
/// from creation then upload the frames individually
/// after, but if you care not for performance, spriteAppendFrame
/// can be used and is a bit easier to use. If there is no animation
/// you can just set the latter two arguments to 0/false.
Sprite* createSprite(uint32 animationLength, uint16 frameDelay, bool looping);

/// \brief Adds a frame to a sprite
void spriteAppendFrame(Sprite* sprite, Frame* frame);

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
Sprite* loadSpriteFromSheet(Texture* spriteSheet, uint32 cellCount, uint32 xInSheet, uint32 yInSheet, uint32 cellW, uint32 cellH, uint32 paddingW, uint32 paddingH, uint32 xAlign, uint16 frameDelay, bool looping);

/// \brief Updates a sprite's animation
///
/// Don't run this if updateOnDraw is on, also don't
/// run this multiple times a frame
void updateSprite(Sprite* sprite);

/// \brief Draws a sprite on screen
void drawSprite(Renderer* renderer, Sprite* sprite, sint32 x, sint32 y);

/// \brief Draws a specific frame of a sprite without screwing with the animation
void drawSpriteFrame(Renderer* renderer, Sprite* sprite, sint32 x, sint32 y, uint32 frame);

/// \brief Frees a sprite
void freeSprite(Sprite* sprite, bool freeFrames, bool freeTextures);
//
// Created by lugi1 on 2018-07-05.
//


#include "Sprite.h"
#include <stdio.h>
#include <Sprite.h>
#include "Frame.h"
#include "Renderer.h"
#include "Drawing.h"
#include "JamError.h"
#include <malloc.h>
#include <SDL.h>

///////////////////////////////////////////////////
JamSprite* jamCreateSprite(uint32 animationLength, uint16 frameDelay, bool looping) {
	JamSprite* sprite = (JamSprite*)malloc(sizeof(JamSprite));
	JamFrame** list = (JamFrame**)calloc(sizeof(JamFrame*), animationLength);

	// Check for its validity
	if (sprite != NULL) {
		sprite->frames = list;
		sprite->animationLength = animationLength;
		sprite->frameDelay = frameDelay;
		sprite->currentFrame = 0;
		sprite->frameTime = 0;
		sprite->looping = looping;
		sprite->originX = 0;
		sprite->originY = 0;

		// Check if list is a dud and shouldn't be
		if (list == NULL && animationLength > 0) {
			free(sprite);
			jSetError(ERROR_ALLOC_FAILED, "Could not allocate list. (jamCreateSprite).\n Animation Length: %i\n", animationLength);
			sprite = NULL;
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Could not allocate sprite. (jamCreateSprite).\n");
		free(list);
	}

	return sprite;
}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
void jamSpriteAppendFrame(JamSprite *sprite, JamFrame *frame) {
	JamFrame** buffer;
	int i;

	// Checks of course
	if (sprite != NULL) {
		// Make the new frame buffer one more than the last one to make room for the new piece
		buffer = (JamFrame**)malloc(sizeof(JamFrame*) * (sprite->animationLength + 1));

		// Double check the new one
		if (buffer != NULL) {
			// Copy over the contents
			for (i = 0; i < sprite->animationLength; i++)
				buffer[i] = sprite->frames[i];

			// Add the frame of the hour
			buffer[sprite->animationLength] = frame;

			// Update the new animation length
			sprite->animationLength++;

			// Free the old list, then copy over the new one
			free(sprite->frames);
			sprite->frames = buffer;
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate buffer frame list (jamSpriteAppendFrame).\n");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamSprite does not exist (jamSpriteAppendFrame).\n");
	}
}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
JamSprite* jamLoadSpriteFromSheet(JamTexture *spriteSheet, uint32 cellCount, uint32 xInSheet, uint32 yInSheet,
								  uint32 cellW, uint32 cellH, uint32 paddingW, uint32 paddingH, uint32 xAlign,
								  uint16 frameDelay, bool looping) {
	JamSprite* sprite;
	bool continueGrabbing = true; // Encompasses a few different conditions
	uint32 currentFrame = 0;      // The current frame to grab
	bool failedGrab = false;      // This is for later when check if we got all frames successfully

	// Create a sprite for the sheet
	sprite = jamCreateSprite(cellCount, frameDelay, looping);

	// Check that the sprite and texture creation was successful
	if (sprite != NULL && spriteSheet != NULL) {
		// Loop until we have all frames or run out of room in texture
		while (continueGrabbing) {
			// Grab the current frame
			sprite->frames[currentFrame] = jamCreateFrame(spriteSheet, xInSheet, yInSheet, cellW, cellH);

			// Move the pointer to the next frame
			xInSheet += cellW + paddingW;

			// Do we need to move down a level
			if (xInSheet + cellW > spriteSheet->w) {
				xInSheet = xAlign;
				yInSheet += cellH + paddingH;
			}

			// Check if we are out of bounds or out of frames to process
			if (yInSheet + cellH > spriteSheet->h || currentFrame == cellCount - 1) {
				continueGrabbing = false;
			} else {
				currentFrame++;
			}
		}

		// Now we must check that all the frames were grabbed successfully
		for (currentFrame = 0; currentFrame < cellCount; currentFrame++)
			if (sprite->frames[currentFrame] == NULL)
				failedGrab = true;

		// Stop the whole ordeal if a frame could not be grabbed
		if (failedGrab) {
			jSetError(ERROR_SDL_ERROR, "Failed to load all frames (jamLoadSpriteFromSheet). SDL Error: %s\n", SDL_GetError());
			jamFreeSprite(sprite, true, false);
			sprite = NULL;
		}
	} else {
		if (sprite == NULL) {
			jSetError(ERROR_SDL_ERROR, "Failed to create the sprite (jamLoadSpriteFromSheet).\n");
		}
		if (spriteSheet == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamTexture does not exist (jamLoadSpriteFromSheet).\n");
		}
		jamFreeSprite(sprite, true, false);
		sprite = NULL;
	}

	return sprite;
}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
void jamUpdateSprite(JamSprite *sprite) {
	if (sprite != NULL) {
		// No need to do anything if only 1 frame
		if (sprite->animationLength > 1) {
			// Is it time to change frame?
			if (sprite->frameTime == sprite->frameDelay) {
				// Are we at the final frame?
				if (sprite->currentFrame == sprite->animationLength - 1) {
					// Does the animation loop?
					if (sprite->looping) {
						// Go back to the start of the animation
						sprite->currentFrame = 0;
						sprite->frameTime = 0;
					} else {
						// Break the loop
						sprite->frameTime++;
					}
				} else {
					// Move up a frame
					sprite->currentFrame++;
					sprite->frameTime = 0;
				}
			} else {
				// Just wait a little longer
				sprite->frameTime++;
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamSprite does not exist (jamUpdateSprite).\n");
	}
}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
void jamDrawSprite(JamSprite *sprite, sint32 x, sint32 y, float scaleX, float scaleY, double rot,
				   uint8 alpha, bool updateOnDraw) {
	if (sprite != NULL) {
		// Update the sprite if that is to be done before drawing
		if (updateOnDraw)
			jamUpdateSprite(sprite);

		// Draw it to the screen with all the crazy parameters
		jamDrawTexturePartExt(sprite->frames[sprite->currentFrame]->tex,
							  x,
							  y,
							  sprite->originX,
							  sprite->originY,
							  scaleX,
							  scaleY,
							  rot,
							  alpha,
							  sprite->frames[sprite->currentFrame]->x,
							  sprite->frames[sprite->currentFrame]->y,
							  sprite->frames[sprite->currentFrame]->w,
							  sprite->frames[sprite->currentFrame]->h
		);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamSprite does not exist (jamDrawSprite).\n");
	}
}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
void jamDrawSpriteFrame(JamSprite *sprite, sint32 x, sint32 y, float scaleX, float scaleY,
						double rot, uint8 alpha, uint32 frame) {
	if (sprite != NULL && frame < sprite->animationLength) {
		// Draw it to the screen with all the crazy parameters
		jamDrawTexturePartExt(sprite->frames[frame]->tex,
							  x,
							  y,
							  sprite->originX,
							  sprite->originY,
							  scaleX,
							  scaleY,
							  rot,
							  alpha,
							  sprite->frames[frame]->x,
							  sprite->frames[frame]->y,
							  sprite->frames[frame]->w,
							  sprite->frames[frame]->h
		);
	} else {
		if (sprite == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamSprite does not exist (jamDrawSpriteFrame).\n");
		} else if (frame >= sprite->animationLength) {
			jSetError(ERROR_OUT_OF_BOUNDS, "JamFrame %i out of bounds (jamDrawSpriteFrame).\n", (int) frame);
		}
	}
}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
void jamFreeSprite(JamSprite *sprite, bool freeFrames, bool freeTextures) {
	int i;
	if (sprite != NULL) {
		if (freeFrames) {
			for (i = 0; i < sprite->animationLength; i++)
				jamFreeFrame(sprite->frames[i], freeTextures);
		}
		// Either way the frames list has to go
		free(sprite->frames);

		free(sprite);
	}
}
///////////////////////////////////////////////////
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
#include <Frame.h>

///////////////////////////////////////////////////
JamSprite* jamSpriteCreate(uint32 animationLength, uint16 frameDelay, bool looping) {
	JamSprite* sprite = (JamSprite*)malloc(sizeof(JamSprite));
	JamFrame** list = (JamFrame**)calloc(sizeof(JamFrame*), animationLength);

	// Check for its validity
	if (sprite != NULL) {
		sprite->frames = list;
		sprite->animationLength = animationLength;
		sprite->frameDelay = frameDelay;
		sprite->looping = looping;
		sprite->originX = 0;
		sprite->originY = 0;
		sprite->width = 0;
		sprite->height = 0;

		// Check if list is a dud and shouldn't be
		if (list == NULL && animationLength > 0) {
			free(sprite);
			jSetError(ERROR_ALLOC_FAILED, "Could not allocate list. (jamSpriteCreate).\n Animation Length: %i\n", animationLength);
			sprite = NULL;
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Could not allocate sprite. (jamSpriteCreate)");
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

			// Update the sprite's information
			sprite->animationLength++;
			sprite->width = frame->w;
			sprite->height = frame->h;

			// Free the old list, then copy over the new one
			free(sprite->frames);
			sprite->frames = buffer;
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate buffer frame list (jamSpriteAppendFrame)");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamSprite does not exist (jamSpriteAppendFrame)");
	}
}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
JamSprite* jamSpriteLoadFromSheet(JamTexture *spriteSheet, uint32 cellCount, uint32 xInSheet, uint32 yInSheet,
								  uint32 cellW, uint32 cellH, uint32 paddingW, uint32 paddingH, uint32 xAlign,
								  uint16 frameDelay, bool looping) {
	JamSprite* sprite;
	bool continueGrabbing = true; // Encompasses a few different conditions
	uint32 currentFrame = 0;      // The current frame to grab
	bool failedGrab = false;      // This is for later when check if we got all frames successfully

	// Create a sprite for the sheet
	sprite = jamSpriteCreate(cellCount, frameDelay, looping);

	// Check that the sprite and texture creation was successful
	if (sprite != NULL && spriteSheet != NULL) {
		// Set the new width/height
		sprite->width = cellW;
		sprite->height = cellH;

		// Loop until we have all frames or run out of room in texture
		while (continueGrabbing) {
			// Grab the current frame
			sprite->frames[currentFrame] = jamFrameCreate(spriteSheet, xInSheet, yInSheet, cellW, cellH);

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
			jSetError(ERROR_SDL_ERROR, "Failed to load all frames (jamSpriteLoadFromSheet). SDL Error: %s\n", SDL_GetError());
			jamSpriteFree(sprite, true, false);
			sprite = NULL;
		}
	} else {
		if (sprite == NULL) {
			jSetError(ERROR_SDL_ERROR, "Failed to create the sprite (jamSpriteLoadFromSheet)");
		}
		if (spriteSheet == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamTexture does not exist (jamSpriteLoadFromSheet)");
		}
		jamSpriteFree(sprite, true, false);
		sprite = NULL;
	}

	return sprite;
}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
void jamDrawSpriteFrame(JamSprite *sprite, sint32 x, sint32 y, float scaleX, float scaleY,
						double rot, uint8 alpha, uint32 frame) {
	if (sprite != NULL && frame < sprite->animationLength) {
		uint8 r, g, b, a;
		jamDrawGetColour(&r, &g, &b, &a);
		jamDrawSetColour(r, g, b, alpha);
		vk2dRendererDrawTexture(sprite->frames[frame]->tex, x - sprite->originX, y - sprite->originY, scaleX, scaleY, rot, sprite->originX, sprite->originY);
		jamDrawSetColour(r, g, b, a);
	} else {
		if (sprite == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamSprite does not exist (jamDrawSpriteFrame)");
		} else if (frame >= sprite->animationLength) {
			jSetError(ERROR_OUT_OF_BOUNDS, "JamFrame %i out of bounds (jamDrawSpriteFrame)", (int) frame);
		}
	}
}
///////////////////////////////////////////////////

///////////////////////////////////////////////////
void jamSpriteFree(JamSprite *sprite, bool freeFrames, bool freeTextures) {
	int i;
	if (sprite != NULL) {
		if (freeFrames) {
			for (i = 0; i < sprite->animationLength; i++)
				jamFrameFree(sprite->frames[i], freeTextures);
		}
		// Either way the frames list has to go
		free(sprite->frames);

		free(sprite);
	}
}
///////////////////////////////////////////////////
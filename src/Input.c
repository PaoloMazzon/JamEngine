//
// Created by lugi1 on 2018-07-13.
//

#include <malloc.h>
#include <stdio.h>
#include <Input.h>
#include <JamError.h>
#include <SDL.h>
#include "Vector.h"

/// \brief Keeps track of keyboard/mouse/gamepad input
///
/// The renderer MUST be created before this, also this
/// input struct does not pay any attention to the state
/// of the screen buffer, and thus provides only info
/// on the mouse in relation to the window. If you would
/// like the mouse coordinates in relation to the screen
/// buffer, use the windowToScreenBufferCoordinates function
/// to get the mouse coordinates in the game world.
typedef struct {
	// Keyboard stuff
	const uint8* currentInput; ///< This is the current frame's input
	uint8* previousInput;      ///< This is the last frame's input
	int kbLen;                 ///< The length of the two above arrays

	// Mouse stuff
	int mX;                    ///< The mouse's current x position
	int mY;                    ///< The mouse's current y position
	int mXP;                   ///< Previous mouse x position
	int mYP;                   ///< Previous mouse y position
	uint32 mouseState;         ///< Current frame's mice buttons
	uint32 previousMouseState; ///< Last frame's mice buttons
} _JamInput;

static _JamInput* gInputPointer;

//////////////////////////////////////////////////////////////
void jamInputInit() {
	gInputPointer = (_JamInput*)malloc(sizeof(_JamInput));

	// Double check, of course
	if (gInputPointer != NULL) {
		// Now we grab the first piece
		gInputPointer->currentInput = SDL_GetKeyboardState(&gInputPointer->kbLen);

		// And create the other array
		gInputPointer->previousInput = (Uint8*)malloc((size_t)gInputPointer->kbLen);

		// And again, double check
		if (gInputPointer->previousInput == NULL) {
			free(gInputPointer);
			gInputPointer = NULL;
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate placeholder array (createInput). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate input. SDL Error: %s\n", SDL_GetError());
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputIsInitialized() {
	return gInputPointer != NULL;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamInputUpdate(double screenMultiplier) {
	int i;

	// Check that the gInputPointer exists
	if (gInputPointer != NULL) {
		// Copy the current frame to the old frame
		for (i = 0; i < gInputPointer->kbLen; i++)
			gInputPointer->previousInput[i] = gInputPointer->currentInput[i];

		// Update mouse stuff
		gInputPointer->previousMouseState = gInputPointer->mouseState;
		gInputPointer->mouseState = SDL_GetMouseState(&gInputPointer->mX, &gInputPointer->mY);

		gInputPointer->mXP;
		gInputPointer->mYP;
		gInputPointer->mX = (int)((double)gInputPointer->mX / screenMultiplier);
		gInputPointer->mY = (int)((double)gInputPointer->mY / screenMultiplier);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputCheckKey(JamKeyboardKeys key) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		if (key > 0 && key < gInputPointer->kbLen) {
			ret = gInputPointer->currentInput[key];
		} else {
			jSetError(ERROR_OUT_OF_BOUNDS, "Scancode out of range (jamInputCheckKey). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput doesn't exist (jamInputCheckKey). SDL Error: %s\n", SDL_GetError());
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputCheckKeyPressed(JamKeyboardKeys key) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		if (key > 0 && key < gInputPointer->kbLen) {
			// If it was just pressed, last frame's value would be 0
			ret = gInputPointer->currentInput[key] && !gInputPointer->previousInput[key];
		} else {
			jSetError(ERROR_OUT_OF_BOUNDS, "Scancode out of range (jamInputCheckKeyPressed). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput doesn't exist (jamInputCheckKeyPressed). SDL Error: %s\n", SDL_GetError());
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputCheckKeyReleased(JamKeyboardKeys key) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		if (key > 0 && key < gInputPointer->kbLen) {
			// If it was just released, last frame's value would be 1 and the current 0
			ret = !gInputPointer->currentInput[key] && gInputPointer->previousInput[key];
		} else {
			jSetError(ERROR_OUT_OF_BOUNDS, "Scancode out of range (jamInputCheckKeyReleased). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput doesn't exist (jamInputCheckKeyReleased). SDL Error: %s\n", SDL_GetError());
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
int jamInputGetMouseX() {
	if (gInputPointer != NULL) {
		return gInputPointer->mX;
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput has not been initialized.");
	}

	return 0;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
int jamInputGetMouseY() {
	if (gInputPointer != NULL) {
		return gInputPointer->mY;
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput has not been initialized.");
	}

	return 0;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
double jamInputGetMouseVelocity() {
	if (gInputPointer != NULL) {
		return pointDistance((double)gInputPointer->mX, (double)gInputPointer->mY, (double)gInputPointer->mXP, (double)gInputPointer->mYP);
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput has not been initialized.");
	}

	return 0;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
double jamInputGetMouseDirection() {
	if (gInputPointer != NULL) {
		return pointAngle((double)gInputPointer->mX, (double)gInputPointer->mY, (double)gInputPointer->mXP, (double)gInputPointer->mYP);
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput has not been initialized.");
	}

	return 0;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputCheckMouseButton(uint8 button) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		ret = (gInputPointer->mouseState & button) > 0;
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput doesn't exist (jamInputCheckMouseButton). SDL Error: %s\n", SDL_GetError());
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputCheckMouseButtonPressed(uint8 button) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		ret = !(gInputPointer->previousMouseState & button) && (gInputPointer->mouseState & button);
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput doesn't exist (jamInputCheckMouseButtonPressed). SDL Error: %s\n", SDL_GetError());
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputCheckMouseButtonReleased(uint8 button) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		ret = (gInputPointer->previousMouseState & button) && !(gInputPointer->mouseState & button);
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput doesn't exist (jamInputCheckMouseButtonReleased). SDL Error: %s\n", SDL_GetError());
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamInputQuit() {
	if (gInputPointer != NULL) {
		free(gInputPointer->previousInput);
		free(gInputPointer);
	}
}
//////////////////////////////////////////////////////////////
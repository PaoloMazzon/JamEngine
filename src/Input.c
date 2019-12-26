//
// Created by lugi1 on 2018-07-13.
//

#include <malloc.h>
#include <stdio.h>
#include <Input.h>
#include <JamError.h>
#include <SDL.h>

static JamInput* gInputPointer;

//////////////////////////////////////////////////////////////
void jamInputInit() {
	gInputPointer = (JamInput*)malloc(sizeof(JamInput));

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
		jSetError(ERROR_NULL_POINTER, "JamInput doesn't exist (jamInputCheckKey). SDL Error: %s\n", SDL_GetError());
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
		jSetError(ERROR_NULL_POINTER, "JamInput doesn't exist (jamInputCheckKeyPressed). SDL Error: %s\n", SDL_GetError());
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
		jSetError(ERROR_NULL_POINTER, "JamInput doesn't exist (jamInputCheckKeyReleased). SDL Error: %s\n", SDL_GetError());
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
int jamInputGetMouseX() {
	if (gInputPointer != NULL) {
		return gInputPointer->mX;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamInput has not been initialized.");
	}

	return 0;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
int jamInputGetMouseY() {
	if (gInputPointer != NULL) {
		return gInputPointer->mY;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamInput has not been initialized.");
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
		jSetError(ERROR_NULL_POINTER, "JamInput doesn't exist (jamInputCheckMouseButton). SDL Error: %s\n", SDL_GetError());
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
		jSetError(ERROR_NULL_POINTER, "JamInput doesn't exist (jamInputCheckMouseButtonPressed). SDL Error: %s\n", SDL_GetError());
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
		jSetError(ERROR_NULL_POINTER, "JamInput doesn't exist (jamInputCheckMouseButtonReleased). SDL Error: %s\n", SDL_GetError());
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
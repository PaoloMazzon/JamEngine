//
// Created by lugi1 on 2018-07-13.
//

#include <malloc.h>
#include <stdio.h>
#include <Input.h>
#include <JamError.h>

//////////////////////////////////////////////////////////////
void initInput() {
	gInputPointer = (Input*)malloc(sizeof(Input));

	// Double check, of course
	if (gInputPointer != NULL) {
		// Now we grab the first piece
		gInputPointer->currentInput = SDL_GetKeyboardState(&gInputPointer->kbLen);

		// And create the other array
		gInputPointer->previousInput = (Uint8*)malloc(gInputPointer->kbLen);

		// And again, double check
		if (gInputPointer->previousInput == NULL) {
			free(gInputPointer);
			fprintf(stderr, "Failed to allocate placeholder array (createInput). SDL Error: %s\n", SDL_GetError());
			gInputPointer = NULL;
			jSetError(ERROR_ALLOC_FAILED);
		}
	} else {
		fprintf(stderr, "Failed to allocate input. SDL Error: %s\n", SDL_GetError());
		jSetError(ERROR_ALLOC_FAILED);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void updateInput(double screenMultiplier) {
	int i, wx, wy;

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
	} else {
		fprintf(stderr, "Input doesn't exist (updateInput). SDL Error: %s\n", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckKey(SDL_Scancode key) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		if (key > 0 && key < gInputPointer->kbLen) {
			ret = gInputPointer->currentInput[key];
		} else {
			fprintf(stderr, "Scancode out of range (inputCheckKey). SDL Error: %s\n", SDL_GetError());
			jSetError(ERROR_OUT_OF_BOUNDS);
		}
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckKey). SDL Error: %s\n", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckKeyPressed(SDL_Scancode key) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		if (key > 0 && key < gInputPointer->kbLen) {
			// If it was just pressed, last frame's value would be 0
			ret = gInputPointer->currentInput[key] && !gInputPointer->previousInput[key];
		} else {
			fprintf(stderr, "Scancode out of range (inputCheckKeyPressed). SDL Error: %s\n", SDL_GetError());
			jSetError(ERROR_OUT_OF_BOUNDS);
		}
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckKeyPressed). SDL Error: %s\n", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckKeyReleased(SDL_Scancode key) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		if (key > 0 && key < gInputPointer->kbLen) {
			// If it was just released, last frame's value would be 1 and the current 0
			ret = !gInputPointer->currentInput[key] && gInputPointer->previousInput[key];
		} else {
			fprintf(stderr, "Scancode out of range (inputCheckKeyReleased). SDL Error: %s\n", SDL_GetError());
			jSetError(ERROR_OUT_OF_BOUNDS);
		}
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckKeyReleased). SDL Error: %s\n", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckMouseButton(uint8 button) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		ret = gInputPointer->mouseState & button;
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckMouseButton). SDL Error: %s\n", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckMouseButtonPressed(uint8 button) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		ret = !(gInputPointer->previousMouseState & button) && (gInputPointer->mouseState & button);
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckMouseButtonPressed). SDL Error: %s\n", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckMouseButtonReleased(uint8 button) {
	bool ret = false;

	// Check input exists
	if (gInputPointer != NULL) {
		ret = (gInputPointer->previousMouseState & button) && !(gInputPointer->mouseState & button);
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckMouseButtonReleased). SDL Error: %s\n", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void quitInput() {
	if (gInputPointer != NULL) {
		free(gInputPointer->previousInput);
		free(gInputPointer);
	}
}
//////////////////////////////////////////////////////////////
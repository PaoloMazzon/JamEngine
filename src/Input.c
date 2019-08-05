//
// Created by lugi1 on 2018-07-13.
//

#include <malloc.h>
#include <stdio.h>
#include <Input.h>
#include <JamError.h>

//////////////////////////////////////////////////////////////
Input* createInput() {
	Input* input = (Input*)malloc(sizeof(Input));

	// Double check, of course
	if (input != NULL) {
		// Now we grab the first piece
		input->currentInput = SDL_GetKeyboardState(&input->kbLen);

		// And create the other array
		input->previousInput = (Uint8*)malloc(input->kbLen);

		// And again, double check
		if (input->previousInput == NULL) {
			free(input);
			fprintf(stderr, "Failed to allocate placeholder array (createInput). SDL Error: %s", SDL_GetError());
			input = NULL;
			jSetError(ERROR_ALLOC_FAILED);
		}
	} else {
		fprintf(stderr, "Failed to allocate input. SDL Error: %s", SDL_GetError());
		jSetError(ERROR_ALLOC_FAILED);
	}

	return input;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void updateInput(Input* input, double screenMultiplier) {
	int i, wx, wy;

	// Check that the input exists
	if (input != NULL) {
		// Copy the current frame to the old frame
		for (i = 0; i < input->kbLen; i++)
			input->previousInput[i] = input->currentInput[i];

		// Update mouse stuff
		input->previousMouseState = input->mouseState;
		input->mouseState = SDL_GetMouseState(&input->mX, &input->mY);

		input->mX = (int)((double)input->mX / screenMultiplier);
		input->mY = (int)((double)input->mY / screenMultiplier);
	} else {
		fprintf(stderr, "Input doesn't exist (updateInput). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckKey(Input* input, SDL_Scancode key) {
	bool ret = false;

	// Check input exists
	if (input != NULL) {
		if (key > 0 && key < input->kbLen) {
			ret = input->currentInput[key];
		} else {
			fprintf(stderr, "Scancode out of range (inputCheckKey). SDL Error: %s", SDL_GetError());
			jSetError(ERROR_OUT_OF_BOUNDS);
		}
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckKey). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckKeyPressed(Input* input, SDL_Scancode key) {
	bool ret = false;

	// Check input exists
	if (input != NULL) {
		if (key > 0 && key < input->kbLen) {
			// If it was just pressed, last frame's value would be 0
			ret = input->currentInput[key] && !input->previousInput[key];
		} else {
			fprintf(stderr, "Scancode out of range (inputCheckKeyPressed). SDL Error: %s", SDL_GetError());
			jSetError(ERROR_OUT_OF_BOUNDS);
		}
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckKeyPressed). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckKeyReleased(Input* input, SDL_Scancode key) {
	bool ret = false;

	// Check input exists
	if (input != NULL) {
		if (key > 0 && key < input->kbLen) {
			// If it was just released, last frame's value would be 1 and the current 0
			ret = !input->currentInput[key] && input->previousInput[key];
		} else {
			fprintf(stderr, "Scancode out of range (inputCheckKeyReleased). SDL Error: %s", SDL_GetError());
			jSetError(ERROR_OUT_OF_BOUNDS);
		}
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckKeyReleased). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckMouseButton(Input* input, uint8 button) {
	bool ret = false;

	// Check input exists
	if (input != NULL) {
		ret = input->mouseState & button;
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckMouseButton). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckMouseButtonPressed(Input* input, uint8 button) {
	bool ret = false;

	// Check input exists
	if (input != NULL) {
		ret = !(input->previousMouseState & button) && (input->mouseState & button);
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckMouseButtonPressed). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool inputCheckMouseButtonReleased(Input* input, uint8 button) {
	bool ret = false;

	// Check input exists
	if (input != NULL) {
		ret = (input->previousMouseState & button) && !(input->mouseState & button);
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckMouseButtonReleased). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}

	return ret;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void freeInput(Input* input) {
	if (input != NULL) {
		free(input->previousInput);
		free(input);
	}
}
//////////////////////////////////////////////////////////////
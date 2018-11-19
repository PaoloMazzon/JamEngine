//
// Created by lugi1 on 2018-07-13.
//

#include <malloc.h>
#include <stdio.h>
#include <Input.h>

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
		}
	} else {
		fprintf(stderr, "Failed to allocate input. SDL Error: %s", SDL_GetError());
	}

	return input;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void updateInput(Input* input) {
	int i;

	// Check that the input exists
	if (input != NULL) {
		// Copy the current frame to the old frame
		for (i = 0; i < input->kbLen; i++)
			input->previousInput[i] = input->currentInput[i];

		// Update mouse stuff
		input->previousMouseState = input->mouseState;
		input->mouseState = SDL_GetMouseState(&input->mX, &input->mY);
	} else {
		fprintf(stderr, "Input doesn't exist (updateInput). SDL Error: %s", SDL_GetError());
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
		}
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckKey). SDL Error: %s", SDL_GetError());
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
		}
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckKeyPressed). SDL Error: %s", SDL_GetError());
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
		}
	} else {
		fprintf(stderr, "Input doesn't exist (inputCheckKeyReleased). SDL Error: %s", SDL_GetError());
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
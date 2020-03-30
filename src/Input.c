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

	// Gamepad shenanigans
	SDL_GameController* controllers[8]; ///< The gamepads we get input from
	uint8 numControllers;               ///< How many gamepads there are
	sint16 gamepadControls[8][21];      ///< The triggers, axis, and buttons of any controller
	sint16 gamepadControlsPrev[8][21];  ///< Previous gamepad input
	sint16 deadzone;                    ///< Dead zone for various joystick/trigger things
} _JamInput;

static _JamInput* gInputPointer;

//////////////////////////////////////////////////////////////
void jamInputInit() {
	int i;
	gInputPointer = (_JamInput*)malloc(sizeof(_JamInput));

	if (gInputPointer != NULL) {
		gInputPointer->numControllers = 0;
		// Now we grab the first piece
		gInputPointer->currentInput = SDL_GetKeyboardState(&gInputPointer->kbLen);

		// And create the other array
		gInputPointer->previousInput = (Uint8*)malloc((size_t)gInputPointer->kbLen);

		// By default, the deadzone is a quarter of full
		gInputPointer->deadzone = 0x8000 / 4;

		// Load gamepads
		jamInputRefreshGamepads();
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
uint8 jamInputGetNumGamepads() {
	if (gInputPointer != NULL) {
		return gInputPointer->numControllers;
	} else {
		jSetError(ERROR_NULL_POINTER, "Input not initialized");
	}

	return 0;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamInputRefreshGamepads() {
	int i;
	if (gInputPointer != NULL) {
		// Close the old ones first
		for (i = 0; i < gInputPointer->numControllers; i++)
			SDL_GameControllerClose(gInputPointer->controllers[i]);

		gInputPointer->numControllers = 0;

		// Find the gamepads
		for (i = 0; i < SDL_NumJoysticks(); i++) {
			if (SDL_IsGameController(i)) {
				gInputPointer->controllers[gInputPointer->numControllers] = SDL_GameControllerOpen(i);
				if (gInputPointer->controllers[gInputPointer->numControllers] == NULL)
					jSetError(ERROR_SDL_ERROR, "Failed to open a game controller");
				else
					gInputPointer->numControllers++;
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Input not initialized");
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

		// Gamepad input
		for (i = 0; i < gInputPointer->numControllers; i++) {
			memcpy(gInputPointer->gamepadControlsPrev, gInputPointer->gamepadControls, 8 * 21 * sizeof(sint16));
			gInputPointer->gamepadControls[i][JAM_BUTTON_A]             = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_A) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_B]             = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_B) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_X]             = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_X) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_Y]             = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_Y) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_BACK]          = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_BACK) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_GUIDE]         = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_GUIDE) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_START]         = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_START) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_LEFTSTICK]     = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_LEFTSTICK) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_RIGHTSTICK]    = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_RIGHTSTICK) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_LEFTSHOULDER]  = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_LEFTSHOULDER) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_RIGHTSHOULDER] = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_DPAD_UP]       = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_DPAD_UP) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_DPAD_DOWN]     = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_DPAD_DOWN) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_BUTTON_DPAD_LEFT]     = SDL_GameControllerGetButton(gInputPointer->controllers[i], SDL_CONTROLLER_BUTTON_DPAD_LEFT) * (sint16)0x8000;
			gInputPointer->gamepadControls[i][JAM_AXIS_LEFTX]           = SDL_GameControllerGetAxis(gInputPointer->controllers[i], SDL_CONTROLLER_AXIS_LEFTX);
			gInputPointer->gamepadControls[i][JAM_AXIS_LEFTY]           = SDL_GameControllerGetAxis(gInputPointer->controllers[i], SDL_CONTROLLER_AXIS_LEFTY);
			gInputPointer->gamepadControls[i][JAM_AXIS_RIGHTX]          = SDL_GameControllerGetAxis(gInputPointer->controllers[i], SDL_CONTROLLER_AXIS_RIGHTX);
			gInputPointer->gamepadControls[i][JAM_AXIS_RIGHTY]          = SDL_GameControllerGetAxis(gInputPointer->controllers[i], SDL_CONTROLLER_AXIS_RIGHTY);
			gInputPointer->gamepadControls[i][JAM_AXIS_TRIGGERLEFT]     = SDL_GameControllerGetAxis(gInputPointer->controllers[i], SDL_CONTROLLER_AXIS_TRIGGERLEFT);
			gInputPointer->gamepadControls[i][JAM_AXIS_TRIGGERRIGHT]    = SDL_GameControllerGetAxis(gInputPointer->controllers[i], SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
		}
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputGamepadIsConnected(int gamepad) {
	if (gInputPointer != NULL && gInputPointer->numControllers > gamepad) {
		return SDL_GameControllerGetAttached(gInputPointer->controllers[gamepad]);
	} else if (gInputPointer == NULL) {
		jSetError(ERROR_NULL_POINTER, "Input not initialized");
	}

	return 0;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
float jamInputCheckGamepad(int gamepad, JamGamepadTriggers trigger) {
	if (gInputPointer != NULL && trigger < 21 && gInputPointer->numControllers > gamepad) {
		return (float)gInputPointer->gamepadControls[gamepad][trigger] / (float)0x8000;
	} else {
		if (trigger >= 21)
			jSetError(ERROR_OUT_OF_BOUNDS, "Trigger value out of bounds");
		if (gInputPointer == NULL)
			jSetError(ERROR_NULL_POINTER, "Input not initialized");
	}

	return 0;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputCheckGamepadPressed(int gamepad, JamGamepadTriggers trigger) {
	if (gInputPointer != NULL && trigger < 21 && gInputPointer->numControllers > gamepad) {
		if (trigger == JAM_AXIS_LEFTX || trigger == JAM_AXIS_LEFTY ||
				trigger == JAM_AXIS_RIGHTX || trigger == JAM_AXIS_RIGHTY) {
			return abs(gInputPointer->gamepadControls[gamepad][trigger]) > gInputPointer->deadzone &&
				   abs(gInputPointer->gamepadControlsPrev[gamepad][trigger]) < gInputPointer->deadzone;
		} else if (trigger == JAM_AXIS_TRIGGERLEFT || trigger == JAM_AXIS_TRIGGERRIGHT) {
			return gInputPointer->gamepadControls[gamepad][trigger] > gInputPointer->deadzone &&
				   gInputPointer->gamepadControlsPrev[gamepad][trigger] < gInputPointer->deadzone;
		} else {
			return gInputPointer->gamepadControls[gamepad][trigger] && !gInputPointer->gamepadControlsPrev[trigger][gamepad];
		}
	} else {
		if (trigger >= 21)
			jSetError(ERROR_OUT_OF_BOUNDS, "Trigger value out of bounds");
		if (gInputPointer == NULL)
			jSetError(ERROR_NULL_POINTER, "Input not initialized");
	}

	return 0;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
bool jamInputCheckGamepadReleased(int gamepad, JamGamepadTriggers trigger) {
	if (gInputPointer != NULL && trigger < 21 && gInputPointer->numControllers > gamepad) {
		if (trigger == JAM_AXIS_LEFTX || trigger == JAM_AXIS_LEFTY ||
			trigger == JAM_AXIS_RIGHTX || trigger == JAM_AXIS_RIGHTY) {
			return abs(gInputPointer->gamepadControls[gamepad][trigger]) < gInputPointer->deadzone &&
				   abs(gInputPointer->gamepadControlsPrev[gamepad][trigger]) > gInputPointer->deadzone;
		} else if (trigger == JAM_AXIS_TRIGGERLEFT || trigger == JAM_AXIS_TRIGGERRIGHT) {
			return gInputPointer->gamepadControls[gamepad][trigger] < gInputPointer->deadzone &&
				   gInputPointer->gamepadControlsPrev[gamepad][trigger] > gInputPointer->deadzone;
		} else {
			return !gInputPointer->gamepadControls[gamepad][trigger] && gInputPointer->gamepadControlsPrev[trigger][gamepad];
		}
	} else {
		if (trigger >= 21)
			jSetError(ERROR_OUT_OF_BOUNDS, "Trigger value out of bounds");
		if (gInputPointer == NULL)
			jSetError(ERROR_NULL_POINTER, "Input not initialized");
	}

	return 0;
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
	int i;
	if (gInputPointer != NULL) {
		for (i = 0; i < gInputPointer->numControllers; i++)
			SDL_GameControllerClose(gInputPointer->controllers[i]);

		free(gInputPointer->previousInput);
		free(gInputPointer);
	}
}
//////////////////////////////////////////////////////////////
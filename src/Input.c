//
// Created by lugi1 on 2018-07-13.
//

#include <malloc.h>
#include <stdio.h>
#include <Input.h>
#include <StringUtil.h>
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

//////////////////////////////////////////////////////////////////////////////////////////////////////
JamControlMap* jamControlMapCreate() {
	JamControlMap* map = (JamControlMap*)malloc(sizeof(JamControlMap));
	_JamInputList** bucket;

	if (map != NULL) {
		// We zero the bucket because we may need to save it and as such iterate it
		bucket = (_JamInputList**)calloc(INPUT_BUCKET_SIZE, sizeof(_JamInputList*));

		if (bucket != NULL) {
			map->bucket = bucket;
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create bucket");
			free(map);
			map = NULL;
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create map");
	}

	return map;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void jamControlMapFree(JamControlMap* map) {
	int i;
	_JamInputList* next, *prev;
	if (map != NULL) {
		for (i = 0; i < INPUT_BUCKET_SIZE; i++) {
			if (map->bucket[i] != NULL) {
				free(map->bucket[i]->name);
				free(map->bucket[i]->inputList);
				next = map->bucket[i]->next;

				while (next != NULL) {
					free(next->name);
					free(next->inputList);
					prev = next->next;
					free(next);
					next = prev;
				}

				free(map->bucket[i]);
			}
		}

		free(map->bucket);
		free(map);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
static _JamInputList* _jamInputListCreate(const char* name) {
    _JamInputList* list = (_JamInputList*)malloc(sizeof(_JamInputList));
	if (list != NULL) {
		list->name = (char*)malloc(strlen(name) + 1);
		list->next = NULL;
		list->size = 0;
		list->inputList = NULL;

		if (list->name != NULL) {
			strcpy(list->name, name);
		} else {
			free(list);
			jSetError(ERROR_ALLOC_FAILED, "Failed to copy string");
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create input list");
	}

	return list;
}

// Makes sure a control exists in a map, returns the list or NULL if it failed
static _JamInputList* _jamControlMapAssertControl(JamControlMap* map, const char* name, bool createIfNotFound) {
	_JamInputList* list, *next;
	list = NULL;
	int pos = (int)jamHashString(name, INPUT_BUCKET_SIZE);
	
	if (map != NULL) {
		// There is already stuff here
		if (map->bucket[pos] != NULL) {
			// Either find the element in the linked list or find the last element in the linked list
			next = map->bucket[pos];
		    while (next->next != NULL && strcmp(next->name, name) != 0)
				next = next->next;

			if (strcmp(next->name, name) == 0) {
				// This is it
				list = next;
			} else if (createIfNotFound) {
				// We must create it
				list = _jamInputListCreate(name);
			    next->next = list;
			}
		} else if (createIfNotFound) {
			// We need to make it here
			list = _jamInputListCreate(name);
			map->bucket[pos] = list;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Map does not exist");
	}

	return list;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
uint8 jamControlMapAddInput(JamControlMap* map, const char* control, int code, int gamepad, JamInputType type, JamInputState state, float multiplier) {
	_JamInputList* list = _jamControlMapAssertControl(map, control, true);
	_JamInputBinding* newVec;
	uint8 inputLoc = 0;

	// Asserting the list guarantees the map exists
	if (list != NULL) {
		// Extend the current list's list to allow another input
		newVec = (_JamInputBinding*)realloc(list->inputList, sizeof(_JamInputBinding) * (list->size + 1));

		// Fill the struct with its new values
		if (newVec != NULL) {
			newVec[list->size].type = type;
			newVec[list->size].code = code;
			newVec[list->size].gamepad = gamepad;
			newVec[list->size].modifier = multiplier;
			newVec[list->size].state = state;
			list->inputList = newVec;
			list->size += 1;
		} else {
			jSetError(ERROR_REALLOC_FAILED, "Failed to resize input vector");
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Could not assert the input list");
	}

	return inputLoc;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
JamControlMap* jamControlMapLoad(JamBuffer* buffer) {
	uint8 numInputs;
	uint16 nameSize;
	char* tempName;
	_JamInputBinding* list;
	_JamInputBinding bind;
	int numControls;
	char string[] = "12345\0";
	bool error = false;
	JamControlMap* map = jamControlMapCreate();
	int i, j;

	if (buffer != NULL && map != NULL) {
		buffer->pointer = 0;
		jamBufferReadByteX(buffer, string, INPUT_VERSION_STRING_SIZE);
		if (strcmp(INPUT_VERSION_STRING, string) == 0) {
			jamBufferReadByte4(buffer, &numControls);

			// We load each control now
			for (i = 0; i < numControls && !error; i++) {
				// Create name and list memory first
				jamBufferReadByte2(buffer, &nameSize);
				jamBufferReadByte1(buffer, &numInputs);
				tempName = (char*)malloc(nameSize + 1);
				list = (_JamInputBinding*)malloc(numInputs * sizeof(_JamInputBinding));
				if (tempName != NULL && list != NULL) {
					jamBufferReadByteX(buffer, tempName, nameSize);
					tempName[nameSize] = 0;

					// Now load each input
					for (j = 0; j < numInputs; j++) {
						jamBufferReadByteX(buffer, &bind, sizeof(_JamInputBinding));
						jamControlMapAddInput(map, tempName, bind.code, bind.gamepad, bind.type, bind.state, bind.modifier);
					}
				} else {
					error = true;
					free(list);
					free(tempName);
					jSetError(ERROR_ALLOC_FAILED, "Failed to copy name");
				}
			}
		} else {
			jSetError(ERROR_WARNING, "Control map buffer version incompatible");
		}
	} else {
		if (buffer == NULL) {
			jamControlMapFree(map);
			jSetError(ERROR_NULL_POINTER, "buffer doesn't exist");
		}
		if (map == NULL)
			jSetError(ERROR_ALLOC_FAILED, "Failed to create map");
	}

	return map;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void jamControlMapRemoveInput(JamControlMap* map, const char* control, uint8 input) {
	int i;
	_JamInputList* list = _jamControlMapAssertControl(map, control, false);

	if (list != NULL && input < list->size) {
		for (i = input; i < list->size - 1; i++)
			list->inputList[i] = list->inputList[i + 1];
		list->inputList = (_JamInputBinding*)realloc(list->inputList, (list->size - 1) * sizeof(_JamInputBinding));
		list->size--;
	} else {
		jSetError(ERROR_NULL_POINTER, "Control [%s] doesn't exist", control);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void jamControlMapRemoveControl(JamControlMap* map, const char* control) {
	int pos = (int)jamHashString(control, INPUT_BUCKET_SIZE);
	_JamInputList* next, *prev;

	if (map != NULL) {
		if (map->bucket[pos] != NULL) {
			// Locate the list in the bucket/linked list
			next = map->bucket[pos];
			prev = NULL;
			while (next->next != NULL && strcmp(control, next->name) != 0) {
				prev = next;
				next = next->next;
			}

			// We don't know if we found it or hit the end of the linked list yet
			if (strcmp(control, next->name) == 0) {
				free(next->inputList);
				free(next->name);
				if (prev != NULL)
					prev->next = next->next;
				else
					map->bucket[pos] = NULL;
				free(next);

			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Map doesn't exist");
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
float jamControlMapCheck(JamControlMap* map, const char* control) {
	float check = 0;
	uint8 i;
	_JamInputList* list = _jamControlMapAssertControl(map, control, false);
	_JamInputBinding* bind;

	if (list != NULL) {
		for (i = 0; i < list->size; i++) {
			bind = &list->inputList[i];
			if (bind->type == JAM_KEYBOARD_INPUT) {
				if (bind->state == JAM_INPUT_PRESSED)
					check += (float)jamInputCheckKeyPressed((JamKeyboardKeys)bind->code) * bind->modifier;
				else if (bind->state == JAM_INPUT_RELEASED)
					check += (float)jamInputCheckKeyReleased((JamKeyboardKeys)bind->code) * bind->modifier;
				else if (bind->state == JAM_INPUT_ACTIVE)
					check += (float)jamInputCheckKey((JamKeyboardKeys)bind->code) * bind->modifier;
			} else if (bind->type == JAM_GAMEPAD_INPUT) {
				if (bind->state == JAM_INPUT_PRESSED)
					check += (float)jamInputCheckGamepadPressed(bind->gamepad, (JamGamepadTriggers)bind->code) * bind->modifier;
				else if (bind->state == JAM_INPUT_RELEASED)
					check += (float)jamInputCheckGamepadReleased(bind->gamepad, (JamGamepadTriggers)bind->code) * bind->modifier;
				else if (bind->state == JAM_INPUT_ACTIVE)
					check += jamInputCheckGamepad(bind->gamepad, (JamGamepadTriggers)bind->code) * bind->modifier;
			} else if (bind->type == JAM_MOUSE_INPUT) {
				if (bind->state == JAM_INPUT_PRESSED)
					check += (float)jamInputCheckMouseButtonPressed((uint8)bind->code) * bind->modifier;
				else if (bind->state == JAM_INPUT_RELEASED)
					check += (float)jamInputCheckMouseButtonReleased((uint8)bind->code) * bind->modifier;
				else if (bind->state == JAM_INPUT_ACTIVE)
					check += (float)jamInputCheckMouseButton((uint8)bind->code) * bind->modifier;
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Control [%s] not found", control);
	}

	// Clamp between -1 and 1
	return (check > 1) ? 1 : (check < -1 ? -1 : check);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
JamBuffer* jamControlMapSerialize(JamControlMap* map) {
	JamBuffer* buffer = NULL;
	int i, j;
	uint64 totalSize = 4 + INPUT_VERSION_STRING_SIZE;
	int count = 0;
	_JamInputList *next;
	uint16 len;

	if (map != NULL) {
		// We need to count the total inputs as well as their name's size first
		for (i = 0; i < INPUT_BUCKET_SIZE; i++) {
			next = map->bucket[i];
			while (next != NULL) {
				count++;
				totalSize += strlen(next->name) + 3;
				totalSize += sizeof(_JamInputBinding) * next->size;
				next = next->next;
			}
		}

		buffer = jamBufferCreate(totalSize);

		// Now we actually load the buffer
		if (buffer != NULL) {
			jamBufferAddByteX(buffer, INPUT_VERSION_STRING, INPUT_VERSION_STRING_SIZE);
			jamBufferAddByte4(buffer, &count);
			for (i = 0; i < INPUT_BUCKET_SIZE; i++) {
				next = map->bucket[i];
				while (next != NULL) {
					// First the information on this control
					len = (uint16)strlen(next->name);
					jamBufferAddByte2(buffer, &len);
					jamBufferAddByte1(buffer, &next->size);
					jamBufferAddByteX(buffer, next->name, len);

					// Now all of the inputs
					for (j = 0; j < next->size; j++) {
						jamBufferAddByte4(buffer, &next->inputList[j].code);
						jamBufferAddByte4(buffer, &next->inputList[j].gamepad);
						jamBufferAddByte4(buffer, &next->inputList[j].type);
						jamBufferAddByte4(buffer, &next->inputList[j].state);
						jamBufferAddByte4(buffer, &next->inputList[j].modifier);
					}

					next = next->next;
				}
			}
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create buffer");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Map doesn't exist");
	}

	return buffer;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
uint8 jamInputGetNumGamepads() {
	if (gInputPointer != NULL) {
		return gInputPointer->numControllers;
	} else {
		jSetError(ERROR_NULL_POINTER, "Input not initialized");
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool jamInputIsInitialized() {
	return gInputPointer != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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

			// Deadzone
			if (abs(gInputPointer->gamepadControls[i][JAM_AXIS_LEFTX]) < gInputPointer->deadzone)
				gInputPointer->gamepadControls[i][JAM_AXIS_LEFTX] = 0;
			if (abs(gInputPointer->gamepadControls[i][JAM_AXIS_LEFTY]) < gInputPointer->deadzone)
				gInputPointer->gamepadControls[i][JAM_AXIS_LEFTY] = 0;
			if (abs(gInputPointer->gamepadControls[i][JAM_AXIS_RIGHTX]) < gInputPointer->deadzone)
				gInputPointer->gamepadControls[i][JAM_AXIS_RIGHTX] = 0;
			if (abs(gInputPointer->gamepadControls[i][JAM_AXIS_RIGHTY]) < gInputPointer->deadzone)
				gInputPointer->gamepadControls[i][JAM_AXIS_RIGHTY] = 0;
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
bool jamInputGamepadIsConnected(int gamepad) {
	if (gInputPointer != NULL && gInputPointer->numControllers > gamepad) {
		return SDL_GameControllerGetAttached(gInputPointer->controllers[gamepad]);
	} else if (gInputPointer == NULL) {
		jSetError(ERROR_NULL_POINTER, "Input not initialized");
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
int jamInputGetMouseX() {
	if (gInputPointer != NULL) {
		return gInputPointer->mX;
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput has not been initialized.");
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
int jamInputGetMouseY() {
	if (gInputPointer != NULL) {
		return gInputPointer->mY;
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput has not been initialized.");
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
double jamInputGetMouseVelocity() {
	if (gInputPointer != NULL) {
		return pointDistance((double)gInputPointer->mX, (double)gInputPointer->mY, (double)gInputPointer->mXP, (double)gInputPointer->mYP);
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput has not been initialized.");
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
double jamInputGetMouseDirection() {
	if (gInputPointer != NULL) {
		return pointAngle((double)gInputPointer->mX, (double)gInputPointer->mY, (double)gInputPointer->mXP, (double)gInputPointer->mYP);
	} else {
		jSetError(ERROR_NULL_POINTER, "_JamInput has not been initialized.");
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void jamInputQuit() {
	int i;
	if (gInputPointer != NULL) {
		for (i = 0; i < gInputPointer->numControllers; i++)
			SDL_GameControllerClose(gInputPointer->controllers[i]);

		free(gInputPointer->previousInput);
		free(gInputPointer);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

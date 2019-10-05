/// \file Input.h
/// \author lugi1
/// \brief Declares the input handler and some functions to make it easier to use
#pragma once
#include <SDL.h>
#include "Constants.h"
#define MOUSE_LEFT_BUTTON SDL_BUTTON(1)
#define MOUSE_MIDDLE_BUTTON SDL_BUTTON(2)
#define MOUSE_RIGHT_BUTTON SDL_BUTTON(3)

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
	const Uint8* currentInput; ///< This is the current frame's input
	Uint8* previousInput; ///< This is the last frame's input
	int kbLen; ///< The length of the two above arrays

	// Mouse stuff
	int mX; ///< The mouse's current x position
	int mY; ///< The mouse's current y position
	uint32 mouseState; ///< Current frame's mice buttons
	uint32 previousMouseState; ///< Last frame's mice buttons
} Input;

Input* gInputPointer;

/// \brief Creates an input struct for keeping track of user-input
/// \throws ERROR_ALLOC_FAILED
void initInput();

/// \brief Updates an input struct
///
/// This must either be ran before you run rendererProcEvents or the
/// input class given to rendererProcEvents for input to work properly
///
/// \throws ERROR_NULL_POINTER
void updateInput(double screenMultiplier);

/// \brief Checks if a key is currently pressed
/// \throws ERROR_OUT_OF_BOUNDS
/// \throws ERROR_NULL_POINTER
bool inputCheckKey(SDL_Scancode key);

/// \brief Checks if a key was just pressed
/// \throws ERROR_OUT_OF_BOUNDS
/// \throws ERROR_NULL_POINTER
bool inputCheckKeyPressed(SDL_Scancode key);

/// \brief Checks if a key was just released
/// \throws ERROR_OUT_OF_BOUNDS
/// \throws ERROR_NULL_POINTER
bool inputCheckKeyReleased(SDL_Scancode key);

/// \brief Checks if a mouse button is currently pressed
/// \throws ERROR_NULL_POINTER
bool inputCheckMouseButton(uint8 button);

/// \brief Checks if a mouse button was just pressed
/// \throws ERROR_NULL_POINTER
bool inputCheckMouseButtonPressed(uint8 button);

/// \brief Checks if a mouse button was just released
/// \throws ERROR_NULL_POINTER
bool inputCheckMouseButtonReleased(uint8 button);

/// \brief Clears an Input from memory
void quitInput();
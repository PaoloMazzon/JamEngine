/// \file Input.h
/// \author lugi1
/// \brief Declares the input handler and some functions to make it easier to use
#pragma once
#include "Constants.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOUSE_LEFT_BUTTON 1
#define MOUSE_MIDDLE_BUTTON 2
#define MOUSE_RIGHT_BUTTON 4

/// \brief The keyboard bindings needed to check keyboard presses
///
/// This is basically just the SDL2 SDL_SCANCODE_* stuff renamed
/// for this engine so SDL2 isn't a dependency when using this engine
/// in its library form (as well as portability to other backends)
typedef enum {
	JAM_KB_UNKNOWN = 0,
	JAM_KB_A = 4,
	JAM_KB_B = 5,
	JAM_KB_C = 6,
	JAM_KB_D = 7,
	JAM_KB_E = 8,
	JAM_KB_F = 9,
	JAM_KB_G = 10,
	JAM_KB_H = 11,
	JAM_KB_I = 12,
	JAM_KB_J = 13,
	JAM_KB_K = 14,
	JAM_KB_L = 15,
	JAM_KB_M = 16,
	JAM_KB_N = 17,
	JAM_KB_O = 18,
	JAM_KB_P = 19,
	JAM_KB_Q = 20,
	JAM_KB_R = 21,
	JAM_KB_S = 22,
	JAM_KB_T = 23,
	JAM_KB_U = 24,
	JAM_KB_V = 25,
	JAM_KB_W = 26,
	JAM_KB_X = 27,
	JAM_KB_Y = 28,
	JAM_KB_Z = 29,
	JAM_KB_1 = 30,
	JAM_KB_2 = 31,
	JAM_KB_3 = 32,
	JAM_KB_4 = 33,
	JAM_KB_5 = 34,
	JAM_KB_6 = 35,
	JAM_KB_7 = 36,
	JAM_KB_8 = 37,
	JAM_KB_9 = 38,
	JAM_KB_0 = 39,
	JAM_KB_RETURN = 40,
	JAM_KB_ESCAPE = 41,
	JAM_KB_BACKSPACE = 42,
	JAM_KB_TAB = 43,
	JAM_KB_SPACE = 44,
	JAM_KB_MINUS = 45,
	JAM_KB_EQUALS = 46,
	JAM_KB_LEFTBRACKET = 47,
	JAM_KB_RIGHTBRACKET = 48,
	JAM_KB_BACKSLASH = 49,
	JAM_KB_NONUSHASH = 50,
	JAM_KB_SEMICOLON = 51,
	JAM_KB_APOSTROPHE = 52,
	JAM_KB_GRAVE = 53,
	JAM_KB_COMMA = 54,
	JAM_KB_PERIOD = 55,
	JAM_KB_SLASH = 56,
	JAM_KB_CAPSLOCK = 57,
	JAM_KB_F1 = 58,
	JAM_KB_F2 = 59,
	JAM_KB_F3 = 60,
	JAM_KB_F4 = 61,
	JAM_KB_F5 = 62,
	JAM_KB_F6 = 63,
	JAM_KB_F7 = 64,
	JAM_KB_F8 = 65,
	JAM_KB_F9 = 66,
	JAM_KB_F10 = 67,
	JAM_KB_F11 = 68,
	JAM_KB_F12 = 69,
	JAM_KB_PRINTSCREEN = 70,
	JAM_KB_SCROLLLOCK = 71,
	JAM_KB_PAUSE = 72,
	JAM_KB_INSERT = 73,
	JAM_KB_HOME = 74,
	JAM_KB_PAGEUP = 75,
	JAM_KB_DELETE = 76,
	JAM_KB_END = 77,
	JAM_KB_PAGEDOWN = 78,
	JAM_KB_RIGHT = 79,
	JAM_KB_LEFT = 80,
	JAM_KB_DOWN = 81,
	JAM_KB_UP = 82,
	JAM_KB_NUMLOCKCLEAR = 83,
	JAM_KB_KP_DIVIDE = 84,
	JAM_KB_KP_MULTIPLY = 85,
	JAM_KB_KP_MINUS = 86,
	JAM_KB_KP_PLUS = 87,
	JAM_KB_KP_ENTER = 88,
	JAM_KB_KP_1 = 89,
	JAM_KB_KP_2 = 90,
	JAM_KB_KP_3 = 91,
	JAM_KB_KP_4 = 92,
	JAM_KB_KP_5 = 93,
	JAM_KB_KP_6 = 94,
	JAM_KB_KP_7 = 95,
	JAM_KB_KP_8 = 96,
	JAM_KB_KP_9 = 97,
	JAM_KB_KP_0 = 98,
	JAM_KB_KP_PERIOD = 99
} JamKeyboardKeys;

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
	uint8* previousInput; ///< This is the last frame's input
	int kbLen; ///< The length of the two above arrays

	// Mouse stuff
	int mX; ///< The mouse's current x position
	int mY; ///< The mouse's current y position
	uint32 mouseState; ///< Current frame's mice buttons
	uint32 previousMouseState; ///< Last frame's mice buttons
} JamInput;

/// \brief Creates an input struct for keeping track of user-input
/// \throws ERROR_ALLOC_FAILED
void jamInputInit();

/// \brief Returns true if input was initialized properly or false otherwise
bool jamInputIsInitialized();

/// \brief Updates an input struct
///
/// This must either be ran before you run rendererProcEvents or the
/// input class given to rendererProcEvents for input to work properly
///
/// \throws ERROR_NULL_POINTER
void jamInputUpdate(double screenMultiplier);

/// \brief Checks if a key is currently pressed
/// \throws ERROR_OUT_OF_BOUNDS
/// \throws ERROR_NULL_POINTER
bool jamInputCheckKey(JamKeyboardKeys key);

/// \brief Checks if a key was just pressed
/// \throws ERROR_OUT_OF_BOUNDS
/// \throws ERROR_NULL_POINTER
bool jamInputCheckKeyPressed(JamKeyboardKeys key);

/// \brief Checks if a key was just released
/// \throws ERROR_OUT_OF_BOUNDS
/// \throws ERROR_NULL_POINTER
bool jamInputCheckKeyReleased(JamKeyboardKeys key);

/// \brief Grabs the mouse's X position
/// \throws ERROR_NULL_POINTER
int jamInputGetMouseX();

/// \brief Grabs the mouse's Y position
/// \throws ERROR_NULL_POINTER
int jamInputGetMouseY();

/// \brief Checks if a mouse button is currently pressed
/// \throws ERROR_NULL_POINTER
bool jamInputCheckMouseButton(uint8 button);

/// \brief Checks if a mouse button was just pressed
/// \throws ERROR_NULL_POINTER
bool jamInputCheckMouseButtonPressed(uint8 button);

/// \brief Checks if a mouse button was just released
/// \throws ERROR_NULL_POINTER
bool jamInputCheckMouseButtonReleased(uint8 button);

/// \brief Clears an Input from memory
void jamInputQuit();

#ifdef __cplusplus
}
#endif
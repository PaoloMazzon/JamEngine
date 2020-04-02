/// \file Input.h
/// \author lugi1
/// \brief Declares the input handler and some functions to make it easier to use
#pragma once
#include "Constants.h"
#include "Buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOUSE_LEFT_BUTTON 1
#define MOUSE_MIDDLE_BUTTON 2
#define MOUSE_RIGHT_BUTTON 4
#define MOUSE_UNKNOWN 0

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
	JAM_KB_KP_PERIOD = 99,
	JAM_KB_NONUSBACKSLASH = 100,
	JAM_KB_APPLICATION = 101,
	JAM_KB_POWER = 102,
	JAM_KB_KP_EQUALS = 103,
	JAM_KB_F13 = 104,
	JAM_KB_F14 = 105,
	JAM_KB_F15 = 106,
	JAM_KB_F16 = 107,
	JAM_KB_F17 = 108,
	JAM_KB_F18 = 109,
	JAM_KB_F19 = 110,
	JAM_KB_F20 = 111,
	JAM_KB_F21 = 112,
	JAM_KB_F22 = 113,
	JAM_KB_F23 = 114,
	JAM_KB_F24 = 115,
	JAM_KB_EXECUTE = 116,
	JAM_KB_HELP = 117,
	JAM_KB_MENU = 118,
	JAM_KB_SELECT = 119,
	JAM_KB_STOP = 120,
	JAM_KB_AGAIN = 121,
	JAM_KB_UNDO = 122,
	JAM_KB_CUT = 123,
	JAM_KB_COPY = 124,
	JAM_KB_PASTE = 125,
	JAM_KB_FIND = 126,
	JAM_KB_MUTE = 127,
	JAM_KB_VOLUMEUP = 128,
	JAM_KB_VOLUMEDOWN = 129,
	JAM_KB_KP_COMMA = 133,
	JAM_KB_KP_EQUALSAS400 = 134,
	JAM_KB_INTERNATIONAL1 = 135,
	JAM_KB_INTERNATIONAL2 = 136,
	JAM_KB_INTERNATIONAL3 = 137,
	JAM_KB_INTERNATIONAL4 = 138,
	JAM_KB_INTERNATIONAL5 = 139,
	JAM_KB_INTERNATIONAL6 = 140,
	JAM_KB_INTERNATIONAL7 = 141,
	JAM_KB_INTERNATIONAL8 = 142,
	JAM_KB_INTERNATIONAL9 = 143,
	JAM_KB_LANG1 = 144,
	JAM_KB_LANG2 = 145,
	JAM_KB_LANG3 = 146,
	JAM_KB_LANG4 = 147,
	JAM_KB_LANG5 = 148,
	JAM_KB_LANG6 = 149,
	JAM_KB_LANG7 = 150,
	JAM_KB_LANG8 = 151,
	JAM_KB_LANG9 = 152,
	JAM_KB_ALTERASE = 153,
	JAM_KB_SYSREQ = 154,
	JAM_KB_CANCEL = 155,
	JAM_KB_CLEAR = 156,
	JAM_KB_PRIOR = 157,
	JAM_KB_RETURN2 = 158,
	JAM_KB_SEPARATOR = 159,
	JAM_KB_OUT = 160,
	JAM_KB_OPER = 161,
	JAM_KB_CLEARAGAIN = 162,
	JAM_KB_CRSEL = 163,
	JAM_KB_EXSEL = 164,
	JAM_KB_KP_00 = 176,
	JAM_KB_KP_000 = 177,
	JAM_KB_THOUSANDSSEPARATOR = 178,
	JAM_KB_DECIMALSEPARATOR = 179,
	JAM_KB_CURRENCYUNIT = 180,
	JAM_KB_CURRENCYSUBUNIT = 181,
	JAM_KB_KP_LEFTPAREN = 182,
	JAM_KB_KP_RIGHTPAREN = 183,
	JAM_KB_KP_LEFTBRACE = 184,
	JAM_KB_KP_RIGHTBRACE = 185,
	JAM_KB_KP_TAB = 186,
	JAM_KB_KP_BACKSPACE = 187,
	JAM_KB_KP_A = 188,
	JAM_KB_KP_B = 189,
	JAM_KB_KP_C = 190,
	JAM_KB_KP_D = 191,
	JAM_KB_KP_E = 192,
	JAM_KB_KP_F = 193,
	JAM_KB_KP_XOR = 194,
	JAM_KB_KP_POWER = 195,
	JAM_KB_KP_PERCENT = 196,
	JAM_KB_KP_LESS = 197,
	JAM_KB_KP_GREATER = 198,
	JAM_KB_KP_AMPERSAND = 199,
	JAM_KB_KP_DBLAMPERSAND = 200,
	JAM_KB_KP_VERTICALBAR = 201,
	JAM_KB_KP_DBLVERTICALBAR = 202,
	JAM_KB_KP_COLON = 203,
	JAM_KB_KP_HASH = 204,
	JAM_KB_KP_SPACE = 205,
	JAM_KB_KP_AT = 206,
	JAM_KB_KP_EXCLAM = 207,
	JAM_KB_KP_MEMSTORE = 208,
	JAM_KB_KP_MEMRECALL = 209,
	JAM_KB_KP_MEMCLEAR = 210,
	JAM_KB_KP_MEMADD = 211,
	JAM_KB_KP_MEMSUBTRACT = 212,
	JAM_KB_KP_MEMMULTIPLY = 213,
	JAM_KB_KP_MEMDIVIDE = 214,
	JAM_KB_KP_PLUSMINUS = 215,
	JAM_KB_KP_CLEAR = 216,
	JAM_KB_KP_CLEARENTRY = 217,
	JAM_KB_KP_BINARY = 218,
	JAM_KB_KP_OCTAL = 219,
	JAM_KB_KP_DECIMAL = 220,
	JAM_KB_KP_HEXADECIMAL = 221,
	JAM_KB_LCTRL = 224,
	JAM_KB_LSHIFT = 225,
	JAM_KB_LALT = 226,
	JAM_KB_LGUI = 227,
	JAM_KB_RCTRL = 228,
	JAM_KB_RSHIFT = 229,
	JAM_KB_RALT = 230,
	JAM_KB_RGUI = 231,
	JAM_KB_MODE = 257,
	JAM_KB_AUDIONEXT = 258,
	JAM_KB_AUDIOPREV = 259,
	JAM_KB_AUDIOSTOP = 260,
	JAM_KB_AUDIOPLAY = 261,
	JAM_KB_AUDIOMUTE = 262,
	JAM_KB_MEDIASELECT = 263,
	JAM_KB_WWW = 264,
	JAM_KB_MAIL = 265,
	JAM_KB_CALCULATOR = 266,
	JAM_KB_COMPUTER = 267,
	JAM_KB_AC_SEARCH = 268,
	JAM_KB_AC_HOME = 269,
	JAM_KB_AC_BACK = 270,
	JAM_KB_AC_FORWARD = 271,
	JAM_KB_AC_STOP = 272,
	JAM_KB_AC_REFRESH = 273,
	JAM_KB_AC_BOOKMARKS = 274,
	JAM_KB_BRIGHTNESSDOWN = 275,
	JAM_KB_BRIGHTNESSUP = 276,
	JAM_KB_DISPLAYSWITCH = 277,
	JAM_KB_KBDILLUMTOGGLE = 278,
	JAM_KB_KBDILLUMDOWN = 279,
	JAM_KB_KBDILLUMUP = 280,
	JAM_KB_EJECT = 281,
	JAM_KB_SLEEP = 282,
	JAM_KB_APP1 = 283,
	JAM_KB_APP2 = 284,
} JamKeyboardKeys;

/// \brief The various controls of a gamepad
///
/// This is similar to the SDL_CONTROLLER* values
/// but jamEngine does a lot more for gamepads to
/// make them a nicer experience.
typedef enum {
	JAM_GAMEPAD_UNKNOWN = 50,
	JAM_AXIS_LEFTX = 0,
	JAM_AXIS_LEFTY = 1,
	JAM_AXIS_RIGHTX = 2,
	JAM_AXIS_RIGHTY = 3,
	JAM_AXIS_TRIGGERLEFT = 4,
	JAM_AXIS_TRIGGERRIGHT = 5,
	JAM_BUTTON_A = 6,
	JAM_BUTTON_B = 7,
	JAM_BUTTON_X = 8,
	JAM_BUTTON_Y = 9,
	JAM_BUTTON_BACK = 10,
	JAM_BUTTON_GUIDE = 11,
	JAM_BUTTON_START = 12,
	JAM_BUTTON_LEFTSTICK = 13,
	JAM_BUTTON_RIGHTSTICK = 14,
	JAM_BUTTON_LEFTSHOULDER = 15,
	JAM_BUTTON_RIGHTSHOULDER = 16,
	JAM_BUTTON_DPAD_UP = 17,
	JAM_BUTTON_DPAD_DOWN = 18,
	JAM_BUTTON_DPAD_LEFT = 19,
	JAM_BUTTON_DPAD_RIGHT = 20
} JamGamepadTriggers;

/// \brief Describes the different input methods available
typedef enum {
	JAM_KEYBOARD_INPUT = 0, ///< Input originating from the keyboard
	JAM_GAMEPAD_INPUT = 1,  ///< Input originating from a gamepad
	JAM_MOUSE_INPUT = 2     ///< Input originating from the mouse (specifically the buttons, not the movement)
} JamInputType;

/// \brief Describes what state we want an input to be in
typedef enum {
	JAM_INPUT_PRESSED = 0,  ///< Only when this input is pressed
	JAM_INPUT_RELEASED = 1, ///< Only when this input is released
	JAM_INPUT_ACTIVE = 2    ///< Any time while this input is held down
} JamInputState;

/// \brief Used internally by _JamControlList, not to be used by the end user
typedef struct {
	int code;            ///< Keycode or gamepad code of this input
	int gamepad;         ///< What gamepad we're checking (if applicable)
	JamInputType type;   ///< The type of input we want
	JamInputState state; ///< State we're looking for (pressed, active, released)
	float modifier;      ///< Multiplier of this input
} _JamInputBinding;

/// \brief Used internally by JamControlMap, not to be used by the end user
///
/// inputList, unlike most other lists in JamEngine is a vector in the sense
/// that it is a list of _JamInputBinding's themselves and not a list of pointers
/// to _JamInputBinding's.
typedef struct {
	_JamInputBinding* inputList;  ///< Inputs to add together
	uint8 size;                   ///< Number of inputs connected to this control
	char* name;                   ///< Name of this input
	void* next;                   ///< Next in the link list should there be a collision
} _JamInputList;

/// \brief A fancy way to track input without having to check individual keys
///
/// The core idea behind a control map is that you bind any number of inputs
/// (keys, gamepad buttons, triggers) to a single control with modifiers that
/// outputs a single float value. For example, a sample control we'll call
/// movement might be bound to the following keys:
///
///  + the left key, with a -1 modifier
///  + the right key, with a +1 modifer
///  + the first gamepad's left x-axis
///
/// All three of those inputs would be multiplied by their individual multiplier
/// and added together (while being bound from [-1, 1]) when that control is
/// checked. In practice, this means you could break down your horizontal and
/// vertical movement down to 2 lines of code without ever having to check gamepads
/// or keys individually (meaning rebinding them is also easier).
typedef struct {
	_JamInputList** bucket; ///< The control bucket (for hashing)
} JamControlMap;

/// \brief Creates a control map
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ALLOC_FAILED
JamControlMap* jamControlMapCreate();

/// \brief Loads a control map from a string previously created in jamControlMapSerialize
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ALLOC_FAILED
JamControlMap* jamControlMapLoad(JamBuffer* buffer);

/// \brief Frees a control map
void jamControlMapFree(JamControlMap* map);

/// \brief Adds an input to a control
/// \param map Control map to add to
/// \param control Specific control to add the input to
/// \param code Keycode/gamepad code/mouse button to trigger off (JAM_KB_F for example)
/// \param gamepad Which gamepad to trigger off, if applicable (-1 for any)
/// \param type Type of input we are checking (mouse, keyboard, gamepad)
/// \param state State of the control we are looking for (just pressed, just released, or currently held)
/// \param multiplier Multiplier of this control
/// \return Returns the specific input this is bound to if you plan on changing it later
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OUT_OF_BOUNDS
uint8 jamControlMapAddInput(JamControlMap* map, const char* control, int code, int gamepad, JamInputType type, JamInputState state, float multiplier);

/// \brief Removes an input from a control
/// \throws ERROR_NULL_POINTER
void jamControlMapRemoveInput(JamControlMap* map, const char* control, uint8 input);

/// \brief Remove a control from a control map
/// \throws ERROR_NULL_POINTER
void jamControlMapRemoveControl(JamControlMap* map, const char* control);

/// \brief Checks a control in a control map and returns anything from -1 to 1
float jamControlMapCheck(JamControlMap* map, const char* control);

/// \brief Turns a control map into a JamBuffer, which you must free yourself
///
/// You're not really supposed to modify or load this data yourself, but here
/// is the format this outputs in anyway for documentation's sake.
///
/// First is the header information
///
///  + 5 bytes containing the version string (no terminating 0)
///  + 4 bytes denoting the number of controls in the map
///
/// Now the body, which is each individual control and its list of inputs
///
///  + 2 bytes denoting the size of the name of this control in bytes
///  + 1 bytes denoting the number of inputs for this control
///  + Some number of bytes which contains the name itself (see the first bullet point above)
///  + Sequentially, all of the inputs (1:1 copies of their _JamInputBinding in that order)
///
/// The order of the _JamInputBinding data is the same as it is listed in this
/// header file (and most likely the doxygen documentation).
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ALLOC_FAILED
JamBuffer* jamControlMapSerialize(JamControlMap* map);

/// \brief Creates an input struct for keeping track of user-input
/// \throws ERROR_ALLOC_FAILED
void jamInputInit();

/// \brief Returns true if input was initialized properly or false otherwise
bool jamInputIsInitialized();

/// \brief Forces input to reassess the number of gamepads connected to the system
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_NULL_POINTER
void jamInputRefreshGamepads();

/// \brief Gets the number of connected gamepads
/// \throws ERROR_NULL_POINTER
uint8 jamInputGetNumGamepads();

/// \brief Checks if a gamepad is still connected or not
///
/// If for example you would check gamepad 1 knowing full well that there
/// never was a gamepad 1 then the function will just return false and not
/// throw any errors. ERROR_NULL_POINTER is thrown by calling this before
/// input has been initialized.
///
/// \throws ERROR_NULL_POINTER
bool jamInputGamepadIsConnected(int gamepad);

/// \brief Updates an input struct
///
/// This must either be ran before you run rendererProcEvents or the
/// input class given to rendererProcEvents for input to work properly
///
/// \throws ERROR_NULL_POINTER
void jamInputUpdate(double screenMultiplier);

/// \brief Checks a trigger (or button) on a gamepad
/// \throws ERROR_NULL_POINTER
float jamInputCheckGamepad(int gamepad, JamGamepadTriggers trigger);

/// \brief Checks a trigger (or button) on a gamepad pressed
///
/// The base jamInputCheckGamepad returns a float because axis and
/// triggers can be various states between on and off. Released and
/// pressed, however, is either true or false and to check if axis
/// and triggers have been released/pressed it checks if their val
/// is above the deadzone.
///
/// \throws ERROR_NULL_POINTER
bool jamInputCheckGamepadPressed(int gamepad, JamGamepadTriggers trigger);

/// \brief Checks a trigger (or button) on a gamepad released
///
/// The base jamInputCheckGamepad returns a float because axis and
/// triggers can be various states between on and off. Released and
/// pressed, however, is either true or false and to check if axis
/// and triggers have been released/pressed it checks if their val
/// is above the deadzone.
///
/// \throws ERROR_NULL_POINTER
bool jamInputCheckGamepadReleased(int gamepad, JamGamepadTriggers trigger);

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

/// \brief Gets the mouse's scalar velocity
/// \throws ERROR_NULL_POINTER
double jamInputGetMouseVelocity();

/// \brief Gets the direction of the mouse's velocity in radians
/// \throws ERROR_NULL_POINTER
double jamInputGetMouseDirection();

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

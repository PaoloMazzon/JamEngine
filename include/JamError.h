/// \file JamError.h
/// \author plo
/// \brief Handles in-engine errors to make it easier than constantly
/// checking stderr

#pragma once
#include "Constants.h"

// Error codes

///< For warnings
#define ERROR_WARNING           0b0000000000000000

///< A malloc/calloc failed; most likely out of memory
#define ERROR_ALLOC_FAILED      0b0000000000000001

///< A realloc failed; most likely out of memory
#define ERROR_REALLOC_FAILED    0b0000000000000010

///< Failed to open a file; lots of reasons this can happen
#ifdef ERROR_OPEN_FAILED
#undef ERROR_OPEN_FAILED
#endif
#define ERROR_OPEN_FAILED       0b0000000000000100

///< A null pointer was passed into a JamEngine function that expected otherwise (certain functions are ok with it)
#define ERROR_NULL_POINTER      0b0000000000001000

///< While asset loading, an asset depended on an another asset that doesn't exist. For example, a sprite uses a texture that doesn't exist.
#define ERROR_ASSET_NOT_FOUND   0b0000000000010000

///< An asset of type x was expected but the asset was not that type
#define ERROR_ASSET_WRONG_TYPE  0b0000000000100000

///< There was some sort of file reading/writing error
#define ERROR_FILE_FAILED       0b0000000001000000

///< This can be a few things; from a buffer hit its limit to you tried to access an array outside of its memory
#define ERROR_OUT_OF_BOUNDS     0b0000000010000000

///< When something of a certain format was expected, but not received. For example, when you pass a sprite of 20 frames into drawSortedMap which expects a sprite of 48 frames.
#define ERROR_INCORRECT_FORMAT  0b0000000100000000

///< This encompasses all SDL errors, SDL's own error-checking functions can be used past this. There is no point wrapping every SDL error here.
#define ERROR_SDL_ERROR         0b0000001000000000

///< At least one tilemap could not be loaded from a tmx
#define ERROR_TMX_TILEMAP_ERROR 0b0000010000000000

///< At least one entity could not be loaded from a tmx
#define ERROR_TMX_ENTITY_ERROR  0b0000100000000000

#define jSetError(errorCode, format, ...) __jSetError(errorCode, format, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

/// \brief Sets an error flag
void __jSetError(uint16 errorCode, const char* format, const char* file, const char* function, int line,  ...);

/// \brief Grabs all errors and clears them
///
/// You have to & the errors to check them or just
/// check if the return is 0 to know weather or not
/// there were any errors
uint16 jGetError();
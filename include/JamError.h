/// \file JamError.h
/// \author plo
/// \brief Handles in-engine errors to make it easier than constantly
/// checking stderr

#pragma once
#include "Constants.h"

///< The global error code that stores only what errors have happened
///< If it is 0, there are no recent errors
uint8 jErrorCode;

// Error codes

///< A malloc/calloc failed; most likely out of memory
#define ERROR_ALLOC_FAILED     0b00000001

///< A realloc failed; most likely out of memory
#define ERROR_REALLOC_FAILED   0b00000010

///< Failed to open a file; lots of reasons this can happen
#define ERROR_OPEN_FAILED      0b00000100

///< A null pointer was passed into a JamEngine function that expected otherwise
///< (certain functions are ok with it)
#define ERROR_NULL_POINTER     0b00001000

///< While asset loading, an asset depended on an another asset that doesn't exist.
///< For example, a sprite uses a texture that doesn't exist.
#define ERROR_ASSET_NOT_FOUND  0b00010000

///< An asset of type x was expected but the asset was not that type
#define ERROR_ASSET_WRONG_TYPE 0b00100000

/// \brief Sets an error flag
void jSetError(uint8 errorCode);

/// \brief Grabs all errors and clears them
///
/// You have to & the errors to check them or just
/// check if the return is 0 to know weather or not
/// there were any errors
uint8 jGetError();
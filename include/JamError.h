/// \file JamError.h
/// \author plo
/// \brief Handles in-engine errors to make it easier than constantly
/// checking stderr

#pragma once
#include "Constants.h"

// Error codes
#define ERROR_ALLOC_FAILED 0b00000001
#define ERROR_OPEN_FAILED  0b00000010

///< The global error code that stores only what errors have happened
///< If it is 0, there are no recent errors
uint8 jErrorCode = 0;

/// \brief Sets an error flag
inline void jSetError(uint8 errorCode) {
	jErrorCode = errorCode | jErrorCode;
}

/// \brief Grabs all errors and clears them
///
/// You have to & the errors to check them or just
/// check if the return is 0 to know weather or not
/// there were any errors
inline uint8 jGetError() {
	uint8 oldErrors = jErrorCode;
	jErrorCode = 0;
	return oldErrors;
}

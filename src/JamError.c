//
// Created by plo on 7/29/19.
//
#include "JamError.h"
#include <stdarg.h>
#include <stdio.h>

/////////////////////////////////////////////////////////
void jSetError(uint16 errorCode, const char* format, ...) {
	// First just add the error flag to the error code
	jErrorCode = errorCode | jErrorCode;

	// Output error to file
	FILE* out = fopen(LOG_FILENAME, "aw");
	va_list* args;
	va_start(args, format);
	vfprintf(out, format, args);
	vfprintf(stderr, format, args);
	va_end(args);
}
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
uint16 jGetError() {
	uint16 oldErrors = jErrorCode;
	jErrorCode = 0;
	return oldErrors;
}
/////////////////////////////////////////////////////////
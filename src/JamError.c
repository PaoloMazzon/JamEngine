//
// Created by plo on 7/29/19.
//
#include "JamError.h"
#include <stdarg.h>
#include <stdio.h>

static uint16 jErrorCode;

/////////////////////////////////////////////////////////
void __jSetError(uint16 errorCode, const char* format, const char* function, int line, ...) {
	// First just add the error flag to the error code
	jErrorCode = errorCode | jErrorCode;

	// Output error to file
	FILE* out = fopen(LOG_FILENAME, "a");
	va_list args;
	va_start(args, line);
	fprintf(out, "[%s:%i] ", function, line);
	fprintf(stderr, "[%s:%i] ", function, line);
	vfprintf(out, format, args);
	fprintf(out, "\n");
	va_end(args);
	va_start(args, line);
	vfprintf(stderr, format, args);
	fprintf(stderr, "\n");
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

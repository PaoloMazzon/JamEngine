//
// Created by plo on 7/29/19.
//
#include "JamError.h"
#include <stdarg.h>
#include <stdio.h>

static uint16 jErrorCode;

/////////////////////////////////////////////////////////
void __jSetError(uint16 errorCode, const char* format, const char* file, const char* function, int line, ...) {
	// First just add the error flag to the error code
	jErrorCode = errorCode | jErrorCode;

	// Output error to file
	FILE* out = fopen(LOG_FILENAME, "aw");
	va_list* args;
	va_start(args, format);
	fprintf(out, "[%s:%s:%i] ", file, function, line);
	fprintf(stderr, "[%s:%s:%i] ", file, function, line);
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
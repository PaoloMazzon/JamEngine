/// \file JSON.c
/// \author Paolo Mazzon
#include "JSON.h"
#include "Buffer.h"
#include "JamError.h"
#include <malloc.h>
#include <stdio.h>

// Parses a json object from a string; the string needs not end at
// the end of the object but must start at the opening {. Whitespace
// is not allowed, use the whitespace remover
static JamJSONObject *_jamJSONParseObject(char *buffer) {
	JamJSONObject *json = calloc(1, sizeof(JamJSONObject));
}

static char *_jamJSONLoadFileNoWhiteSpace(const char *filename) {
	FILE* file = fopen(filename, "rb");
	char *buffer; // Just the file contents
	char *string = NULL; // Will store the final whitespace-less string
	char *oldString;
	uint32 size = 0;
	uint32 i;
	bool inQuotes = false;
	bool clear = false;

	if (file != NULL) {
		// Find file size
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);

		buffer = malloc(size);
		string = malloc(size);

		if (buffer != NULL) {
			// Fill the buffer
			fread(buffer, 1, size, file);

			if (ferror(file) == 0)
				clear = true;
			else
				jSetError(ERROR_FILE_FAILED, "Failed to read from file %s", filename);
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate strings for file %s", filename);
		}
		fclose(file);

		// We are okay to remove whitespace
		if (clear) {
			uint32 counter = 0;
			for (i = 0; i < size; i++) {
				if (buffer[i] == '"')
					inQuotes = !inQuotes;
				if ((buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\r' && buffer[i] != '\t') || inQuotes) {
					string[counter] = buffer[i];
					counter++;
				}
			}

			// Now that the characters are copied, resize the string to what it needs to be plus a \0
			free(buffer);
			oldString = string;
			string = realloc(oldString, counter + 1);

			if (string != NULL) {
				string[counter] = 0;
			} else {
				free(string);
				jSetError(ERROR_REALLOC_FAILED, "Failed to resize string for file %s", filename);
			}
		}
	} else {
		jSetError(ERROR_FILE_FAILED, "Failed to open file %s", filename);
	}

	return string;
}

///////////////////////////////////////////////////////////////////
/* Algorithm breakdown
 *  1. Load file into string, remove all whitespace
 *  2. Call object parsing functions on buffer (next bits outline said function)
 *  3. Count number of values in this object
 *  4. Allocate that much memory
 *  5. Populate array with values, recursively calling object parser as needed
 */
JamJSONObject *jamJSONParse(const char *filename) {
	JamBuffer *file = _jamJSONLoadFileNoWhiteSpace(filename);
	JamJSONObject *out = NULL;

	if (file != NULL) {
		out = _jamJSONParseObject((char*)file->buffer);
	} else {
		jSetError(ERROR_OPEN_FAILED, "Buffer could not be opened");
	}

	jamBufferFree(file);
	return out;
}
///////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////
void jamJSONFree(JamJSONObject *object) {
	uint32 i;
	if (object != NULL) {
		for (i = 0; i < object->size; i++) {
			free(object->values[i].key);
			if (object->values[i].type == ot_String)
				free(object->values[i].string);
			else if (object->values[i].type == ot_Object)
				jamJSONFree(object->values[i].object);
		}
		free(object->values);
	}
}
///////////////////////////////////////////////////////////////////
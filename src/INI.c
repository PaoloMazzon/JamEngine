#include "INI.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "File.h"
#include "JamError.h"

//////////////////////////////////////////////////////
INI* createINI() {
	INI* ini = (INI*)calloc(1, sizeof(INI));

	if (ini == NULL) {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create INI. (createINI)\n");
	}

	return ini;
}
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
INI* loadINI(const char* filename) {
	char *currentHeader, *key, *val;
	char* equalCharPos;
	int lineLength;
	int i;
	INI* ini = createINI();
	StringList* file = loadStringList(filename);
	currentHeader = NULL;

	if (ini != NULL && file != NULL) {
		// Loop through file line by line
		for (i = 0; i < file->size; i++) {
			// We need lineLength many times, and we also need to
			// know where the = is if applicable
			lineLength = strlen(file->strList[i]);
			equalCharPos = strchr(file->strList[i], '=');

			// Check for empty lines and comments
			if (lineLength != 0 && file->strList[i][0] != '#') {
				// Find out if this is a header or setter
				if (file->strList[i][0] == '[' && file->strList[i][lineLength - 1] == ']') {
					// Copy the header name to our current header,
					// then throw it in the garbage pile since it
					// will be dynamically allocated
					currentHeader = (char*)calloc(lineLength - 2 + 1, 1); // -2 for [] + 1 for terminating 0

					if (currentHeader != NULL) {
						strncpy(currentHeader, file->strList[i] + 1, lineLength - 2);
						throwInGarbageINI(ini, currentHeader);
					} else {
						jSetError(ERROR_ALLOC_FAILED, "Failed to allocate header (loadINI)\n");
					}
				} else if (equalCharPos != NULL && currentHeader != NULL && equalCharPos != file->strList[i]) {
					// setter
					key = (char*)calloc(equalCharPos - file->strList[i] + 1, 1);
					val = (char*)calloc(lineLength - (equalCharPos - file->strList[i]), 1);

					if (key != NULL && val != NULL) {
						// We're clear, copy strings and throw into garbage pile
						strncpy(key, file->strList[i], equalCharPos - file->strList[i]);
						strncpy(val, equalCharPos + 1, lineLength - (equalCharPos - file->strList[i]));
						throwInGarbageINI(ini, key);
						throwInGarbageINI(ini, val);
						setKeyINI(ini, currentHeader, key, val);
					} else {
						free(key);
						free(val);
						jSetError(ERROR_ALLOC_FAILED, "Failed to allocate key or val (loadINI)\n");
					}
				}
			}
		}
	} else {
		if (ini == NULL)
			jSetError(ERROR_ALLOC_FAILED, "INI could not be created for file [%s] (loadINI)\n", filename);
		if (file == NULL)
			jSetError(ERROR_OPEN_FAILED, "File [%s] could not be loaded (loadINI)\n", filename);
	}
	
	freeStringList(file);
	return ini;
}
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
void outputINI(INI* ini, FILE* stream) {
	int i;

	if (ini != NULL && stream != NULL) {
		for (i = 0; i < ini->numberOfHeaders; i++) {
			fprintf(stream, "[%s]\n", ini->headerNames[i]);
			outputSMap(ini->headers[i], stream);
			fprintf(stream, "\n");
		}
	} else {
		if (ini == NULL)
			jSetError(ERROR_NULL_POINTER, "INI does not exist (ouputINI)\n");
		if (stream == NULL)
			jSetError(ERROR_NULL_POINTER, "Stream does not exist (ouputINI)\n");
	}
}
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
void setKeyINI(INI* ini, const char* header, const char* key, char* val) {
	// -1 at the end of the loop implies the header
	// does not yet exist
	int index = -1;
	SMap* tempSMap = NULL; // If the header doesn't exist
	SMap** newHeaders;
	char** newHeaderNames;
	int i;
	
	if (ini != NULL) {
		// Loop through each header and find the right one
		for (i = 0; i < ini->numberOfHeaders; i++)
			if (strcmp(ini->headerNames[i], header) == 0)
				index = i;

		// Did we find the header or not?
		if (index != -1) {
			setSMapVal(ini->headers[index], key, val);
		} else {
			// We are now going to try and make the header
			// list 1 bigger and throw a new SMap to the
			// end of it.
			tempSMap = createSMap();
			newHeaders = (SMap**)realloc(ini->headers, (ini->numberOfHeaders + 1) * sizeof(SMap));
			newHeaderNames = (char**)realloc(ini->headerNames, (ini->numberOfHeaders + 1) * sizeof(char*));
			
			if (tempSMap != NULL && newHeaders != NULL && newHeaderNames != NULL) {

				newHeaders[ini->numberOfHeaders] = tempSMap;
				newHeaderNames[ini->numberOfHeaders] = (char*)header;
				setSMapVal(tempSMap, key, val);
				ini->numberOfHeaders++;
				ini->headers = newHeaders;
				ini->headerNames = newHeaderNames;
			} else {
				jSetError(ERROR_REALLOC_FAILED, "Failed create header (setKeyINI)\n");
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "INI does not exist for header:key pair %s:%s (setKeyINI)\n", header, key);
	}
}
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
char* getKeyINI(INI* ini, const char* header, const char* key, char* def) {
	int i;
	char* ret = def;

	if (ini != NULL) {
		// We loop through the headers and find the right
		// one by comparing strings. We then queue the SMap
		// function to do the rest for us. We don't break
		// the loop because I don't like spaghetti and
		// if the user is using only these functions then
		// every header will have a unique name.
		for (i = 0; i < ini->numberOfHeaders; i++) {
			if (strcmp(ini->headerNames[i], header) == 0)
				ret = (char*)getSMapVal(ini->headers[i], (const char*)key, def);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "INI does not exist for header:key pair %s:%s (getKeyINI)\n", header, key);
	}

	return ret;
}
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
void throwInGarbageINI(INI* ini, char* string) {
	char** tempList;
	
	if (ini != NULL) {
		// Reallocate it but 1 bigger
		tempList = (char**)realloc(ini->garbagePile, (ini->sizeOfGarbagePile + 1) * sizeof(char*));

		// Only put the string into the pile if the new version exists
		if (tempList != NULL) {
			ini->garbagePile = tempList;
			ini->garbagePile[ini->sizeOfGarbagePile] = string;
			ini->sizeOfGarbagePile++;
		} else {
			jSetError(ERROR_REALLOC_FAILED, "Failed to resize garbage pile (throwInGarbageINI)\n");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "INI does not exist (throwInGarbageINI)\n");
	}
}
//////////////////////////////////////////////////////

//////////////////////////////////////////////////////
void freeINI(INI* ini) {
	int i;
	if (ini != NULL) {
		for (i = 0; i < ini->sizeOfGarbagePile; i++)
			free(ini->garbagePile[i]);
		for (i = 0; i < ini->numberOfHeaders; i++)
			freeSMap(ini->headers[i]);
		free(ini->headers);
		free(ini->headerNames);
		free(ini->garbagePile);
		free(ini);
	}
}
//////////////////////////////////////////////////////
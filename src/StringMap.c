//
// Created by lugi1 on 2018-11-11.
//

#include "StringMap.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "File.h"
#include "JamError.h"

///////////////////////////////////////////////////////////
static void jamThrowInGarbageStringMap(JamStringMap *smap, char *garbage) {
	char** tempPointer;
	// Check that it exists
	if (smap != NULL) {
		tempPointer = (char**)realloc((void*)smap->garbage, (1 + smap->sizeOfGarbagePile) * sizeof(char*));

		if (tempPointer != NULL) {
			// Make the map one bigger and load it up
			smap->sizeOfGarbagePile++;
			smap->garbage = tempPointer;
			smap->garbage[smap->sizeOfGarbagePile - 1] = garbage;
		} else {
			jSetError(ERROR_REALLOC_FAILED, "Failed to increase size of garbage pile (jamThrowInGarbageStringMap)");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Passed JamStringMap does not exist (jamThrowInGarbageStringMap)");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
JamStringMap* jamStringMapCreate() {
	JamStringMap* smap = (JamStringMap*)malloc(sizeof(JamStringMap));

	// Check that it worked
	if (smap != NULL) {
		smap->garbage = NULL;
		smap->vals = NULL;
		smap->keys = NULL;
		smap->size = 0;
		smap->sizeOfGarbagePile = 0;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate JamStringMap (jamStringMapCreate)");
	}

	return smap;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
JamStringMap* jamStringMapLoad(const char *filename) {
	JamStringMap* map = jamStringMapCreate();
	JamStringList* list = jamStringListLoad(filename);
	char* key;
	char* val;
	int len, i, j;
	int delimPos;

	// Check the stuff
	if (list != NULL && map != NULL) {
		// Now we begin looping through every line
		for (i = 0; i < list->size; i++) {
			len = (int)strlen(list->strList[i]);
			delimPos = -1;

			// We don't need empty lines/comments
			if (len > 0) {
				if (list->strList[i][0] != '#') {
					// Find the delimiter
					for (j = 0; j < len; j++)
						if (list->strList[i][j] == '=')
							delimPos = j;

					// If it wasn't found, there is nothing of value on this line
					if (delimPos != -1) {
						// Create two strings to go into the map, then stick them on the garbage pile
						key = (char*)calloc(1, (size_t)(delimPos + 1));
						val = (char*)calloc(1, (size_t)(len - delimPos));

						if (key != NULL && val != NULL) {
							// Separate and copy the strings
							memcpy(key, list->strList[i], (size_t)delimPos);
							memcpy(val, list->strList[i] + delimPos + 1, (size_t)(len - delimPos));

							// Then we put them into the map and keep track of garbage
							jamStringMapSet(map, key, val);
							jamThrowInGarbageStringMap(map, key);
							jamThrowInGarbageStringMap(map, val);
						} else {
							jSetError(ERROR_ALLOC_FAILED, "Could not allocate strings, line %i (jamStringMapLoad)", i + 1);
						}
					} else {
						jSetError(ERROR_INCORRECT_FORMAT, "Error in JamStringMap file, line %i '%s' (jamStringMapLoad)", i + 1, list->strList[i]);
					}
				}
			}
		}
	} else {
		if (list == NULL) {
			jSetError(ERROR_OPEN_FAILED, "File could not be opened (jamStringMapLoad)");
		}
		if (map == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "JamStringMap could not be allocated (jamStringMapLoad)");
		}
		jamStringMapFree(map);
	}

	// Clean up
	jamStringListFree(list);

	return map;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamStringMapOutput(JamStringMap *smap, FILE *stream) {
	int i;

	// Check both things exist of course
	if (smap != NULL && stream != NULL) {
		// Loop through the map and print it all
		for (i = 0; i < smap->size; i++)
			fprintf(stream, "%s=%s\n", smap->keys[i], smap->vals[i]);
	} else {
		if (smap == NULL)
			jSetError(ERROR_NULL_POINTER, "Passed JamStringMap does not exist (jamStringMapOutput)");
		if (stream == NULL)
			jSetError(ERROR_NULL_POINTER, "Passed FILE does not exist (jamStringMapOutput)");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamStringMapSet(JamStringMap *smap, const char *key, char *val) {
	bool found = false;
	int i;

	// Check that it exists
	if (smap != NULL) {
		// First we see if the key already exists
		for (i = 0; i < smap->size && !found; i++) {
			if (strcmp(smap->keys[i], key) == 0) {
				smap->vals[i] = val;
				found = true;
			}
		}

		// We did not find it
		if (!found) {
			// Make the map one bigger and load it up
			smap->size++;
			smap->keys = (char**)realloc((void*)smap->keys, smap->size * sizeof(char*));
			smap->vals = (char**)realloc((void*)smap->vals, smap->size * sizeof(char*));
			smap->keys[smap->size - 1] = (char*)key;
			smap->vals[smap->size - 1] = val;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Passed JamStringMap does not exist (jamStringMapSet with key %s)", key);
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
const char* jamStringMapGet(JamStringMap *smap, const char *key, char *def) {
	char* ret = def;
	int i;

	// First we make sure we weren't passed a dud
	if (smap != NULL) {
		// Loop through the map and attempt to find it
		for (i = 0; i < smap->size && ret == def; i++) {
			if (strcmp(smap->keys[i], key) == 0) {
				ret = smap->vals[i];
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Passed JamStringMap does not exist (jamStringMapGet for key %s)", key);
	}

	return ret;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void jamStringMapFree(JamStringMap *smap) {
	int i;
	if (smap != NULL) {
		free(smap->keys);
		free(smap->vals);
		for (i = 0; i < smap->sizeOfGarbagePile; i++)
			free(smap->garbage[i]);
	}
	free(smap);
}
///////////////////////////////////////////////////////////
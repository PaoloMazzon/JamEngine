//
// Created by lugi1 on 2018-11-11.
//

#include "StringMap.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "File.h"

///////////////////////////////////////////////////////////
SMap* createSMap() {
	SMap* smap = (SMap*)malloc(sizeof(SMap));

	// Check that it worked
	if (smap != NULL) {
		smap->garbage = NULL;
		smap->vals = NULL;
		smap->keys = NULL;
		smap->size = 0;
		smap->sizeOfGarbagePile = 0;
	} else {
		fprintf(stderr, "Failed to allocate SMap (createSMap)\n");
	}

	return smap;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
SMap* loadSMap(const char* filename) {
	SMap* map = createSMap();
	StringList* list = loadStringList(filename);
	char* key;
	char* val;
	int len, i, j;
	int delimPos;

	// Check the stuff
	if (list != NULL && map != NULL) {
		// Now we begin looping through every line
		for (i = 0; i < list->size; i++) {
			len = strlen(list->strList[i]);
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
							setSMapVal(map, key, val);
							throwAStringIntoTheGarbage(map, key);
							throwAStringIntoTheGarbage(map, val);
						} else {
							fprintf(stderr, "Could not allocate strings, line %i (loadSMap)\n", i + 1);
						}
					} else {
						fprintf(stderr, "Error in SMap file, line %i '%s' (loadSMap)\n", i + 1, list->strList[i]);
					}
				}
			}
		}
	} else {
		if (list == NULL)
			fprintf(stderr, "File could not be opened (loadSMap)\n");
		if (map == NULL)
			fprintf(stderr, "SMap could not be allocated (loadSMap)\n");
		freeSMap(map);
	}

	// Clean up
	freeStringList(list);

	return map;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void outputSMap(SMap* smap, FILE* stream) {
	int i;

	// Check both things exist of course
	if (smap != NULL && stream != NULL) {
		// Loop through the map and print it all
		for (i = 0; i < smap->size; i++)
			fprintf(stream, "%s=%s\n", smap->keys[i], smap->vals[i]);
	} else {
		if (smap == NULL)
			fprintf(stderr, "Passed SMap does not exist (outputSMap)\n");
		if (stream == NULL)
			fprintf(stderr, "Passed FILE does not exist (outputSMap)\n");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void setSMapVal(SMap* smap, const char* key, char* val) {
	bool found = false;
	int i;

	// Check that it exists
	if (smap != NULL) {
		// First we see if the key already exists
		for (i = 0; i < smap->size; i++) {
			if (strcmp(smap->keys[i], key) == 0) {
				smap->vals[i] = val;
				found = true;

				// I realize this is bad but this case is very simple and would
				// be a pain in the ass/less efficient to turn into a while loop
				// so whatever we'll just break out of the for
				break;
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
		fprintf(stderr, "Passed SMap does not exist (setSMapVal with key %s)\n", key);
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
const char* getSMapVal(SMap* smap, const char* key, char* def) {
	char* ret = def;
	int i;

	// First we make sure we weren't passed a dud
	if (smap != NULL) {
		// Loop through the map and attempt to find it
		for (i = 0; i < smap->size; i++) {
			if (strcmp(smap->keys[i], key) == 0) {
				ret = smap->vals[i];

				// I realize this is bad but this case is very simple and would
				// be a pain in the ass/less efficient to turn into a while loop
				// so whatever we'll just break out of the for
				break;
			}
		}
	} else {
		fprintf(stderr, "Passed SMap does not exist (getSMapVal for key %s)", key);
	}

	return ret;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void throwAStringIntoTheGarbage(SMap* smap, char* garbage) {
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
			fprintf(stderr, "Failed to increase size of garbage pile (throwAStringIntoTheGarbage)\n");
		}
	} else {
		fprintf(stderr, "Passed SMap does not exist (throwAStringIntoTheGarbage)\n");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void freeSMap(SMap* smap) {
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
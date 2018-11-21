//
// Created by lugi1 on 2018-11-11.
//

#include "StringMap.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

///////////////////////////////////////////////////////////
SMap* createSMap() {
	SMap* smap = (SMap*)malloc(sizeof(SMap));

	// Check that it worked
	if (smap != NULL) {
		smap->vals = NULL;
		smap->keys = NULL;
		smap->size = 0;
	} else {
		fprintf(stderr, "Failed to allocate SMap (createSMap)\n");
	}

	return smap;
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
SMap* loadSMap(const char* filename) {
	FILE* file = fopen(filename, "r");
	SMap* map = createSMap();

	// Check the stuff
	if (file != NULL && map != NULL) {
		// Now we begin looping through every line
		// TODO: Finish File.c and use that interface here so it not a massive pain in the ass
	} else {
		if (file == NULL)
			fprintf(stderr, "File could not be opened (loadSMap)\n");
		if (map == NULL)
			fprintf(stderr, "SMap could not be allocated (loadSMap)\n");
		freeSMap(map);
	}

	// Clean up
	fclose(file);

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
			fprintf(stream, "%s=\"%s\"\n", smap->keys[i], smap->vals[i]);
	} else {
		if (smap == NULL)
			fprintf(stderr, "Passed SMap does not exist (outputSMap)\n");
		if (stream == NULL)
			fprintf(stderr, "Passed FILE does not exist (outputSMap)\n");
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
void setSMapVal(SMap* smap, const char* key, const char* val) {
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
		if (!found) { // TODO: Fix this unsafe realloc usage
			// Make the map one bigger and load it up
			smap->size++;
			smap->keys = (const char**)realloc((void*)smap->keys, smap->size * sizeof(const char**));
			smap->vals = (const char**)realloc((void*)smap->vals, smap->size * sizeof(const char**));
			smap->keys[smap->size - 1] = key;
			smap->vals[smap->size - 1] = val;
		}
	} else {
		fprintf(stderr, "Passed SMap does not exist (setSMapVal with key %s)\n", key);
	}
}
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
const char* getSMapVal(SMap* smap, const char* key, const char* def) {
	const char* ret = def;
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
void freeSMap(SMap* smap) {
	if (smap != NULL) {
		free(smap->keys);
		free(smap->vals);
	}
	free(smap);
}
///////////////////////////////////////////////////////////
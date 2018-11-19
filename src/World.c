//
// Created by lugi1 on 2018-07-09.
//

#include "World.h"
#include <stdio.h>
#include <malloc.h>

///////////////////////////////////////////////////////
World* createWorld() {
	World* world = (World*)calloc(1, sizeof(World));

	// Double check that it worked
	if (world == NULL)
		fprintf(stderr, "Could not allocate world (createWorld)\n");

	return world;
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void freeWorld(World* world) {
	free(world);
}
///////////////////////////////////////////////////////
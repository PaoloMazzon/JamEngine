//
// Created by lugi1 on 2018-07-09.
//

#include "World.h"
#include <stdio.h>
#include <malloc.h>

///////////////////////////////////////////////////////
World* createWorld() {
	World* world = (World*)calloc(1, sizeof(World));


	if (world != NULL) {

	} else {
		fprintf(stderr, "Could not allocate world (createWorld)\n");
	}

	return world;
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void setWorldFilterTypeRectangle(World* world, uint16 inRangeRectangleWidth, uint16 inRangeRectangleHeight) {
	if (world != NULL) {

	} else {
		fprintf(stderr, "World does not exist (setWorldFilterTypeRectangle)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void setWorldFilterTypeCircle(World* world, uint16 inRangeRadius) {
	if (world != NULL) {

	} else {
		fprintf(stderr, "World does not exist (setWorldFilterTypeCircle)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void worldAddEntity(World* world, Entity* entity) {
	if (world != NULL) {

	} else {
		fprintf(stderr, "World does not exist (worldAddEntity)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void worldAddTileMap(World* world, TileMap* tileMap) {
	if (world != NULL) {

	} else {
		fprintf(stderr, "World does not exist (worldAddTileMap)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void worldRemoveEntity(World* world, uint64 entityID) {
	if (world != NULL) {

	} else {
		fprintf(stderr, "World does not exist (worldRemoveEntity)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void filterEntitiesByProximity(World* world, int pointX, int pointY) {
	if (world != NULL) {

	} else {
		fprintf(stderr, "World does not exist (filterEntitiesByProximity)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void freeWorld(World* world) {
	int i;
	if (world != NULL) {
		// There is a lot of lists to free
		for (i = 0; i < MAX_ENTITY_TYPES; i++)
			freeEntityList(world->entityTypes[i], false);
		freeEntityList(world->entityByRange[ENTITIES_IN_RANGE], false);
		freeEntityList(world->entityByRange[ENTITIES_OUT_OF_RANGE], false);
		freeEntityList(world->worldEntities, true);
		free(world->worldMaps);
	} else {
		fprintf(stderr, "World does not exist (freeWorld)\n");
	}
}
///////////////////////////////////////////////////////
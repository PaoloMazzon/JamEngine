//
// Created by lugi1 on 2018-07-09.
//

#include <stdio.h>
#include <malloc.h>
#include <World.h>
#include <Entity.h>
#include <Vector.h>
#include <EntityList.h>
#include <Sprite.h>
#include <BehaviourMap.h>
#include <JamEngine.h>
#include "JamError.h"

// Updates an entity's position in the spatial map
// This function will add the entity to the world if its id
// is not yet assigned, or update the position if the xPrev
// or yPrev are different
static inline void _updateEntInMap(JamWorld* world, JamEntity* ent) {
	if (ent->id == 1) { // Its not yet in the world

	} else { // Its position just needs to be updated

	}
}

///////////////////////////////////////////////////////
JamWorld* jamWorldCreate(int gridWidth, int gridHeight, int cellWidth, int cellHeight) {
	JamWorld* world = (JamWorld*)calloc(1, sizeof(JamWorld));
	bool error = false;
	int i;

	// Allocate the map first, then the 2D grid, then the countless lists
	if (world != NULL) {
		world->entityGrid = (JamEntityList**)malloc(((gridWidth * gridHeight) + 1) * sizeof(JamEntityList));
		world->worldEntities = jamEntityListCreate();
		world->gridWidth = gridWidth;
		world->gridHeight = gridHeight;
		world->cellWidth = cellWidth;
		world->cellHeight = cellHeight;

		if (world->entityGrid != NULL) {
			for (i = 0; i < (gridWidth * gridHeight) + 1; i++) {
				world->entityGrid[i] = jamEntityListCreate();
				if (world->entityGrid[i] == NULL)
					error = true;
			}

			// If any list could not be created, delet
			if (error)
				jamWorldFree(world);
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate spatial map's grid");
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate world");
	}

	return world;
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamWorldAddEntity(JamWorld *world, JamEntity *entity) {
	if (world != NULL && entity != NULL) {
		// First it gets put into the space map
		_updateEntInMap(world, entity);

		// Then we add it to the entity list and assign it an id
		entity->id = jamEntityListAdd(world->worldEntities, entity);
	} else {
		if (world == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamWorld does not exist (jamWorldAddEntity)");
		}
		if (entity == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamEntity does not exist (jamWorldAddEntity)");
		}
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
JamEntity* jamWorldFindEntity(JamWorld *world, int id) {
	JamEntity* ent = NULL;

	if (world != NULL) {
		if (world->worldEntities->size > id && id > -1) {
			ent = world->worldEntities->entities[id];
		} else {
			jSetError(ERROR_OUT_OF_BOUNDS, "Entity ID out of bounds");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "World does not exist");
	}

	return ent;
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamWorldProcFrame(JamWorld *world) {
	int i;
	JamEntity* ent;

	if (world != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_NULL_POINTER, "JamWorld does not exist (jamWorldProcFrame)");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamWorldRemoveEntity(JamWorld *world, int id) {
	int i;
	JamEntity* ent;

	if (world != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_NULL_POINTER, "World does not exist");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamWorldFilter(JamWorld *world, int pointX, int pointY) {
	uint32 i, j;
	JamEntity* entity;

	if (world != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_NULL_POINTER, "JamWorld does not exist (jamWorldFilter)");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamWorldFree(JamWorld *world) {
	int i;
	if (world != NULL) {
		// TODO: This
		free(world->worldMaps);
	}
}
///////////////////////////////////////////////////////

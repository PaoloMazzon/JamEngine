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
#include "JamError.h"

// Places an entity into a world's spatial map in all applicable boxes and updates
// the entity's relavent values
static inline int _refreshGridPos(int a, int b, int c, int d) { // TODO: Place entity in maps while counting unique positions
	int nums[] = {a, b, c, d};
	int unique = 1;
	bool instanceUnique;
	int i, j;

	for (i = 1; i < 4; i++) {
		instanceUnique = true;
		for (j = i - 1; j >= 0; j--) {
			if (nums[j] == nums[i])
				instanceUnique = false;
		}

		// We mark down that this entry is unique while also placing the entity here
		if (instanceUnique) {
			unique++;
		}
	}

	return unique;
}

// Calculates a spatial map's grid x value from a double x value in the world
static inline int _gridXFromDouble(JamWorld* world, double x) {
	return (int)(x / (double)world->cellWidth);
}

// Calculates a spatial map's grid y value from a double y value in the world
static inline int _gridYFromDouble(JamWorld* world, double y) {
	return (int)(y / (double)world->cellHeight);
}

// Updates an entity's position in the spatial map
// This function will add the entity to the world if its id
// is not yet assigned, or update the position if the xPrev
// or yPrev are different
static inline void _updateEntInMap(JamWorld* world, JamEntity* ent) {
	// Grab the corners of the entity then calculate its corners' positions
	// in the spatial map
	double x1, y1, x2, y2;
	int topLeft, topRight, bottomLeft, bottomRight;

	// We only calculate all the values if we need to
	if (ent->id == ID_NOT_ASSIGNED) { // Its not yet in the world, just add it
		// We only need the current corners
		x1 = jamEntityVisibleX1(ent, ent->x);
		y1 = jamEntityVisibleY1(ent, ent->y);
		x2 = jamEntityVisibleX2(ent, ent->x);
		y2 = jamEntityVisibleY2(ent, ent->y);
		topLeft = (_gridYFromDouble(world, y1) * world->gridWidth) + _gridXFromDouble(world, x1);
		topRight = (_gridYFromDouble(world, y1) * world->gridWidth) + _gridXFromDouble(world, x2);
		bottomLeft = (_gridYFromDouble(world, y2) * world->gridWidth) + _gridXFromDouble(world, x1);
		bottomRight = (_gridYFromDouble(world, y2) * world->gridWidth) + _gridXFromDouble(world, x2);

		// Place the entity into the appropriate cells and update the entity's world-related values
		_refreshGridPos(topLeft, topRight, bottomLeft, bottomRight);
	} else if (ent->x != ent->xPrev || ent->y != ent->yPrev) { // Its position needs to be updated, remove the old locations first
		// First we calculate the spots to remove this entity from
		x1 = jamEntityVisibleX1(ent, ent->x);
		y1 = jamEntityVisibleY1(ent, ent->y);
		x2 = jamEntityVisibleX2(ent, ent->x);
		y2 = jamEntityVisibleY2(ent, ent->y);
		// TODO: This

		// Now we place it into its new spot
		// TODO: This
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

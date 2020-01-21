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

// Places an entity into a world's spatial map in all applicable boxes and updates
// the entity's relavent values
static void _refreshGridPos(JamWorld* world, JamEntity* ent, int a, int b, int c, int d) {
	int nums[] = {a, b, c, d};
	bool instanceUnique;
	int i, j;

	// The first occurrence is guaranteed unique so just pop it in now
	ent->cells = 1;
	ent->cellsIn[0] = a;
	ent->cellsLoc[0] = jamEntityListAdd(world->entityGrid[nums[0]], ent);

	for (i = 1; i < 4; i++) {
		instanceUnique = true;
		for (j = i - 1; j >= 0; j--) {
			if (nums[j] == nums[i])
				instanceUnique = false;
		}

		// We mark down that this entry is unique while also placing the entity here
		if (instanceUnique) {
			ent->cells++;
			ent->cellsIn[i] = nums[i];
			ent->cellsLoc[0] = jamEntityListAdd(world->entityGrid[nums[i]], ent);
		}
	}
}

// Calculates a coordinate's equivalent cell in a world's space map, accounting for out of bounds values
static inline int _gridPosFromCoords(JamWorld* world, double x, double y) {
	int xInGrid = (int)(x / (double)world->cellWidth);
	int yInGrid = (int)(y / (double)world->cellHeight);

	if (xInGrid >= world->gridWidth || xInGrid < 0 || yInGrid >= world->gridHeight || yInGrid < 0)
		return world->gridWidth * world->gridHeight;
	else
		return (yInGrid * world->gridWidth) + xInGrid;
}

// Updates an entity's position in the spatial map
// This function will add the entity to the world if its id
// is not yet assigned, or update the position if the xPrev
// or yPrev are different
static void _updateEntInMap(JamWorld* world, JamEntity* ent) {
	// Grab the corners of the entity then calculate its corners' positions
	// in the spatial map
	double x1, y1, x2, y2;
	int topLeft, topRight, bottomLeft, bottomRight;
	int i;

	// We only need to process this entity if it is either A) Not already in the world or
	// B) its position has changed.
	if (ent->id != ID_NOT_ASSIGNED || ent->xPrev != ent->x || ent->yPrev != ent->y) {
		// If its not in the world, add it
		if (ent->id == ID_NOT_ASSIGNED)
			ent->id = jamEntityListAdd(world->worldEntities, ent);
		else // Otherwise, remove it from its old locations
			for (i = 0; i < ent->cells; i++)
				world->entityGrid[ent->cellsIn[i]]->entities[ent->cellsLoc[i]] = NULL;

		// Find the entity's corners then drop them into the grid
		x1 = jamEntityVisibleX1(ent, ent->x);
		y1 = jamEntityVisibleY1(ent, ent->y);
		x2 = jamEntityVisibleX2(ent, ent->x);
		y2 = jamEntityVisibleY2(ent, ent->y);
		topLeft = _gridPosFromCoords(world, x1, y1);
		topRight = _gridPosFromCoords(world, x2, y1);
		bottomLeft = _gridPosFromCoords(world, x1, y2);
		bottomRight = _gridPosFromCoords(world, x2, y2);

		// Place the entity into the appropriate cells and update the entity's world-related values
		_refreshGridPos(world, ent, topLeft, topRight, bottomLeft, bottomRight);
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
		// Place it into the spatial map
		_updateEntInMap(world, entity);
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

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

static bool gThreadComplete;

/*
 * Most of the following functions are for managing the world's spatial
 * hash map. Since the user never needs to actually interact with the
 * space map beyond configuring a couple settings, the function that interact
 * with a world's space map are all static (I also don't want the average
 * user to have access to space map functions because messing this up
 * is a very quick way to get segfaults/memory leaks/thread leaks)
 */

/// \brief Places an entity into all non-duplicate cells in the space map
static void _refreshGridPos(JamWorld* world, JamEntity* ent, int a, int b, int c, int d) {
	int nums[] = {a, b, c, d};
	bool instanceUnique;
	int uniqueAccumulator = 0; // This ensures that cellsIn/cellsLoc will be in order
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
			uniqueAccumulator++;
			ent->cellsIn[uniqueAccumulator] = nums[i];
			ent->cellsLoc[uniqueAccumulator] = jamEntityListAdd(world->entityGrid[nums[i]], ent);
		}
	}
}

/// \brief Calculates an in-grid position from a real x value
static inline int _gridPosFromRealX(JamWorld* world, double x) {
	return (int)(x / (double)world->cellWidth);
}

/// \brief Calculates an in-grid position from a real y value
static inline int _gridPosFromRealY(JamWorld* world, double y) {
	return (int)(y / (double)world->cellHeight);
}

/// \brief Finds the entity list associated with a position in the space map
static inline JamEntityList* _getListAtPos(JamWorld* world, int xInGrid, int yInGrid) {
	if (xInGrid >= world->gridWidth || xInGrid < 0 || yInGrid >= world->gridHeight || yInGrid < 0)
		return world->entityGrid[world->gridWidth * world->gridHeight];
	else
		return world->entityGrid[(yInGrid * world->gridWidth) + xInGrid];
}

/// \brief Calculates the corresponding cell in a space map given a real x/y
static int _gridPosFromCoords(JamWorld* world, double x, double y) {
	int xInGrid = (int)(x / (double)world->cellWidth);
	int yInGrid = (int)(y / (double)world->cellHeight);

	if (xInGrid >= world->gridWidth || xInGrid < 0 || yInGrid >= world->gridHeight || yInGrid < 0)
		return world->gridWidth * world->gridHeight;
	else
		return (yInGrid * world->gridWidth) + xInGrid;
}

/// \brief Updates an entity's position in a world's spatial map
///
/// If the entity is already in the world and its position has
/// changed, it will be removed from its old position and stuck
/// into its new one. Otherwise, it is simply added to its position
/// in the map.
static void _updateEntInMap(JamWorld* world, JamEntity* ent) {
	// Grab the corners of the entity then calculate its corners' positions
	// in the spatial map
	double x1, y1, x2, y2;
	int topLeft, topRight, bottomLeft, bottomRight;
	int i;

	// We only need to process this entity if it is either A) Not already in the world or
	// B) its position has changed.
	if (ent->id == ID_NOT_ASSIGNED || ent->xPrev != ent->x || ent->yPrev != ent->y) {
		// If its not in the world, add it and potentially call its initialization function
		if (ent->id == ID_NOT_ASSIGNED) {
			ent->id = jamEntityListAdd(world->worldEntities, ent);

			if (ent->behaviour != NULL && ent->behaviour->onCreation != NULL)
				(*ent->behaviour->onCreation)(world, ent);
		}
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

/// \brief Safely calls an entity's behaviour's onFrame function as well as updates its position in the world
static void _updateEntity(JamWorld* world, JamEntity* ent) {
	if (ent != NULL) {
		if (ent->procs++ == 0) {
			if (ent->behaviour != NULL && ent->behaviour->onFrame != NULL)
				(*ent->behaviour->onFrame)(world, ent);

			// Update the entity's position in the grid
			_updateEntInMap(world, ent);

			// Update previous coordinates
			ent->xPrev = ent->x;
			ent->yPrev = ent->y;
		}
	}
}

/// \brief Safely call an entity's behaviour's onDraw function or draws it if it doesn't have one
static void _drawEntity(JamWorld* world, JamEntity* ent) {
	if (ent != NULL && ent->procs == 1) {
		if (ent->behaviour != NULL && ent->behaviour->onDraw != NULL)
			(*ent->behaviour->onDraw)(world, ent);
		else if (ent->behaviour == NULL ||
				 (ent->behaviour != NULL && ent->behaviour->onDraw == NULL))
			jamDrawEntity(ent);
	}

	if (ent != NULL && ent->procs == ent->cells)
		ent->procs = 0;
}

/// \brief Filters the entities in the space map into a new filtered cache.
static void* _filterEntitiesIntoCache(void* voidWorld) {
	JamWorld* world = voidWorld;

	pthread_mutex_lock(&world->entityAddingLock);
	pthread_mutex_unlock(&world->entityAddingLock);

	return NULL;
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

		// Setup the mutexes
		pthread_mutexattr_t t;
		pthread_mutexattr_init(&t);
		pthread_mutexattr_setprotocol(&t, PTHREAD_MUTEX_DEFAULT);
		pthread_mutex_init(&world->entityCacheMutex, &t);
		pthread_mutexattr_init(&t);
		pthread_mutexattr_setprotocol(&t, PTHREAD_MUTEX_DEFAULT);
		pthread_mutex_init(&world->entityAddingLock, &t);

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
		// We can't add entities while filtering is occurring lest the cache end up wrong
		pthread_mutex_lock(&world->entityAddingLock);

		// Place it into the spatial map
		_updateEntInMap(world, entity);

		// All entities added are automatically added to the cache
		if (world->cacheInRangeEntities)
			jamEntityListAdd(world->inRangeCache, entity);

		pthread_mutex_unlock(&world->entityAddingLock);
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
	int i, j, k;
	int cellStartX, cellStartY;
	int cellEndX, cellEndY;
	JamEntityList* currentList;
	JamEntity* ent;

	if (world != NULL) {
		// If we're using the in-range cache we must lock the cache
		if (world->cacheInRangeEntities) {
			// If the cache changes while we're using it here we're most likely getting a segfault
			pthread_mutex_lock(&world->entityCacheMutex);

			// Process frames
			for (i = 0; i < world->inRangeCache->size; i++)
				_updateEntity(world, world->inRangeCache->entities[i]);

			// Process drawing functions
			for (i = 0; i < world->inRangeCache->size; i++)
				_drawEntity(world, world->inRangeCache->entities[i]);

			pthread_mutex_unlock(&world->entityCacheMutex);
		} else {
			// In this case, we are to just go through the space map and find all entities in the viewport
			// Calculate the starting and ending cells
			cellStartX = _gridPosFromRealX(world, jamRendererGetCameraX() - world->procDistance);
			cellStartY = _gridPosFromRealY(world, jamRendererGetCameraY() - world->procDistance);
			cellEndX = _gridPosFromRealX(world, jamRendererGetCameraX() + jamRendererGetBufferWidth() + world->procDistance);
			cellEndY = _gridPosFromRealY(world, jamRendererGetCameraY() + jamRendererGetBufferHeight() + world->procDistance);

			for (i = cellStartY; i <= cellEndY; i++) {
				for (j = cellStartX; j <= cellEndX; j++) {
					currentList = _getListAtPos(world, j, i);

					// Call this one's frame update
					for (k = 0; k < currentList->size; k++)
						_updateEntity(world, currentList->entities[k]);

					// Now to draw it
					for (k = 0; k < currentList->size; k++)
						_drawEntity(world, currentList->entities[k]);
				}
			}
		}
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
void jamWorldFilter(JamWorld *world, int pointX, int pointY) { // TODO: Fix this
	uint32 i, j;
	JamEntity* entity;
	pthread_attr_t attr;

	if (world != NULL && world->cacheInRangeEntities) {
		// Create the new filtering thread
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&world->cacheBuilderThread, &attr, _filterEntitiesIntoCache, world);
	} else {
		if (world == NULL)
			jSetError(ERROR_NULL_POINTER, "JamWorld does not exist (jamWorldFilter)");
		else
			jSetError(ERROR_WARNING, "Attempting to filter a world with filtering disabled");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamWorldFree(JamWorld *world) {
	int i;
	if (world != NULL) {
		for (i = 0; i < (world->gridWidth * world->gridHeight) + 1; i++)
			jamEntityListFree(world->entityGrid[i], false);
		for (i = 0; i < MAX_TILEMAPS; i++)
			jamTileMapFree(world->worldMaps[i]);
		for (i = 0; i < world->worldEntities->size; i++)
			if (world->worldEntities->entities[i] != NULL &&
				world->worldEntities->entities[i]->behaviour != NULL &&
					world->worldEntities->entities[i]->behaviour->onDestruction != NULL)
				(*world->worldEntities->entities[i]->behaviour->onDestruction)(world, world->worldEntities->entities[i]);

		free(world->entityGrid);
		jamEntityListFree(world->inRangeCache, false);
		jamEntityListFree(world->worldEntities, true);
		free(world);
	}
}
///////////////////////////////////////////////////////

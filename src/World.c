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

///////////////////////////////////////////////////////
JamWorld* jamCreateWorld() {
	JamWorld* world = (JamWorld*)calloc(1, sizeof(JamWorld));
	int i;
	bool error = false;

	if (world != NULL) {
		// We now need to initialize the 8 or so entity lists that come with a world
		for (i = 0; i < MAX_ENTITY_TYPES; i++) {
			world->entityTypes[i] = jamEntityListCreate();
			if (world->entityTypes[i] == NULL)
				error = true;
		}
		world->worldEntities = jamEntityListCreate();
		world->entityByRange[ENTITIES_IN_RANGE] = jamEntityListCreate();
		world->entityByRange[ENTITIES_OUT_OF_RANGE] = jamEntityListCreate();

		if (error || world->worldEntities == NULL || world->entityByRange[ENTITIES_OUT_OF_RANGE] == NULL || world->entityByRange[ENTITIES_IN_RANGE] == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate entity lists (jamCreateWorld)");
			// keep on rocking in the
			jamFreeWorld(world);
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Could not allocate world (jamCreateWorld)");
	}

	return world;
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamSetWorldFilterTypeRectangle(JamWorld *world, uint16 inRangeRectangleWidth, uint16 inRangeRectangleHeight) {
	if (world != NULL) {
		world->distanceFilteringType = ft_Rectangle;
		world->inRangeRectangleWidth = inRangeRectangleWidth;
		world->inRangeRectangleHeight = inRangeRectangleHeight;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamWorld does not exist (jamSetWorldFilterTypeRectangle)");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamSetWorldFilterTypeCircle(JamWorld *world, uint16 inRangeRadius) {
	if (world != NULL) {
		world->distanceFilteringType = ft_Proximity;
		world->inRangeRadius = inRangeRadius;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamWorld does not exist (jamSetWorldFilterTypeCircle)");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamWorldAddEntity(JamWorld *world, JamEntity *entity) {
	if (world != NULL && entity != NULL && entity->type < MAX_ENTITY_TYPES) {
		// Put the entity in each of the three lists it belongs to
		jamEntityListAdd(world->entityByRange[ENTITIES_IN_RANGE], entity);
		jamEntityListAdd(world->entityTypes[entity->type], entity);
		entity->id = jamEntityListAdd(world->worldEntities, entity);

		// Attempt to call the entity's onCreation function
		if (entity->behaviour != NULL && entity->behaviour->onCreation != NULL) {
			(*entity->behaviour->onCreation)(world, entity);
		}
	} else {
		if (world == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamWorld does not exist (jamWorldAddEntity)");
		}
		if (entity == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamEntity does not exist (jamWorldAddEntity)");
		}
		if (entity != NULL && entity->type >= MAX_ENTITY_TYPES) {
			jSetError(ERROR_INCORRECT_FORMAT, "JamEntity type is invalid (jamWorldAddEntity)");
		}
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
JamEntity* jamFindEntityFromID(JamWorld* world, int id) {
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
		// Process all the entities in range
		for (i = 0; i < world->entityByRange[ENTITIES_IN_RANGE]->size; i++) {
			ent = world->entityByRange[ENTITIES_IN_RANGE]->entities[i];
			if (ent != NULL && ent->behaviour != NULL && ent->behaviour->onFrame != NULL)
				(*ent->behaviour->onFrame)(world, ent);
		}

		// Draw all the entities in range
		for (i = 0; i < world->entityByRange[ENTITIES_IN_RANGE]->size; i++) {
			ent = world->entityByRange[ENTITIES_IN_RANGE]->entities[i];
			if (ent != NULL && ent->behaviour != NULL) {
				if (ent->behaviour->onDraw != NULL)
					(*ent->behaviour->onDraw)(world, ent);
				else
					jamEntityDraw(ent);
			} else if (ent != NULL && ent->behaviour == NULL) {
				jamEntityDraw(ent);
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
	JamEntityType type = et_None;
	JamEntity* ent;

	if (world != NULL) {
		// Make sure its present in the master list
		ent = jamFindEntityFromID(world, id);
		if (ent != NULL) {
			// We found it, call its destroy function if applicable
			if (ent->behaviour != NULL && ent->behaviour->onDestruction != NULL)
				(*ent->behaviour->onDestruction)(world, ent);

			// Record the type and destroy it
			type = ent->type;
			jamEntityFree(ent, false, false, false);
			world->worldEntities->entities[id] = NULL;
		}

		// Find and get rid of the remaining dangling pointers
		if (type != et_None) {
			for (i = 0; i < world->entityTypes[type]->size; i++)
				if (world->entityTypes[type]->entities[i] != NULL && world->entityTypes[type]->entities[i] == ent)
					world->entityTypes[type]->entities[i] = NULL;
			for (i = 0; i < world->entityByRange[ENTITIES_IN_RANGE]->size; i++)
				if (world->entityByRange[ENTITIES_IN_RANGE]->entities[i] != NULL && world->entityByRange[ENTITIES_IN_RANGE]->entities[i] == ent)
					world->entityByRange[ENTITIES_IN_RANGE]->entities[i] = NULL;
			for (i = 0; i < world->entityByRange[ENTITIES_OUT_OF_RANGE]->size; i++)
				if (world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] != NULL && world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] == ent)
					world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] = NULL;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "World does not exist");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamWorldRotateEntity(JamWorld *world, JamEntity *entity) {
	int i = 0;
	bool found = false;

	if (world != NULL && entity != NULL) {
		// Find the entity in the in range list, if it's not there we don't need to do anything
		while (i < world->entityByRange[ENTITIES_IN_RANGE]->size && !found) {
			if (world->entityByRange[ENTITIES_IN_RANGE]->entities[i] == entity) {
				world->entityByRange[ENTITIES_IN_RANGE]->entities[i] = NULL;
				jamEntityListAdd(world->entityByRange[ENTITIES_OUT_OF_RANGE], entity);
				found = true;
			}
			i++;
		}

		// If we found it also remove it from the in-range list
		for (i = 0; i < world->entityTypes[entity->type]->size && found; i++) {
			if (world->entityTypes[entity->type]->entities[i] == entity) {
				world->entityTypes[entity->type]->entities[i] = NULL;
				found = false;
			}
		}
	} else {
		if (world == NULL)
			jSetError(ERROR_NULL_POINTER, "JamWorld does not exist (jamWorldRotateEntity)");
		if (entity == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity does not exist");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamFilterEntitiesByProximity(JamWorld *world, int pointX, int pointY) {
	uint32 i, j;
	JamEntity* entity;
	bool foundInTypeList = false;

	if (world != NULL) {
		// Essentially the same code for the two different types of ranges b/c
		// it is cheaper than running the same if statement every iteration
		if (world->distanceFilteringType == ft_Rectangle) {
			// Situation 1: It was in range but now is not
			for (i = 0; i < world->entityByRange[ENTITIES_IN_RANGE]->size; i++) {
				entity = world->entityByRange[ENTITIES_IN_RANGE]->entities[i];
				if (entity != NULL && !pointInRectangle(entity->x + entity->sprite->originX, entity->y + entity->sprite->originY,
														pointX, pointY, world->inRangeRectangleWidth, world->inRangeRectangleHeight)) {
					world->entityByRange[ENTITIES_IN_RANGE]->entities[i] = NULL;
					jamEntityListAdd(world->entityByRange[ENTITIES_OUT_OF_RANGE], entity);

					// Find it in the type list and remove it
					foundInTypeList = false;
					for (j = 0; (j < world->entityTypes[entity->type]->size && !foundInTypeList); j++) {
						if (entity == world->entityTypes[entity->type]->entities[j]) {
							world->entityTypes[entity->type]->entities[j] = NULL;
							foundInTypeList = true;
						}
					}
				}
			}

			// Situation 2: It was out of range but now its in range
			for (i = 0; i < world->entityByRange[ENTITIES_OUT_OF_RANGE]->size; i++) {
				entity = world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i];
				if (entity != NULL && pointInRectangle(entity->x + entity->sprite->originX, entity->y + entity->sprite->originY,
														pointX, pointY, world->inRangeRectangleWidth, world->inRangeRectangleHeight)) {
					world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] = NULL;
					jamEntityListAdd(world->entityByRange[ENTITIES_IN_RANGE], entity);
					jamEntityListAdd(world->entityTypes[entity->type], entity);
				}
			}
		} else {
			// Situation 1: It was in range but now is not
			for (i = 0; i < world->entityByRange[ENTITIES_IN_RANGE]->size; i++) {
				entity = world->entityByRange[ENTITIES_IN_RANGE]->entities[i];
				if (entity != NULL && !pointInCircle(entity->x + entity->sprite->originX, entity->y + entity->sprite->originY,
														pointX, pointY, world->inRangeRadius)) {
					world->entityByRange[ENTITIES_IN_RANGE]->entities[i] = NULL;
					jamEntityListAdd(world->entityByRange[ENTITIES_OUT_OF_RANGE], entity);

					// Find it in the type list and remove it
					foundInTypeList = false;
					for (j = 0; (j < world->entityTypes[entity->type]->size && !foundInTypeList); j++) {
						if (entity == world->entityTypes[entity->type]->entities[j]) {
							world->entityTypes[entity->type]->entities[j] = NULL;
							foundInTypeList = true;
						}
					}
				}
			}

			// Situation 2: It was out of range but now its in range
			for (i = 0; i < world->entityByRange[ENTITIES_OUT_OF_RANGE]->size; i++) {
				entity = world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i];
				if (entity != NULL && pointInCircle(entity->x + entity->sprite->originX, entity->y + entity->sprite->originY,
													   pointX, pointY, world->inRangeRadius)) {
					world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] = NULL;
					jamEntityListAdd(world->entityByRange[ENTITIES_IN_RANGE], entity);
				}
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamWorld does not exist (jamFilterEntitiesByProximity)");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamFreeWorld(JamWorld *world) {
	int i;
	if (world != NULL) {
		// Free all the entities first
		for (i = 0; i < world->worldEntities->size; i++)
			if (world->worldEntities->entities[i] != NULL)
				jamWorldRemoveEntity(world, world->worldEntities->entities[i]->id);

		// Free the tile maps
		for (i = 0; i < MAX_TILEMAPS; i++)
			jamFreeTileMap(world->worldMaps[i]);

		// There is a lot of lists to free
		for (i = 0; i < MAX_ENTITY_TYPES; i++)
			jamEntityListFree(world->entityTypes[i], false);
		jamEntityListFree(world->entityByRange[ENTITIES_IN_RANGE], false);
		jamEntityListFree(world->entityByRange[ENTITIES_OUT_OF_RANGE], false);
		jamEntityListFree(world->worldEntities, true);
		free(world->worldMaps);
	}
}
///////////////////////////////////////////////////////

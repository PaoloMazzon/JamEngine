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
World* createWorld(JamRenderer* renderer) {
	World* world = (World*)calloc(1, sizeof(World));
	int i;
	bool error = false;

	if (world != NULL && renderer != NULL) {
		// We now need to initialize the 8 or so entity lists that come with a world
		for (i = 0; i < MAX_ENTITY_TYPES; i++) {
			world->entityTypes[i] = createEntityList();
			if (world->entityTypes[i] == NULL)
				error = true;
		}
		world->worldEntities = createEntityList();
		world->entityByRange[ENTITIES_IN_RANGE] = createEntityList();
		world->entityByRange[ENTITIES_OUT_OF_RANGE] = createEntityList();
		world->renderer = renderer;

		if (error || world->worldEntities == NULL || world->entityByRange[ENTITIES_OUT_OF_RANGE] == NULL || world->entityByRange[ENTITIES_IN_RANGE] == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate entity lists (createWorld)\n");
			// keep on rocking in the
			freeWorld(world);
		}
	} else {
		if (world == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Could not allocate world (createWorld)\n");
		} 
		if (renderer == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamRenderer does not exist (createWorld)\n");
		}
	}

	return world;
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void setWorldFilterTypeRectangle(World* world, uint16 inRangeRectangleWidth, uint16 inRangeRectangleHeight) {
	if (world != NULL) {
		world->distanceFilteringType = ft_Rectangle;
		world->inRangeRectangleWidth = inRangeRectangleWidth;
		world->inRangeRectangleHeight = inRangeRectangleHeight;
	} else {
		jSetError(ERROR_NULL_POINTER, "World does not exist (setWorldFilterTypeRectangle)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void setWorldFilterTypeCircle(World* world, uint16 inRangeRadius) {
	if (world != NULL) {
		world->distanceFilteringType = ft_Proximity;
		world->inRangeRadius = inRangeRadius;
	} else {
		jSetError(ERROR_NULL_POINTER, "World does not exist (setWorldFilterTypeCircle)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void worldAddEntity(World* world, JamEntity* entity) {
	if (world != NULL && entity != NULL && entity->type < MAX_ENTITY_TYPES) {
		// Put the entity in each of the three lists it belongs to
		addEntityToList(world->entityByRange[ENTITIES_IN_RANGE], entity);
		addEntityToList(world->entityTypes[entity->type], entity);
		addEntityToList(world->worldEntities, entity);

		// Attempt to call the entity's onCreation function
		if (entity->behaviour != NULL && entity->behaviour->onCreation != NULL) {
			(*entity->behaviour->onCreation)(world->renderer, world, entity);
		}
	} else {
		if (world == NULL) {
			jSetError(ERROR_NULL_POINTER, "World does not exist (worldAddEntity)\n");
		}
		if (entity == NULL) {
			jSetError(ERROR_NULL_POINTER, "JamEntity does not exist (worldAddEntity)\n");
		}
		if (entity != NULL && entity->type >= MAX_ENTITY_TYPES) {
			jSetError(ERROR_INCORRECT_FORMAT, "JamEntity type is invalid (worldAddEntity)\n");
		}
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void worldProcFrame(World* world) {
	int i;
	JamEntity* ent;

	if (world != NULL) {
		// Process all the entities in range
		for (i = 0; i < world->entityByRange[ENTITIES_IN_RANGE]->size; i++) {
			ent = world->entityByRange[ENTITIES_IN_RANGE]->entities[i];
			if (ent != NULL && ent->behaviour != NULL && ent->behaviour->onFrame != NULL)
				(*ent->behaviour->onFrame)(world->renderer, world, ent);
		}

		// Draw all the entities in range
		for (i = 0; i < world->entityByRange[ENTITIES_IN_RANGE]->size; i++) {
			ent = world->entityByRange[ENTITIES_IN_RANGE]->entities[i];
			if (ent != NULL && ent->behaviour != NULL) {
				if (ent->behaviour->onDraw != NULL)
					(*ent->behaviour->onDraw)(world->renderer, world, ent);
				else
					jamDrawEntity(world->renderer, ent);
			} else if (ent != NULL && ent->behaviour == NULL) {
				jamDrawEntity(world->renderer, ent);
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "World does not exist (worldProcFrame)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void worldRemoveEntity(World* world, JamEntity* entity) {
	int i;
	JamEntityType type = et_None;

	if (world != NULL) {
		// First find it in the master list
		for (i = 0; i < world->worldEntities->size; i++) {
			if (world->worldEntities->entities[i] != NULL && world->worldEntities->entities[i] == entity) {
				// We found it, call its destroy function if applicable
				if (world->worldEntities->entities[i]->behaviour != NULL && world->worldEntities->entities[i]->behaviour->onDestruction != NULL)
					(*world->worldEntities->entities[i]->behaviour->onDestruction)(world->renderer, world, world->worldEntities->entities[i]);

				// Record the type and destroy it
				type = world->worldEntities->entities[i]->type;
				jamFreeEntity(world->worldEntities->entities[i], false, false, false);
				world->worldEntities->entities[i] = NULL;
			}
		}

		// Clear it from the other lists if it actually exists
		if (type != et_None) {
			for (i = 0; i < world->entityTypes[type]->size; i++)
				if (world->entityTypes[type]->entities[i] != NULL && world->entityTypes[type]->entities[i] == entity)
					world->entityTypes[type]->entities[i] = NULL;
			for (i = 0; i < world->entityByRange[ENTITIES_IN_RANGE]->size; i++)
				if (world->entityByRange[ENTITIES_IN_RANGE]->entities[i] != NULL && world->entityByRange[ENTITIES_IN_RANGE]->entities[i] == entity)
					world->entityByRange[ENTITIES_IN_RANGE]->entities[i] = NULL;
			for (i = 0; i < world->entityByRange[ENTITIES_OUT_OF_RANGE]->size; i++)
				if (world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] != NULL && world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] == entity)
					world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] = NULL;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "World does not exist (worldRemoveEntity)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void worldRotateEntity(World* world, JamEntity* entity) {
	int i = 0;
	bool found = false;

	if (world != NULL) {
		// Find the entity in the in range list, if it's not there we don't need to do anything
		while (i < world->entityByRange[ENTITIES_IN_RANGE]->size && !found) {
			if (world->entityByRange[ENTITIES_IN_RANGE]->entities[i] == entity) {
				world->entityByRange[ENTITIES_IN_RANGE]->entities[i] = NULL;
				addEntityToList(world->entityByRange[ENTITIES_OUT_OF_RANGE], entity);
				found = true;
			}
			i++;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "World does not exist (worldRotateEntity)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void filterEntitiesByProximity(World* world, int pointX, int pointY) {
	uint32 i;
	JamEntity* entity;

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
					addEntityToList(world->entityByRange[ENTITIES_OUT_OF_RANGE], entity);
				}
			}

			// Situation 2: It was out of range but now its in range
			for (i = 0; i < world->entityByRange[ENTITIES_OUT_OF_RANGE]->size; i++) {
				entity = world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i];
				if (entity != NULL && pointInRectangle(entity->x + entity->sprite->originX, entity->y + entity->sprite->originY,
														pointX, pointY, world->inRangeRectangleWidth, world->inRangeRectangleHeight)) {
					world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] = NULL;
					addEntityToList(world->entityByRange[ENTITIES_IN_RANGE], entity);
				}
			}
		} else {
			// Situation 1: It was in range but now is not
			for (i = 0; i < world->entityByRange[ENTITIES_IN_RANGE]->size; i++) {
				entity = world->entityByRange[ENTITIES_IN_RANGE]->entities[i];
				if (entity != NULL && !pointInCircle(entity->x + entity->sprite->originX, entity->y + entity->sprite->originY,
														pointX, pointY, world->inRangeRadius)) {
					world->entityByRange[ENTITIES_IN_RANGE]->entities[i] = NULL;
					addEntityToList(world->entityByRange[ENTITIES_OUT_OF_RANGE], entity);
				}
			}

			// Situation 2: It was out of range but now its in range
			for (i = 0; i < world->entityByRange[ENTITIES_OUT_OF_RANGE]->size; i++) {
				entity = world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i];
				if (entity != NULL && pointInCircle(entity->x + entity->sprite->originX, entity->y + entity->sprite->originY,
													   pointX, pointY, world->inRangeRadius)) {
					world->entityByRange[ENTITIES_OUT_OF_RANGE]->entities[i] = NULL;
					addEntityToList(world->entityByRange[ENTITIES_IN_RANGE], entity);
				}
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "World does not exist (filterEntitiesByProximity)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void freeWorld(World* world) {
	int i;
	if (world != NULL) {
		// Free all the entities first
		for (i = 0; i < world->worldEntities->size; i++)
			if (world->worldEntities->entities[i] != NULL)
				worldRemoveEntity(world, world->worldEntities->entities[i]);

		// There is a lot of lists to free
		for (i = 0; i < MAX_ENTITY_TYPES; i++)
			freeEntityList(world->entityTypes[i], false);
		freeEntityList(world->entityByRange[ENTITIES_IN_RANGE], false);
		freeEntityList(world->entityByRange[ENTITIES_OUT_OF_RANGE], false);
		freeEntityList(world->worldEntities, true);
		free(world->worldMaps);
	}
}
///////////////////////////////////////////////////////
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
JamWorld* jamWorldCreate() {
	JamWorld* world = (JamWorld*)calloc(1, sizeof(JamWorld));

	if (world != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Could not allocate world (jamWorldCreate)");
	}

	return world;
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void jamWorldAddEntity(JamWorld *world, JamEntity *entity) {
	if (world != NULL && entity != NULL) {
		// TODO: This
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

//
// Created by lugi1 on 2018-07-09.
//

#include <stdio.h>
#include <malloc.h>
#include <World.h>
#include <Entity.h>

///////////////////////////////////////////////////////
World* createWorld() {
	World* world = (World*)calloc(1, sizeof(World));
	int i;
	bool error;

	if (world != NULL) {
		// We now need to initialize the 8 or so entity lists that come with a world
		for (i = 0; i < MAX_ENTITY_TYPES; i++)
			(world->entityTypes[i] = createEntityList()) || (error = true);
		(world->worldEntities = createEntityList()) || (error = true);
		(world->entityByRange[ENTITIES_IN_RANGE] = createEntityList()) || (error = true);
		(world->entityByRange[ENTITIES_OUT_OF_RANGE] = createEntityList()) || (error = true);

		if (error) {
			fprintf(stderr, "Failed to allocate entity lists (createWorld)\n");
			// keep on rocking in the
			freeWorld(world);
		}
	} else {
		fprintf(stderr, "Could not allocate world (createWorld)\n");
	}

	return world;
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void setWorldFilterTypeRectangle(World* world, uint16 inRangeRectangleWidth, uint16 inRangeRectangleHeight) {
	if (world != NULL) {
		world->distanceFilteringType = fRectangle;
		world->inRangeRectangleWidth = inRangeRectangleWidth;
		world->inRangeRectangleHeight = inRangeRectangleHeight;
	} else {
		fprintf(stderr, "World does not exist (setWorldFilterTypeRectangle)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void setWorldFilterTypeCircle(World* world, uint16 inRangeRadius) {
	if (world != NULL) {
		world->distanceFilteringType = fProximity;
		world->inRangeRadius = inRangeRadius;
	} else {
		fprintf(stderr, "World does not exist (setWorldFilterTypeCircle)\n");
	}
}
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
void worldAddEntity(World* world, Entity* entity) {
	if (world != NULL && entity != NULL && entity->type < MAX_ENTITY_TYPES) {
		// Put the entity in each of the three lists it belongs to
		addEntityToList(world->entityByRange[ENTITIES_IN_RANGE], entity);
		addEntityToList(world->entityTypes[entity->type], entity);
		addEntityToList(world->worldEntities, entity);
	} else {
		if (world == NULL)
			fprintf(stderr, "World does not exist (worldAddEntity)\n");
		if (entity == NULL)
			fprintf(stderr, "Entity does not exist (worldAddEntity)\n");
		if (entity != NULL && entity->type >= MAX_ENTITY_TYPES)
			fprintf(stderr, "Entity type is invalid (worldAddEntity)\n");
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
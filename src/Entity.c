//
// Created by lugi1 on 2018-07-19.
//

#include "Entity.h"
#include "TileMap.h"
#include <stdio.h>
#include <Entity.h>
#include "JamError.h"

//////////////////////////////////////////////////////////
Entity* createEntity(Sprite* sprite, Hitbox* hitbox, double x, double y, double hitboxOffsetX, double hitboxOffsetY, Behaviour* behaviour) {
	Entity* ent = (Entity*)malloc(sizeof(Entity));

	// Check that it worked
	if (ent != NULL) {
		ent->sprite = sprite;
		ent->hitbox = hitbox;
		ent->x = x;
		ent->y = y;
		ent->scaleX = 1;
		ent->scaleY = 1;
		ent->hitboxOffsetX = hitboxOffsetX;
		ent->hitboxOffsetY = hitboxOffsetY;
		ent->hSpeed = 0;
		ent->vSpeed = 0;
		ent->friction = 0;
		ent->processPriority = 0;
		ent->type = logic;
		ent->behaviour = behaviour;
		ent->userData = NULL;
	} else {
		fprintf(stderr, "Failed to create Entity struct (createEntity).\n");
		jSetError(ERROR_ALLOC_FAILED);
	}

	return ent;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
Entity* copyEntity(Entity* baseEntity, double x, double y) {
	Entity* newEnt = NULL;

	if (baseEntity != NULL) {
		newEnt = createEntity(baseEntity->sprite, baseEntity->hitbox, x, y, baseEntity->hitboxOffsetX, baseEntity->hitboxOffsetY, baseEntity->behaviour);
		if (newEnt != NULL) {
			newEnt->type = baseEntity->type;
			newEnt->processPriority = baseEntity->processPriority;
			newEnt->behaviour = baseEntity->behaviour;
			newEnt->scaleX = baseEntity->scaleX;
			newEnt->scaleY = baseEntity->scaleY;
			
			#ifdef ENTITY_ENABLE_SPEED
			newEnt->hSpeed = baseEntity->hSpeed;
			newEnt->vSpeed = baseEntity->vSpeed;
			#endif
			#ifdef ENTITY_ENABLE_FRICTION
			newEnt->friction = baseEntity->friction;
			#endif
			#ifdef ENTITY_ENABLE_Z
			newEnt->z = baseEntity->z;
			#endif
		}
	} else {
		fprintf(stderr, "Base entity doesn't exist (copyEntity)\n");
		jSetError(ERROR_NULL_POINTER);
	}

	return newEnt;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void drawEntity(Renderer* renderer, Entity* entity) {
	if (entity != NULL && renderer != NULL) {
		drawSprite(renderer, entity->sprite, (sint32)entity->x, (sint32)entity->y, entity->scaleX, entity->scaleY);
	} else {
		if (entity == NULL)
			fprintf(stderr, "Entity does not exist (drawEntity).\n");
		if (renderer == NULL)
			fprintf(stderr, "Renderer does not exist (drawEntity).\n");
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool checkEntityCollision(int x, int y, Entity* entity1, Entity* entity2) {
	bool coll = false;
	double x1, y1, x2, y2; // Accounting for origins

	// Check both things exist
	if (entity1 != NULL && entity2 != NULL && entity1->hitbox != NULL
		&& entity2->hitbox != NULL) {
		// Load up the values
		x1 = x;
		y1 = y;
		x2 = entity2->x;
		y2 = entity2->y;

		// Account for rectangular origins
		if (entity1->hitbox->type == hitRectangle) {
			x1 -= entity1->hitboxOffsetX;
			y1 -= entity1->hitboxOffsetY;
		}
		if (entity2->hitbox->type == hitRectangle) {
			x2 -= entity2->hitboxOffsetX;
			y2 -= entity2->hitboxOffsetY;
		}

		// Now check the collision itself
		coll = checkHitboxCollision(entity1->hitbox, x1, y1, entity2->hitbox, x2, y2);
	} else {
		if (entity1 == NULL) {
			fprintf(stderr, "entity1 does not exist (checkEntityCollision).\n");
			jSetError(ERROR_NULL_POINTER);
		} else {
			if (entity1->sprite == NULL) {
				fprintf(stderr, "entity1 does not have a sprite (checkEntityCollision).\n");
				jSetError(ERROR_INCORRECT_FORMAT);
			}
			if (entity1->hitbox == NULL) {
				fprintf(stderr, "entity1 does not have a hitbox (checkEntityCollision).\n");
				jSetError(ERROR_INCORRECT_FORMAT);
			}
		}
		if (entity2 == NULL) {
			fprintf(stderr, "entity2 does not exist (checkEntityCollision).\n");
			jSetError(ERROR_NULL_POINTER);
		} else {
			if (entity2->sprite == NULL) {
				fprintf(stderr, "entity2 does not have a sprite (checkEntityCollision).\n");
				jSetError(ERROR_INCORRECT_FORMAT);
			}
			if (entity2->hitbox == NULL) {
				fprintf(stderr, "entity2 does not have a hitbox (checkEntityCollision).\n");
				jSetError(ERROR_INCORRECT_FORMAT);
			}
		}
	}

	return coll;
}
//////////////////////////////////////////////////////////

int roundToInt(double x) {
	x += 0.5;
	return (int)(x);
}

//////////////////////////////////////////////////////////
bool checkEntityTileMapCollision(Entity* entity, TileMap* tileMap, double rx, double ry) {
	bool coll = false;
	double x, y; // Accounting for origins

	// Check both things exist
	if (entity != NULL && entity->hitbox != NULL && tileMap != NULL) {
		x = rx + entity->hitboxOffsetX;
		y = ry + entity->hitboxOffsetY;

		// Now check the collision itself
		coll = checkMapCollision(tileMap, roundToInt(x), roundToInt(y), (int)entity->hitbox->width, (int)entity->hitbox->height);
	} else {
		if (entity == NULL) {
			fprintf(stderr, "entity1 does not exist (checkEntityTileMapCollision).\n");
			jSetError(ERROR_NULL_POINTER);
		} else {
			if (entity->sprite == NULL) {
				fprintf(stderr, "entity does not have a sprite (checkEntityTileMapCollision).\n");
				jSetError(ERROR_INCORRECT_FORMAT);
			}
			if (entity->hitbox == NULL) {
				fprintf(stderr, "entity does not have a hitbox (checkEntityTileMapCollision).\n");
				jSetError(ERROR_INCORRECT_FORMAT);
			}
		}
		if (tileMap == NULL) {
			fprintf(stderr, "tileMap does not exist (checkEntityTileMapCollision).\n");
			jSetError(ERROR_NULL_POINTER);
		}
	}

	return coll;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void freeEntity(Entity* entity, bool destroyHitbox, bool destroySprite, bool destroyFrames) {
	if (entity != NULL) {
		if (destroyHitbox)
			freeHitbox(entity->hitbox);
		if (destroySprite)
			freeSprite(entity->sprite, destroyFrames, false);
		free(entity);
	}
}
//////////////////////////////////////////////////////////
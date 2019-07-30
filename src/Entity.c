//
// Created by lugi1 on 2018-07-19.
//

#include "Entity.h"
#include "TileMap.h"
#include <stdio.h>
#include "JamError.h"

//////////////////////////////////////////////////////////
Entity* createEntity(Sprite* sprite, Hitbox* hitbox, int x, int y, int id) {
	Entity* ent = (Entity*)malloc(sizeof(Entity));

	// Check that it worked
	if (ent != NULL) {
		ent->sprite = sprite;
		ent->hitbox = hitbox;
		ent->x = x;
		ent->y = y;
		ent->entityID = id;
		ent->hSpeed = 0;
		ent->vSpeed = 0;
		ent->friction = 0;
		ent->processPriority = 0;
	} else {
		fprintf(stderr, "Failed to create Entity struct (createEntity).\n");
		jSetError(ERROR_ALLOC_FAILED);
	}

	return ent;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
Entity* copyEntity(Entity* baseEntity) {
	// TODO: This
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void drawEntity(Renderer* renderer, Entity* entity) {
	if (entity != NULL && renderer != NULL) {
		drawSprite(renderer, entity->sprite, (sint32)entity->x, (sint32)entity->y);
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
		&& entity1->sprite != NULL && entity2->hitbox != NULL
		&& entity2->sprite != NULL) {
		// Load up the values
		x1 = x;
		y1 = y;
		x2 = entity2->x;
		y2 = entity2->y;

		// Account for rectangular origins
		if (entity1->hitbox->type == hitRectangle) {
			x1 -= entity1->sprite->originX;
			y1 -= entity1->sprite->originY;
		}
		if (entity2->hitbox->type == hitRectangle) {
			x2 -= entity2->sprite->originX;
			y2 -= entity2->sprite->originY;
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

//////////////////////////////////////////////////////////
bool checkEntityTileMapCollision(Entity* entity, TileMap* tileMap, int rx, int ry) {
	bool coll = false;
	double x, y; // Accounting for origins

	// Check both things exist
	if (entity != NULL && entity->hitbox != NULL && entity->sprite != NULL && tileMap != NULL) {
		x = rx;
		y = ry;

		// Account for rectangular origins
		if (entity->hitbox->type == hitRectangle) {
			x -= entity->sprite->originX;
			y -= entity->sprite->originY;
		}

		// Now check the collision itself
		coll = checkMapCollFast(tileMap, (int)x, (int)y, (int)entity->hitbox->width, (int)entity->hitbox->height);
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
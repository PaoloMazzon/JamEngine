//
// Created by lugi1 on 2018-07-19.
//

#include "Entity.h"
#include "TileMap.h"
#include <stdio.h>
#include <Entity.h>
#include <Sprite.h>
#include "JamError.h"

int roundToInt(double x) {
	x += 0.5;
	return (int)(x);
}

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
		ent->alpha = 1;
		ent->rot = 0;
		ent->updateOnDraw = true;
		ent->hitboxOffsetX = hitboxOffsetX;
		ent->hitboxOffsetY = hitboxOffsetY;
		ent->hSpeed = 0;
		ent->vSpeed = 0;
		ent->friction = 0;
		ent->type = logic;
		ent->behaviour = behaviour;
		ent->userData = NULL;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create Entity struct (createEntity).\n");
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
			newEnt->rot = baseEntity->rot;
			newEnt->alpha = baseEntity->alpha;
			newEnt->updateOnDraw = baseEntity->updateOnDraw;
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
		jSetError(ERROR_NULL_POINTER, "Base entity doesn't exist (copyEntity)\n");
	}

	return newEnt;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void drawEntity(Renderer* renderer, Entity* entity) {
	if (entity != NULL && renderer != NULL) {
		if (entity->sprite != NULL)
		drawSprite(renderer, entity->sprite, (sint32)entity->x - entity->sprite->originX, (sint32)entity->y - entity->sprite->originY, entity->scaleX, entity->scaleY, entity->rot, entity->alpha, entity->updateOnDraw);
	} else {
		if (entity == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity does not exist (drawEntity).\n");
		if (renderer == NULL)
			jSetError(ERROR_NULL_POINTER, "Renderer does not exist (drawEntity).\n");
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
			jSetError(ERROR_NULL_POINTER, "entity1 does not exist (checkEntityCollision).\n");
		} else {
			if (entity1->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity1 does not have a sprite (checkEntityCollision).\n");
			}
			if (entity1->hitbox == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity1 does not have a hitbox (checkEntityCollision).\n");
			}
		}
		if (entity2 == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity2 does not exist (checkEntityCollision).\n");
		} else {
			if (entity2->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity2 does not have a sprite (checkEntityCollision).\n");
			}
			if (entity2->hitbox == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity2 does not have a hitbox (checkEntityCollision).\n");
			}
		}
	}

	return coll;
}
//////////////////////////////////////////////////////////

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
			jSetError(ERROR_NULL_POINTER, "entity1 does not exist (checkEntityTileMapCollision).\n");
		} else {
			if (entity->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity does not have a sprite (checkEntityTileMapCollision).\n");
			}
			if (entity->hitbox == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity does not have a hitbox (checkEntityTileMapCollision).\n");
			}
		}
		if (tileMap == NULL) {
			jSetError(ERROR_NULL_POINTER, "tileMap does not exist (checkEntityTileMapCollision).\n");
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
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
JamEntity* jamCreateEntity(JamSprite *sprite, JamHitbox *hitbox, double x, double y, double hitboxOffsetX,
						   double hitboxOffsetY, JamBehaviour *behaviour) {
	JamEntity* ent = (JamEntity*)malloc(sizeof(JamEntity));

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
		ent->type = et_Unassigned;
		ent->behaviour = behaviour;
		ent->userData = NULL;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create JamEntity struct (jamCreateEntity).\n");
	}

	return ent;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
JamEntity* jamCopyEntity(JamEntity *baseEntity, double x, double y) {
	JamEntity* newEnt = NULL;

	if (baseEntity != NULL) {
		newEnt = jamCreateEntity(baseEntity->sprite, baseEntity->hitbox, x, y, baseEntity->hitboxOffsetX,
								 baseEntity->hitboxOffsetY, baseEntity->behaviour);
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
		jSetError(ERROR_NULL_POINTER, "Base entity doesn't exist (jamCopyEntity)\n");
	}

	return newEnt;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void jamDrawEntity(JamRenderer *renderer, JamEntity *entity) {
	if (entity != NULL && renderer != NULL) {
		if (entity->sprite != NULL)
			jamDrawSprite(renderer, entity->sprite, (sint32) entity->x - entity->sprite->originX,
						  (sint32) entity->y - entity->sprite->originY, entity->scaleX, entity->scaleY, entity->rot,
						  entity->alpha, entity->updateOnDraw);
	} else {
		if (entity == NULL)
			jSetError(ERROR_NULL_POINTER, "JamEntity does not exist (jamDrawEntity).\n");
		if (renderer == NULL)
			jSetError(ERROR_NULL_POINTER, "JamRenderer does not exist (jamDrawEntity).\n");
	}
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool jamCheckEntityCollision(int x, int y, JamEntity *entity1, JamEntity *entity2) {
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
		if (entity1->hitbox->type == ht_Rectangle) {
			x1 -= entity1->hitboxOffsetX;
			y1 -= entity1->hitboxOffsetY;
		}
		if (entity2->hitbox->type == ht_Rectangle) {
			x2 -= entity2->hitboxOffsetX;
			y2 -= entity2->hitboxOffsetY;
		}

		// Now check the collision itself
		coll = jamCheckHitboxCollision(entity1->hitbox, x1, y1, entity2->hitbox, x2, y2);
	} else {
		if (entity1 == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity1 does not exist (jamCheckEntityCollision).\n");
		} else {
			if (entity1->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity1 does not have a sprite (jamCheckEntityCollision).\n");
			}
			if (entity1->hitbox == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity1 does not have a hitbox (jamCheckEntityCollision).\n");
			}
		}
		if (entity2 == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity2 does not exist (jamCheckEntityCollision).\n");
		} else {
			if (entity2->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity2 does not have a sprite (jamCheckEntityCollision).\n");
			}
			if (entity2->hitbox == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity2 does not have a hitbox (jamCheckEntityCollision).\n");
			}
		}
	}

	return coll;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool jamCheckEntityTileMapCollision(JamEntity *entity, JamTileMap *tileMap, double rx, double ry) {
	bool coll = false;
	double x, y; // Accounting for origins

	// Check both things exist
	if (entity != NULL && entity->hitbox != NULL && tileMap != NULL) {
		x = rx + entity->hitboxOffsetX;
		y = ry + entity->hitboxOffsetY;

		// Now check the collision itself
		coll = jamCheckMapCollision(tileMap, roundToInt(x), roundToInt(y), (int) entity->hitbox->width,
									(int) entity->hitbox->height);
	} else {
		if (entity == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity1 does not exist (jamCheckEntityTileMapCollision).\n");
		} else {
			if (entity->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity does not have a sprite (jamCheckEntityTileMapCollision).\n");
			}
			if (entity->hitbox == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity does not have a hitbox (jamCheckEntityTileMapCollision).\n");
			}
		}
		if (tileMap == NULL) {
			jSetError(ERROR_NULL_POINTER, "tileMap does not exist (jamCheckEntityTileMapCollision).\n");
		}
	}

	return coll;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void jamFreeEntity(JamEntity *entity, bool destroyHitbox, bool destroySprite, bool destroyFrames) {
	if (entity != NULL) {
		if (destroyHitbox)
			jamFreeHitbox(entity->hitbox);
		if (destroySprite)
			jamFreeSprite(entity->sprite, destroyFrames, false);
		free(entity);
	}
}
//////////////////////////////////////////////////////////
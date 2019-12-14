//
// Created by lugi1 on 2018-07-19.
//

#include "Entity.h"
#include "TileMap.h"
#include <stdio.h>
#include <Entity.h>
#include <Sprite.h>
#include "JamError.h"
#include <malloc.h>
#include <math.h>

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
		ent->data = NULL;
		ent->id = -1;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create JamEntity struct (jamCreateEntity)");
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
		jSetError(ERROR_NULL_POINTER, "Base entity doesn't exist (jamCopyEntity)");
	}

	return newEnt;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void jamDrawEntity(JamEntity *entity) {
	if (entity != NULL) {
		if (entity->sprite != NULL)
			jamDrawSprite(entity->sprite, (sint32) entity->x - entity->sprite->originX,
						  (sint32) entity->y - entity->sprite->originY, entity->scaleX, entity->scaleY, entity->rot,
						  entity->alpha, entity->updateOnDraw);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamEntity does not exist (jamDrawEntity)");
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
		x1 = x + entity1->hitboxOffsetX;
		y1 = y + entity1->hitboxOffsetY;
		x2 = entity2->x + entity2->hitboxOffsetX;
		y2 = entity2->y + entity2->hitboxOffsetY;
		
		// Now check the collision itself
		coll = jamCheckHitboxCollision(entity1->hitbox, x1, y1, entity2->hitbox, x2, y2);
	} else {
		if (entity1 == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity1 does not exist (jamCheckEntityCollision)");
		} else {
			if (entity1->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity1 does not have a sprite (jamCheckEntityCollision)");
			}
		}
		if (entity2 == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity2 does not exist (jamCheckEntityCollision)");
		} else {
			if (entity2->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity2 does not have a sprite (jamCheckEntityCollision)");
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
		coll = jamCheckMapCollision(tileMap, (int)round(x), (int)round(y), (int) entity->hitbox->width,
									(int) entity->hitbox->height);
	} else {
		if (entity == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity1 does not exist (jamCheckEntityTileMapCollision)");
		} else {
			if (entity->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity does not have a sprite (jamCheckEntityTileMapCollision)");
			}
			if (entity->hitbox == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity does not have a hitbox (jamCheckEntityTileMapCollision)");
			}
		}
		if (tileMap == NULL) {
			jSetError(ERROR_NULL_POINTER, "tileMap does not exist (jamCheckEntityTileMapCollision)");
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

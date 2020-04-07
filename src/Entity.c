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
#include <Drawing.h>
#include <math.h>
#include <TileMap.h>
#include <Hitbox.h>

// Grab Entity's hitbox coordinates, accounting for origins (does not check for null pointers)
static inline double _getEntHitX(JamEntity* ent, double x) {
	return x - ent->sprite->originX + ent->hitboxOffsetX;
}
static inline double _getEntHitY(JamEntity* ent, double y) {
	return y - ent->sprite->originY + ent->hitboxOffsetY;
}
static inline double _getEntHitX2(JamEntity* ent, double x) {
	return x - ent->sprite->originX + ent->hitbox->width + ent->hitboxOffsetX;
}
static inline double _getEntHitY2(JamEntity* ent, double y) {
	return y - ent->sprite->originY + ent->hitbox->height + ent->hitboxOffsetY;
}

// Rounds a double down to an int
static inline int _roundDoubleToInt(double x) {
	return (int)round(x);
}

// Processes an entity's animation
void _updateEntSprite(JamEntity* ent) {
	if (ent->updateOnDraw) {
		ent->frameTimer += jamRendererGetDelta();
		if (ent->frameTimer >= ent->sprite->frameDelay) {
			ent->frameTimer = 0;
			ent->currentFrame++;
			if (ent->currentFrame >= ent->sprite->animationLength) {
				if (!ent->sprite->looping) {
					ent->updateOnDraw = false;
					ent->currentFrame = ent->sprite->animationLength - 1;
				} else {
					ent->currentFrame = 0;
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////
JamEntity* jamEntityCreate(JamSprite *sprite, JamHitbox *hitbox, double x, double y, double hitboxOffsetX,
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
		ent->type = 0;
		ent->behaviour = behaviour;
		ent->data = NULL;
		ent->id = ID_NOT_ASSIGNED;
		ent->xPrev = 0;
		ent->yPrev = 0;
		ent->proc = false;
		ent->draw = false;
		ent->cells = 0;
		ent->destroy = false;
		ent->inCache = false;
		ent->frameTimer = 0;
		ent->currentFrame = 0;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create JamEntity struct");
	}

	return ent;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
JamEntity* jamEntityCopy(JamEntity *baseEntity, double x, double y) {
	JamEntity* newEnt = NULL;

	if (baseEntity != NULL) {
		newEnt = jamEntityCreate(baseEntity->sprite, baseEntity->hitbox, x, y, baseEntity->hitboxOffsetX,
								 baseEntity->hitboxOffsetY, baseEntity->behaviour);
		if (newEnt != NULL) {
			newEnt->type = baseEntity->type;
			newEnt->rot = baseEntity->rot;
			newEnt->alpha = baseEntity->alpha;
			newEnt->updateOnDraw = baseEntity->updateOnDraw;
			newEnt->behaviour = baseEntity->behaviour;
			newEnt->scaleX = baseEntity->scaleX;
			newEnt->scaleY = baseEntity->scaleY;
			newEnt->hSpeed = baseEntity->hSpeed;
			newEnt->vSpeed = baseEntity->vSpeed;
			newEnt->friction = baseEntity->friction;
			newEnt->z = baseEntity->z;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Base entity doesn't exist");
	}

	return newEnt;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void jamDrawEntity(JamEntity *entity) {
	if (entity != NULL) {
		if (entity->sprite != NULL) {
			_updateEntSprite(entity);
			jamDrawSpriteFrame(
					entity->sprite,
					_roundDoubleToInt(entity->x),
					_roundDoubleToInt(entity->y),
					entity->scaleX,
					entity->scaleY,
					entity->rot,
					entity->alpha,
					entity->currentFrame
			);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamEntity does not exist");
	}
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool jamEntityCheckCollision(double x, double y, JamEntity *entity1, JamEntity *entity2) {
	bool coll = false;
	double x1, y1, x2, y2; // Accounting for origins

	// Check both things exist
	if (entity1 != NULL && entity2 != NULL && entity1->hitbox != NULL
		&& entity2->hitbox != NULL) {
		// Load up the values
		x1 = _getEntHitX(entity1, x);
		y1 = _getEntHitY(entity1, y);
		x2 = _getEntHitX(entity2, entity2->x);
		y2 = _getEntHitY(entity2, entity2->y);
		
		// Now check the collision itself
		coll = jamHitboxCollision(entity1->hitbox, x1, y1, entity2->hitbox, x2, y2);
	} else {
		if (entity1 == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity1 does not exist");
		} else {
			if (entity1->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity1 does not have a sprite");
			}
		}
		if (entity2 == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity2 does not exist");
		} else {
			if (entity2->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity2 does not have a sprite");
			}
		}
	}

	return coll;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool jamEntityTileMapCollision(JamEntity *entity, JamTileMap *tileMap, double rx, double ry) {
	bool coll = false;
	double x, y; // Accounting for origins

	// Check both things exist
	if (entity != NULL && entity->hitbox != NULL && tileMap != NULL) {
		x = _getEntHitX(entity, rx);
		y = _getEntHitY(entity, ry);

		// Now check the collision itself
		coll = jamTileMapCollision(
				tileMap,
				_roundDoubleToInt(x),
				_roundDoubleToInt(y),
				(int)entity->hitbox->width,
				(int)entity->hitbox->height
		);
	} else {
		if (entity == NULL) {
			jSetError(ERROR_NULL_POINTER, "entity1 does not exist");
		} else {
			if (entity->sprite == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity does not have a sprite");
			}
			if (entity->hitbox == NULL) {
				jSetError(ERROR_INCORRECT_FORMAT, "entity does not have a hitbox");
			}
		}
		if (tileMap == NULL) {
			jSetError(ERROR_NULL_POINTER, "tileMap does not exist");
		}
	}

	return coll;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void jamEntitySnapX(JamEntity *entity, JamTileMap *tilemap, int direction) {
	int gridX, gridY;
	int gridChecks = 0;
	bool cornerColliding = false;
	int i;
	int cellsTall;

	if (entity != NULL && tilemap != NULL && entity->hitbox != NULL && entity->sprite != NULL) {
		// Calculate how tall the entity is as well as how many cells over to start in
		gridX = direction == 1 ? ((uint32)floor(entity->hitbox->width / (double)tilemap->cellWidth)) : 0;
		gridY = (_roundDoubleToInt(_getEntHitY(entity, entity->y)) - tilemap->yInWorld) / tilemap->cellHeight;
		gridX += (_roundDoubleToInt(_getEntHitX(entity, entity->x)) - tilemap->xInWorld) / tilemap->cellWidth;

		// Cells tall is a bit painful since something that is 32px tall can take either 2 or 3 cells
		// in a 16px tall map. As such, we use bottom cell - top cell to calculate this
		cellsTall = (_roundDoubleToInt(_getEntHitY2(entity, entity->y - 1)) / (int)tilemap->cellHeight)
				- (_roundDoubleToInt(_getEntHitY(entity, entity->y)) / (int)tilemap->cellHeight);
		cellsTall++;

		// Find the first collision near the entity
		while (!cornerColliding && gridChecks++ < MAX_GRID_CHECKS) {
			for (i = gridY; i < gridY + cellsTall && !cornerColliding; i++) {
				if (jamTileMapGet(tilemap, (uint32) gridX, (uint32) i) != NULL) {
					cornerColliding = true;
				}
			}
			if (!cornerColliding)
				gridX += direction;
		}

		if (cornerColliding)
			entity->x = gridX * tilemap->cellWidth + tilemap->xInWorld +
						(direction == 1 ? (-entity->sprite->width + (entity->sprite->width - entity->hitbox->width - entity->hitboxOffsetX) + entity->sprite->originX)
										: (entity->sprite->originX - entity->hitboxOffsetX + tilemap->cellWidth));
	} else {
		if (entity == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity doesn't exist");
		else if (entity->hitbox == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity's hitbox doesn't exist");
		if (tilemap == NULL)
			jSetError(ERROR_NULL_POINTER, "TileMap doesn't exist");
		if (entity->sprite == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity's sprite doesn't exist");
	}
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void jamEntitySnapY(JamEntity *entity, JamTileMap *tilemap, int direction) {
	int gridX, gridY;
	int gridChecks = 0;
	bool cornerColliding = false;
	int i;
	int cellsWide;

	if (entity != NULL && tilemap != NULL && entity->hitbox != NULL && entity->sprite != NULL) {
		// Calculate how tall the entity is as well as how many cells over to start in
		gridY = direction == 1 ? ((uint32)floor(entity->hitbox->height / (double)tilemap->cellHeight)) : 0;
		gridY += (_roundDoubleToInt(_getEntHitY(entity, entity->y)) - tilemap->yInWorld) / tilemap->cellHeight;
		gridX = (_roundDoubleToInt(_getEntHitX(entity, entity->x)) - tilemap->xInWorld) / tilemap->cellWidth;

		// Cells tall is a bit painful since something that is 32px tall can take either 2 or 3 cells
		// in a 16px tall map. As such, we use bottom cell - top cell to calculate this
		cellsWide = (_roundDoubleToInt(_getEntHitX2(entity, entity->x - 1)) / (int)tilemap->cellWidth)
					- (_roundDoubleToInt(_getEntHitX(entity, entity->x)) / (int)tilemap->cellWidth);
		cellsWide++;

		// Find the first collision near the entity
		while (!cornerColliding && gridChecks++ < MAX_GRID_CHECKS) {
			for (i = gridX; i < gridX + cellsWide && !cornerColliding; i++) {
				if (jamTileMapGet(tilemap, (uint32) i, (uint32) gridY) != NULL) {
					cornerColliding = true;
				}
			}
			if (!cornerColliding)
				gridY += direction;
		}

		if (cornerColliding)
			entity->y = gridY * tilemap->cellHeight + tilemap->yInWorld +
						(direction == 1 ? (-entity->sprite->height + (entity->sprite->height - entity->hitbox->height - entity->hitboxOffsetY) + entity->sprite->originY)
										: (entity->sprite->originY - entity->hitboxOffsetY + tilemap->cellHeight));
	} else {
		if (entity == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity doesn't exist");
		else if (entity->hitbox == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity's hitbox doesn't exist");
		if (tilemap == NULL)
			jSetError(ERROR_NULL_POINTER, "TileMap doesn't exist");
		if (entity->sprite == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity's sprite doesn't exist");
	}
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void jamEntitySetSprite(JamEntity* ent, JamSprite* spr) {
	if (ent != NULL && ent->sprite != spr) {
		ent->sprite = spr;
		ent->frameTimer = 0;
		ent->currentFrame = 0;
		ent->updateOnDraw = true;
	} else {
		if (ent == NULL)
			jSetError(ERROR_NULL_POINTER, "Entity doesn't exist");
	}
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
double jamEntityVisibleX1(JamEntity* entity, double x) {
	if (entity != NULL) {
		if (entity->sprite != NULL)
			return x - entity->sprite->originX;
		else
			return x;
	} else {
		jSetError(ERROR_NULL_POINTER, "Entity does not exist");
	}

	return 0;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
double jamEntityVisibleY1(JamEntity* entity, double y) {
	if (entity != NULL) {
		if (entity->sprite != NULL)
			return y - entity->sprite->originY;
		else
			return y;
	} else {
		jSetError(ERROR_NULL_POINTER, "Entity does not exist");
	}

	return 0;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
double jamEntityVisibleX2(JamEntity* entity, double x) {
	if (entity != NULL) {
		if (entity->sprite != NULL)
			return x - entity->sprite->originX + entity->sprite->width;
		else
			return x;
	} else {
		jSetError(ERROR_NULL_POINTER, "Entity does not exist");
	}

	return 0;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
double jamEntityVisibleY2(JamEntity* entity, double y) {
	if (entity != NULL) {
		if (entity->sprite != NULL)
			return y - entity->sprite->originY + entity->sprite->height;
		else
			return y;
	} else {
		jSetError(ERROR_NULL_POINTER, "Entity does not exist");
	}

	return 0;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void jamEntityFree(JamEntity *entity, bool destroyHitbox, bool destroySprite, bool destroyFrames) {
	if (entity != NULL) {
		if (destroyHitbox)
			jamHitboxFree(entity->hitbox);
		if (destroySprite)
			jamSpriteFree(entity->sprite, destroyFrames, false);
		free(entity);
	}
}
//////////////////////////////////////////////////////////

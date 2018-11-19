//
// Created by lugi1 on 2018-07-19.
//

#pragma once
#include "Sprite.h"
#include "Hitbox.h"
#include "TileMap.h"

/// \brief Defines an in-game entity
///
/// Speed and friction is a convenience and the struct
/// will do nothing with them. processPriority is for
/// entity lists
typedef struct {
	Sprite* sprite;      ///< This entity's sprite (NULL is safe)
	Hitbox* hitbox;      ///< This entity's hitbox (NULL is safe)
	int x;               ///< X position in the game world
	int y;               ///< Y position in the game world
	double hSpeed;       ///< Horizontal speed
	double vSpeed;       ///< Vertical speed
	double friction;     ///< Friction for speed calculations
	int processPriority; ///< Entities are processed in descending order
} Entity;

/// \brief Creates/initializes an entity class
Entity* createEntity(Sprite* sprite, Hitbox* hitbox, int x, int y);

/// \brief Draws an entity
void drawEntity(Renderer* renderer, Entity* entity);

/// \brief Checks if two entities are colliding with one another
///
/// X/Y coordinates are relative to entity1
bool checkEntityCollision(int x, int y, Entity* entity1, Entity* entity2);

/// \brief Checks if an entity is colliding with a tile map
bool checkEntityTileMapCollision(Entity* entity, TileMap* tileMap);

/// \brief Frees an entity from memory
///
/// No matter what you specify with the boolean arguments,
/// the sprite's frames' textures will NOT be freed.
void freeEntity(Entity* entity, bool destroyHitbox, bool destroySprite, bool destroyFrames);
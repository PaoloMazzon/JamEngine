/// \file Entity.h
/// \author lugi1
/// \brief To make in-game objects easier to manage, we have entities
///
/// Entities are basically grand wrappers of many components; they
/// group together sprites hitboxes, coordiantes, and some other helpful
/// things in one package so you don't have 5 different arrays for one
/// object.

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
	EntityType type;     ///< Type of entity this is
	double x;            ///< X position in the game world
	double y;            ///< Y position in the game world
	double hSpeed;       ///< Horizontal speed
	double vSpeed;       ///< Vertical speed
	double friction;     ///< Friction for speed calculations
	int processPriority; ///< Entities are processed in descending order
	int entityID;        ///< An ID that will be assigned by the asset loader (or user)
	uint16 behaviour;    ///< Very open-ended, the engine does nothing with this
} Entity;

/// \brief Creates/initializes an entity class
Entity* createEntity(Sprite* sprite, Hitbox* hitbox, int x, int y, int id);

/// \brief Draws an entity
void drawEntity(Renderer* renderer, Entity* entity);

/// \brief Checks if two entities are colliding with one another
///
/// This function uses the rx/ry coordinates for entity 1, no its x/y
bool checkEntityCollision(int x, int y, Entity* entity1, Entity* entity2);

/// \brief Checks if an entity is colliding with a tile map
///
/// This function uses the rx/ry coordinates for the entity, no the entity's x/y
bool checkEntityTileMapCollision(Entity* entity, TileMap* tileMap, int rx, int ry);

/// \brief Frees an entity from memory
///
/// No matter what you specify with the boolean arguments,
/// the sprite's frames' textures will NOT be freed.
void freeEntity(Entity* entity, bool destroyHitbox, bool destroySprite, bool destroyFrames);
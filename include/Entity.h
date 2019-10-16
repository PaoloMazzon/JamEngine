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
#include "BehaviourMap.h"

/// \brief Defines an in-game entity
///
/// Speed, friction, and Z coordinate is just a convinience
/// and none of the Entity functions will touch them. This
/// is a 2D engine and thus does not care about the Z coord.
/// There are cases in 2D games you want it, but that's not
/// the engine's problem. Since not every use case needs
/// these things, you can turn each individually off in the
/// Constants.h file.
typedef struct _Entity {
	Sprite* sprite;       ///< This entity's sprite (NULL is safe)
	Hitbox* hitbox;       ///< This entity's hitbox (NULL is safe)
	EntityType type;      ///< Type of entity this is
	double x;             ///< X position in the game world
	double y;             ///< Y position in the game world
	Behaviour* behaviour; ///< Behaviour mapping of this entity (AssetManagers will resolve this)
	double hitboxOffsetX; ///< The hitbox's offset from the entity, this ignore the sprite's origin
	double hitboxOffsetY; ///< The hitbox's offset from the entity, this ignore the sprite's origin
	void* userData;       ///< A place for the programmer to store their own variables and such

	// Drawing control
	double rot;           ///< The rotation of the entity when drawn
	uint8 alpha;          ///< The alpha of the entity when drawn
	bool updateOnDraw;    ///< Weather or not the sprite will update on drawing
	float scaleX;         ///< The x scale of the entity (for sprite rendering)
	float scaleY;         ///< The y scale of the entity (for sprite rendering)

#ifdef ENTITY_ENABLE_SPEED
	double hSpeed;       ///< Horizontal speed (Can be disabled to save memory)
	double vSpeed;       ///< Vertical speed (Can be disabled to save memory)
#endif
#ifdef ENTITY_ENABLE_FRICTION
	double friction;     ///< Friction for speed calculations (Can be disabled to save memory)
#endif
#ifdef ENTITY_ENABLE_Z
	double z;            ///< Z position in the game world (Can be disabled to save memory)
#endif
} Entity;

/// \brief Creates/initializes an entity class
/// \throws ERROR_ALLOC_FAILED
Entity* createEntity(Sprite* sprite, Hitbox* hitbox, double x, double y, double hitboxOffsetX, double hitboxOffsetY, Behaviour* behaviour);

/// \brief Makes a 1:1 copy of an entity and returns the new copy
Entity* copyEntity(Entity* baseEntity, double x, double y);

/// \brief Draws an entity
/// \throws ERROR_NULL_POINTER
void drawEntity(Renderer* renderer, Entity* entity);

/// \brief Checks if two entities are colliding with one another
///
/// This function uses the rx/ry coordinates for entity 1, no its x/y
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_INCORRECT_FORMAT
bool checkEntityCollision(int x, int y, Entity* entity1, Entity* entity2);

/// \brief Checks if an entity is colliding with a tile map
///
/// This function uses the rx/ry coordinates for the entity, no the entity's x/y
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_INCORRECT_FORMAT
bool checkEntityTileMapCollision(Entity* entity, TileMap* tileMap, double rx, double ry);

/// \brief Frees an entity from memory
///
/// No matter what you specify with the boolean arguments,
/// the sprite's frames' textures will NOT be freed.
void freeEntity(Entity* entity, bool destroyHitbox, bool destroySprite, bool destroyFrames);
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

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Defines an in-game entity
///
/// Since all drawing/hitbox functions are done with ints
/// and an entity's coordinates are in doubles, it is made
/// sure that the doubles are always rounded to ints the
/// same way. In other words, it is guaranteed that entities
/// are drawn and collision-tested with the same set of coords.
///
/// \warning Do not change/use the following variables: `xPrev`,
/// `yPrev`, `procs`, and `cells`. These variables are required
/// by whatever world this entity belongs to and changing them
/// could very easily cause dangling pointers and segfaults.
typedef struct _JamEntity {
	JamSprite* sprite;       ///< This entity's sprite (NULL is safe)
	JamHitbox* hitbox;       ///< This entity's hitbox (NULL is safe)
	uint32 type;             ///< Type of entity this is
	double x;                ///< X position in the game world
	double y;                ///< Y position in the game world
	JamBehaviour* behaviour; ///< Behaviour mapping of this entity (AssetManagers will resolve this)
	double hitboxOffsetX;    ///< The hitbox's offset from the entity, this ignore the sprite's origin
	double hitboxOffsetY;    ///< The hitbox's offset from the entity, this ignore the sprite's origin
	void* data;              ///< A place for the programmer to store their own variables and such
	int id;                  ///< The ID of this entity (assigned by whatever world this entity belongs to)

	// Drawing control
	double rot;        ///< The rotation of the entity when drawn
	uint8 alpha;       ///< The alpha of the entity when drawn
	bool updateOnDraw; ///< Weather or not the sprite will update on drawing
	float scaleX;      ///< The x scale of the entity (for sprite rendering)
	float scaleY;      ///< The y scale of the entity (for sprite rendering)

	// Things to help worlds
	double xPrev;          ///< Last frame's x position (JamWorlds will handle this automatically)
	double yPrev;          ///< Last frame's y position (JamWorlds will handle this automatically)
	bool proc;             ///< Weather or not this entity has been processed this frame
	bool draw;             ///< Weather or not this entity has been drawn this frame
	uint32 cells;          ///< How many cells this entity is in in the world map
	int cellsIn[4];        ///< The specific cells this entity is in
	int cellsLoc[4];       ///< Where in the entity list this entity is
	volatile bool inCache; ///< Weather or not this specific entity is in entity cache
	bool destroy;          ///< Weather or not this entity will be destroyed the next time its processed

	// Utilities not utilized by the engine
	double hSpeed;   ///< Horizontal speed
	double vSpeed;   ///< Vertical speed
	double friction; ///< Friction for speed calculations
	double z;        ///< Z position in the game world
} JamEntity;

/// \brief Creates/initializes an entity class
/// \throws ERROR_ALLOC_FAILED
JamEntity* jamEntityCreate(JamSprite *sprite, JamHitbox *hitbox, double x, double y, double hitboxOffsetX,
						   double hitboxOffsetY, JamBehaviour *behaviour);

/// \brief Makes a 1:1 copy of an entity and returns the new copy
JamEntity* jamEntityCopy(JamEntity *baseEntity, double x, double y);

/// \brief Draws an entity
/// \throws ERROR_NULL_POINTER
void jamDrawEntity(JamEntity *entity);

/// \brief Checks if two entities are colliding with one another
///
/// This function uses the x/y coordinates for entity 1, not its x/y
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_INCORRECT_FORMAT
bool jamEntityCheckCollision(double x, double y, JamEntity *entity1, JamEntity *entity2);

/// \brief Checks if an entity is colliding with a tile map
///
/// This function uses the rx/ry coordinates for the entity, not the entity's x/y
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_INCORRECT_FORMAT
bool jamEntityTileMapCollision(JamEntity *entity, JamTileMap *tileMap, double rx, double ry);

/// \brief Sets an entity's x component as close to a tilemap's edge as possible
///
/// `direction` should be either 1 or -1 to represent right and left respectively.
/// This function also requires the entity to meet the following requirements:
///
///  + It has a sprite
///  + It has a hitbox
///  + The hitbox is a rectangle
///
/// \throws ERROR_NULL_POINTER
void jamEntitySnapX(JamEntity *entity, JamTileMap *tilemap, int direction);

/// \brief Sets an entity's y component as close to a tilemap's edge as possible
///
/// `direction` should be either 1 or -1 to represent right and left respectively
/// This function also requires the entity to meet the following requirements:
///
///  + It has a sprite
///  + It has a hitbox
///  + The hitbox is a rectangle
///
/// \throws ERROR_NULL_POINTER
void jamEntitySnapY(JamEntity *entity, JamTileMap *tilemap, int direction);

/// \brief Calculates the visible x1 (top-left) of an entity
/// \param entity Entity to calculate for
/// \param x x Value to start from (usually just the entity's current x value)
/// \return The calculated value
///
/// \throws ERROR_NULL_POINTER
double jamEntityVisibleX1(JamEntity* entity, double x);

/// \brief Calculates the visible y1 (top-left) of an entity
/// \param entity Entity to calculate for
/// \param y y Value to start from (usually just the entity's current y value)
/// \return The calculated value
///
/// \throws ERROR_NULL_POINTER
double jamEntityVisibleY1(JamEntity* entity, double y);

/// \brief Calculates the visible x2 (bottom-right) of an entity
/// \param entity Entity to calculate for
/// \param x x Value to start from (usually just the entity's current x value)
/// \return The calculated value
///
/// \throws ERROR_NULL_POINTER
double jamEntityVisibleX2(JamEntity* entity, double x);

/// \brief Calculates the visible y2 (bottom-right) of an entity
/// \param entity Entity to calculate for
/// \param y y Value to start from (usually just the entity's current y value)
/// \return The calculated value
///
/// \throws ERROR_NULL_POINTER
double jamEntityVisibleY2(JamEntity* entity, double y);

/// \brief Frees an entity from memory
///
/// No matter what you specify with the boolean arguments,
/// the sprite's frames' textures will NOT be freed.
void jamEntityFree(JamEntity *entity, bool destroyHitbox, bool destroySprite, bool destroyFrames);

#ifdef __cplusplus
}
#endif

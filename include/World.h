/// \file World.h
/// \author lugi1
/// \brief Declares the world struct and functions surrounding it

#pragma once
#include "TileMap.h"
#include "Entity.h"
#include "EntityList.h"

/// \typedef A typedef that doesn't need to exist, but is just easy
typedef TileMap** TileMapList;

/// \brief A thing that holds lots of info for convenience
///
/// This data structure is a bit of a heavier one, so don't use twenty
/// at once. All entities in the world must be placed in worldEntities,
/// in fact, don't put anything anywhere yourself, let the functions do
/// it for you. Entities in the worldEntities list are freed with the
/// struct, and thus, all entities given to a world should be copies
/// and not an original entity from an asset loader. Tile maps are not
/// freed by this struct. All entity lists besides worldEntities are
/// volatile and for that reason you should never assign a value on
/// your own there. Feel free to loop them though.
typedef struct {
	// Core data
	TileMapList worldMaps;        ///< This is the struct that represents the collisions in this world
	EntityList* worldEntities;    ///< The full list of entities in this world

	// Higher-level abstractions - everything here is stored in core data
	EntityList* entityByRange[2];              ///< List of lists of entities that are in or out of range
	EntityList* entityTypes[MAX_ENTITY_TYPES]; ///< List of lists of entities by type, sorted by the world
	FilterType distanceFilteringType;          ///< Do we filter entities by ones on screen or by a given distance?

	// Information needed for filtering entities
	union {
		struct {
			uint16 inRangeRectangleWidth;  ///< For determining if an entity is in range or not
			uint16 inRangeRectangleHeight; ///< For determining if an entity is in range or not
		};
		uint16 inRangeRadius; ///< For determining if an entity is in range or not
	};
} World;

/// \brief Creates a world to work with
World* createWorld();

/// \brief Sets up a rectangular filter in a world
void setWorldFilterTypeRectangle(World* world, uint16 inRangeRectangleWidth, uint16 inRangeRectangleHeight);

/// \brief Sets up a circular filter in a world
void setWorldFilterTypeCircle(World* world, uint16 inRangeRadius);

/// \brief Adds an entity to the world
void worldAddEntity(World* world, Entity* entity);

/// \brief Adds a tilemap to the world
void worldAddTileMap(World* world, TileMap* tileMap);

/// \brief Removes an entity from the world
void worldRemoveEntity(World* world, uint64 entityID);

/// \brief Sorts a world's entities into lists filtered by distance
/// \warning This function is very heavy on the CPU
///
/// pointX and pointY mean different things depending on if the in range
/// thing is a rectangle or circle. If its a rectangle, pointX/Y are the
/// top-left of it; if its a circle, its the centre of it.
void filterEntitiesByProximity(World* world, int pointX, int pointY);

/// \brief Frees a world
/// \warning This will free all entities the world has, but not tile maps
void freeWorld(World* world);
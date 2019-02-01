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
/// This data structure is a bit of a heavier one, so do use with caution.
/// The two important pieces is worldMaps and worldEntities - every entity
/// or map used in this struct MUST be saved here because these are the
/// only two that get cleaned up. If you have big worlds with many screens
/// worth of entities, you can use filterEntitiesByProximity to immediately
/// filter all entities that are in/out of range into the entityByRange
/// array. Any entity within range is stored at index ENTITIES_IN_RANGE,
/// otherwise it is at index ENTITIES_OUT_OF_RANGE. This is incredibly
/// useful for only processing things that are on screen.
typedef struct {
	// Core data
	TileMapList worldMaps;    ///< This is the struct that represents the collisions in this world
	EntityList* worldEntities; ///< The full list of entities in this world

	// Higher-level abstractions - everything here is stored in core data
	EntityList* entityByRange[2];              ///< List of lists of entities that are in or out of range
	EntityList* entityTypes[MAX_ENTITY_TYPES]; ///< List of lists of entities by type, sorted by the world
	FilterType distanceFilteringType;         ///< Do we filter entities by ones on screen or by a given distance?

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

/// \brief Sorts a world's entities into lists filtered by distance
/// \warning This function is very heavy on the CPU
///
/// pointX and pointY mean different things depending on if the in range
/// thing is a rectangle or circle. If its a rectangle, pointX/Y are the
/// top-left of it; if its a circle, its the centre of it.
void filterEntitiesByProximity(World* world, int pointX, int pointY);

/// \brief Frees a world
///
/// This free function does not handle
/// any data inside the world, you must
/// free that yourself.
void freeWorld(World* world);
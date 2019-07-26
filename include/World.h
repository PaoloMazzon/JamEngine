/// \file World.h
/// \author lugi1
/// \brief Declares the world struct and functions surrounding it

#pragma once
#include "TileMap.h"
#include "Entity.h"
#include "EntityList.h"

/// \brief A thing that holds lots of info for convenience
///
/// This data structure is a bit of a heavier one, so use with caution
/// and really, you're better off limiting the number of these you use
/// to low single-digits. Worlds sort entities into several categories:
/// in-range, out-of-range, each different entity type, and a grand list
/// of every entity in the world. Keep in mind that every entity in the
/// world will show up in (exactly) three lists: in-range or out-of-range,
/// the list for its respective entity type, and the master list of all
/// entities. This means if you are going to process one each frame, it
/// is recommended you pick one list and just process that and use the
/// others for sorting/finding entities. In fact, the in-range/out-of-range
/// lists were more or less made for the actual processing so you only
/// need to process the ones a reasonable distance from the player and
/// can just ignore entities (or have some meta-game processor) way off
/// in the game world.
///
/// The list of TileMaps in this struct is just an array of size MAX_TILEMAPS
/// because typically you're not going to have many tile maps at once,
/// maybe just background and foreground + some other stuff, but really
/// the MAX_TILEMAPS is arbitrary. If you want more or less, change it. Just know
/// that this was a choice deliberately made because the overhead of
/// using malloc and pointers was much greater than a simple little array.
///
/// Sidenote, as a byproduct of how memory is managed in this engine, worlds
/// always free any entities inside them when it is time to be freed. This
/// means a few things
///
/// + Entity sprites/hitboxes will ***NOT*** be freed by the world
/// + It is safest to use copies of "base" entities so the world can go about freeing the copies
/// and you can concern yourself only with handling the base ones/the base ones' assets
/// + Always use copies when using an asset handler's entities (let the asset handler deal with its own memory)
/// + If you don't use copies of "base" entities, just make sure you cleanup the sprites and hitboxes
///
/// In general, the best practice is to use an AssetHandler and use copies
/// of entities in world, as it makes it memory cleanup very clear. AssetHandler
/// loads entities from files, you make copies of the asset handler's entities
/// and give them to world to play with, world frees all of the copies,
/// AssetHandler frees all of the base entities and its other resources.
typedef struct {
	// Core data
	TileMap* worldMaps[MAX_TILEMAPS]; ///< This is the struct that represents the collisions in this world
	EntityList* worldEntities; ///< The full list of entities in this world

	// Higher-level abstractions - everything here is stored in core data
	EntityList* entityByRange[2]; ///< List of lists of entities that are in or out of range
	EntityList* entityTypes[MAX_ENTITY_TYPES]; ///< List of lists of entities by type, sorted by the world
	FilterType distanceFilteringType; ///< Do we filter entities by ones on screen or by a given distance?

	// Information needed for filtering entities
	union {
		struct {
			uint16 inRangeRectangleWidth; ///< For determining if an entity is in range or not
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
///
/// By default, all entities are in range.
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
///
/// Please be aware that this will not free tile maps, only the entities
/// it holds and things that createWorld allocated itself. This is because
/// tile maps are typically handled by an asset handler where as entities
/// in a world should be copies of entities from an asset handler.
void freeWorld(World* world);
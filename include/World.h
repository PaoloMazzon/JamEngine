/// \file World.h
/// \author lugi1
/// \brief Declares the world struct and functions surrounding it

#pragma once
#include "TileMap.h"
#include "Entity.h"
#include "EntityList.h"
#include "Renderer.h"

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
/// Here is a quick rundown on how worlds manage memory (besides them obviously
/// cleaning up anything they themselves allocate like entity lists)
///
/// + Entity sprites/hitboxes will ***NOT*** be freed by the world
/// + Always use copies when using an asset handler's entities (let the asset handler deal with its own memory)
/// + If you don't use copies of "base" entities, just make sure you cleanup the sprites and hitboxes
/// + JamWorlds are in charge of cleaning up any tile maps they hold, but not the internal frames of said tile map (use a JamAssetHandler)
///
/// In general, the best practice is to use an AssetHandler and use copies
/// of entities in world, as it makes it memory cleanup very clear. AssetHandler
/// loads entities from files, you make copies of the asset handler's entities
/// and give them to world to play with, world frees all of the copies,
/// AssetHandler frees all of the base entities and its other resources.
///
/// Another important note is that doxygen is not a fan of anonymous unions/
/// structs and as such it is hard to tell that there is a union holding a struct
/// and uint16. The union holds two uint16's. These are inRangeRadius, inRangeRectangleWidth,
/// and inRangeRectangleHeight respectively.
///
/// \warning Never change a value in this struct yourself, always use the
/// funtions for it or you will very quickly be looking at lots of memory
/// leaks.
typedef struct _JamWorld {
	// Core data
	JamTileMap* worldMaps[MAX_TILEMAPS]; ///< This is the struct that represents the collisions in this world
	JamEntityList* worldEntities; ///< The full list of entities in this world

	// Higher-level abstractions - everything here is stored in core data
	JamEntityList* entityByRange[2]; ///< List of lists of entities that are in or out of range
	JamEntityList* entityTypes[MAX_ENTITY_TYPES]; ///< List of lists of entities by type, sorted by the world
	JamFilterType distanceFilteringType; ///< Do we filter entities by ones on screen or by a given distance?

	// Information needed for filtering entities
	union {
		struct {
			uint16 inRangeRectangleWidth; ///< For determining if an entity is in range or not
			uint16 inRangeRectangleHeight; ///< For determining if an entity is in range or not
		};
		uint16 inRangeRadius; ///< For determining if an entity is in range or not
	};
} JamWorld;

/// \brief Creates a world to work with
/// \throws ERROR_ALLOC_FAILED
JamWorld* jamCreateWorld();

/// \brief Sets up a rectangular filter in a world
/// \throws ERROR_NULL_POINTER
void jamSetWorldFilterTypeRectangle(JamWorld *world, uint16 inRangeRectangleWidth, uint16 inRangeRectangleHeight);

/// \brief Sets up a circular filter in a world
/// \throws ERROR_NULL_POINTER
void jamSetWorldFilterTypeCircle(JamWorld *world, uint16 inRangeRadius);

/// \brief Adds an entity to the world
///
/// By default, all entities are in range. Also, the entity's behaviour
/// pointer is resolved when it is added through this function
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_INCORRECT_FORMAT
void jamWorldAddEntity(JamWorld *world, JamEntity *entity);

/// \brief Processes and draws the entities in the world
///
/// This function will run all in-range entities onFrame function
/// and onDraw function (if applicable). That said, every onFrame
/// function will be ran before the first onDraw function is.
///
/// \throws ERROR_NULL_POINTER
void jamWorldProcFrame(JamWorld *world);

/// \brief Moves an entity from in range to out of range
///
/// This may seem useless since there is already a function that
/// does this automatically, but this can be useful if you have
/// a bunch of entities that need to be removed all the time and
/// you'd rather just stop processing the entity and just clean it
/// up later. This function does nothing if the entity is not in range.
///
/// \throws ERROR_NULL_POINTER
void jamWorldRotateEntity(JamWorld *world, JamEntity *entity);

/// \brief Removes an entity from the world
///
/// This function does indeed destroy the entity it removes
///
/// \warning This function is somewhat heavy due to the fact that
/// it will loop over at least 3 different entity lists trying to
/// find all references to the specific entity
/// \throws ERROR_NULL_POINTER
void jamWorldRemoveEntity(JamWorld *world, JamEntity *entity);

/// \brief Sorts a world's entities into lists filtered by distance
///
/// pointX and pointY mean different things depending on if the in range
/// thing is a rectangle or circle. If its a rectangle, pointX/Y are the
/// top-left of it; if its a circle, its the centre of it.
///
/// \warning This function is very heavy on the CPU
/// \throws ERROR_NULL_POINTER
void jamFilterEntitiesByProximity(JamWorld *world, int pointX, int pointY);

/// \brief Frees a world
///
/// Please be aware that this will not free tile maps, only the entities
/// it holds and things that createWorld allocated itself. This is because
/// tile maps are typically handled by an asset handler where as entities
/// in a world should be copies of entities from an asset handler.
void jamFreeWorld(JamWorld *world);
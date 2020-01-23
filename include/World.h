/// \file World.h
/// \author lugi1
/// \brief Declares the world struct and functions surrounding it

#pragma once
#include "TileMap.h"
#include "Entity.h"
#include "EntityList.h"
#include "Renderer.h"
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief A thing that holds lots of info for convenience
typedef struct _JamWorld {
	JamTileMap* worldMaps[MAX_TILEMAPS]; ///< Worlds can store tilemaps for convenience, its best if you use constants to denote their meaning and not [0] or whatever
	JamEntityList* worldEntities;        ///< The full list of entities in this world
	JamEntityList* inRangeCache;         ///< "In-range" entities that have been placed here via
	bool cacheInRangeEntities;           ///< Weather or not to use inRangeCache instead of the space map frame-to-frame
	pthread_mutex_t entityCacheMutex;    ///< The mutex for when the old cache gets replaced by the new cache
	int procDistance;                    ///< How many pixels outside the viewport to still process/draw entities

	/* Spatial hash maps (or organizing entities into a grid in layman's terms)
	 * For the uninitialized, this is a fairly simple concept to understand but
	 * a little technical in terms of the details. Instead of storing the world's
	 * entities in a list and drawing the list, we sort the entities into a spatial
	 * map which is essentially just storing entities by their position in the
	 * game world (Storing all entities in a list and using only that is still an
	 * option, and in some cases its more efficient - all entities are still stored
	 * in the master list for a couple reasons, but that isn't normally used for
	 * rendering/processing frames).
	 *
	 * As an important note, the entityGrid is always of size (in cells) `(gridWidth * gridHeight) + 1`,
	 * as the last cell is used to store each entity that is out of bounds. This is why
	 * you may see `(gridWidth * gridHeight) + 1` in various functions.
	 */
	JamEntityList** entityGrid; ///< List of entity lists, made 2D through trickery (I hate 2D arrays)
	int gridWidth;              ///< Width of the grid in cells
	int gridHeight;             ///< Height of the grid in cells
	int cellWidth;              ///< Width of any given cell in pixels
	int cellHeight;             ///< Height of any given cell in pixels
} JamWorld;

/// \brief Creates a world to work with
/// \throws ERROR_ALLOC_FAILED
JamWorld* jamWorldCreate(int gridWidth, int gridHeight, int cellWidth, int cellHeight);

/// \brief Adds an entity to the world
///
/// By default, all entities are in range. Also, the entity's behaviour
/// pointer is resolved when it is added through this function
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_INCORRECT_FORMAT
void jamWorldAddEntity(JamWorld *world, JamEntity *entity);

/// \brief Finds an entity in the world using its ID
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OUT_OF_BOUNDS
JamEntity* jamWorldFindEntity(JamWorld *world, int id);

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
/// \throws ERROR_NULL_POINTER
void jamWorldRotateEntity(JamWorld *world, int id);

/// \brief Removes an entity from the world
/// \throws ERROR_NULL_POINTER
void jamWorldRemoveEntity(JamWorld *world, int id);

/// \brief Caches in-range entities if that is enabled
/// \throws ERROR_NULL_POINTER
void jamWorldFilter(JamWorld *world, int pointX, int pointY);

/// \brief Frees a world
///
/// Please be aware that this will not free tile maps, only the entities
/// it holds and things that createWorld allocated itself. This is because
/// tile maps are typically handled by an asset handler where as entities
/// in a world should be copies of entities from an asset handler.
void jamWorldFree(JamWorld *world);

#ifdef __cplusplus
}
#endif

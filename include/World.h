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
	JamEntityList* worldEntities;     ///< The full list of entities in this world
	JamEntityList* inRangeCache;      ///< "In-range" entities that have been placed here via
	bool cacheInRangeEntities;        ///< Weather or not to use inRangeCache instead of the space map frame-to-frame
	pthread_mutex_t entityCacheMutex; ///< The mutex for when the old cache gets replaced by the new cache
	pthread_mutex_t entityAddingLock; ///< The lock that prevents entities from being added while caching is occurring
	pthread_mutex_t cacheBoolLock;    ///< The lock responsible for preventing entities from being removed from cache while updating
	int procDistance;                 ///< How many pixels outside the viewport to still process/draw entities
	pthread_t cacheBuilderThread;     ///< Thread responsible for filtering the entities into the cache

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
/// \param gridWidth Width of the spatial map in cells
/// \param gridHeight Height of the spatial map in cells
/// \param cellWidth Width of each cell in pixels
/// \param cellHeight Height of each cell in pixels
/// \param cache Weather or not to cache in-range entities in another thread
///
/// \throws ERROR_ALLOC_FAILED
JamWorld* jamWorldCreate(int gridWidth, int gridHeight, int cellWidth, int cellHeight, bool cache);

/// \brief Finds any collisions between ent and other entities in the world
///
/// This function is meant to be looped until NULL is returned, if you don't
/// do this it is not guaranteed to work after the first time. For example,
/// you might use this to damage all the enemies currently touching a sword
/// by
///
/// 	JamEntity* ent = jamWorldEntityCollision(world, entity, x, y);
/// 	while (ent != NULL) {
/// 		ent->hp -= 10;
/// 		ent = jamWorldEntityCollision(world, entity, x, y);
///		}
///
///
/// \throws ERROR_NULL_POINTER
JamEntity* jamWorldEntityCollision(JamWorld* world, JamEntity* ent, double x, double y);

/// \brief Enables entity caching for a given world if it isn't already enabled
///
/// Assuming this function works, it will immediately halt the program to
/// build the in-range cache, then after that you must call the function
/// manually to do it (but of course jamWorldFilter filters in another
/// thread unlike this function).
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ALLOC_FAILED
void jamWorldEnableCaching(JamWorld* world);

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

/// \brief Finds an entity's id from the entity's type (if there are multiple, the first one found is returned)
/// \throws ERROR_NULL_POINTER
int jamWorldFindEntityType(JamWorld* world, uint32 type);

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

/// \brief Lets the world know that an entity is to be destroyed
void jamWorldDestroyEntity(JamEntity* entity);

/// \brief Caches in-range entities if that is enabled
///
/// To be a bit more technical, this function starts the caching
/// process in another thread, and once that thread has successfully
/// finished filtering entities into a new cache, it will quickly
/// swap out the old cache with the new one and free the old one.
/// For all intents and purposes, this bit doesn't matter, but it
/// is important to note that this is not instantaneous and the new
/// cache is not guaranteed to be fully processed until after this
/// function is called a second time; although it will likely be
/// done filtering quite quickly.
///
/// \throws ERROR_NULL_POINTER
void jamWorldFilter(JamWorld *world);

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

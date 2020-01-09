/// \file SpatialMap.h
/// \author plo
/// \brief Spatial hash maps are for making large amounts of entities quicker to process

#pragma once
#include "Entity.h"
#include "EntityList.h"
#include "Constants.h"

/// \brief Spatial hash map for JamEntitys
///
/// This will sort entities into a grid for easy lookup when it
/// comes time to process and draw entities. You are best setting
/// the cell width and height around double the average entity
/// size, but play around with it to get the best performance.
/// Any entity that would be added outside of the grid is instead
/// added the extraneous list at the end of entityGrid which is
/// known as the out-of-range list.
///
/// You are allowed to just set the width and height to 0 if your
/// game would be better suited to storing entities in a list than
/// a spatial hash map (for example, a space game where there are
/// few entities but they are quite far apart which would consume
/// a great deal of memory).
typedef struct {
	JamEntityList** entityGrid; ///< List of entity lists, made 2D through trickery (I hate 2D arrays)
	uint32 gridWidth;           ///< Width of the grid in cells
	uint32 gridHeight;          ///< Height of the grid in cells
	int cellWidth;              ///< Width of any given cell in pixels
	int cellHeight;             ///< Height of any given cell in pixels
	int xInWorld;               ///< X position in the game world of the map
	int yInWorld;               ///< Y position in the game world of the map
} JamSpatialMap;

/// \brief Creates a new spatial map
/// \param gridWidth Width in cells of the map
/// \param gridHeight Height in cells of the map
/// \param cellWidth Width in pixels of each cell
/// \param cellHeight Height in pixels of each cell
/// \return The new map or `NULL` if an error occurs
/// \throws ERROR_ALLOC_FAILED
JamSpatialMap* jamSpatialMapCreate(uint32 gridWidth, uint32 gridHeight, uint32 cellWidth, uint32 cellHeight);

/// \brief Adds an entity to a spatial map
/// \param spaceMap Spatial map to add an entity to
/// \param entity Entity to add the the spatial map
///
/// This will place the entity into the correct grid position automatically
///
/// \throws ERROR_NULL_POINTER
void jamSpatialMapAdd(JamSpatialMap* spaceMap, JamEntity* entity);

/// \brief Frees a spatial map, but it does not touch the entities within it
/// \param spaceMap Spatial map to be freed
void jamSpatialMapFree(JamSpatialMap* spaceMap);
/// \file SpatialMap.h
/// \author plo
/// \brief Spatial hash maps are for making large amounts of entities quicker to process

#pragma once
#include "Entity.h"
#include "Constants.h"

/// \brief Spatial hash map for JamEntitys
typedef struct {
	Entity** entityGrid; ///< List of entity pointers, made 2D through some trickery
	uint32 gridWidth;    ///< Width of the grid in cells
	uint32 gridHeight;   ///< Height of the grid in cells
	int cellWidth;       ///< Width of any given cell in pixels
	int cellHeight;      ///< Height of any given cell in pixels
} JamSpatialMap;

/// \brief Creates a new spatial map
JamSpatialMap* jamSpatialMapCreate(uint32 gridWidth, uint32 gridHeight, uint32 cellWidth, uint32 cellHeight);
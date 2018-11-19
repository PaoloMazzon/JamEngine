//
// Created by lugi1 on 2018-08-07.
//

#pragma once

#include "Constants.h"

/// \brief Holds information vital to a fast tile-grid
///
/// The grid is stored in memory as a 1-D array that
/// looks like this (using coordinates):
/// [(0, 0), (1, 0), (2, 0), (3, 0), (0, 1), (1, 1), (2, 1), (3, 1), (0, 2)]
/// If loading a tile map from a file, it might be best
/// just to access the grid directly instead of doing
/// 100 different function calls that each have a bit
/// of overhead in the line of error-checking.
typedef struct {
	uint32 width;      ///< Grid's width
	uint32 height;     ///< Grid's height
	uint32 cellWidth;  ///< Width of any given cell in the map
	uint32 cellHeight; ///< Height of any given cell in the map
	bool* grid;       ///< The internal grid of size w * h
} TileMap;

/// \brief Creates a tile map
///
/// Tile maps are initialized with false as every value
TileMap* createTileMap(uint32 width, uint32 height, uint32 cellWidth, uint32 cellHeight);

/// \brief Loads a tile map from file
///
/// Tile maps loaded from files accept "*" as a true value
/// all else as false
TileMap* loadTileMap(const char* filename, uint32 width, uint32 height, uint32 cellWidth, uint32 cellHeight);

/// \brief Sets a position in a tile map
/// Returns true if it worked
bool setMapPos(TileMap* tileMap, uint32 x, uint32 y, bool val);

/// \brief Gets a position in a tile map
bool getMapPos(TileMap* tileMap, uint32 x, uint32 y);

/// \brief Checks For a collision
///
/// This check is only reliable if the rectangle who's
/// collision we're checking has a width and height less
/// than or equal to the cell width and height of the
/// grid. If this is not the case, you must do a full
/// check as the fast one will not find all.
bool checkMapCollFast(TileMap* tileMap, int x, int y, int w, int h);

// TODO: Make full collision check function

/// \brief Frees a tile map from memory
void freeTileMap(TileMap* tileMap);
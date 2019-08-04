/// \file TileMap.h
/// \author lugi1
/// \brief This file declares the tile map struct and
/// various functions to manage it.
///
/// The quickest way to do collisions is via tile map
/// since it is just a 2D grid. Drawing.h can make auto
/// -tiles from one of these as well.
#pragma once

#include "Constants.h"
#include "Sprite.h"

/// \brief A simple struct that makes collision checking and tile graphics easier
///
/// Internally, this struct is just a width * height 16-bit grid. You don't need
/// to use the graphical part of it, but the grid is 16-bit to allow for lots of
/// graphical tiles and you only need a 1-bit grid (which, yes is not possible
/// with base datatypes but you can get tricky with bool arrays) for that situation.
/// The collisionRangeStart/End variables define what portion of the tile sheet
/// is dedicated to solid objects so the collision detection works properly.
typedef struct {
	uint32 width;               ///< Grid's width
	uint32 height;              ///< Grid's height
	uint32 cellWidth;           ///< Width of any given cell in the map
	uint32 cellHeight;          ///< Height of any given cell in the map
	uint16* grid;               ///< The internal grid of size w * h
	uint16 collisionRangeStart; ///< The start of the range of collide-able tiles
	uint16 collisionRangeEnd;   ///< The end of the range of collide-able tiles
	Sprite* tileSheet;          ///< The sprite sheet holding all tiles
} TileMap;

/// \brief Creates a tile map
///
/// Tile maps are initialized with false as every value
///
/// \throws ERROR_ALLOC_FAILED
TileMap* createTileMap(uint32 width, uint32 height, uint32 cellWidth, uint32 cellHeight);

/// \brief Loads a tile map from file
///
/// Tile maps loaded from files accept "*" as a true value
/// all else as false. Should just be a plain text file. For
/// example, a 4 * 4 grid might look like
/// 
/// 	..*.
/// 	*..*
/// 	*..*
/// 	****
///
/// \throws ERROR_FILE_FAILED
/// \throws ERROR_OPEN_FAILED
/// \throws ERROR_ALLOC_FAILED
TileMap* loadTileMap(const char* filename, uint32 width, uint32 height, uint32 cellWidth, uint32 cellHeight);

/// \brief Sets a position in a tile map
/// Returns true if it worked
///
/// \throws ERROR_NULL_POINTER
bool setMapPos(TileMap* tileMap, uint32 x, uint32 y, uint16 val);

/// \brief Gets a position in a tile map
/// \throws ERROR_NULL_POINTER
uint16 getMapPos(TileMap* tileMap, uint32 x, uint32 y);

/// \brief Checks For a collision
///
/// This check is only reliable if the rectangle who's
/// collision we're checking has a width and height less
/// than or equal to the cell width and height of the
/// grid. If this is not the case, you must do a full
/// check as the fast one will not find all.
///
/// \throws ERROR_NULL_POINTER
bool checkMapCollFast(TileMap* tileMap, double x, double y, double w, double h);


/// \brief Frees a tile map from memory
void freeTileMap(TileMap* tileMap);
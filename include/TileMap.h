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
/// Internally, this struct is just a width * height grid of JamFrame pointers.
/// Any NULL pointer is considered not a collision, and anything not NULL is
/// considered a collision. Also, the tile map will not free any of the frames
/// it holds because it is meant to be used with a JamAssetHandler (so basically
/// if you use it without it just free the frames yourself).
///
/// If you wish to use this simply as a collision grid and not to draw things with,
/// you can technically just set the pointers to something, but its dangerous.
/// Instead, it would be wiser to create an empty frame struct and assign that
/// just in case you later try to access it and get a segfault.
///
/// Although you can set any grid spot to any arbitrary frame (including NULL
/// pointers), you are best off making sure each frame is exactly cellWidth width
/// and cellHeight height. If frames in the grid do not match the expected cellWidth
/// and cellHeight, you will more likely than not get really wonky collisions and
/// strange rendering.
typedef struct {
	int xInWorld;               ///< X position in the world of the grid (for collisions only, drawing ignores this)
	int yInWorld;               ///< Y position in the world of the grid (for collisions only, drawing ignores this)
	uint32 width;               ///< Grid's width
	uint32 height;              ///< Grid's height
	uint32 cellWidth;           ///< Width of any given cell in the map
	uint32 cellHeight;          ///< Height of any given cell in the map
	JamFrame** grid;            ///< Internal grid of w*h (it is a 1D array of JamFrame pointers)
} JamTileMap;

/// \brief Creates a tile map
///
/// Tile maps are initialized with false as every value
///
/// \throws ERROR_ALLOC_FAILED
JamTileMap* jamCreateTileMap(uint32 width, uint32 height, uint32 cellWidth, uint32 cellHeight);

/// \brief Sets a position in a tile map
/// Returns true if it worked
///
/// \throws ERROR_NULL_POINTER
bool jamSetMapPos(JamTileMap *tileMap, uint32 x, uint32 y, JamFrame* val);

/// \brief Gets a position in a tile map
/// \throws ERROR_NULL_POINTER
JamFrame* jamGetMapPos(JamTileMap *tileMap, uint32 x, uint32 y);

/// \brief Auto-tiles a tile map using a 48-frame sprite
///
/// This function takes a 48-frame spritesheet and automatically places
/// the frame in their proper spot in the map; it automatically sorts the
/// tiles so that they all look in place. It is incredibly useful for quick
/// mock-up projects and things like roguelikes that randomly generate their
/// levels.
///
/// This function only checks if a map spot is not NULL to check for a spot,
/// it does not care about anything past that. This means it is safe
/// to use a dummy frame to denote a tile, or even just random data since
/// this function will never access data inside the pointers in the grid.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_INCORRECT_FORMAT
void jamAutoTileMap(JamTileMap *map, JamSprite *spr);

/// \brief Checks For a collision
///
/// This check is only reliable if the rectangle who's
/// collision we're checking has a width and height less
/// than or equal to the cell width and height of the
/// grid. If this is not the case, you must do a full
/// check as the fast one will not find all.
///
/// \throws ERROR_NULL_POINTER
bool jamCheckMapCollFast(JamTileMap *tileMap, int x, int y, int w, int h);

/// \brief Checks for a collision in a tile map
///
/// This collision check takes more memory and cycles to
/// perform than checkMapCollFast but can handle any rectangle
/// whereas the aforementioned can only properly check for
/// rectangles that are smaller/equal to the size of the
/// tile map's cells.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OUT_OF_BOUNDS
bool jamCheckMapCollision(JamTileMap *tileMap, int x, int y, int w, int h);

/// \brief Frees a tile map from memory
void jamFreeTileMap(JamTileMap *tileMap);
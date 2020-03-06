//
// Created by lugi1 on 2018-08-07.
//

#include "TileMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <TileMap.h>
#include "JamError.h"
#include <math.h>
#include <Sprite.h>

//////////////////////////////////////////////////////////
JamTileMap* jamTileMapCreate(uint32 width, uint32 height, uint32 cellWidth, uint32 cellHeight) {
	JamTileMap* map = (JamTileMap*)malloc(sizeof(JamTileMap));

	// Check it worked
	if (map != NULL) {
		// Make the internal map
		map->grid = (JamFrame**)calloc(sizeof(JamFrame*), width * height);

		// Check this one as well
		if (map->grid != NULL) {
			// Load up the values
			map->width = width;
			map->height = height;
			map->cellWidth = cellWidth;
			map->cellHeight = cellHeight;
			map->xInWorld = 0;
			map->yInWorld = 0;
		} else {
			free(map);
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate tile map's grid.");
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate tile map.");
	}

	return map;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool jamTileMapSet(JamTileMap *tileMap, uint32 x, uint32 y, JamFrame *val) {
	bool worked = false;

	// Make sure the map is here
	if (tileMap != NULL && tileMap->grid != NULL) {
		// Are we inside the grid
		if (x >= 0 && x < tileMap->width && y >= 0 && y < tileMap->height) {
			// It did work
			worked = true;

			// Now set the value
			tileMap->grid[y * tileMap->width + x] = val;
		}
	} else {
		if (tileMap != NULL)
			jSetError(ERROR_NULL_POINTER, "Map does not exist (jamTileMapSet)");
		else
			jSetError(ERROR_NULL_POINTER, "Map grid does not exist (jamTileMapSet)");
	}

	return worked;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
JamFrame* jamTileMapGet(JamTileMap *tileMap, uint32 x, uint32 y) {
	JamFrame* val = 0;

	// Make sure the map is here
	if (tileMap != NULL && tileMap->grid != NULL) {
		// Now get the value if it is a real part of the grid
		if (x >= 0 && x < tileMap->width && y >= 0 && y < tileMap->height)
			val = tileMap->grid[y * tileMap->width + x];
	} else {
		if (tileMap != NULL)
			jSetError(ERROR_NULL_POINTER, "Map does not exist");
		else
			jSetError(ERROR_NULL_POINTER, "Map grid does not exist");
	}

	return val;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool jamTileMapFastCollision(JamTileMap *tileMap, int x, int y, int w, int h) {
	bool coll = false;
	int x1, y1, x2, y2;

	// Make sure the map is here
	if (tileMap != NULL && tileMap->grid != NULL) {
		// First get all the values
		x1 = (x / tileMap->cellWidth) - tileMap->xInWorld;
		y1 = (y / tileMap->cellHeight) - tileMap->yInWorld;
		x2 = ((x + w - 1) / tileMap->cellWidth) - tileMap->xInWorld;
		y2 = ((y + h - 1) / tileMap->cellHeight) - tileMap->yInWorld;

		// Now check for a collision by just checking each corner
		if (x1 >= 0 && x1 < tileMap->width && y1 >= 0 && y1 < tileMap->height
			&& x2 >= 0 && x2 < tileMap->width && y2 >= 0 && y2 < tileMap->height)
			coll = ((tileMap->grid[y1 * tileMap->width + x1] != NULL) ||
					(tileMap->grid[y1 * tileMap->width + x2] != NULL) ||
					(tileMap->grid[y2 * tileMap->width + x1] != NULL) ||
					(tileMap->grid[y2 * tileMap->width + x2] != NULL));
	} else {
		if (tileMap != NULL)
			jSetError(ERROR_NULL_POINTER, "Map does not exist (jamTileMapFastCollision)");
		else
			jSetError(ERROR_NULL_POINTER, "Map grid does not exist (jamTileMapFastCollision)");
	}

	return coll;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool jamTileMapCollision(JamTileMap *tileMap, int x, int y, int w, int h) {
	// I have no idea why this is needed but it is so whatever
	w--;
	h--;

	bool coll = false;
	int i, j, xInRect, yInRect, xFactor, yFactor;
	xInRect = x;
	yInRect = y;
	int verticesX[MAX_GRID_VERTICES];
	int verticesY[MAX_GRID_VERTICES];
	int currentVertex = 0;

	if (tileMap != NULL && tileMap->grid != NULL) {
		xFactor = (int)ceil((double)w / (double)tileMap->cellWidth) + 1;
		yFactor = (int)ceil((double)h / (double)tileMap->cellHeight) + 1;

		// We won't try collisions if this rectangle requires too many vertices
		if (xFactor * yFactor < MAX_GRID_VERTICES) {
			for (i = 0; i < yFactor; i++) {
				for (j = 0; j < xFactor; j++) {
					// Set the current vertex
					verticesX[currentVertex] = (xInRect / tileMap->cellWidth) - tileMap->xInWorld;
					verticesY[currentVertex] = (yInRect / tileMap->cellHeight) - tileMap->yInWorld;
					currentVertex++;

					// Calculate where to go from here
					if (xInRect + tileMap->cellWidth > x + w)
						xInRect = x + w;
					else
						xInRect += tileMap->cellWidth;
				}
				// Calculate where to go from here
				xInRect = x;
				if (yInRect + tileMap->cellHeight > y + h)
					yInRect = y + h;
				else
					yInRect += tileMap->cellHeight;
			}

			// Actually check those vertices for a collision
			for (i = 0; i < currentVertex && !coll; i++) {
				// Make sure it's in bounds
				if (verticesX[i] >= 0 && verticesX[i] < tileMap->width && verticesY[i] >= 0 && verticesY[i] < tileMap->height)
					coll = tileMap->grid[verticesY[i] * tileMap->width + verticesX[i]] != NULL;
			}
		} else {
			jSetError(ERROR_OUT_OF_BOUNDS, "Rectangle  [%i, %i, %i, %i] requires too many vertices (jamTileMapCollision)", x, y, w, h);
		}

	} else {
		if (tileMap != NULL)
			jSetError(ERROR_NULL_POINTER, "Map does not exist (jamTileMapCollision)");
		else
			jSetError(ERROR_NULL_POINTER, "Map grid does not exist (jamTileMapCollision)");
	}

	return coll;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamTileMapAuto(JamTileMap *map, JamSprite *spr) {
	uint32 i, j;
	bool n, e, s, w, ne, nw, se, sw;
	int frame;

	// First confirm the things exist
	if (spr != NULL && map != NULL && (spr->animationLength == 48 || spr->animationLength == 47)) {

		// We gotta run through every cell
		for (i = 0; i < map->height; i++) {
			for (j = 0; j < map->width; j++) {
				if (jamTileMapGet(map, j, i) != NULL) {
					// Where there are adjacent tiles
					w = (jamTileMapGet(map, j - 1, i) != NULL);
					e = (jamTileMapGet(map, j + 1, i) != NULL);
					n = (jamTileMapGet(map, j, i - 1) != NULL);
					s = (jamTileMapGet(map, j, i + 1) != NULL);
					ne = (jamTileMapGet(map, j + 1, i - 1) != NULL);
					nw = (jamTileMapGet(map, j - 1, i - 1) != NULL);
					se = (jamTileMapGet(map, j + 1, i + 1) != NULL);
					sw = (jamTileMapGet(map, j - 1, i + 1) != NULL);

					// Calculate the tile
					if (!w && !e && !s && n/* && !nw && !ne && !se && !sw*/) {
						frame = 0;
					} else if (!w && e && !s && !n/* && !nw && !ne && !se && !sw*/) {
						frame = 1;
					} else if (!w && !e && s && !n/* && !nw && !ne && !se && !sw*/) {
						frame = 2;
					} else if (w && !e && !s && !n/* && !nw && !ne && !se && !sw*/) {
						frame = 3;
					} else if (!w && e && !s && n && !ne) {
						frame = 4;
					} else if (!w && e && s && !n && !se) {
						frame = 5;
					} else if (w && !e && s && !n && !sw) {
						frame = 6;
					} else if (w && !e && !s && n && !nw) {
						frame = 7;
					} else if (!w && e && !s && n && ne && !sw) {
						frame = 8;
					} else if (!w && e && s && !n && se) {//(!w && e && s && !n && !nw && !ne && se && !sw) {
						frame = 9;
					} else if (w && !e && s && !n && sw) {
						frame = 10;
					} else if (w && !e && !s && n && nw) {
						frame = 11;
					} else if (!w && e && s && n && !ne && !se) {
						frame = 12;
					} else if (w && e && s && !n && !se && !sw) {
						frame = 13;
					} else if (w && !e && s && n && !nw && !sw) {
						frame = 14;
					} else if (w && e && !s && n && !nw && !ne) {
						frame = 15;
					} else if (!w && e && s && n && ne && !se) {
						frame = 16;
					} else if (!w && e && s && n && !ne && se) {
						frame = 17;
					} else if (!w && e && s && n && ne && se) {
						frame = 18;
					} else if (w && e && s && !n && se && !sw) {
						frame = 19;
					} else if (w && e && s && !n && !nw && !se) {
						frame = 20;
					} else if (w && e && s && !n && se && sw) {
						frame = 21;
					} else if (w && !e && s && n && !nw && sw) {
						frame = 22;
					} else if (w && !e && s && n && nw && !sw) {
						frame = 23;
					} else if (w && !e && s && n && nw && sw) {
						frame = 24;
					} else if (w && e && !s && n && !nw && ne) {
						frame = 25;
					} else if (w && e && !s && n && nw && !ne) {
						frame = 26;
					} else if (w && e && !s && n && nw && ne) {
						frame = 27;
					} else if (w && e && s && n && !nw && !ne && !se && !sw) {
						frame = 28;
					} else if (w && e && s && n && !nw && ne && !se && !sw) {
						frame = 29;
					} else if (w && e && s && n && !nw && !ne && se && !sw) {
						frame = 30;
					} else if (w && e && s && n && !nw && !ne && !se && sw) {
						frame = 31;
					} else if (w && e && s && n && nw && !ne && !se && !sw) {
						frame = 32;
					} else if (w && e && s && n && !nw && ne && se && !sw) {
						frame = 33;
					} else if (w && e && s && n && !nw && !ne && se && sw) {
						frame = 34;
					} else if (w && e && s && n && nw && !ne && !se && sw) {
						frame = 35;
					} else if (w && e && s && n && nw && ne && !se && !sw) {
						frame = 36;
					} else if (w && e && s && n && !nw && ne && !se && sw) {
						frame = 37;
					} else if (w && e && s && n && nw && !ne && se && !sw) {
						frame = 38;
					} else if (w && e && s && n && !nw && ne && se && sw) {
						frame = 39;
					} else if (w && e && s && n && nw && ne && se && !sw) {
						frame = 40;
					} else if (w && e && s && n && nw && ne && !se && sw) {
						frame = 41;
					} else if (w && e && s && n && nw && !ne && se && sw) {
						frame = 42;
					} else if (w && e && s && n && nw && ne && se && sw) {
						frame = 43;
					} else if (!w && !e && s && n/* && !nw && !ne && !se && !sw*/) {
						frame = 45;
					} else if (w && e && !s && !n/* && !nw && !ne && !se && !sw*/) {
						frame = 46;
					} else { // No blocks around
						frame = 44;
					}

					jamTileMapSet(map, j, i, spr->frames[frame]);
				}
			}
		}
	} else {
		if (map == NULL)
			jSetError(ERROR_NULL_POINTER, "Map doesn't exist (jamDrawSortedMap)");
		if (spr == NULL)
			jSetError(ERROR_NULL_POINTER, "JamSprite doesn't exist (jamDrawSortedMap)");
		else if (!(spr->animationLength == 48 || spr->animationLength == 47))
			jSetError(ERROR_INCORRECT_FORMAT, "JamSprite does not contain 48 frames(jamDrawSortedMap)");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void jamTileMapFree(JamTileMap *tileMap) {
	if (tileMap != NULL) {
		free(tileMap->grid);
		free(tileMap);
	}
}
//////////////////////////////////////////////////////////
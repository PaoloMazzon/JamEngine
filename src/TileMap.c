//
// Created by lugi1 on 2018-08-07.
//

#include "TileMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <TileMap.h>
#include "JamError.h"
#include <math.h>

//////////////////////////////////////////////////////////
TileMap* createTileMap(uint32 width, uint32 height, uint32 cellWidth, uint32 cellHeight) {
	TileMap* map = (TileMap*)malloc(sizeof(TileMap));

	// Check it worked
	if (map != NULL) {
		// Make the internal map
		map->grid = (uint16*)calloc(2, width * height);

		// Check this one as well
		if (map->grid != NULL) {
			// Load up the values
			map->width = width;
			map->height = height;
			map->cellWidth = cellWidth;
			map->cellHeight = cellHeight;
			map->collisionRangeStart = 1;
			map->collisionRangeEnd = 1;
			map->xInWorld = 0;
			map->yInWorld = 0;
		} else {
			free(map);
			fprintf(stderr, "Failed to allocate tile map's grid.\n");
			jSetError(ERROR_ALLOC_FAILED);
		}
	} else {
		fprintf(stderr, "Failed to allocate tile map.\n");
		jSetError(ERROR_ALLOC_FAILED);
	}

	return map;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
TileMap* loadTileMap(const char* filename, uint32 width, uint32 height, uint32 cellWidth, uint32 cellHeight) {
	// Open the file and create the line for temporary storage and create the tile map
	uint32 x, y;
	FILE* file = fopen(filename, "r");
	TileMap* map = createTileMap(width, height, cellWidth, cellHeight);

	// Check it worked
	if (map != NULL && file != NULL) {
		// Load the map information from the file
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				if (fgetc(file) == '*')
					setMapPos(map, x, y, true);
			}
			if (fgetc(file) == '\r')
				fgetc(file);
		}

		// Check if the file screwed up somehow
		if (ferror(file) != 0) {
			fprintf(stderr, "Could not load tile map from file (loadTileMap).\n");
			jSetError(ERROR_FILE_FAILED);
			freeTileMap(map);
		}
	} else {
		if (map == NULL) {
			fprintf(stderr, "Map could not be allocated (loadTileMap).\n");
			jSetError(ERROR_ALLOC_FAILED);
		}
		if (file == NULL) {
			fprintf(stderr, "File could not be opened (loadTileMap).\n");
			jSetError(ERROR_OPEN_FAILED);
		}
		freeTileMap(map);
	}

	fclose(file);

	return map;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool setMapPos(TileMap* tileMap, uint32 x, uint32 y, uint16 val) {
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
			fprintf(stderr, "Map does not exist (setMapPos).\n");
		else
			fprintf(stderr, "Map grid does not exist (setMapPos).\n");
		jSetError(ERROR_NULL_POINTER);
	}

	return worked;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
uint16 getMapPos(TileMap* tileMap, uint32 x, uint32 y) {
	uint16 val = 0;

	// Make sure the map is here
	if (tileMap != NULL && tileMap->grid != NULL) {
		// Now get the value if it is a real part of the grid
		if (x >= 0 && x < tileMap->width && y >= 0 && y < tileMap->height)
			val = tileMap->grid[y * tileMap->width + x];
	} else {
		if (tileMap != NULL)
			fprintf(stderr, "Map does not exist (getMapPos).\n");
		else
			fprintf(stderr, "Map grid does not exist (getMapPos).\n");
		jSetError(ERROR_NULL_POINTER);
	}

	return val;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool checkMapCollFast(TileMap* tileMap, int x, int y, int w, int h) {
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
			coll = ((tileMap->grid[y1 * tileMap->width + x1] >= tileMap->collisionRangeStart && tileMap->grid[y1 * tileMap->width + x1] <= tileMap->collisionRangeEnd) ||
					(tileMap->grid[y1 * tileMap->width + x2] >= tileMap->collisionRangeStart && tileMap->grid[y1 * tileMap->width + x2] <= tileMap->collisionRangeEnd) ||
					(tileMap->grid[y2 * tileMap->width + x1] >= tileMap->collisionRangeStart && tileMap->grid[y2 * tileMap->width + x1] <= tileMap->collisionRangeEnd) ||
					(tileMap->grid[y2 * tileMap->width + x2] >= tileMap->collisionRangeStart && tileMap->grid[y2 * tileMap->width + x2] <= tileMap->collisionRangeEnd));
	} else {
		if (tileMap != NULL)
			fprintf(stderr, "Map does not exist (checkMapCollFast).\n");
		else
			fprintf(stderr, "Map grid does not exist (checkMapCollFast).\n");
		jSetError(ERROR_NULL_POINTER);
	}

	return coll;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
bool checkMapCollision(TileMap* tileMap, int x, int y, int w, int h) {
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

	// Make sure the map is here
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
					coll = tileMap->grid[verticesY[i] * tileMap->width + verticesX[i]] >= tileMap->collisionRangeStart && tileMap->grid[verticesY[i] * tileMap->width + verticesX[i]] <= tileMap->collisionRangeEnd;
			}
		} else {
			fprintf(stderr, "Rectangle  [%i, %i, %i, %i] requires too many vertices (checkMapCollision)\n", x, y, w, h);
			jSetError(ERROR_OUT_OF_BOUNDS);
		}

	} else {
		if (tileMap != NULL)
			fprintf(stderr, "Map does not exist (checkMapCollision).\n");
		else
			fprintf(stderr, "Map grid does not exist (checkMapCollision).\n");
		jSetError(ERROR_NULL_POINTER);
	}

	return coll;
}
//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
void freeTileMap(TileMap* tileMap) {
	if (tileMap != NULL) {
		free(tileMap->grid);
		free(tileMap);
	}
}
//////////////////////////////////////////////////////////
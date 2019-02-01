//
// Created by lugi1 on 2018-08-07.
//

#include "TileMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <TileMap.h>

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
		} else {
			free(map);
			fprintf(stderr, "Failed to allocate tile map's grid.\n");
		}
	} else {
		fprintf(stderr, "Failed to allocate tile map.\n");
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
			freeTileMap(map);
		}
	} else {
		if (map == NULL)
			fprintf(stderr, "Map could not be allocated (loadTileMap).\n");
		if (file == NULL)
			fprintf(stderr, "File could not be opened (loadTileMap).\n");
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
		x1 = x / tileMap->cellWidth;
		y1 = y / tileMap->cellHeight;
		x2 = (x + w) / tileMap->cellWidth;
		y2 = (y + h) / tileMap->cellHeight;

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
#include <stdio.h>
#include <stdlib.h>
#include <TileMap.h>
#include <Sprite.h>
#include <Frame.h>
#include "JamEngine.h"
#include "TMXWorldLoader.h"
#include "JamError.h"
#include "tmx.h"

JamAsset* createAsset(void*, enum JamAssetType);

///////////////////////////////////////////////////////////////////////////////
char* genRandomString() {
	char* output = malloc(10);
	int i;

	if (output != NULL) {
		for (i = 0; i < 9; i++)
			output[i] = (char)(65 + (rand() % (26 * 2)));
		output[9] = 0;
	}

	return output;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This function is meant for jamLoadWorldFromTMX and is not safe to call
bool loadObjectLayerIntoWorld(JamAssetHandler* handler, JamWorld* world, tmx_layer* layer) {
	JamEntity* tempEntity;
	tmx_object* currentObject = layer->content.objgr->head;
	bool failedToLoad = false;

	// Loop the linked list
	while (currentObject != NULL) {
		tempEntity = jamCopyEntity(jamGetEntityFromHandler(handler, currentObject->type), currentObject->x,
								   currentObject->y);

		if (tempEntity != NULL) {
			jamWorldAddEntity(world, tempEntity);

			// Adjust scale
			if (tempEntity->sprite != NULL && tempEntity->sprite->animationLength > 0) {
				tempEntity->scaleX = (float)currentObject->width  / tempEntity->sprite->frames[0]->w;
				tempEntity->scaleY = (float)currentObject->height / tempEntity->sprite->frames[0]->h;
			}

			// This is to account for the difference in sprite origins between JamEngine and Tiled
			tempEntity->y -= currentObject->height;

			// Load properties of the et_Object into the new entity
			if (currentObject->visible)
				tempEntity->alpha = 255;
			else
				tempEntity->alpha = 0;
			tempEntity->rot = currentObject->rotation;
		} else {
			failedToLoad = true;
			fprintf(stderr, "Failed to create entity ID %i (loadObjectLayerIntoWorld)\n", currentObject->id);
		}
		
		currentObject = currentObject->next;
	}

	return !failedToLoad;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This function is meant for jamLoadWorldFromTMX and is not safe to call
JamTileMap* createTileMapFromTMXLayer(JamAssetHandler* handler, tmx_layer* layer, uint32 mapW, uint32 mapH, uint32 tileW, uint32 tileH, tmx_map* tmx) {
	JamTileMap* map = jamCreateTileMap(mapW, mapH, tileW, tileH);
	JamSprite* src = jamGetSpriteFromHandler(handler, layer->name);
	tmx_tile* tile;
	uint32 i;

	if (map != NULL && src != NULL) {
		// Give the map some basic data
		map->tileSheet = src;
		map->collisionRangeStart = 1;
		map->collisionRangeEnd = (uint16)src->animationLength;

		// Loop through each tile on this layer and give it to the map
		for (i = 0; i < mapW * mapH; i++) {
			tile = tmx_get_tile(tmx, (unsigned int) layer->content.gids[i]);
			if (tile == NULL)
				map->grid[i] = 0;
			else
				map->grid[i] = (uint16)(tile->id + 1);
		}
	} else {
		fprintf(stderr, "Failed to create JamTileMap from name %s (createTileMapFromTMXLayer)\n", layer->name);
	}

	return map;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
JamWorld* jamLoadWorldFromTMX(JamAssetHandler *handler, JamRenderer *renderer, const char *tmxFilename) {
	tmx_map* tmx = tmx_load(tmxFilename);
	JamWorld* world = jamCreateWorld(renderer);
	JamTileMap* currentTileMap = NULL;
	uint32 mapW, mapH, tileW, tileH;
	tmx_layer* currentLayer;
	unsigned char worldLayerPointer = 0;
	char* randomString;

	if (handler != NULL && tmx != NULL && world != NULL) {
		// Load the preliminary information from the map
		currentLayer = tmx->ly_head;
		mapW = tmx->width;
		mapH = tmx->height;
		tileW = tmx->tile_width;
		tileH = tmx->tile_height;

		// Warn the user if they are trying to load anything but an orthogonal map
		if (tmx->orient != O_ORT)
			fprintf(stderr, "Warning: jamLoadWorldFromTMX does not support any view besides orthogonal. Attempting to load anyway.\n");

		while (currentLayer != NULL) {
			if (currentLayer->type == L_OBJGR) {
				if (!loadObjectLayerIntoWorld(handler, world, currentLayer)) {
					jSetError(ERROR_TMX_ENTITY_ERROR, "JamEntity layer %s could not be loaded (jamLoadWorldFromTMX)\n", currentLayer->name);
				}
			}
			else if (currentLayer->type == L_LAYER) {
				currentTileMap = createTileMapFromTMXLayer(handler, currentLayer, mapW, mapH, tileW, tileH, tmx);
				if (currentTileMap != NULL) {
					// Is there room in the world?
					if (worldLayerPointer < MAX_TILEMAPS - 1) {
						// Put the map into the world and throw it into the asset handler as well so it gets cleaned up later
						world->worldMaps[worldLayerPointer++] = currentTileMap;
						randomString = genRandomString();
						jamLoadAssetIntoHandler(handler, createAsset(currentTileMap, at_TileMap), randomString);
						jamThrowInGarbageINI(handler->localINI, randomString);
					} else {
						jamFreeTileMap(currentTileMap);
						jSetError(ERROR_TMX_TILEMAP_ERROR, "Tile layer %s could not be loaded because the world has no more map slots (jamLoadWorldFromTMX)\n", currentLayer->name);
					}
				} else {
					jSetError(ERROR_TMX_TILEMAP_ERROR, "Tile layer %s could not be loaded (jamLoadWorldFromTMX)\n", currentLayer->name);
				}
			}

			// Go to the next layer in the list
			if (currentLayer->type == L_GROUP) {
				currentLayer = currentLayer->content.group_head;
			} else {
				currentLayer = currentLayer->next;
			}
		}
	} else {
		if (handler == NULL) {
			jSetError(ERROR_NULL_POINTER, "Handler doesn't exist (jamLoadWorldFromTMX)\n");
		}
		if (tmx == NULL) {
			jSetError(ERROR_OPEN_FAILED, "Failed to open tmx file [%s] (jamLoadWorldFromTMX)\n", tmx_strerr());
		}
		if (world == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create the world (jamLoadWorldFromTMX)\n");
		}
	}

	tmx_map_free(tmx);
	return world;
}
///////////////////////////////////////////////////////////////////////////////
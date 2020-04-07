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
JamAsset* jamGetAssetFromHandler(JamAssetHandler *assetHandler, const char* key);

///////////////////////////////////////////////////////////////////////////////
// This function is meant for jamTMXLoadWorld and is not safe to call
bool loadObjectLayerIntoWorld(JamAssetHandler* handler, JamWorld* world, tmx_layer* layer) {
	JamEntity* tempEntity;
	tmx_object* currentObject = layer->content.objgr->head;
	bool failedToLoad = false;
	JamAsset* asset;

	// Loop the linked list
	while (currentObject != NULL) {
		// Locate the entity in the handler or use the type
		asset = jamGetAssetFromHandler(handler, currentObject->type);

		if (asset != NULL && asset->type == at_Entity) {
			tempEntity = jamEntityCopy(asset->entity, currentObject->x,
									   currentObject->y);
		} else {
			tempEntity = jamEntityCreate(NULL, NULL, currentObject->x, currentObject->y, 0, 0, NULL);
			tempEntity->type = (uint32)atof(currentObject->type);
		}

		if (tempEntity != NULL) {
			jamWorldAddEntity(world, tempEntity);

			// Adjust scale
			if (tempEntity->sprite != NULL) {
				tempEntity->scaleX = (float)currentObject->width  / tempEntity->sprite->width;
				tempEntity->scaleY = (float)currentObject->height / tempEntity->sprite->height;
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
			jSetError(ERROR_ALLOC_FAILED, "Failed to create entity ID %i (loadObjectLayerIntoWorld)", currentObject->id);
		}

		currentObject = currentObject->next;
	}

	return !failedToLoad;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This function is meant for jamTMXLoadWorld and is not safe to call
JamTileMap* createTileMapFromTMXLayer(JamAssetHandler* handler, tmx_layer* layer, uint32 mapW, uint32 mapH, uint32 tileW, uint32 tileH, tmx_map* tmx) {
	JamTileMap* map = jamTileMapCreate(mapW, mapH, tileW, tileH);
	JamSprite* currentSprite;
	tmx_tile* tile;
	bool mapLoadFailed = false;
	uint32 i;

	if (map != NULL) {
		// Loop through each tile on this layer and give it to the map
		for (i = 0; (i < mapW * mapH) && (!mapLoadFailed); i++) {
			tile = tmx_get_tile(tmx, (unsigned int) layer->content.gids[i]);

			if (tile != NULL) {
				// Find the frame in the handler and load it into the map
				currentSprite = jamAssetHandlerGetSprite(handler, tile->tileset->name);
				if (currentSprite != NULL && tile->id < currentSprite->animationLength) {
					map->grid[i] = currentSprite->frames[tile->id];
				} else {
					jSetError(ERROR_TMX_TILEMAP_ERROR,
							  "Tile layer %s contains tiles not present in the handler or the tile is out of range of the sprite");
					mapLoadFailed = true;
					jamTileMapFree(map);
					map = NULL;
				}
			}
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create JamTileMap from name %s (createTileMapFromTMXLayer)", layer->name);
	}

	return map;
}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
JamWorld* jamTMXLoadWorld(JamAssetHandler *handler, const char *tmxFilename) {
	tmx_map* tmx = tmx_load(tmxFilename);
	JamWorld* world;
	JamTileMap* currentTileMap = NULL;
	uint32 mapW, mapH, tileW, tileH;
	tmx_layer* currentLayer;
	unsigned char worldLayerPointer = 0;

	if (handler != NULL && tmx != NULL) {
		// Load the preliminary information from the map
		currentLayer = tmx->ly_head;
		mapW = tmx->width;
		mapH = tmx->height;
		tileW = tmx->tile_width;
		tileH = tmx->tile_height;
		world = jamWorldCreate(mapW / 2, mapH / 2, tileW * 2, tileH * 2, false);

		if (world != NULL) {
			// Warn the user if they are trying to load anything but an orthogonal map
			if (tmx->orient != O_ORT)
				jSetError(ERROR_WARNING,
						  "jamTMXLoadWorld does not support any view besides orthogonal. Attempting to load anyway");

			while (currentLayer != NULL) {
				if (currentLayer->type == L_OBJGR) {
					if (!loadObjectLayerIntoWorld(handler, world, currentLayer)) {
						jSetError(ERROR_TMX_ENTITY_ERROR, "JamEntity layer %s could not be loaded (jamTMXLoadWorld)",
								  currentLayer->name);
					}
				} else if (currentLayer->type == L_LAYER) {
					currentTileMap = createTileMapFromTMXLayer(handler, currentLayer, mapW, mapH, tileW, tileH, tmx);
					if (currentTileMap != NULL) {
						// Is there room in the world?
						if (worldLayerPointer < MAX_TILEMAPS - 1) {
							world->worldMaps[worldLayerPointer++] = currentTileMap;
						} else {
							jamTileMapFree(currentTileMap);
							jSetError(ERROR_TMX_TILEMAP_ERROR,
									  "Tile layer %s could not be loaded because the world has no more map slots (jamTMXLoadWorld)",
									  currentLayer->name);
						}
					} else {
						jSetError(ERROR_TMX_TILEMAP_ERROR, "Tile layer %s could not be loaded (jamTMXLoadWorld)",
								  currentLayer->name);
					}
				}

				// Go to the next layer in the list
				if (currentLayer->type == L_GROUP) {
					currentLayer = currentLayer->content.group_head;
				} else {
					currentLayer = currentLayer->next;
				}
			}
		}
	} else {
		if (handler == NULL) {
			jSetError(ERROR_NULL_POINTER, "Handler doesn't exist (jamTMXLoadWorld)");
		}
		if (tmx == NULL) {
			jSetError(ERROR_OPEN_FAILED, "Failed to open tmx file [%s] (jamTMXLoadWorld)", tmx_strerr());
		}
		if (world == NULL) {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create the world (jamTMXLoadWorld)");
		}
	}

	tmx_map_free(tmx);
	return world;
}
///////////////////////////////////////////////////////////////////////////////

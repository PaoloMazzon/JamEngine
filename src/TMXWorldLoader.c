#include <stdio.h>
#include <stdlib.h>
#include "JamEngine.h"
#include "TMXWorldLoader.h"
#include "JamError.h"
#include "tmx.h"

/* General tmx loading outline
 * 1. setup
 *  a) load .tmx file
 *  b) create world
 *  c) store width/height/tile width/tile height for later
 *
 * 2. loop through tmx layers
 *  a) if its an object layer
 *   i) create a copy of the entity from the asset loader using the tmx object type as the asset name
 *   ii) load extra data from the map into the entity (like width/height)
 *   iii) add the entity to the world
 *  b) if its a tile layer
 *   i) create a new tileset with denoted width/height
 *   ii) use the name of the layer to load the sprite from the asset loader into the tilemap
 *   iii) load the data from the tileset layer into the tilemap
 *   iv) put the new tilemap into the world
 */

Asset* createAsset(void*, enum AssetType);

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
// This function is meant for loadWorldFromTMX and is not safe to call
bool loadObjectLayerIntoWorld(AssetHandler* handler, World* world, tmx_layer* layer) {
	Entity* tempEntity;
	tmx_object* currentObject = layer->tmx_object_group->head;
	bool failedToLoad = false;

	// Loop the linked list
	while (currentObject != NULL) {
		tempEntity = copyEntity(assetGetEntity(handler, currentObject->type), currentObject->x, currentObject->y);

		if (tempEntity != NULL) {
			worldAddentity(world, tempEntity);
			// TODO: Add support for tmx custom properties to entity values
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
// This function is meant for loadWorldFromTMX and is not safe to call
TileMap* createTileMapFromTMXLayer(AssetHandler* handler, tmx_layer* layer, uint32 mapW, uint32 mapH, uint32 tileW, uint32 tileH) {

}
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
World* loadWorldFromTMX(AssetHandler* handler, Renderer* renderer, const char* tmxFilename) {
	tmx_map* tmx = tmx_load(tmxFilename);
	World* world = createWorld(renderer);
	TileMap* currentTileMap = NULL;
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
			fprintf(stderr, "Warning: loadWorldFromTMX does not support any view besides orthogonal. Attempting to load anyway.\n");

		while (currentLayer != NULL) {
			if (currentLayer->type == L_OBJGR) {
				if (!loadObjectLayerIntoWorld(handler, world, currentLayer)) {
					jSetError(ERROR_TMX_ENTITY_ERROR);
					fprintf(stderr, "Entity layer %s could not be loaded (loadWorldFromTMX)\n", currentLayer->name);
				}
			}
			else if (currentLayer->type == L_LAYER) {
				currentTileMap = createTileMapFromTMXLayer(handler, currentLayer, mapW, mapH, tileW, tileH);
				if (currentTileMap != NULL) {
					// Is there room in the world?
					if (worldLayerPointer < MAX_TILEMAPS - 1) {
						// Put the map into the world and throw it into the asset handler as well so it gets cleaned up later
						world->worldMaps[++worldLayerPointer] = currentTileMap;
						randomString = genRandomString();
						loadAssetIntoHandler(handler, createAsset(currentTileMap, tileAsset), randomString);
						throwInGarbageINI(handler->localINI, randomString);
					} else {
						freeTileMap(currentTileMap);
						jSetError(ERROR_TMX_TILEMAP_ERROR);
						fprintf(stderr, "Tile layer %s could not be loaded because the world has no more map slots (loadWorldFromTMX)\n", currentLayer->name);
					}
				} else {
					jSetError(ERROR_TMX_TILEMAP_ERROR);
					fprintf(stderr, "Tile layer %s could not be loaded (loadWorldFromTMX)\n", currentLayer->name);
				}
			}

			// Go to the next layer in the list
			currentLayer = currentLayer->next;
		}
	} else {
		if (handler == NULL) {
			fprintf(stderr, "Handler doesn't exist (loadWorldFromTMX)\n");
			jSetError(ERROR_NULL_POINTER);
		}
		if (tmx == NULL) {
			fprintf(stderr, "Failed to open tmx file [%s] (loadWorldFromTMX)\n", tmx_strerr());
			jSetError(ERROR_OPEN_FAILED);
		}
		if (world == NULL) {
			fprintf(stderr, "Failed to create the world (loadWorldFromTMX)\n");
			jSetError(ERROR_ALLOC_FAILED);
		}
	}

	tmx_map_free(tmx);
	return world;
}
///////////////////////////////////////////////////////////////////////////////
#include "TMXWorldLoader.h"
#include <stdio.h>
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

///////////////////////////////////////////////////////////////////////////////
World* loadWorldFromTMX(AssetHandler* handler, Renderer* renderer, const char* tmxFilename) {
	tmx_map* tmx = tmx_load(tmxFilename);
	World* world = createWorld(renderer);
	TileMap* currentTileMap = NULL;
	Entity* currentEntity = NULL;
	uint32 tileW, tileH;

	if (handler != NULL && tmx != NULL && world != NULL) {
		// TODO: This
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
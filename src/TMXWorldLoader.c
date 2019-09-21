#include "TMXWorldLoader.h"
#include <stdio.h>
#include "JamError.h"
#include "tmx.h"

///////////////////////////////////////////////////////////////////////////////
World* loadWorldFromTMX(AssetHandler* handler, Renderer* renderer, const char* tmxFilename) {
	tmx_map* tmx = tmx_load(tmxFilename);
	World* world = createWorld(renderer);

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
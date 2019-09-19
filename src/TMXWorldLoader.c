#include "TMXWorldLoader.h"
#include <stdio>
#include "JamError.h"

///////////////////////////////////////////////////////////////////////////////
World* loadWorldFromTMX(AssetHandler* handler, const char* tmxFilename) {
	if (handler != NULL) {
		// TODO: This
	} else {
		fprintf(stderr, "Handler doesn't exist (loadWorldFromTMX)\n");
		jSetError(ERROR_NULL_POINTER);
	}
}
///////////////////////////////////////////////////////////////////////////////
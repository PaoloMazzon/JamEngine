#include <WorldHandler.h>
#include <malloc.h>
#include <JamError.h>
#include <memory.h>

typedef struct {
	const char* name;                      // Name of this world entry
	const char* filename;                  // Filename of the .tmx world
	void (*onCreate)(WORLD_HANDLER_ARGS);  // Function to call on creation of the world
	void (*onFrame)(WORLD_HANDLER_ARGS);   // Called every frame of the world
	void (*onCleanup)(WORLD_HANDLER_ARGS); // Called before the world is destroyed
} _JamWorldEntry;

typedef struct {
	_JamWorldEntry** worlds; // The world entries themselves
	int size;                // Number of world entries
} _JamWorldHandler;

_JamWorldEntry* _jamWorldEntryCreate(const char* filename, const char* name, void (*onCreate)(WORLD_HANDLER_ARGS), void (*onFrame)(WORLD_HANDLER_ARGS), void (*onCleanup)(WORLD_HANDLER_ARGS)) {
	if (filename != NULL || name != NULL) {
		_JamWorldEntry *entry = malloc(sizeof(_JamWorldEntry));
		if (entry != NULL) {
			entry->name = name != NULL ? strcpy(malloc(strlen(name) + 1), name) : NULL;
			entry->name = filename != NULL ? strcpy(malloc(strlen(filename) + 1), filename) : NULL;
			entry->onFrame = onFrame;
			entry->onCleanup = onCleanup;
			entry->onCreate = onCreate;
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to create world entry");
		}

		return entry;
	} else {
		jSetError(ERROR_NULL_POINTER, "Can't provide no name and no filename");
	}
	return false;
}

void _jamWorldEntryFree(_JamWorldEntry* entry) {
	if (entry != NULL) {
		free((void*)entry->name);
		free((void*)entry->filename);
		free(entry);
	}
}

static _JamWorldHandler* gHandler;
static int gCurrentWorld;
static bool gSwitch;

///////////////////////////////////////////////////////////////////////////////////////////////////////
void jamWorldHandlerInit() {
	if (gHandler == NULL) {
		gHandler = calloc(1, sizeof(_JamWorldHandler));
		if (gHandler == NULL)
			jSetError(ERROR_ALLOC_FAILED, "Failed to initialize world handler");
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void jamWorldHandlerQuit() {
	int i;
	if (gHandler != NULL) {
		for (i = 0; i < gHandler->size; i++)
			_jamWorldEntryFree(gHandler->worlds[i]);
		free(gHandler->worlds);
		free(gHandler);
		gHandler = NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void jamWorldHandlerSwitch(const char* name) {
	int i;
	if (gHandler != NULL) {
		if (name == WORLD_QUIT) {
			gSwitch = true;
			gCurrentWorld = -1;
		} else {
			for (i = 0; i < gHandler->size && !gSwitch; i++) {
				if (strcmp(name, gHandler->worlds[i]->name) == 0) {
					gSwitch = true;
					gCurrentWorld = i;
				}
			}
		}

	} else {
		jSetError(ERROR_NULL_POINTER, "World handler not initialized");
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void jamWorldHandlerNext() {
	if (gHandler != NULL) {
		gCurrentWorld++;
		gSwitch = true;
		if (gCurrentWorld == gHandler->size)
			gCurrentWorld = 0;
	} else {
		jSetError(ERROR_NULL_POINTER, "World handler not initialized");
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void jamWorldHandlerRestart() {
	if (gHandler != NULL) {
		gSwitch = true;
	} else {
		jSetError(ERROR_NULL_POINTER, "World handler not initialized");
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
	void jamWorldHandlerAdd(const char* filename, const char* name, void (*onCreate)(WORLD_HANDLER_ARGS), void (*onFrame)(WORLD_HANDLER_ARGS), void (*onCleanup)(WORLD_HANDLER_ARGS)) {
		if (gHandler != NULL) {
			// TODO: This
		} else {
			jSetError(ERROR_NULL_POINTER, "World handler not initialized");
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void jamWorldHandlerRun(JamAssetHandler* assetHandler) {
	if (gHandler != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_NULL_POINTER, "World handler not initialized");
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
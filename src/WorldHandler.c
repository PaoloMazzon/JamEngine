#include <WorldHandler.h>
#include <malloc.h>
#include <JamError.h>
#include <memory.h>
#include <File.h>

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
			if (name == NULL) {
				entry->name = jamGetNameOfFile(filename);
			} else {
				entry->name = strcpy(malloc(strlen(name) + 1), name);
			}
			entry->filename = filename != NULL ? strcpy(malloc(strlen(filename) + 1), filename) : NULL;
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

static _JamWorldHandler* gHandler = NULL;
static int gCurrentWorld = 0;
static bool gSwitch = 0;

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
	bool found = false;
	if (gHandler != NULL) {
		if (name == WORLD_QUIT) {
			gSwitch = true;
			gCurrentWorld = -1;
		} else {
			for (i = 0; i < gHandler->size && !gSwitch; i++) {
				if (strcmp(name, gHandler->worlds[i]->name) == 0) {
					gSwitch = true;
					gCurrentWorld = i;
					found = true;
				}
			}

			if (!found)
				jSetError(ERROR_WORLD_NOT_FOUND, "World [%s] doesn't exist", name);
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
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void jamWorldHandlerAdd(const char* filename, const char* name, void (*onCreate)(WORLD_HANDLER_ARGS), void (*onFrame)(WORLD_HANDLER_ARGS), void (*onCleanup)(WORLD_HANDLER_ARGS)) {
	_JamWorldEntry** newList;
	_JamWorldEntry* newEntry;
	if (gHandler != NULL) {
		newList = realloc(gHandler->worlds, (gHandler->size + 1) * sizeof(_JamWorldEntry*));
		newEntry = _jamWorldEntryCreate(filename, name, onCreate, onFrame, onCleanup);

		if (newList != NULL && newEntry != NULL) {
			newList[gHandler->size] = newEntry;
			gHandler->size++;
			gHandler->worlds = newList;
		} else {
			if (newList == NULL)
				jSetError(ERROR_REALLOC_FAILED, "Failed to reallocate list");
			_jamWorldEntryFree(newEntry);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "World handler not initialized");
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void jamWorldHandlerRun(JamAssetHandler* assetHandler) {
	if (gHandler != NULL && gHandler->size > 0) {
		// Load the world and call the on create function
		JamWorld* world = gHandler->worlds[gCurrentWorld]->filename != NULL ?
				jamTMXLoadWorld(assetHandler, gHandler->worlds[gCurrentWorld]->filename) : NULL;
		if (gHandler->worlds[gCurrentWorld]->onCreate != NULL)
			(gHandler->worlds[gCurrentWorld]->onCreate)(world, assetHandler);

		while (gCurrentWorld != -1 && jamRendererProcEvents() && !jGetError()) {
			// Process the frame
			if (gHandler->worlds[gCurrentWorld]->onFrame != NULL)
				(gHandler->worlds[gCurrentWorld]->onFrame)(world, assetHandler);
			else
				jamWorldProcFrame(world);

			// Possibly swap worlds
			if (gSwitch && gCurrentWorld != -1) {
				// Call cleanup function, destroy the world, create the new one, and call its creation function
				if (gHandler->worlds[gCurrentWorld]->onCleanup != NULL)
					(gHandler->worlds[gCurrentWorld]->onCleanup)(world, assetHandler);

				jamWorldFree(world);
				world = gHandler->worlds[gCurrentWorld]->filename != NULL ?
								  jamTMXLoadWorld(assetHandler, gHandler->worlds[gCurrentWorld]->filename) : NULL;
				if (gHandler->worlds[gCurrentWorld]->onCreate != NULL)
					(gHandler->worlds[gCurrentWorld]->onCreate)(world, assetHandler);

				gSwitch = false;
			}

			jamRendererProcEndFrame();
		}

		// Clean up the world/call its destroy function
		if (gHandler->worlds[gCurrentWorld]->onCleanup != NULL)
			(gHandler->worlds[gCurrentWorld]->onCleanup)(world, assetHandler);
		jamWorldFree(world);
	} else {
		if (gHandler == NULL)
			jSetError(ERROR_NULL_POINTER, "World handler not initialized");
		else
			jSetError(ERROR_WARNING, "Warning: There are no worlds in the handler");
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
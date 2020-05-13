/// \file WorldHandler.h
/// \author plo
/// \brief Allows for easy control over multiple levels
///
/// The world handler is a way to manage multiple worlds and switch
/// between them easily. You load worlds into the handler with jamWorldHandlerAdd
/// and switch between them with jamWorldHandlerSwitch or jamWorldHandlerNext. The
/// latter will switch to the next world in the handler since jamWorldHandlerAdd
/// adds worlds sequentially. You can also add worlds without .tmx files associated
/// and just functions for like a menu or something.

#pragma once
#include "World.h"
#include "JamEngine.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WORLD_HANDLER_ARGS JamWorld* self, JamAssetHandler* assetHandler
#define WORLD_QUIT NULL

/// \brief Starts the world handler subsystem (the renderer does this)
/// \throws ERROR_ALLOC_FAILED
void jamWorldHandlerInit();

/// \brief Cleans up the world handler subsystem (the renderer does this) (don't call this while its running)
void jamWorldHandlerQuit();

/// \brief Adds a world to the world handler
/// \param filename Filename of the .tmx file to load for this world or NULL if you wish only to have the functions (for a menu or something)
/// \param name Name of the world or NULL to use the filename (without the path or extension, ie "assets/level1.tmx" would be "level1")
/// \param onCreate After the tmx is loaded and the world is about to be ran, onCreate is called
/// \param onFrame Called every frame to update the world, jamWorldProcFrame should be called in it (use NULL to make the handler call jamWorldProcFrame for this world)
/// \param onCleanup Right before the world is freed
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_REALLOC_FAILED
/// \throws ERROR_NULL_POINTER
void jamWorldHandlerAdd(const char* filename, const char* name, void (*onCreate)(WORLD_HANDLER_ARGS), void (*onFrame)(WORLD_HANDLER_ARGS), void (*onCleanup)(WORLD_HANDLER_ARGS));

/// \brief Switches to a different world (name) at the end of the frame
/// \param name Name of the world to switch to or WORLD_QUIT to leave the world handler loop
///
/// This will only apply once per frame at most, even if its called again after the first
/// time each frame it will do nothing (unless it is told to quit).
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_WORLD_NOT_FOUND
void jamWorldHandlerSwitch(const char* name);

/// \brief Switches to the next world in the handler (since worlds are added sequentially)
///
/// This will wrap back to the starting world if its at the last level.
///
/// \throws ERROR_NULL_POINTER
void jamWorldHandlerNext();

/// \brief Reloads the current room (frees the world then reloads the tmx)
void jamWorldHandlerRestart();

/// \brief Runs the world handler, loading and running the first world in the handler
/// \throws ERROR_NULL_POINTER
void jamWorldHandlerRun(JamAssetHandler* assetHandler);

#ifdef __cplusplus
}
#endif
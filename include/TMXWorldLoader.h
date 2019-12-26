/// \file TMXWorldLoader.h
/// \author plo
/// \brief Loads a world from an asset handler and tmx file

#pragma once
#include "World.h"
#include "AssetHandler.h"
#include "Renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Loads a world from a tmx level file
/// \param handler The handler that already has its assets loaded 
/// \param tmxFilename The filename of the tmx file to load
///
/// Refer to the World documentation for how this function behaves.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OPEN_FAILED
/// \throws ERROR_ALLOC_FAILED
JamWorld* jamTMXLoadWorld(JamAssetHandler *handler, const char *tmxFilename);

#ifdef __cplusplus
}
#endif
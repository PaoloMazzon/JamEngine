/// \file TMXWorldLoader.h
/// \author plo
/// \brief Loads a world from an asset handler and tmx file

#pragma once
#include "World.h"
#include "AssetHandler.h"
#include "Renderer.h"

/// \brief Loads a world from a tmx level file
/// \param handler The handler that already has its assets loaded 
/// \param tmxFilename The filename of the tmx file to load
///
/// This function loads a .tmx file made from a level-editing software called Tiled
/// and turns it into a Jam Engine World. This function, however, expects a very
/// specific setup to be able to load .tmx worlds properly. Every tileset layer in
/// Tiled must be named after its corresponding sprite in the AssetHandler and every
/// object must have its type set to the name of its corresponding entity in the
/// AssetHandler. Aside from this, you can have as many layers/tilesets/whatever as
/// you want in Tiled as this function will only load tileset and object layers.
/// TileMaps need not be created in the AssetHandler, this function will create
/// whatever TileMaps it needs and it will also put them into the AssetHandler so
/// they may be destroyed by the AssetHandler later when freeAssetHandler is called.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OPEN_FAILED
/// \throws ERROR_ALLOC_FAILED
World* loadWorldFromTMX(AssetHandler* handler, Renderer* renderer, const char* tmxFilename);
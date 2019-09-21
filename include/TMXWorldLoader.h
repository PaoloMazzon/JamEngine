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
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_OPEN_FAILED
/// \throws ERROR_ALLOC_FAILED
World* loadWorldFromTMX(AssetHandler* handler, Renderer* renderer, const char* tmxFilename);
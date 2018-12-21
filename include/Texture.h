/// \file Texture.h
/// \author lugi1
/// \brief Because of circular dependencies, this file only declares texture functions.

#pragma once
#include <SDL.h>
#include "Constants.h"
#include "Renderer.h"

/// \brief Creates a texture that can be rendered to
Texture* createTexture(Renderer* renderer, int w, int h);

/// \brief Loads a texture from a file
Texture* loadTexture(Renderer* renderer, const char* filename);

/// \brief Frees a texture from memory
void freeTexture(Texture* tex);
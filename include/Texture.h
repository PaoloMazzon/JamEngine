//
// Created by lugi1 on 2018-07-07.
//

#pragma once
#include <SDL.h>
#include "Constants.h"
#include "Renderer.h"

/// \brief Creates a texture that can be rendered to
Texture* createTexture(Renderer* renderer, uint32 w, uint32 h);

/// \brief Loads a texture from a file
Texture* loadTexture(Renderer* renderer, const char* filename);

/// \brief Frees a texture from memory
void freeTexture(Texture* tex);
/// \file Texture.h
/// \author lugi1
/// \brief Because of circular dependencies, this file only declares texture functions.

#pragma once
#include <SDL.h>
#include "Constants.h"
#include "Renderer.h"

/// \brief Simple texture class with some basic info
///
/// Weather or not a texture is able to be rendered
/// to depends on if it was used to load a file or
/// created without loading a file
struct _Texture {
	SDL_Texture* tex; ///< The internal SDL2 texture
	uint32 w; ///< Texture's width
	uint32 h; ///< Texture's height
	bool renderTarget; ///< Weather or not this texture can be rendered to
};

/// \brief Creates a texture that can be rendered to
/// \throws ERROR_SDL_ERROR
Texture* createTexture(Renderer* renderer, int w, int h);

/// \brief Loads a texture from a file
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ALLOC_FAILED
Texture* loadTexture(Renderer* renderer, const char* filename);

/// \brief Frees a texture from memory
void freeTexture(Texture* tex);
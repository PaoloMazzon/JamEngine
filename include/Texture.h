/// \file Texture.h
/// \author lugi1
/// \brief Because of circular dependencies, this file only declares texture functions.

#pragma once
#include "Constants.h"
#include "Renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Simple texture class with some basic info
///
/// Weather or not a texture is able to be rendered
/// to depends on if it was used to load a file or
/// created without loading a file
struct _JamTexture {
	void* tex; ///< The internal SDL2 texture
	sint32 w; ///< Texture's width
	sint32 h; ///< Texture's height
	bool renderTarget; ///< Weather or not this texture can be rendered to
};

/// \brief Creates a texture that can be rendered to
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_ALLOC_FAILED
JamTexture* jamTextureCreate(int w, int h);

/// \brief Creates a JamTexture from a SDL2 texture
///
/// Once you pass the SDL2 texture to this function,
/// it belongs to the texture. Do not free the texture
/// you pass here yourself as the texture will free
/// it when you call jamTextureFree.
///
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ALLOC_FAILED
JamTexture* jamTextureCreateFromTex(void* texture);

/// \brief Loads a texture from a file
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_ALLOC_FAILED
JamTexture* jamTextureLoad(const char *filename);

/// \brief Frees a texture from memory
void jamTextureFree(JamTexture *tex);

#ifdef __cplusplus
}
#endif
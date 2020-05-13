//
// Created by lugi1 on 2018-07-07.
//

#include "Texture.h"
#include "Renderer.h"
#include <stdio.h>
#include <SDL.h>
#include "JamError.h"

unsigned char *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
void stbi_image_free(void *retval_from_stbi_load);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
int shift = (req_format == STBI_rgb) ? 8 : 0;
  	const Uint32 rmask = 0xff000000 >> shift;
  	const Uint32 gmask = 0x00ff0000 >> shift;
  	const Uint32 bmask = 0x0000ff00 >> shift;
	const Uint32 amask = 0x000000ff >> shift;
#else // little endian, like x86
	const Uint32 rmask = 0x000000ff;
	const Uint32 gmask = 0x0000ff00;
	const Uint32 bmask = 0x00ff0000;
	const Uint32 amask = 0xff000000;
#endif

// This is essentially a hidden function, you can use it in your games (all the
// error checking and such is there) but its meant for in-engine use.
SDL_Texture* jamSDLTextureLoad(const char* filename) {
	int x, y, n;
	SDL_Texture* tex = NULL;

	unsigned char* pixels = stbi_load(filename, &x, &y, &n, 4);
	SDL_Surface *img = SDL_CreateRGBSurfaceFrom(pixels, x, y, 32, x * 4, rmask, gmask, bmask, amask);

	if (img != NULL) {
		tex = SDL_CreateTextureFromSurface(jamRendererGetInternalRenderer(), img);
		if (tex == NULL)
			jSetError(ERROR_SDL_ERROR, "SDL Error while converting to texture [%s]: %s", filename, SDL_GetError());
	} else {
		if (pixels == NULL)
			jSetError(ERROR_ALLOC_FAILED, "Failed to load image %s", filename);
		else
			jSetError(ERROR_SDL_ERROR, "SDL error loading texture %s: %s", filename, SDL_GetError());
	}
	SDL_FreeSurface(img);
	stbi_image_free(pixels);

	return tex;
}

// Essentially the same as above but doesn't convert to a texture
SDL_Surface* jamSDLSurfaceLoad(const char* filename) {
	int x, y, n;

	unsigned char* pixels = stbi_load(filename, &x, &y, &n, 4);
	SDL_Surface *img = SDL_CreateRGBSurfaceFrom(pixels, x, y, 32, x * 4, rmask, gmask, bmask, amask);

	if (img == NULL) {
		if (pixels == NULL)
			jSetError(ERROR_ALLOC_FAILED, "Failed to load image %s", filename);
		else
			jSetError(ERROR_SDL_ERROR, "SDL error loading texture %s: %s", filename, SDL_GetError());
	}
	stbi_image_free(pixels);

	return img;
}

////////////////////////////////////////////////////////////////
JamTexture* jamTextureCreate(int w, int h) {
	// Create the basic tex struct
	JamTexture* tex = (JamTexture*)malloc(sizeof(JamTexture));

	// Check if we were given a dud
	if (tex != NULL) {
		// Now we create the actual SDL texture, telling SDL we want
		// a texture that is not only in 32-bit RGBA format, but also
		// a texture we can render things to
		tex->tex = SDL_CreateTexture(jamRendererGetInternalRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);

		// And once again, check if we got a dud back
		if (tex->tex != NULL) {
			tex->w = w;
			tex->h = h;
			tex->renderTarget = true;
			SDL_SetTextureBlendMode(tex->tex, SDL_BLENDMODE_BLEND);
		} else {
			free(tex);
			tex = NULL;
			jSetError(ERROR_SDL_ERROR, "Failed to create SDL texture (jamTextureCreate). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate JamTexture. SDL Error: %s\n", SDL_GetError());
	}

	// If we get a dud back, it returns null which informs the user
	// that something went wrong. If we get back a proper texture
	// struct, it returns that so we need not wrap our return in some
	// fail-safe statements.
	return tex;
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
JamTexture* jamTextureCreateFromTex(void* texture) {
	JamTexture* tex = (JamTexture*)malloc(sizeof(JamTexture));

	if (tex != NULL && texture != NULL) {
		tex->tex = texture;
		SDL_QueryTexture(texture, NULL, NULL, &tex->w, &tex->h);

	} else {
		if (tex == NULL)
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate JamTexture. SDL Error: %s\n", SDL_GetError());
		if (texture == NULL)
			jSetError(ERROR_NULL_POINTER, "Texture does not exist");
	}

	return tex;
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
JamTexture* jamTextureLoad(const char *filename) {
	JamTexture* tex = NULL;
	SDL_Texture* sdltex;
	int x, y, z;

	if (jamRendererGetInternalRenderer() != NULL) {
		tex = (JamTexture*) malloc(sizeof(JamTexture));

		if (tex != NULL) {
			sdltex = jamSDLTextureLoad(filename);

			// And once again, check if we got a dud back
			if (sdltex != NULL) {
				// Put the texture into the struct and query its info
				tex->tex = sdltex;
				SDL_QueryTexture(tex->tex, NULL, NULL, &tex->w, &tex->h);
				tex->renderTarget = false;
			} else {
				free(tex);
				tex = NULL;
				jSetError(ERROR_SDL_ERROR, "Failed to create tex", SDL_GetError());
			}
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate JamTexture. SDL Error: %s\n", SDL_GetError());

		}
	} else {
		if (jamRendererGetInternalRenderer() == NULL)
			jSetError(ERROR_NULL_POINTER, "Cannot load texture, null renderer passed. SDL Error: %s\n", SDL_GetError());
		else
			jSetError(ERROR_NULL_POINTER, "Cannot load texture, renderer contains no internal renderer. SDL Error: %s\n", SDL_GetError());
	}

	// If we get a dud back, it returns null which informs the user
	// that something went wrong. If we get back a proper texture
	// struct, it returns that so we need not wrap our return in some
	// fail-safe statements.
	return tex;
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
void jamTextureFree(JamTexture *tex) {
	if (tex != NULL) {
		SDL_DestroyTexture(tex->tex);
		free(tex);
	}
}
////////////////////////////////////////////////////////////////
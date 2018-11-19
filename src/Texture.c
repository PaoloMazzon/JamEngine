//
// Created by lugi1 on 2018-07-07.
//

#include "Texture.h"
#include "Renderer.h"
#include <stdio.h>
#include <SDL_image.h>

////////////////////////////////////////////////////////////////
Texture* createTexture(Renderer* renderer, uint32 w, uint32 h) {
	// Create the basic tex struct
	Texture* tex = (Texture*)malloc(sizeof(Texture));

	// Check if we were given a dud
	if (tex != NULL) {
		// Now we create the actual SDL texture, telling SDL we want
		// a texture that is not only in 32-bit RGBA format, but also
		// a texture we can render things to
		tex->tex = SDL_CreateTexture(renderer->internalRenderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);

		// And once again, check if we got a dud back
		if (tex->tex != NULL) {
			tex->w = w;
			tex->h = h;
			tex->renderTarget = true;
		} else {
			free(tex);
			tex = NULL;
			fprintf(stderr, "Failed to create SDL texture (createTexture). SDL Error: %s SDL Error: %s\n", SDL_GetError(), SDL_GetError());
		}
	} else {
		fprintf(stderr, "Failed to allocate Texture. SDL Error: %s\n", SDL_GetError());
	}

	// If we get a dud back, it returns null which informs the user
	// that something went wrong. If we get back a proper texture
	// struct, it returns that so we need not wrap our return in some
	// fail-safe statements.
	return tex;
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
Texture* loadTexture(Renderer* renderer, const char* filename) {
	Texture* tex = NULL;

	// Check that the renderer isn't a dud
	if (renderer != NULL && renderer->internalRenderer != NULL) {
		// Create the basic tex struct
		tex = (Texture*) malloc(sizeof(Texture));

		// Check if we were given a dud
		if (tex != NULL) {
			// Loading from an image is a two-part process: loading
			// the image from a file to a surface, then putting
			// that software-level surface onto a hardware-level
			// texture.
			SDL_Surface *img = IMG_Load(filename);

			// And once again, check if we got a dud back
			if (img != NULL) {
				// Now we place that surface onto a texture
				tex->tex = SDL_CreateTextureFromSurface(renderer->internalRenderer, img);

				// Cleanup irregardless of what happens next
				SDL_FreeSurface(img);

				// And of course check that it was successful
				if (tex->tex != NULL) {
					// This is really just a fancy way of loading width and height
					// into the texture struct. SDL2 will actually load
					// the width and height into the arguments passed,
					// hence why pointers to our width and height is passed
					// and not the values themselves.
					SDL_QueryTexture(tex->tex, NULL, NULL, (int*)&tex->w, (int*)&tex->h);

					// In SDL2, you can't load an image and draw on the same texture
					tex->renderTarget = false;
				} else {
					free(tex);
					tex = NULL;
					fprintf(stderr, "Failed to create texture from surface (loadTexture). SDL Error: %s\n", SDL_GetError());
				}
			} else {
				free(tex);
				tex = NULL;
				fprintf(stderr, "Failed to create SDL surface (loadTexture). SDL Error: %s\n", SDL_GetError());
			}
		} else {
			fprintf(stderr, "Failed to allocate Texture. SDL Error: %s\n", SDL_GetError());
		}
	} else {
		if (renderer == NULL)
			fprintf(stderr, "Cannot load texture, null renderer passed. SDL Error: %s\n", SDL_GetError());
		else
			fprintf(stderr, "Cannot load texture, renderer contains no internal renderer. SDL Error: %s\n", SDL_GetError());
	}

	// If we get a dud back, it returns null which informs the user
	// that something went wrong. If we get back a proper texture
	// struct, it returns that so we need not wrap our return in some
	// fail-safe statements.
	return tex;
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
void freeTexture(Texture* tex) {
	if (tex != NULL) {
		SDL_DestroyTexture(tex->tex);
		free(tex);
	}
}
////////////////////////////////////////////////////////////////
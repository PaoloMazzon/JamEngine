#include "Texture.h"
#include "Renderer.h"
#include <SDL.h>
#include "JamError.h"

unsigned char *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
void stbi_image_free(void *retval_from_stbi_load);

extern Uint32 rmask;
extern Uint32 gmask;
extern Uint32 bmask;
extern Uint32 amask;

// Loads a surface using a filename, call with NULL filename to free the surface.
// (Don't use this for long-term surface usage)
SDL_Surface* jamSDLSurfaceLoad(const char* filename) {
	int x, y, n;
	static SDL_Surface* img;
	static unsigned char* pixels;

	if (filename != NULL) {
		pixels = stbi_load(filename, &x, &y, &n, 4);
		img = SDL_CreateRGBSurfaceFrom(pixels, x, y, 32, x * 4, rmask, gmask, bmask, amask);

		if (img == NULL) {
			if (pixels == NULL)
				jSetError(ERROR_ALLOC_FAILED, "Failed to load image %s", filename);
			else
				jSetError(ERROR_SDL_ERROR, "SDL error loading texture %s: %s", filename, SDL_GetError());
		}

		return img;
	} else {
		stbi_image_free(pixels);
		pixels = NULL;
		SDL_FreeSurface(img);
		img = NULL;
		return NULL;
	}
}

////////////////////////////////////////////////////////////////
JamTexture* jamTextureCreate(int w, int h) {
	// Create the basic tex struct
	JamTexture* tex = (JamTexture*)malloc(sizeof(JamTexture));

	if (tex != NULL) {
		tex->tex = vk2dTextureCreate(vk2dRendererGetDevice(), w, h);

		if (tex->tex != NULL) {
			tex->w = w;
			tex->h = h;
			tex->img = NULL;
			tex->renderTarget = true;
		} else {
			free(tex);
			tex = NULL;
			jSetError(ERROR_SDL_ERROR, "Failed to create SDL texture (jamTextureCreate). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate JamTexture. SDL Error: %s\n", SDL_GetError());
	}

	return tex;
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
JamTexture* jamTextureLoad(const char *filename) {
	JamTexture* tex = malloc(sizeof(JamTexture));

	if (tex != NULL) {
		tex->img = vk2dImageLoad(vk2dRendererGetDevice(), filename);

		if (tex->img != NULL) {
			tex->tex = vk2dTextureLoad(tex->img, 0, 0, tex->img->width, tex->img->height);

			if (tex->tex != NULL) {
				tex->renderTarget = false;
				tex->w = tex->img->width;
				tex->h = tex->img->height;
			} else {
				jSetError(ERROR_VULKAN_ERROR, "Failed to create a texture from %s", filename);
			}
		} else {
			jSetError(ERROR_FILE_FAILED, "Failed to create texture's image for %s", filename);
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate texture");
	}

	return tex;
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
JamTexture *jamTextureFromSurface(void *surface) {
	JamTexture* tex = malloc(sizeof(JamTexture));

	if (tex != NULL) {
		tex->img = vk2dImageFromSurface(vk2dRendererGetDevice(), surface);

		if (tex->img != NULL) {
			tex->tex = vk2dTextureLoad(tex->img, 0, 0, ((SDL_Surface*)surface)->w, ((SDL_Surface*)surface)->h);
			if (tex->tex != NULL) {
				tex->w = ((SDL_Surface*)surface)->w;
				tex->h = ((SDL_Surface*)surface)->h;
				tex->renderTarget = false;
			} else {
				jSetError(ERROR_VULKAN_ERROR, "Failed to create Vulkan texture");
				free(tex);
				tex = NULL;
			}
		} else {
			jSetError(ERROR_VULKAN_ERROR, "Failed to create Vulkan image");
			free(tex);
			tex = NULL;
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate texture");
	}

	return tex;
}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
void jamTextureFree(JamTexture *tex) {
	if (tex != NULL) {
		vk2dRendererWait();
		vk2dImageFree(tex->img);
		vk2dTextureFree(tex->tex);
		free(tex);
	}
}
////////////////////////////////////////////////////////////////
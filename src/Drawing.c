//
// Created by lugi1 on 2018-07-13.
//

#include "Drawing.h"
#include <stdio.h>
#include <Renderer.h>
#include <TileMap.h>
#include <Vector.h>
#include "JamError.h"
#include <SDL.h>

//////////////////////////////////////////////////////////////
void jamDrawSetColour(uint8 r, uint8 g, uint8 b, uint8 a) {
	if (jamRendererGetInternalRenderer() != NULL) {
		SDL_SetRenderDrawColor(jamRendererGetInternalRenderer(), r, g, b, a);
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer has not been initialized (jamDrawSetColour)");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawGetColour(uint8 *r, uint8 *g, uint8 *b, uint8 *a) {
	if (jamRendererGetInternalRenderer() != NULL) {
		SDL_GetRenderDrawColor(jamRendererGetInternalRenderer(), r, g, b, a);
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer has not been initialized (jamDrawGetColour)");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawRectangle(int x, int y, int w, int h) {
	SDL_Rect rectangle;
	jamCalculateForCamera(&x, &y);

	// Very simple, just check for jamRendererGetInternalRenderer() then draw the rectangle
	if (jamRendererGetInternalRenderer() != NULL) {
		rectangle.x = x;
		rectangle.y = y;
		rectangle.w = w;
		rectangle.h = h;
		SDL_RenderDrawRect(jamRendererGetInternalRenderer(), &rectangle);
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer has not been initialized (jamDrawRectangle)");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawCircle(int x, int y, int r) {
	if (jamRendererGetInternalRenderer() != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer has not been initialized (jamDrawCircle)");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawPolygon(JamPolygon *poly, int x, int y) {
	unsigned int i;
	if (jamRendererGetInternalRenderer() != NULL && poly != NULL) {
		for (i = 0; i < poly->vertices; i++) {
			if (i == 0)
				SDL_RenderDrawLine(jamRendererGetInternalRenderer(), x + (int)poly->xVerts[poly->vertices - 1], y + (int)poly->yVerts[poly->vertices - 1], x + (int)poly->xVerts[0], y + (int)poly->yVerts[0]);
			else
				SDL_RenderDrawLine(jamRendererGetInternalRenderer(), x + (int)poly->xVerts[i - 1], y + (int)poly->yVerts[i - 1], x + (int)poly->xVerts[i], y + (int)poly->yVerts[i]);
		}
	} else {
		if (jamRendererGetInternalRenderer() == NULL)
			jSetError(ERROR_NULL_POINTER, "JamRenderer does not exist.");
		if (poly == NULL)
			jSetError(ERROR_NULL_POINTER, "JamPolygon does not exist.");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawFillColour(uint8 r, uint8 g, uint8 b, uint8 a) {
	uint8 oR, oG, oB, oA;
	SDL_BlendMode texMode;
	SDL_Texture* renderTarget;

	if (jamRendererGetInternalRenderer() != NULL) {
		// Grab the old colour
		SDL_GetRenderDrawColor(jamRendererGetInternalRenderer(), &oR, &oG, &oB, &oA);

		// Get and set blend mode
		renderTarget = SDL_GetRenderTarget(jamRendererGetInternalRenderer());
		SDL_GetTextureBlendMode(renderTarget, &texMode);
		SDL_SetTextureBlendMode(renderTarget, SDL_BLENDMODE_BLEND);

		// Set the new colour
		SDL_SetRenderDrawColor(jamRendererGetInternalRenderer(), r, g, b, a);

		// Draw the actual thing
		SDL_RenderClear(jamRendererGetInternalRenderer());

		// Reset drawing things
		SDL_SetRenderDrawColor(jamRendererGetInternalRenderer(), oR, oG, oB, oA);
		SDL_SetTextureBlendMode(renderTarget, texMode);
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer has not been initialized (jamDrawFillColour)");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawTexture(JamTexture *texture, sint32 x, sint32 y) {
	SDL_Rect dest;
	jamCalculateForCamera(&x, &y);

	// Check for both pieces
	if (jamRendererGetInternalRenderer() != NULL && texture != NULL) {
		dest.x = x;
		dest.y = y;
		dest.w = texture->w;
		dest.h = texture->h;
		SDL_RenderCopy(jamRendererGetInternalRenderer(), texture->tex, NULL, &dest);
	} else {
		if (jamRendererGetInternalRenderer() == NULL)
			jSetError(ERROR_NULL_POINTER, "JamRenderer not present (jamDrawTexture). SDL Error: %s", SDL_GetError());
		if (texture == NULL)
			jSetError(ERROR_NULL_POINTER, "JamTexture not present (jamDrawTexture). SDL Error: %s", SDL_GetError());
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawTileMap(JamTileMap *map, int x, int y, uint32 xInMapStart, uint32 yInMapStart,
					uint32 xInMapFinish, uint32 yInMapFinish) {
	int i, j, originalX;
	originalX = x;
	JamFrame* val;
	if (jamRendererGetInternalRenderer() != NULL && map != NULL) {
		if (xInMapFinish == 0) xInMapFinish = map->width - 1;
		if (yInMapFinish == 0) yInMapFinish = map->height - 1;

		for (i = yInMapStart; i <= yInMapFinish; i++) {
			for (j = xInMapStart; j <= xInMapFinish; j++) {
				val = jamGetMapPos(map, (uint16) j, (uint16) i);
				if (val != NULL)
					jamDrawFrame(val, x, y);
				x += map->cellWidth;
			}
			x = originalX;
			y += map->cellHeight;
		}
	} else {
		if (jamRendererGetInternalRenderer() == NULL)
			jSetError(ERROR_NULL_POINTER, "JamRenderer does not exist (jamDrawTileMap)");
		if (map == NULL)
			jSetError(ERROR_NULL_POINTER, "Map does not exist (jamDrawTileMap)");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawTextureExt(JamTexture *texture, sint32 x, sint32 y, sint32 originX, sint32 originY,
					   float scaleX, float scaleY, double rot, Uint8 alpha) {
	SDL_Rect dest;
	SDL_Point origin;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	Uint8 previousAlpha;
	jamCalculateForCamera(&x, &y);

	// Check for both pieces
	if (jamRendererGetInternalRenderer() != NULL && texture != NULL) {
		// Store, then set alpha
		if (SDL_GetTextureAlphaMod(texture->tex, &previousAlpha) > -1) {
			SDL_SetTextureAlphaMod(texture->tex, alpha);

			// Handle the flip/scale relationship
			if (scaleX < 1) {
				flip = SDL_FLIP_HORIZONTAL;
				scaleX *= -1;
			}
			if (scaleY < 1) {
				flip |= SDL_FLIP_VERTICAL;
				scaleY *= -1;
			}

			// Load up all the values
			dest.x = x;
			dest.y = y;
			dest.w = (int)(texture->w * scaleX);
			dest.h = (int)(texture->h * scaleY);
			origin.x = originX;
			origin.y = originY;

			SDL_RenderCopyEx(jamRendererGetInternalRenderer(), texture->tex, NULL, &dest, rot, &origin, flip);
			SDL_SetTextureAlphaMod(texture->tex, previousAlpha);
		} else {
			jSetError(ERROR_SDL_ERROR, "JamTexture does not support alpha (jamDrawTextureExt). SDL Error: %s", SDL_GetError());
		}
	} else {
		if (jamRendererGetInternalRenderer() == NULL)
			jSetError(ERROR_NULL_POINTER, "JamRenderer not present (jamDrawTextureExt). SDL Error: %s", SDL_GetError());
		if (texture == NULL)
			jSetError(ERROR_NULL_POINTER, "JamTexture not present (jamDrawTextureExt). SDL Error: %s", SDL_GetError());
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawTexturePart(JamTexture *texture, sint32 x, sint32 y, sint32 texX, sint32 texY,
						sint32 texW, sint32 texH) {
	SDL_Rect dest;
	SDL_Rect src;
	jamCalculateForCamera(&x, &y);

	// Check for both pieces
	if (jamRendererGetInternalRenderer() != NULL && texture != NULL) {
		dest.x = x;
		dest.y = y;
		dest.w = texW;
		dest.h = texH;
		src.x = texX;
		src.y = texY;
		src.w = texW;
		src.h = texH;
		SDL_RenderCopy(jamRendererGetInternalRenderer(), texture->tex, &src, &dest);
	} else {
		if (jamRendererGetInternalRenderer() == NULL)
			jSetError(ERROR_NULL_POINTER, "JamRenderer not present (jamDrawTexturePart). SDL Error: %s", SDL_GetError());
		if (texture == NULL)
			jSetError(ERROR_NULL_POINTER, "JamTexture not present (jamDrawTexturePart). SDL Error: %s", SDL_GetError());
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawTexturePartExt(JamTexture *texture, sint32 x, sint32 y, sint32 originX,
						   sint32 originY, float scaleX, float scaleY, double rot, Uint8 alpha, sint32 texX,
						   sint32 texY, sint32 texW, sint32 texH) {
	SDL_Rect dest;
	SDL_Rect src;
	SDL_Point origin;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	Uint8 previousAlpha;
	jamCalculateForCamera(&x, &y);

	// Check for both pieces
	if (jamRendererGetInternalRenderer() != NULL && texture != NULL) {
		// Store, then set alpha
		if (SDL_GetTextureAlphaMod(texture->tex, &previousAlpha) > -1) {
			SDL_SetTextureAlphaMod(texture->tex, alpha);

			// Handle the flip/scale relationship
			if (scaleX < 0) {
				flip = SDL_FLIP_HORIZONTAL;
				scaleX *= -1;
			}
			if (scaleY < 0) {
				flip |= SDL_FLIP_VERTICAL;
				scaleY *= -1;
			}

			// Load up all the values
			dest.x = x;
			dest.y = y;
			dest.w = (int)(texW * scaleX);
			dest.h = (int)(texH * scaleY);
			origin.x = originX;
			origin.y = originY;
			src.x = texX;
			src.y = texY;
			src.w = texW;
			src.h = texH;

			SDL_RenderCopyEx(jamRendererGetInternalRenderer(), texture->tex, &src, &dest, rot, &origin, flip);
			SDL_SetTextureAlphaMod(texture->tex, previousAlpha);
		} else {
			jSetError(ERROR_SDL_ERROR, "JamTexture does not support alpha (jamDrawTextureExt). SDL Error: %s", SDL_GetError());
		}
	} else {
		if (jamRendererGetInternalRenderer() == NULL)
			jSetError(ERROR_NULL_POINTER, "JamRenderer not present (jamDrawTextureExt). SDL Error: %s", SDL_GetError());
		if (texture == NULL)
			jSetError(ERROR_NULL_POINTER, "JamTexture not present (jamDrawTextureExt). SDL Error: %s", SDL_GetError());
	}
}
//////////////////////////////////////////////////////////////
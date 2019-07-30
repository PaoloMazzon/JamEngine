//
// Created by lugi1 on 2018-07-13.
//

#include "Drawing.h"
#include <stdio.h>
#include <Renderer.h>
#include "JamError.h"

//////////////////////////////////////////////////////////////
void drawSetColour(Renderer* renderer, uint8 r, uint8 g, uint8 b, uint8 a) {
	if (renderer != NULL) {
		SDL_SetRenderDrawColor(renderer->internalRenderer, r, g, b, a);
	} else {
		fprintf(stderr, "Passed renderer does not exist (drawSetColour).\n");
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawGetColour(Renderer* renderer, uint8* r, uint8* g, uint8* b, uint8* a) {
	if (renderer != NULL) {
		SDL_GetRenderDrawColor(renderer->internalRenderer, r, g, b, a);
	} else {
		fprintf(stderr, "Passed renderer does not exist (drawGetColour).\n");
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawRectangle(Renderer* renderer, int x, int y, int w, int h) {
	SDL_Rect rectangle;

	// Very simple, just check for renderer then draw the rectangle
	if (renderer != NULL) {
		rectangle.x = x;
		rectangle.y = y;
		rectangle.w = w;
		rectangle.h = h;
		SDL_RenderDrawRect(renderer->internalRenderer, &rectangle);
	} else {
		fprintf(stderr, "Passed renderer does not exist (drawRectangle).\n");
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawCircle(Renderer* renderer, int x, int y, int r) {
	if (renderer != NULL) {
		// TODO: This
	} else {
		fprintf(stderr, "Passed renderer does not exist (drawCircle).\n");
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawFillColour(Renderer* renderer, uint8 r, uint8 g, uint8 b, uint8 a) {
	uint8 oR, oG, oB, oA;
	SDL_BlendMode texMode;
	SDL_Texture* renderTarget;

	if (renderer != NULL) {
		// Grab the old colour
		SDL_GetRenderDrawColor(renderer->internalRenderer, &oR, &oG, &oB, &oA);

		// Get and set blend mode
		renderTarget = SDL_GetRenderTarget(renderer->internalRenderer);
		SDL_GetTextureBlendMode(renderTarget, &texMode);
		SDL_SetTextureBlendMode(renderTarget, SDL_BLENDMODE_BLEND);

		// Set the new colour
		SDL_SetRenderDrawColor(renderer->internalRenderer, r, g, b, a);

		// Draw the actual thing
		SDL_RenderClear(renderer->internalRenderer);

		// Reset drawing things
		SDL_SetRenderDrawColor(renderer->internalRenderer, oR, oG, oB, oA);
		SDL_SetTextureBlendMode(renderTarget, texMode);
	} else {
		fprintf(stderr, "Passed renderer does not exist (drawFillColour).\n");
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawTexture(Renderer* renderer, Texture* texture, sint32 x, sint32 y) {
	SDL_Rect dest;

	// Check for both pieces
	if (renderer != NULL && texture != NULL) {
		dest.x = x;
		dest.y = y;
		dest.w = texture->w;
		dest.h = texture->h;
		SDL_RenderCopy(renderer->internalRenderer, texture->tex, NULL, &dest);
	} else {
		if (renderer == NULL)
			fprintf(stderr, "Renderer not present (drawTexture). SDL Error: %s", SDL_GetError());
		if (texture == NULL)
			fprintf(stderr, "Texture not present (drawTexture). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawSortedMap(Renderer* renderer, Sprite* spr, TileMap* map, int x, int y, uint32 startingCellX, uint32 startingCellY) {
	int i, j;
	bool n, e, s, w, ne, nw, se, sw;
	int frame;

	// First confirm the things exist
	if (renderer != NULL && spr != NULL && map != NULL && (spr->animationLength == 48 || spr->animationLength == 47)) {
		// We gotta run through every cell
		for (i = map->height - 1; i >= 0; i--) {
			for (j = 0; j < map->width; j++) {
				if (getMapPos(map, (uint32)j, (uint32)i)) {
					// Where there are adjacent tiles
					w = (getMapPos(map, (uint32)j - 1, (uint32)i) >= map->collisionRangeStart && getMapPos(map, (uint32)j - 1, (uint32)i) <= map->collisionRangeEnd);
					e = (getMapPos(map, (uint32)j + 1, (uint32)i) >= map->collisionRangeStart && getMapPos(map, (uint32)j + 1, (uint32)i) <= map->collisionRangeEnd);
					n = (getMapPos(map, (uint32)j, (uint32)i - 1) >= map->collisionRangeStart && getMapPos(map, (uint32)j, (uint32)i - 1) <= map->collisionRangeEnd);
					s = (getMapPos(map, (uint32)j, (uint32)i + 1) >= map->collisionRangeStart && getMapPos(map, (uint32)j, (uint32)i + 1) <= map->collisionRangeEnd);
					ne = (getMapPos(map, (uint32)j + 1, (uint32)i - 1) >= map->collisionRangeStart && getMapPos(map, (uint32)j + 1, (uint32)i - 1) <= map->collisionRangeEnd);
					nw = (getMapPos(map, (uint32)j - 1, (uint32)i - 1) >= map->collisionRangeStart && getMapPos(map, (uint32)j - 1, (uint32)i - 1) <= map->collisionRangeEnd);
					se = (getMapPos(map, (uint32)j + 1, (uint32)i + 1) >= map->collisionRangeStart && getMapPos(map, (uint32)j + 1, (uint32)i + 1) <= map->collisionRangeEnd);
					sw = (getMapPos(map, (uint32)j - 1, (uint32)i + 1) >= map->collisionRangeStart && getMapPos(map, (uint32)j - 1, (uint32)i + 1) <= map->collisionRangeEnd);

					// Calculate the tile
					if (!w && !e && !s && n/* && !nw && !ne && !se && !sw*/) {
						frame = 0;
					} else if (!w && e && !s && !n/* && !nw && !ne && !se && !sw*/) {
						frame = 1;
					} else if (!w && !e && s && !n/* && !nw && !ne && !se && !sw*/) {
						frame = 2;
					} else if (w && !e && !s && !n/* && !nw && !ne && !se && !sw*/) {
						frame = 3;
					} else if (!w && e && !s && n && !ne) {
						frame = 4;
					} else if (!w && e && s && !n && !se) {
						frame = 5;
					} else if (w && !e && s && !n && !sw) {
						frame = 6;
					} else if (w && !e && !s && n && !nw) {
						frame = 7;
					} else if (!w && e && !s && n && ne && !sw) {
						frame = 8;
					} else if (!w && e && s && !n && se) {//(!w && e && s && !n && !nw && !ne && se && !sw) {
						frame = 9;
					} else if (w && !e && s && !n && sw) {
						frame = 10;
					} else if (w && !e && !s && n && nw) {
						frame = 11;
					} else if (!w && e && s && n && !ne && !se) {
						frame = 12;
					} else if (w && e && s && !n && !se && !sw) {
						frame = 13;
					} else if (w && !e && s && n && !nw && !sw) {
						frame = 14;
					} else if (w && e && !s && n && !nw && !ne) {
						frame = 15;
					} else if (!w && e && s && n && ne && !se) {
						frame = 16;
					} else if (!w && e && s && n && !ne && se) {
						frame = 17;
					} else if (!w && e && s && n && ne && se) {
						frame = 18;
					} else if (w && e && s && !n && se && !sw) {
						frame = 19;
					} else if (w && e && s && !n && !nw && !se) {
						frame = 20;
					} else if (w && e && s && !n && se && sw) {
						frame = 21;
					} else if (w && !e && s && n && !nw && sw) {
						frame = 22;
					} else if (w && !e && s && n && nw && !sw) {
						frame = 23;
					} else if (w && !e && s && n && nw && sw) {
						frame = 24;
					} else if (w && e && !s && n && !nw && ne) {
						frame = 25;
					} else if (w && e && !s && n && nw && !ne) {
						frame = 26;
					} else if (w && e && !s && n && nw && ne) {
						frame = 27;
					} else if (w && e && s && n && !nw && !ne && !se && !sw) {
						frame = 28;
					} else if (w && e && s && n && !nw && ne && !se && !sw) {
						frame = 29;
					} else if (w && e && s && n && !nw && !ne && se && !sw) {
						frame = 30;
					} else if (w && e && s && n && !nw && !ne && !se && sw) {
						frame = 31;
					} else if (w && e && s && n && nw && !ne && !se && !sw) {
						frame = 32;
					} else if (w && e && s && n && !nw && ne && se && !sw) {
						frame = 33;
					} else if (w && e && s && n && !nw && !ne && se && sw) {
						frame = 34;
					} else if (w && e && s && n && nw && !ne && !se && sw) {
						frame = 35;
					} else if (w && e && s && n && nw && ne && !se && !sw) {
						frame = 36;
					} else if (w && e && s && n && !nw && ne && !se && sw) {
						frame = 37;
					} else if (w && e && s && n && nw && !ne && se && !sw) {
						frame = 38;
					} else if (w && e && s && n && !nw && ne && se && sw) {
						frame = 39;
					} else if (w && e && s && n && nw && ne && se && !sw) {
						frame = 40;
					} else if (w && e && s && n && nw && ne && !se && sw) {
						frame = 41;
					} else if (w && e && s && n && nw && !ne && se && sw) {
						frame = 42;
					} else if (w && e && s && n && nw && ne && se && sw) {
						frame = 43;
					} else if (!w && !e && s && n/* && !nw && !ne && !se && !sw*/) {
						frame = 45;
					} else if (w && e && !s && !n/* && !nw && !ne && !se && !sw*/) {
						frame = 46;
					} else { // No blocks around
						frame = 44;
					}

					drawSpriteFrame(renderer, spr, x + (j * map->cellWidth + startingCellX), y + (i * map->cellHeight + startingCellY), (uint32)frame);
				}
			}
		}
	} else {
		if (renderer == NULL) {
			fprintf(stderr, "Renderer doesn't exist (drawSortedMap)\n");
			jSetError(ERROR_NULL_POINTER);
		}
		if (map == NULL) {
			fprintf(stderr, "Map doesn't exist (drawSortedMap)\n");
			jSetError(ERROR_NULL_POINTER);
		}
		if (spr == NULL) {
			fprintf(stderr, "Sprite doesn't exist (drawSortedMap)\n");
			jSetError(ERROR_NULL_POINTER);
		} else if (!(spr->animationLength == 48 || spr->animationLength == 47)) {
			fprintf(stderr, "Sprite does not contain 48 frames(drawSortedMap)\n");
			jSetError(ERROR_INCORRECT_FORMAT);
		}
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawTextureExt(Renderer* renderer, Texture* texture, sint32 x, sint32 y, sint32 originX, sint32 originY, float scaleX, float scaleY, double rot, Uint8 alpha) {
	SDL_Rect dest;
	SDL_Point origin;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	Uint8 previousAlpha;

	// Check for both pieces
	if (renderer != NULL && texture != NULL) {
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

			SDL_RenderCopyEx(renderer->internalRenderer, texture->tex, NULL, &dest, rot, &origin, flip);
			SDL_SetTextureAlphaMod(texture->tex, previousAlpha);
		} else {
			fprintf(stderr, "Texture does not support alpha (drawTextureExt). SDL Error: %s", SDL_GetError());
			jSetError(ERROR_SDL_ERROR);
		}
	} else {
		if (renderer == NULL)
			fprintf(stderr, "Renderer not present (drawTextureExt). SDL Error: %s", SDL_GetError());
		if (texture == NULL)
			fprintf(stderr, "Texture not present (drawTextureExt). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawTexturePart(Renderer* renderer, Texture* texture, sint32 x, sint32 y, sint32 texX, sint32 texY, sint32 texW, sint32 texH) {
	SDL_Rect dest;
	SDL_Rect src;

	// Check for both pieces
	if (renderer != NULL && texture != NULL) {
		dest.x = x;
		dest.y = y;
		dest.w = texW;
		dest.h = texH;
		src.x = texX;
		src.y = texY;
		src.w = texW;
		src.h = texH;
		SDL_RenderCopy(renderer->internalRenderer, texture->tex, &src, &dest);
	} else {
		if (renderer == NULL)
			fprintf(stderr, "Renderer not present (drawTexturePart). SDL Error: %s", SDL_GetError());
		if (texture == NULL)
			fprintf(stderr, "Texture not present (drawTexturePart). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawTileMap(Renderer* renderer, TileMap* tileMap, int x, int y, uint16 xInMapStart, uint16 yInMapStart, uint16 xInMapFinish, uint16 yInMapFinish) {
	// TODO: This
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawTexturePartExt(Renderer* renderer, Texture* texture, sint32 x, sint32 y, sint32 originX, sint32 originY, float scaleX, float scaleY, double rot, Uint8 alpha, sint32 texX, sint32 texY, sint32 texW, sint32 texH) {
	SDL_Rect dest;
	SDL_Rect src;
	SDL_Point origin;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	Uint8 previousAlpha;

	// Check for both pieces
	if (renderer != NULL && texture != NULL) {
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

			SDL_RenderCopyEx(renderer->internalRenderer, texture->tex, &src, &dest, rot, &origin, flip);
			SDL_SetTextureAlphaMod(texture->tex, previousAlpha);
		} else {
			fprintf(stderr, "Texture does not support alpha (drawTextureExt). SDL Error: %s", SDL_GetError());
			jSetError(ERROR_SDL_ERROR);
		}
	} else {
		if (renderer == NULL)
			fprintf(stderr, "Renderer not present (drawTextureExt). SDL Error: %s", SDL_GetError());
		if (texture == NULL)
			fprintf(stderr, "Texture not present (drawTextureExt). SDL Error: %s", SDL_GetError());
		jSetError(ERROR_NULL_POINTER);
	}
}
//////////////////////////////////////////////////////////////
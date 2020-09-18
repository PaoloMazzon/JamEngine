//
// Created by lugi1 on 2018-07-13.
//

#include "Drawing.h"
#include <Renderer.h>
#include <TileMap.h>
#include <SDL.h>

//////////////////////////////////////////////////////////////
void jamDrawSetColour(uint8 r, uint8 g, uint8 b, uint8 a) {
	vec4 colour = {(float)r / 255, (float)g / 255, (float)b / 255, (float)a / 255};
	vk2dRendererSetColourMod(colour);
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawGetColour(uint8 *r, uint8 *g, uint8 *b, uint8 *a) {
	vec4 colour;
	vk2dRendererGetColourMod(colour);
	*r = (uint8)roundf(colour[0] * 255);
	*g = (uint8)roundf(colour[1] * 255);
	*b = (uint8)roundf(colour[2] * 255);
	*a = (uint8)roundf(colour[3] * 255);
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawRectangle(int x, int y, int w, int h) {
	vk2dRendererDrawRectangleOutline(x, y, w, h, 0, 0, 0, 1);
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawRectangleFilled(int x, int y, int w, int h) {
	vk2dRendererDrawRectangle(x, y, w, h, 0, 0, 0);
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawFillColour(uint8 r, uint8 g, uint8 b, uint8 a) {
	uint8 oR, oG, oB, oA;

	// Grab the old colour
	jamDrawGetColour(&oR, &oG, &oB, &oA);
	jamDrawSetColour(r, g, b, a);

	// Get and set blend mode
	vk2dRendererClear();

	// Reset drawing things
	jamDrawSetColour(oR, oG, oB, oA);
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawTexture(JamTexture *texture, sint32 x, sint32 y) {
	vk2dRendererDrawTexture(texture->tex, x, y, 1, 1, 0, 0, 0);
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawTileMap(JamTileMap *map, int x, int y, uint32 xInMapStart, uint32 yInMapStart, uint32 xInMapFinish, uint32 yInMapFinish) {
	int i, j, originalX;
	originalX = x;
	JamFrame* val;

	if (xInMapFinish == 0) xInMapFinish = map->width - 1;
	if (yInMapFinish == 0) yInMapFinish = map->height - 1;

	for (i = yInMapStart; i <= yInMapFinish; i++) {
		for (j = xInMapStart; j <= xInMapFinish; j++) {
			val = jamTileMapGet(map, (uint16) j, (uint16) i);
			if (val != NULL)
				jamDrawFrame(val, x, y);
			x += map->cellWidth;
		}
		x = originalX;
		y += map->cellHeight;
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawTextureExt(JamTexture *texture, sint32 x, sint32 y, sint32 originX, sint32 originY,
					   float scaleX, float scaleY, double rot, Uint8 alpha) {
	Uint8 r, g, b, a;
	jamDrawGetColour(&r, &g, &b, &a);
	jamDrawSetColour(r, g, b, alpha);

	vk2dRendererDrawTexture(texture->tex, x - originX, y - originY, scaleX, scaleY, rot, originX, originY);

	jamDrawSetColour(r, g, b, a);
}
//////////////////////////////////////////////////////////////

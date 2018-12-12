//
// Created by lugi1 on 2018-07-14.
//

#include "Frame.h"
#include "Drawing.h"
#include "Texture.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////
Frame* createFrame(Texture* tex, sint32 x, sint32 y, sint32 w, sint32 h) {
	Frame* frame = (Frame*)malloc(sizeof(Frame));

	// Check for its validity
	if (frame != NULL) {
		frame->tex = tex;
		frame->x = x;
		frame->y = y;
		frame->w = w;
		frame->h = h;
	} else {
		fprintf(stderr, "Could not allocate frame. (createFrame).\n");
	}

	return frame;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void drawFrame(Frame* frame, Renderer* renderer, sint32 x, sint32 y) {
	if (frame != NULL && renderer != NULL) {
		drawTexturePart(renderer, frame->tex, x, y, frame->x, frame->y, frame->w, frame->h);
	} else {
		if (frame != NULL)
			fprintf(stderr, "Frame does not exist. (drawFrame).\n");
		if (renderer != NULL)
			fprintf(stderr, "Renderer does not exist. (drawFrame).\n");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void freeFrame(Frame* frame, bool destroyTexture) {
	if (frame != NULL) {
		if (destroyTexture) {
			freeTexture(frame->tex);
		}
		free(frame);
	}
}
//////////////////////////////////////////////////////////////
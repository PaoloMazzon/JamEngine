//
// Created by lugi1 on 2018-07-14.
//

#include "Frame.h"
#include "Drawing.h"
#include "Texture.h"
#include <stdio.h>
#include <malloc.h>
#include <JamError.h>

//////////////////////////////////////////////////////////////
JamFrame* jamCreateFrame(JamTexture *tex, sint32 x, sint32 y, sint32 w, sint32 h) {
	JamFrame* frame = (JamFrame*)malloc(sizeof(JamFrame));

	// Check for its validity
	if (frame != NULL) {
		frame->tex = tex;
		frame->x = x;
		frame->y = y;
		frame->w = w;
		frame->h = h;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Could not allocate frame. (jamCreateFrame)");
	}

	return frame;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawFrame(JamFrame *frame, sint32 x, sint32 y) {
	if (frame != NULL) {
		jamDrawTexturePart(frame->tex, x, y, frame->x, frame->y, frame->w, frame->h);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamFrame does not exist. (jamDrawFrame)");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamFreeFrame(JamFrame *frame, bool destroyTexture) {
	if (frame != NULL) {
		if (destroyTexture) {
			jamFreeTexture(frame->tex);
		}
		free(frame);
	}
}
//////////////////////////////////////////////////////////////
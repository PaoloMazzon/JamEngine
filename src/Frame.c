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
JamFrame* jamFrameCreate(JamTexture *tex, sint32 x, sint32 y, sint32 w, sint32 h) {
	JamFrame* frame = (JamFrame*)malloc(sizeof(JamFrame));

	// Check for its validity
	if (frame != NULL) {
		frame->tex = vk2dTextureLoad(tex->tex->img, x, y, w, h);
		frame->x = x;
		frame->y = y;
		frame->w = w;
		frame->h = h;
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Could not allocate frame. (jamFrameCreate)");
	}

	return frame;
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamDrawFrame(JamFrame *frame, sint32 x, sint32 y) {
	if (frame != NULL) {
		vk2dRendererDrawTexture(frame->tex, x, y, 1, 1, 0, 0, 0);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamFrame does not exist. (jamDrawFrame)");
	}
}
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void jamFrameFree(JamFrame *frame, bool destroyTexture) {
	if (frame != NULL) {
		if (destroyTexture) {
			jamTextureFree(frame->jtex);
		}
		vk2dTextureFree(frame->tex);
		free(frame);
	}
}
//////////////////////////////////////////////////////////////
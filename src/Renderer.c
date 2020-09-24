//
// Created by lugi1 on 2018-07-05.
//

#include "Renderer.h"
#include "Texture.h"
#include "Clock.h"
#include "JamError.h"
#include <SDL.h>
#include <Audio.h>
#include "Font.h"
#include "WorldHandler.h"

static JamRenderer* gRenderer;

/////////////////////////////////////////////////////////////
void jamRendererInit(int *argc, char **argv, const char *name, uint32 w, uint32 h, double framerate, JamRenderConfig *config) {
	SDL_Window* window;
	VK2DTexture tex;
	gRenderer = (JamRenderer*)malloc(sizeof(JamRenderer));
	jamAudioInit(argc, argv);
	jamWorldHandlerInit();

	// Check if we were given a dud
	if (gRenderer != NULL && jamAudioIsInitialized()) {

		// Ignore any complaints valgrind gives about this, its out
		// of my control
		SDL_Init(SDL_INIT_EVERYTHING);

		// Input depends on SDL being initialized
		jamInputInit();

		// Make the window centered on the screen, and as an OpenGL window
		window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_VULKAN);
		gRenderer->gameWindow = window;

		// Check for a dud again
		if (window != NULL && jamInputIsInitialized()) {
			int32_t res = vk2dRendererInit(window, *config);
			gRenderer->config = *config;

			// Once again, check for a dud
			if (res >= 0) {
				// Last piece - create the internal texture to draw to
				tex = vk2dTextureCreate(vk2dRendererGetDevice(), w, h);

				// More dud checks
				if (tex != NULL) {
					// Load up the gRenderer struct
					gRenderer->screenBuffer = tex;
					gRenderer->sleepEnabled = true;
					if (framerate != 0)
						gRenderer->timePerFrame = 1000000000 / (uint64_t)framerate;
					else
						gRenderer->sleepEnabled = false;
					gRenderer->lastTime = ns();
					gRenderer->delta = 1;
					gRenderer->deltaCap = 2;
					gRenderer->renderingToScreenBuffer = true;
					gRenderer->camera.zoom = 1;
					gRenderer->camera.x = 0;
					gRenderer->camera.y = 0;
					gRenderer->camera.w = w;
					gRenderer->camera.h = h;
					gRenderer->camera.rot = 0;
					vk2dRendererSetCamera(gRenderer->camera);
				} else {
					jamRendererQuit();
					gRenderer = NULL;
					jSetError(ERROR_SDL_ERROR, "Failed to create internal texture (jamRendererInit). SDL Error: %s\n", SDL_GetError());
				}
			} else {
				jamRendererQuit();
				gRenderer = NULL;
				jSetError(ERROR_SDL_ERROR, "Failed to create SDL gRenderer (jamRendererInit). SDL Error: %s\n", SDL_GetError());
			}
		} else {
			jamRendererQuit();
			gRenderer = NULL;
			jSetError(ERROR_SDL_ERROR, "Failed to create SDL window (jamRendererInit). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		free(gRenderer);
		jSetError(ERROR_ALLOC_FAILED, "Failed to create gRenderer struct. SDL Error: %s\n", SDL_GetError());
	}
}
/////////////////////////////////////////////////////////////

SDL_Surface* jamSDLSurfaceLoad(const char* filename);

/////////////////////////////////////////////////////////////
void jamRendererSetIcon(const char* imageFilename) {
	if (gRenderer != NULL) {
		SDL_Surface *surf = jamSDLSurfaceLoad(imageFilename);
		SDL_SetWindowIcon(gRenderer->gameWindow, surf);
		jamSDLSurfaceLoad(NULL);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
uint32 jamRendererGetBufferWidth() {
	if (gRenderer != NULL) {
		return gRenderer->screenBuffer->img->width;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}

	return 0;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
uint32 jamRendererGetBufferHeight() {
	if (gRenderer != NULL) {
		return gRenderer->screenBuffer->img->height;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}

	return 0;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
double jamRendererGetCameraX() {
	if (gRenderer != NULL) {
		return gRenderer->camera.x;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
	return 0;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
double jamRendererGetCameraY() {
	if (gRenderer != NULL) {
		return gRenderer->camera.y;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
	return 0;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererSetCameraPos(double x, double y) {
	if (gRenderer != NULL) {
		gRenderer->camera.x = x;
		gRenderer->camera.y = y;
		vk2dRendererSetCamera(gRenderer->camera);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererMoveCamera(double x_relative, double y_relative) {
	if (gRenderer != NULL) {
		gRenderer->camera.x += x_relative;
		gRenderer->camera.y += y_relative;
		vk2dRendererSetCamera(gRenderer->camera);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
double jamRendererGetFramerate() {
	if (gRenderer != NULL) {
		return gRenderer->framerate;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
	return 0;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool jamRendererReset(uint32 windowWidth, uint32 windowHeight, uint8 fullscreen) {
	// TODO: This
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererIntegerScale() {
	// TODO: This
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererSetVSync(bool vsync) {
	if (gRenderer != NULL) {
		if (vsync)
			SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
		else
			SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererSetAA(bool aa) {
	if (gRenderer != NULL) {
		gRenderer->config.filterMode = aa == true ? ft_Linear : ft_Nearest;
		vk2dRendererSetConfig(gRenderer->config);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererSetTitle(const char *name) {
	if (gRenderer != NULL) {
		SDL_SetWindowTitle(gRenderer->gameWindow, name);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererSetFramerate(double framerate) {
	if (gRenderer != NULL) {
		if (framerate != 0) {
			gRenderer->timePerFrame = 1000000000 / (uint64_t) framerate;
			gRenderer->sleepEnabled = true;
		} else {
			gRenderer->sleepEnabled = false;
		}
		gRenderer->fps = (uint64_t) framerate;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool jamRendererTargetIsScreenBuffer() {
	bool ret = false;

	if (gRenderer != NULL) {
		ret = gRenderer->renderingToScreenBuffer;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamRendererTargetIsScreenBuffer)");
	}

	return ret;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererQuit() {
	if (gRenderer != NULL) {
		vk2dRendererWait();
		vk2dTextureFree(gRenderer->screenBuffer);
		jamInputQuit();
		jamAudioQuit();
		jamFontQuit();
		jamWorldHandlerQuit();
		vk2dRendererQuit();
		SDL_DestroyWindow(gRenderer->gameWindow);
		SDL_Quit();
		free(gRenderer);
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool jamRendererProcEvents() {
	SDL_Event event;
	bool ret = true;
	int wW = 0;

	if (gRenderer != NULL) {
		// Update input
		SDL_GetWindowSize(gRenderer->gameWindow, &wW, NULL);
		jamInputUpdate((double) wW / gRenderer->screenBuffer->img->width);

		SDL_PumpEvents();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				ret = false;
		}

		vec4 colour = {0, 0, 0, 1};
		vk2dRendererStartFrame(colour);
		vk2dRendererSetTarget(gRenderer->screenBuffer);
		vk2dRendererClear();
		vk2dRendererSetTextureCamera(true);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamRendererProcEvents)");
	}

	return ret;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererSetTarget(JamTexture *texture) {
	if (gRenderer != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamRendererSetTarget)");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererConfig(uint32 internalWidth, uint32 internalHeight, uint32 displayWidth,
					   uint32 displayHeight) {
	if (gRenderer != NULL) {
		// TODO: This
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamRendererConfig)");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererConvertCoords(int *x, int *y) {
	SDL_DisplayMode mode;
	double widthRatio, heightRatio;
	int rX = *x;
	int rY = *y;

	if (gRenderer != NULL) {
		// First we need the display mode
		SDL_GetWindowDisplayMode(gRenderer->gameWindow, &mode);

		// Calculate the width and height ratio
		widthRatio = (double)gRenderer->screenBuffer->img->width / (gRenderer->camera.w / gRenderer->camera.zoom);
		heightRatio = (double)gRenderer->screenBuffer->img->height / (gRenderer->camera.h / gRenderer->camera.zoom);
		float vx, vy, vw, vh;
		vk2dRendererGetViewport(&vx, &vy, &vw, &vh);

		// Convert the two numbers
		*x = (int)(((double)rX - vx) * widthRatio);
		*y = (int)(((double)rY - vy) * heightRatio);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamRendererConvertCoords)");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
double jamRendererSetDeltaCap(double cap) {
	if (gRenderer != NULL) {
		return gRenderer->deltaCap = cap;
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer has not been initialized");
	}

	return 1;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
double jamRendererGetDelta() {
	if (gRenderer != NULL) {
		return gRenderer->delta <= gRenderer->deltaCap ? gRenderer->delta : gRenderer->deltaCap;
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer has not been initialized");
	}

	return 1;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererProcEndFrame() {
	if (gRenderer != NULL) {
		vk2dRendererSetTarget(VK2D_TARGET_SCREEN);
		vk2dRendererDrawTexture(gRenderer->screenBuffer, 0, 0, 1, 1, 0, 0, 0);
		vk2dRendererEndFrame();

		// Calculate time in between frames
		gRenderer->between = ns() - gRenderer->lastTime;

		// Sleep for any remaining time/calculate framerate
		if (gRenderer->sleepEnabled)
			if (gRenderer->between < gRenderer->timePerFrame)
				jamSleep(gRenderer->timePerFrame - gRenderer->between);

		// Calculate the framerate and delta
		gRenderer->framerate = 1000000000 / ((double)ns() - (double)gRenderer->lastTime);
		gRenderer->delta = ((double)ns() - (double)gRenderer->lastTime) / (1000000000.0 / 60.0);

		// Update the last time
		gRenderer->lastTime = ns();
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamRendererProcEndFrame)");
	}
}
/////////////////////////////////////////////////////////////


//
// Created by lugi1 on 2018-07-05.
//

#include "Renderer.h"
#include "Texture.h"
#include <stdio.h>
#include <Renderer.h>
#include "Clock.h"
#include "JamError.h"
#include <SDL.h>
#include <Audio.h>

static JamRenderer* gRenderer;

/////////////////////////////////////////////////////////////
void jamInitRenderer(int* argc, char** argv, const char *name, uint32 w, uint32 h, double framerate) {
	SDL_Renderer* sdlRenderer;
	SDL_Window* window;
	JamTexture* tex;
	gRenderer = (JamRenderer*)malloc(sizeof(JamRenderer));
	jamInitInput();
	jamInitAudioPlayer(argc, argv);

	// Check if we were given a dud
	if (gRenderer != NULL && jamInputIsInitialized() && jamAudioIsInitialized()) {

		// Ignore any complaints valgrind gives about this, its out
		// of my control
		SDL_Init(SDL_INIT_EVERYTHING);

		// Make the window centered on the screen, and as an OpenGL window
		window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
		gRenderer->gameWindow = window;

		// Check for a dud again
		if (window != NULL) {
			// Make the internal gRenderer with a couple bits
			//  - It can draw to textures
			//  - It is hardware accelerated
			sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
			gRenderer->internalRenderer = sdlRenderer;

			// Once again, check for a dud
			if (sdlRenderer != NULL) {
				// Last piece - create the internal texture to draw to
				tex = jamCreateTexture(w, h);

				// More dud checks
				if (tex != NULL) {
					// Load up the gRenderer struct
					gRenderer->screenBuffer = tex;
					gRenderer->internalRenderer = sdlRenderer;
					gRenderer->timePerFrame = 1000000000 / (uint64_t)framerate;
					gRenderer->lastTime = ns();
					gRenderer->displayBufferW = w;
					gRenderer->displayBufferH = h;
					gRenderer->displayBufferX = 0;
					gRenderer->displayBufferY = 0;
					gRenderer->cameraX = 0;
					gRenderer->cameraY = 0;
					gRenderer->delta = 1;
					SDL_SetRenderTarget(sdlRenderer, tex->tex);
				} else {
					jamRendererQuit();
					gRenderer = NULL;
					jamFreeTexture(tex);
					jSetError(ERROR_SDL_ERROR, "Failed to create internal texture (jamInitRenderer). SDL Error: %s\n", SDL_GetError());
				}
			} else {
				jamRendererQuit();
				gRenderer = NULL;
				jSetError(ERROR_SDL_ERROR, "Failed to create SDL gRenderer (jamInitRenderer). SDL Error: %s\n", SDL_GetError());
			}
		} else {
			jamRendererQuit();
			gRenderer = NULL;
			jSetError(ERROR_SDL_ERROR, "Failed to create SDL window (jamInitRenderer). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		free(gRenderer);
		jSetError(ERROR_ALLOC_FAILED, "Failed to create gRenderer struct. SDL Error: %s\n", SDL_GetError());
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void* jamRendererGetInternalRenderer() {
	if (gRenderer != NULL) {
		return gRenderer->internalRenderer;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
	return NULL;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
double jamRendererGetCameraX() {
	if (gRenderer != NULL) {
		return gRenderer->cameraX;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
	return 0;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
double jamRendererGetCameraY() {
	if (gRenderer != NULL) {
		return gRenderer->cameraY;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
	return 0;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererSetCameraPos(double x, double y) {
	if (gRenderer != NULL) {
		gRenderer->cameraX = x;
		gRenderer->cameraY = y;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererMoveCamera(double x_relative, double y_relative) {
	if (gRenderer != NULL) {
		gRenderer->cameraX += x_relative;
		gRenderer->cameraY += y_relative;
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
bool jamResetRenderer(uint32 windowWidth, uint32 windowHeight, uint8 fullscreen) {
	SDL_DisplayMode mode;
	bool pass = false;
	int w, h;

	// The preliminary check
	if (gRenderer != NULL) {

		if (fullscreen != 0) {
			if (fullscreen == 1) {
				SDL_SetWindowFullscreen(gRenderer->gameWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
			} else if (fullscreen == 2) {
				// We have to manually set the window to monitor size before going fullscreen
				SDL_GetDesktopDisplayMode(0, &mode);
				SDL_SetWindowFullscreen(gRenderer->gameWindow, SDL_WINDOW_FULLSCREEN);
				SDL_SetWindowDisplayMode(gRenderer->gameWindow, &mode);
			}

			if (fullscreen < 3) {
				// Calculate where on the screen the display buffer should be drawn (center the display buffer)
				SDL_GetWindowSize(gRenderer->gameWindow, &w, &h);
				gRenderer->displayBufferX = (w - gRenderer->displayBufferW) / 2;
				gRenderer->displayBufferY = (h - gRenderer->displayBufferH) / 2;

				// Check for errors
				if (gRenderer->screenBuffer == NULL) {
					jSetError(ERROR_SDL_ERROR,
							  "Failed to create screen buffer in fullscreen (jamResetRenderer). SDL Error: %s\n",
							  SDL_GetError());
				} else {
					pass = true;
				}
			}
		} else {
			// Take it out of full screen and handle sizing and position
			SDL_SetWindowFullscreen(gRenderer->gameWindow, 0);
			SDL_SetWindowSize(gRenderer->gameWindow, windowWidth, windowHeight);
			SDL_SetWindowPosition(gRenderer->gameWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			gRenderer->displayBufferX = 0;
			gRenderer->displayBufferY = 0;

			// Check for errors
			if (gRenderer->screenBuffer == NULL) {
				jSetError(ERROR_SDL_ERROR, "Failed to create screen buffer (jamResetRenderer). SDL Error: %s\n", SDL_GetError());
			} else {
				pass = true;
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamProcEvents)");
	}

	return pass;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamIntegerMaximizeScreenBuffer() {
	int w, h;
	uint32 scale = 1;

	if (gRenderer != NULL) {
		// Get the window size and find the highest scale
		SDL_GetWindowSize(gRenderer->gameWindow, &w, &h);
		while ((scale + 1) * gRenderer->screenBuffer->w < w && (scale + 1) * gRenderer->screenBuffer->h < h)
			scale++;

		// Scale the buffer to max and adjust the display coordinates
		gRenderer->displayBufferW = gRenderer->screenBuffer->w * scale;
		gRenderer->displayBufferH = gRenderer->screenBuffer->h * scale;
		gRenderer->displayBufferX = (w - gRenderer->displayBufferW) / 2; // Problem
		gRenderer->displayBufferY = (h - gRenderer->displayBufferH) / 2;
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamSetVSync(bool vsync) {
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
void jamSetAA(bool aa) {
	if (gRenderer != NULL) {
		if (aa)
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		else
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamSetWindowTitle(const char *name) {
	if (gRenderer != NULL) {
		SDL_SetWindowTitle(gRenderer->gameWindow, name);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamSetFramerate(double framerate) {
	if (gRenderer != NULL) {
		gRenderer->timePerFrame = 1000000000 / (uint64_t) framerate;
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

//////////////////////////////////////////////////////////////
void jamCalculateForCamera(int *x, int *y) {
	if (jamRendererTargetIsScreenBuffer()) {
		*x -= gRenderer->cameraX;
		*y -= gRenderer->cameraY;
	}
}
//////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamRendererQuit() {
	if (gRenderer != NULL) {
		jamFreeTexture(gRenderer->screenBuffer);
		jamQuitInput();
		jamFreeAudioPlayer();
		SDL_DestroyRenderer(gRenderer->internalRenderer);
		SDL_DestroyWindow(gRenderer->gameWindow);
		SDL_Quit();
		free(gRenderer);
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool jamProcEvents() {
	SDL_Event event;
	bool ret = true;
	int wW = 0;

	// The preliminary check
	if (gRenderer != NULL) {
		// Update input
		SDL_GetWindowSize(gRenderer->gameWindow, &wW, NULL);
		jamUpdateInput((double) wW / gRenderer->screenBuffer->w);

		// Update the events queue and keyboard and etc you get the point
		SDL_PumpEvents();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				ret = false;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamProcEvents)");
	}

	return ret;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamSetRenderTarget(JamTexture *texture) {
	// The preliminary check
	if (gRenderer != NULL) {
		if (texture == NULL) {
			gRenderer->renderingToScreenBuffer = true;
			SDL_SetRenderTarget(gRenderer->internalRenderer, gRenderer->screenBuffer->tex);
		} else {
			gRenderer->renderingToScreenBuffer = false;
			SDL_SetRenderTarget(gRenderer->internalRenderer, texture->tex);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamSetRenderTarget)");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool jamConfigScreenBuffer(uint32 internalWidth, uint32 internalHeight, uint32 displayWidth,
						   uint32 displayHeight) {
	bool pass = false;
	JamTexture* tempTex;
	int w, h;

	// Check for gRenderer
	if (gRenderer != NULL) {
		// Create the texture that will likely become the new screen buffer
		tempTex = jamCreateTexture(internalWidth, internalHeight);

		// Check that it worked
		if (tempTex != NULL) {
			// Get display info
			SDL_GetWindowSize(gRenderer->gameWindow, &w, &h);

			// Free the old one, then update values
			jamFreeTexture(gRenderer->screenBuffer);

			gRenderer->screenBuffer = tempTex;
			gRenderer->displayBufferW = displayWidth;
			gRenderer->displayBufferH = displayHeight;
			gRenderer->displayBufferX = (w - displayWidth) / 2;
			gRenderer->displayBufferY = (h - displayHeight) / 2;
		} else {
			jSetError(ERROR_SDL_ERROR, "Failed to create screen buffer (jamConfigScreenBuffer).");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamConfigScreenBuffer)");
	}

	return pass;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamConvertCoords(int *x, int *y) {
	SDL_DisplayMode mode;
	double widthRatio, heightRatio;
	int rX = *x;
	int rY = *y;

	if (gRenderer != NULL) {
		// First we need the display mode
		SDL_GetWindowDisplayMode(gRenderer->gameWindow, &mode);

		// Calculate the width and height ratio
		widthRatio = (double)gRenderer->screenBuffer->w / (double)gRenderer->displayBufferW;
		heightRatio = (double)gRenderer->screenBuffer->h / (double)gRenderer->displayBufferH;

		// Convert the two numbers
		*x = (int)(((double)rX - (double)gRenderer->displayBufferX) * widthRatio);
		*y = (int)(((double)rY - (double)gRenderer->displayBufferY) * heightRatio);
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamConvertCoords)");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
double jamRendererGetDelta() {
	if (gRenderer != NULL) {
		return gRenderer->delta;
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer has not been initialized");
	}

	return 1;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void jamProcEndFrame() {
	SDL_Rect rect;
	Uint8 r, g, b, a;

	// The preliminary check
	if (gRenderer != NULL) {
		rect.x = gRenderer->displayBufferX;
		rect.y = gRenderer->displayBufferY;
		rect.w = gRenderer->displayBufferW;
		rect.h = gRenderer->displayBufferH;

		// Set the render target to the screen
		SDL_SetRenderTarget(gRenderer->internalRenderer, NULL);

		// Draw the texture to screen
		SDL_RenderCopy(gRenderer->internalRenderer, gRenderer->screenBuffer->tex, NULL, &rect);

		// Draw a white border around the drawn screen
		rect.x = gRenderer->displayBufferX - 1;
		rect.y = gRenderer->displayBufferY - 1;
		rect.w = gRenderer->displayBufferW + 2;
		rect.h = gRenderer->displayBufferH + 2;
		SDL_GetRenderDrawColor(gRenderer->internalRenderer, &r, &g, &b, &a);
		SDL_SetRenderDrawColor(gRenderer->internalRenderer, 255, 255, 255, 255);
		SDL_RenderDrawRect(gRenderer->internalRenderer, &rect);
		SDL_SetRenderDrawColor(gRenderer->internalRenderer, r, g, b, a);

		// Update the screen
		SDL_RenderPresent(gRenderer->internalRenderer);

		// Set back to black then clear screen
		SDL_SetRenderDrawColor(gRenderer->internalRenderer, 0, 0, 0, 255);
		SDL_RenderClear(gRenderer->internalRenderer);

		// Reset to backbuffer
		SDL_SetRenderTarget(gRenderer->internalRenderer, gRenderer->screenBuffer->tex);
		SDL_RenderClear(gRenderer->internalRenderer);

		// Calculate time in between frames
		gRenderer->between = ns() - gRenderer->lastTime;

		// Sleep for any remaining time/calculate framerate
		if (gRenderer->between < gRenderer->timePerFrame)
			jamSleep(gRenderer->timePerFrame - gRenderer->between);

		// Calculate the framerate and delta
		gRenderer->framerate = 1000000000 / ((double)ns() - (double)gRenderer->lastTime);
		gRenderer->delta = (double)(ns() - gRenderer->lastTime) / ((double)gRenderer->timePerFrame);

		// Update the last time
		gRenderer->lastTime = ns();
	} else {
		jSetError(ERROR_NULL_POINTER, "JamRenderer has not been initialized (jamProcEndFrame)");
	}
}
/////////////////////////////////////////////////////////////


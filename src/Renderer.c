//
// Created by lugi1 on 2018-07-05.
//

#include "Renderer.h"
#include "Texture.h"
#include <stdio.h>
#include <SDL_video.h>
#include <Renderer.h>
#include "Clock.h"
#include "JamError.h"

/////////////////////////////////////////////////////////////
Renderer* createRenderer(const char* name, uint32 w, uint32 h, double framerate) {
	SDL_Renderer* sdlRenderer;
	SDL_Window* window;
	Texture* tex;
	Renderer* renderer = (Renderer*)malloc(sizeof(Renderer));

	// Check if we were given a dud
	if (renderer != NULL) {

		// Ignore any complaints valgrind gives about this, its out
		// of my control
		SDL_Init(SDL_INIT_EVERYTHING);

		// Make the window centered on the screen, and as an OpenGL window
		window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL);
		renderer->gameWindow = window;

		// Check for a dud again
		if (window != NULL) {
			// Make the internal renderer with a couple bits
			//  - It can draw to textures
			//  - It is hardware accelerated
			sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
			renderer->internalRenderer = sdlRenderer;

			// Once again, check for a dud
			if (sdlRenderer != NULL) {
				// Last piece - create the internal texture to draw to
				tex = createTexture(renderer, w, h);

				// More dud checks
				if (tex != NULL) {
					// Load up the renderer struct
					renderer->screenBuffer = tex;
					renderer->internalRenderer = sdlRenderer;
					renderer->timePerFrame = 1000000000 / (uint64_t)framerate;
					renderer->lastTime = ns();
					renderer->displayBufferW = w;
					renderer->displayBufferH = h;
					renderer->displayBufferX = 0;
					renderer->displayBufferY = 0;
					renderer->cameraX = 0;
					renderer->cameraY = 0;
					SDL_SetRenderTarget(sdlRenderer, tex->tex);
				} else {
					freeRenderer(renderer);
					renderer = NULL;
					freeTexture(tex);
					jSetError(ERROR_SDL_ERROR, "Failed to create internal texture (createRenderer). SDL Error: %s\n", SDL_GetError());
				}
			} else {
				freeRenderer(renderer);
				renderer = NULL;
				jSetError(ERROR_SDL_ERROR, "Failed to create SDL renderer (createRenderer). SDL Error: %s\n", SDL_GetError());
			}
		} else {
			freeRenderer(renderer);
			renderer = NULL;
			jSetError(ERROR_SDL_ERROR, "Failed to create SDL window (createRenderer). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to create renderer struct. SDL Error: %s\n", SDL_GetError());
	}

	return renderer;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool resetWindow(Renderer* renderer, uint32 windowWidth, uint32 windowHeight, uint8 fullscreen) {
	SDL_DisplayMode mode;
	bool pass = false;
	int w, h;

	// The preliminary check
	if (renderer != NULL) {

		if (fullscreen != 0) {
			if (fullscreen == 1) {
				SDL_SetWindowFullscreen(renderer->gameWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
			} else if (fullscreen == 2) {
				// We have to manually set the window to monitor size before going fullscreen
				SDL_GetDesktopDisplayMode(0, &mode);
				SDL_SetWindowFullscreen(renderer->gameWindow, SDL_WINDOW_FULLSCREEN);
				SDL_SetWindowDisplayMode(renderer->gameWindow, &mode);
			}

			if (fullscreen < 3) {
				// Calculate where on the screen the display buffer should be drawn (center the display buffer)
				SDL_GetWindowSize(renderer->gameWindow, &w, &h);
				renderer->displayBufferX = (w - renderer->displayBufferW) / 2;
				renderer->displayBufferY = (h - renderer->displayBufferH) / 2;

				// Check for errors
				if (renderer->screenBuffer == NULL) {
					jSetError(ERROR_SDL_ERROR,
							  "Failed to create screen buffer in fullscreen (resetWindow). SDL Error: %s\n",
							  SDL_GetError());
				} else {
					pass = true;
				}
			}
		} else {
			// Take it out of full screen and handle sizing and position
			SDL_SetWindowFullscreen(renderer->gameWindow, 0);
			SDL_SetWindowSize(renderer->gameWindow, windowWidth, windowHeight);
			SDL_SetWindowPosition(renderer->gameWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
			renderer->displayBufferX = 0;
			renderer->displayBufferY = 0;

			// Check for errors
			if (renderer->screenBuffer == NULL) {
				jSetError(ERROR_SDL_ERROR, "Failed to create screen buffer (resetWindow). SDL Error: %s\n", SDL_GetError());
			} else {
				pass = true;
			}
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist (rendererProcEvents)");
	}

	return pass;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void rendererMaximizeScreenBufferInteger(Renderer* renderer) {
	int w, h;
	uint32 scale = 1;

	if (renderer != NULL) {
		// Get the window size and find the highest scale
		SDL_GetWindowSize(renderer->gameWindow, &w, &h);
		while ((scale + 1) * renderer->screenBuffer->w < w && (scale + 1) * renderer->screenBuffer->h < h)
			scale++;

		// Scale the buffer to max and adjust the display coordinates
		renderer->displayBufferW = renderer->screenBuffer->w * scale;
		renderer->displayBufferH = renderer->screenBuffer->h * scale;
		renderer->displayBufferX = (w - renderer->displayBufferW) / 2; // Problem
		renderer->displayBufferY = (h - renderer->displayBufferH) / 2;
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void rendererSetVerticalSync(Renderer* renderer, bool vsync) {
	if (renderer != NULL) {
		if (vsync)
			SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
		else
			SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void rendererSetAA(Renderer* renderer, bool aa) {
	if (renderer != NULL) {
		if (aa)
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
		else
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void rendererSetWindowTitle(Renderer* renderer, const char* name) {
	if (renderer != NULL) {
		SDL_SetWindowTitle(renderer->gameWindow, name);
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void rendererSetFramerate(Renderer* renderer, double framerate) {
	if (renderer != NULL) {
		renderer->timePerFrame = 1000000000 / (uint64_t) framerate;
		renderer->fps = (uint64_t) framerate;
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool rendererIsScreenBuffer(Renderer* renderer) {
	bool ret = false;

	if (renderer != NULL) {
		ret = renderer->renderingToScreenBuffer;
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist (rendererIsScreenBuffer)\n");
	}

	return ret;
}
/////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
void calculateForCamera(Renderer* renderer, int* x, int* y) {
	if (rendererIsScreenBuffer(renderer)) {
		*x -= renderer->cameraX;
		*y -= renderer->cameraY;
	}
}
//////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void freeRenderer(Renderer* renderer) {
	if (renderer != NULL) {
		freeTexture(renderer->screenBuffer);
		SDL_DestroyRenderer(renderer->internalRenderer);
		SDL_DestroyWindow(renderer->gameWindow);
		SDL_Quit();
		free(renderer);
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool rendererProcEvents(Renderer* renderer) {
	SDL_Event event;
	bool ret = true;
	int wW = 0;

	// The preliminary check
	if (renderer != NULL) {
		// Update input
		SDL_GetWindowSize(renderer->gameWindow, &wW, NULL);
		updateInput((double) wW / renderer->screenBuffer->w);

		// Update the events queue and keyboard and etc you get the point
		SDL_PumpEvents();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				ret = false;
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist (rendererProcEvents)");
	}

	return ret;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void setRenderTarget(Renderer* renderer, Texture* texture) {
	// The preliminary check
	if (renderer != NULL) {
		if (texture == NULL) {
			renderer->renderingToScreenBuffer = true;
			SDL_SetRenderTarget(renderer->internalRenderer, renderer->screenBuffer->tex);
		} else {
			renderer->renderingToScreenBuffer = false;
			SDL_SetRenderTarget(renderer->internalRenderer, texture->tex);
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist (setRenderTarget)\n");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool configScreenBuffer(Renderer *renderer, uint32 internalWidth, uint32 internalHeight, uint32 displayWidth, uint32 displayHeight) {
	bool pass = false;
	Texture* tempTex;
	int w, h;

	// Check for renderer
	if (renderer != NULL) {
		// Create the texture that will likely become the new screen buffer
		tempTex = createTexture(renderer, internalWidth, internalHeight);

		// Check that it worked
		if (tempTex != NULL) {
			// Get display info
			SDL_GetWindowSize(renderer->gameWindow, &w, &h);

			// Free the old one, then update values
			freeTexture(renderer->screenBuffer);

			renderer->screenBuffer = tempTex;
			renderer->displayBufferW = displayWidth;
			renderer->displayBufferH = displayHeight;
			renderer->displayBufferX = (w - displayWidth) / 2;
			renderer->displayBufferY = (h - displayHeight) / 2;
		} else {
			jSetError(ERROR_SDL_ERROR, "Failed to create screen buffer (configScreenBuffer).");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist (configScreenBuffer)\n");
	}

	return pass;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void windowToScreenBufferCoordinates(Renderer* renderer, int* x, int* y) {
	SDL_DisplayMode mode;
	double widthRatio, heightRatio;
	int rX = *x;
	int rY = *y;

	if (renderer != NULL) {
		// First we need the display mode
		SDL_GetWindowDisplayMode(renderer->gameWindow, &mode);

		// Calculate the width and height ratio
		widthRatio = (double)renderer->screenBuffer->w / (double)renderer->displayBufferW;
		heightRatio = (double)renderer->screenBuffer->h / (double)renderer->displayBufferH;

		// Convert the two numbers
		*x = (int)(((double)rX - (double)renderer->displayBufferX) * widthRatio);
		*y = (int)(((double)rY - (double)renderer->displayBufferY) * heightRatio);
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist (windowToScreenBufferCoordinates)\n");
	}
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void rendererProcEndFrame(Renderer* renderer) {
	SDL_Rect rect;

	// The preliminary check
	if (renderer != NULL) {
		rect.x = renderer->displayBufferX;
		rect.y = renderer->displayBufferY;
		rect.w = renderer->displayBufferW;
		rect.h = renderer->displayBufferH;

		// Set the render target to the screen
		SDL_SetRenderTarget(renderer->internalRenderer, NULL);

		// Draw the texture to screen
		SDL_RenderCopy(renderer->internalRenderer, renderer->screenBuffer->tex, NULL, &rect);

		// Update the screen
		SDL_RenderPresent(renderer->internalRenderer);

		// Set back to black then clear screen
		SDL_SetRenderDrawColor(renderer->internalRenderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer->internalRenderer);

		// Reset to backbuffer
		SDL_SetRenderTarget(renderer->internalRenderer, renderer->screenBuffer->tex);
		SDL_RenderClear(renderer->internalRenderer);

		// Calculate time in between frames
		renderer->between = ns() - renderer->lastTime;

		// Sleep for any remaining time/calculate framerate
		if (renderer->between < renderer->timePerFrame)
			sleep(renderer->timePerFrame - renderer->between);

		renderer->framerate = 1000000000 / ((double)ns() - (double)renderer->lastTime);

		// Update the last time
		renderer->lastTime = ns();
	} else {
		jSetError(ERROR_NULL_POINTER, "Renderer does not exist (rendererProcEndFrame)");
	}
}
/////////////////////////////////////////////////////////////


//
// Created by lugi1 on 2018-07-05.
//

#include "Renderer.h"
#include "Texture.h"
#include <stdio.h>
#include <SDL_video.h>
#include "Clock.h"

/////////////////////////////////////////////////////////////
Renderer* createRenderer(const char* name, uint32 w, uint32 h, double framerate) {
	SDL_Renderer* sdlRenderer;
	SDL_Window* window;
	Texture* tex;
	Renderer* renderer = (Renderer*)malloc(sizeof(Renderer));

	// Check if we were given a dud
	if (renderer != NULL) {

		// Essentially boot up SDL
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
					SDL_SetRenderTarget(sdlRenderer, tex->tex);
				} else {
					freeRenderer(renderer);
					renderer = NULL;
					freeTexture(tex);
					fprintf(stderr, "Failed to create internal texture (createRenderer). SDL Error: %s\n", SDL_GetError());
				}
			} else {
				freeRenderer(renderer);
				renderer = NULL;
				fprintf(stderr, "Failed to create SDL renderer (createRenderer). SDL Error: %s\n", SDL_GetError());
			}
		} else {
			freeRenderer(renderer);
			renderer = NULL;
			fprintf(stderr, "Failed to create SDL window (createRenderer). SDL Error: %s\n", SDL_GetError());
		}
	} else {
		fprintf(stderr, "Failed to create renderer struct. SDL Error: %s\n", SDL_GetError());
	}

	return renderer;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
bool resetWindow(Renderer* renderer, const char* name, uint32 w, uint32 h, bool fullscreen, double framerate) {
	SDL_DisplayMode mode;
	bool pass = false;

	// The preliminary check
	if (renderer != NULL) {
		// Just run through the list of things to update
		renderer->timePerFrame = 1000000000 / (uint64_t) framerate;
		renderer->fps = (uint64_t) framerate;
		SDL_SetWindowSize(renderer->gameWindow, w, h);
		SDL_SetWindowTitle(renderer->gameWindow, name);

		if (fullscreen) {
			// We need to find the screen's width and height for the internal renderer
			SDL_GetWindowDisplayMode(renderer->gameWindow, &mode);

			// Free the old screen buffer, then make a new one
			freeTexture(renderer->screenBuffer);
			renderer->screenBuffer = createTexture(renderer, (uint32)mode.w, (uint32)mode.h);
			renderer->displayBufferW = (uint32)mode.w;
			renderer->displayBufferW = (uint32)mode.h;

			SDL_SetWindowFullscreen(renderer->gameWindow, SDL_WINDOW_FULLSCREEN);

			// Check for errors
			if (renderer->screenBuffer == NULL) {
				fprintf(stderr, "Failed to create screen buffer in fullscreen (resetWindow). SDL Error: %s\n", SDL_GetError());
			} else {
				pass = true;
			}
		} else {
			SDL_SetWindowFullscreen(renderer->gameWindow, 0);

			// Free the old screen buffer, then make a new one
			freeTexture(renderer->screenBuffer);
			renderer->screenBuffer = createTexture(renderer, w, h);
			renderer->displayBufferW = w;
			renderer->displayBufferW = h;

			// Check for errors
			if (renderer->screenBuffer == NULL) {
				fprintf(stderr, "Failed to create screen buffer (resetWindow). SDL Error: %s\n", SDL_GetError());
			} else {
				pass = true;
			}
		}
	} else {
		fprintf(stderr, "Renderer does not exist (rendererProcEvents)");
	}

	return pass;
}
/////////////////////////////////////////////////////////////

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
bool rendererProcEvents(Renderer* renderer, Input* input) {
	SDL_Event event;
	bool ret = true;

	// The preliminary check
	if (renderer != NULL) {
		// Check if we are given an input to update
		if (input != NULL)
			updateInput(input);

		// Update the events queue and keyboard and etc you get the point
		SDL_PumpEvents();

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				ret = false;
		}
	} else {
		fprintf(stderr, "Renderer does not exist (rendererProcEvents)");
	}

	return ret;
}
/////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
void setRenderTarget(Renderer* renderer, Texture* texture) {
	// The preliminary check
	if (renderer != NULL) {
		if (texture == NULL) {
			SDL_SetRenderTarget(renderer->internalRenderer, renderer->screenBuffer->tex);
		} else {
			SDL_SetRenderTarget(renderer->internalRenderer, texture->tex);
		}
	} else {
		fprintf(stderr, "Renderer does not exist (setRenderTarget)\n");
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
			SDL_GetRendererOutputSize(renderer->internalRenderer, &w, &h);

			// Free the old one, then update values
			freeTexture(renderer->screenBuffer);

			renderer->screenBuffer = tempTex;
			renderer->displayBufferW = displayWidth;
			renderer->displayBufferH = displayHeight;
			renderer->displayBufferX = (w - displayWidth) / 2;
			renderer->displayBufferY = (h - displayHeight) / 2;
		} else {
			fprintf(stderr, "Failed to create screen buffer (configScreenBuffer).");
		}
	} else {
		fprintf(stderr, "Renderer does not exist (configScreenBuffer)\n");
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
		fprintf(stderr, "Renderer does not exist (windowToScreenBufferCoordinates)\n");
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
		fprintf(stderr, "Renderer does not exist (rendererProcEndFrame)");
	}
}
/////////////////////////////////////////////////////////////


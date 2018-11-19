//
// Created by lugi1 on 2018-07-05.
//

#pragma once
#include <SDL.h>
#include "Constants.h"
#include "Input.h"


/// \brief Simple texture class with some basic info
///
/// Weather or not a texture is able to be rendered
/// to depends on if it was used to load a file or
/// created without loading a file
typedef struct {
	SDL_Texture* tex; ///< The internal SDL2 texture
	uint32 w; ///< Texture's width
	uint32 h; ///< Texture's height
	bool renderTarget; ///< Weather or not this texture can be rendered to
} Texture;

/// \brief The core renderer required for a game
///
/// The user almost never touches the screen directly, there
/// is instead a screen buffer to draw to. All general drawing
/// is done to the display buffer, then at the end of the
/// frame drawn to the screen. This means that the user can
/// set the screen buffer to something other than the window
/// size and choose at what size it will be rendered. For
/// example, you can have the screen buffer be 320*240 then
/// upscale it to 1280*960 for a pixel-art game.
///
/// Updating the game window undoes any changes you make to
/// the size of the rendered buffer.
typedef struct {
	SDL_Renderer* internalRenderer; ///< The SDL2 internal renderer
	SDL_Window* gameWindow; ///< The window to draw to
	Texture* screenBuffer; ///< We don't draw directly to the screen, rather through this texture
	uint32 displayBufferW; ///< The width of the buffer on screen
	uint32 displayBufferH; ///< The height of the buffer on screen
	uint32 displayBufferX; ///< X position on screen of the display buffer
	uint32 displayBufferY; ///< Y position on screen of the display buffer

	uint64_t lastTime; ///< Used for calculating frame wait time
	uint64_t between; ///< Also used for calculating frame wait time
	uint64_t timePerFrame; ///< Number of microseconds per frame
	double framerate; ///< The current framerate
	uint64_t fps; ///< The expected framerate
} Renderer;

/// \brief Initializes a Renderer
Renderer* createRenderer(const char* name, uint32 w, uint32 h, double framerate);

/// \brief Updates a renderer's properties, returns false if it failed
///
/// Also, if it fails, you will be left without a screen buffer and will
/// crash if you do not shut down the game yourself. In other words, if
/// this function returns false, do not continue program execution.
bool resetWindow(Renderer* renderer, const char* name, uint32 w, uint32 h, bool fullscreen, double framerate);

/// \brief Sets the render target, or null for the screen
void setRenderTarget(Renderer* renderer, Texture* texture);

/// \brief Updates the screen buffer. Returns false if this fails.
bool configScreenBuffer(Renderer* renderer, uint32 internalWidth, uint32 internalHeight, uint32 displayWidth, uint32 displayHeight);

/// \brief Converts window coordinates to screen buffer coordinates
void windowToScreenBufferCoordinates(Renderer* renderer, int* x, int* y);

/// \brief Run this every frame at the start of the frame,
///  returns false if it's time to break the game loop
///
/// You may pass an input struct if you wish to have
/// this function update it for you (otherwise you must
/// run the update function yourself before you run this
/// function). Pass NULL if you'd rather do it yourself.
bool rendererProcEvents(Renderer* renderer, Input* input);

/// \brief Run this at the end of a frame to clock and render it
void rendererProcEndFrame(Renderer* renderer);

/// \brief Frees a renderer
void freeRenderer(Renderer* renderer);
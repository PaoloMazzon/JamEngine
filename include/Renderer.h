/// \file Renderer.h
/// \author lugi1
/// \brief Declares the renderer, texture, and a lot of back-end things
///
/// This is the heart of the engine, the main game loop, window, screen
/// buffer, and more is all handled by this thing. Only one instance of
/// the renderer should exist at a time.
#pragma once
#include <SDL.h>
#include "Constants.h"
#include "Input.h"

#define RENDERER_WINDOWED 0
#define RENDERER_BORDERLESS_FULLSCREEN 1
#define RENDERER_FULLSCREEN 2

typedef struct _JamTexture JamTexture;

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
	JamTexture* screenBuffer; ///< We don't draw directly to the screen, rather through this texture
	uint32 displayBufferW; ///< The width of the buffer on screen
	uint32 displayBufferH; ///< The height of the buffer on screen
	uint32 displayBufferX; ///< X position on screen of the display buffer
	uint32 displayBufferY; ///< Y position on screen of the display buffer

	uint64_t lastTime; ///< Used for calculating frame wait time
	uint64_t between; ///< Also used for calculating frame wait time
	uint64_t timePerFrame; ///< Number of microseconds per frame
	double framerate; ///< The current framerate
	uint64_t fps; ///< The expected framerate
	bool renderingToScreenBuffer; ///< Until SDL_GetRenderTarget is figured out, this is a stand in

	double cameraX; ///< X Location of the camera in the game world (offset to render by)
	double cameraY; ///< Y Location of the camera in the game world (offset to render by)
} JamRenderer;

/// \brief Initializes a Renderer
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_ALLOC_FAILED
void jamInitRenderer(const char *name, uint32 w, uint32 h, double framerate);

/// \brief Returns the internal renderer used by JamRenderer
/// \throws ERROR_NULL_POINTER
void* jamRendererGetInternalRenderer();

/// \brief Gets the camera's x position in the game world
/// \throws ERROR_NULL_POINTER
double jamRendererGetCameraX();

/// \brief Gets the camera's y position in the game world
/// \throws ERROR_NULL_POINTER
double jamRendererGetCameraY();

/// \brief Sets the camera's x/y position in the game world
/// \throws ERROR_NULL_POINTER
void jamRendererSetCameraPos(double x, double y);

/// \brief Moves the camera by a horizontal and vertical component
/// \throws ERROR_NULL_POINTER
void jamRendererMoveCamera(double x_relative, double y_relative);

/// \brief Gets the current framerate the renderer is trying to run at
/// \throws ERROR_NULL_POINTER
double jamRendererGetFramerate();

/// \brief Updates a renderer's properties, returns false if it failed
///
/// Also, if it fails, you will be left without a screen buffer and will
/// crash if you do not shut down the game yourself. In other words, if
/// this function returns false, do not continue program execution.
/// This function also does not change the screen buffer, size, if you
/// want that done you will have to do that separately via `configScreenBuffer`.
///
/// windowWidth and windowHeight do not matter if fullscreen is true.
///
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_NULL_POINTER
bool jamResetRenderer(uint32 windowWidth, uint32 windowHeight, uint8 fullscreen);

/// \brief Upscales your screen buffer as much as it can while retaining its aspect ratio and integer scaling
///
/// This function takes your screen buffer and pixel-perfectly scales it to
/// the maximum size it can be given the current window resolution
///
/// \throws ERROR_NULL_POINTER
void jamIntegerMaximizeScreenBuffer();

/// \brief Sets the title of the window for the renderer
void jamSetWindowTitle(const char *name);

/// \brief Enables or disables anti-aliasing
///
/// Certain platforms may not support anti-aliasing, so just make
/// sure your platform does before turning it on (Most do).
///
/// \throws ERROR_NULL_POINTER
void jamSetAA(bool aa);

/// \brief Enables or disables vertical synchronization
///
/// Vertical synchronization (vsync) prevents programs from getting
/// screen tear at the cost of a bit of performance.
///
/// \throws ERROR_NULL_POINTER
void jamSetVSync(bool vsync);

/// \brief Sets the renderer's framerate
/// \throws ERROR_NULL_POINTER
void jamSetFramerate(double framerate);

/// \brief Checks if the render target is the screen buffer or not
/// \throws ERROR_NULL_POINTER
bool jamRendererTargetIsScreenBuffer();

/// \brief Calculates x/y for the renderer's camera
///
/// If renderer is NULL, this function won't complain or anything,
/// so you can throw this anywhere. This only changes the x and y
/// if the renderer's render target is the screen.
void jamCalculateForCamera(int *x, int *y);

/// \brief Sets the render target, or null for the screen
/// \throws ERROR_NULL_POINTER
void jamSetRenderTarget(JamTexture *texture);

/// \brief Updates the screen buffer. Returns false if this fails.
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_SDL_ERROR
bool jamConfigScreenBuffer(uint32 internalWidth, uint32 internalHeight, uint32 displayWidth,
						   uint32 displayHeight);

/// \brief Converts window coordinates to screen buffer coordinates
///
/// This is really just for the input module, you most likely will
/// never need to use this. Nonetheless, it is available lest you
/// have some crazy shenanigans going on.
///
/// \throws ERROR_NULL_POINTER
void jamConvertCoords(int *x, int *y);

/// \brief Run this every frame at the start of the frame,
///  returns false if it's time to break the game loop
/// \throws ERROR_NULL_POINTER
bool jamProcEvents();

/// \brief Run this at the end of a frame to clock and render it
/// \throws ERROR_NULL_POINTER
void jamProcEndFrame();

/// \brief Frees a renderer
void jamFreeRenderer();
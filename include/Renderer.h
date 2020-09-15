/// \file Renderer.h
/// \author lugi1
/// \brief Declares the renderer, texture, and a lot of back-end things
///
/// This is the heart of the engine, the main game loop, window, screen
/// buffer, and more is all handled by this thing. Only one instance of
/// the renderer should exist at a time.
#pragma once
#include "Constants.h"
#include "Input.h"
#include "VK2D/VK2D.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RENDERER_WINDOWED 0
#define RENDERER_BORDERLESS_FULLSCREEN 1
#define RENDERER_FULLSCREEN 2

typedef struct _JamTexture JamTexture;
typedef struct VK2DRendererConfig JamRenderConfig;

/// \brief The core renderer required for a game, this is just informative, you'll likely never touch this struct
typedef struct {
	void* gameWindow;         ///< The SDL2 window to draw to
	VK2DTexture screenBuffer; ///< Drawing is through this texture for proper upscaling and shader support
	VK2DCamera camera;        ///< Camera
	JamRenderConfig config;   ///< VK2D's renderer doesn't keep track of config in a way that would be good for this engine

	uint64 lastTime;              ///< Used for calculating frame wait time
	uint64 between;               ///< Also used for calculating frame wait time
	uint64 timePerFrame;          ///< Number of microseconds per frame
	double framerate;             ///< The current framerate
	uint64 fps;                   ///< The expected framerate
	bool renderingToScreenBuffer; ///< Keeps track of the current render target
	bool sleepEnabled;            ///< For unlocking framerates

	double deltaCap; ///< Maximum returnable delta
	double delta;    ///< The delta multiplier (actual_frame_time/expected_frame_time used to make up for fluctuating framerates)
} JamRenderer;

/// \brief Initializes a Renderer
/// \param argc Command line parameters argument count (&argc)
/// \param arv Command line parameters
/// \param name Window title
/// \param w Width of the window (and consequentially the initial screen buffer)
/// \param h Height of the window (and consequentially the initial screen buffer)
/// \param framerate Framerate to run the game at (or 0 to unlock the framerate)
///
/// \throws ERROR_SDL_ERROR
/// \throws ERROR_ALLOC_FAILED
void jamRendererInit(int *argc, char **argv, const char *name, uint32 w, uint32 h, double framerate, JamRenderConfig *config);

/// \brief Sets the window's icon to an image (png, jpg, bmp, etc...)
/// \throws ERROR_NULL_POINTER
void jamRendererSetIcon(const char* imageFilename);

/// \brief Gets the width of the buffer thats drawn on (viewport, if you will)
/// \throws ERROR_NULL_POINTER
uint32 jamRendererGetBufferWidth();

/// \brief Gets the height of the buffer thats drawn on (viewport, if you will)
/// \throws ERROR_NULL_POINTER
uint32 jamRendererGetBufferHeight();

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

/// \brief Sets the maximum value jamRendererGetDelta() will return (even if the actual value is larger)
double jamRendererSetDeltaCap(double cap);

/// \brief Grabs the delta multiplier for this frame
///
/// The delta multiplier is a way of allowing game logic to operate (relatively)
/// independent of framerate. The delta multiplier is calculated by taking the
/// actual amount of time it took to process a given frame and divides it by the
/// theoretical amount of time a 60 fps frame should take. For example, if a game
/// is running at ~30 fps, then it would be 60 / ~30 which is a delta multiplier
/// of ~2, and if it were running at ~120 fps, then it would be 60 / ~120 which is
/// a delta multiplier of ~0.5. You can then take this value and multiply things
/// like movement speed against it, which will make your movement look similar at
/// any framerate. Do note that the delta multiplier will always be calculated as
/// is your framerate is 60, even if your framerate is not 60.
///
/// \throws ERROR_NULL_POINTER
double jamRendererGetDelta();

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
bool jamRendererReset(uint32 windowWidth, uint32 windowHeight, uint8 fullscreen);

/// \brief Upscales your screen buffer as much as it can while retaining its aspect ratio and integer scaling
///
/// This function takes your screen buffer and pixel-perfectly scales it to
/// the maximum size it can be given the current window resolution
///
/// \throws ERROR_NULL_POINTER
void jamRendererIntegerScale();

/// \brief Sets the title of the window for the renderer
void jamRendererSetTitle(const char *name);

/// \brief Enables or disables anti-aliasing
///
/// Certain platforms may not support anti-aliasing, so just make
/// sure your platform does before turning it on (Most do).
///
/// \throws ERROR_NULL_POINTER
void jamRendererSetAA(bool aa);

/// \brief Enables or disables vertical synchronization
///
/// Vertical synchronization (vsync) prevents programs from getting
/// screen tear at the cost of a bit of performance.
///
/// \throws ERROR_NULL_POINTER
void jamRendererSetVSync(bool vsync);

/// \brief Sets the renderer's framerate
///
/// Set `framerate` to 0 to unlock the framerate.
///
/// \throws ERROR_NULL_POINTER
void jamRendererSetFramerate(double framerate);

/// \brief Checks if the render target is the screen buffer or not
/// \throws ERROR_NULL_POINTER
bool jamRendererTargetIsScreenBuffer();

/// \brief Sets the render target, or null for the screen
/// \throws ERROR_NULL_POINTER
void jamRendererSetTarget(JamTexture *texture);

/// \brief Updates the screen buffer. Returns false if this fails.
/// \throws ERROR_NULL_POINTER
/// \throws ERROR_SDL_ERROR
void jamRendererConfig(uint32 internalWidth, uint32 internalHeight, uint32 displayWidth,
					   uint32 displayHeight);

/// \brief Run this every frame at the start of the frame,
///  returns false if it's time to break the game loop
/// \throws ERROR_NULL_POINTER
bool jamRendererProcEvents();

/// \brief Run this at the end of a frame to clock and render it
/// \throws ERROR_NULL_POINTER
void jamRendererProcEndFrame();

/// \brief Frees a renderer
void jamRendererQuit();

#ifdef __cplusplus
}
#endif

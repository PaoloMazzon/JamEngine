//
// Created by lugi1 on 2018-07-04.
//

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <Renderer.h>
#include <Hitbox.h>
#include <Sprite.h>
#include "JamEngine.h"

/////////////////// Constants ///////////////////
#define GAME_WIDTH 480
#define GAME_HEIGHT 360
#define VIEW_MULTIPLIER 1
#define SCREEN_WIDTH GAME_WIDTH * VIEW_MULTIPLIER
#define SCREEN_HEIGHT GAME_HEIGHT * VIEW_MULTIPLIER
#define BLOCK_WIDTH 16
#define BLOCK_HEIGHT 16

// Takes a number and returns either -1, 1, or 0 depending on weather its positive negative or neither
static inline double sign(double number) {
	if (number > 0)
		return 1.0;
	else if (number < 0)
		return -1.0;
	else
		return 0.0;
}

void onPlayerFrame(JamRenderer* renderer, JamWorld* world, JamEntity* self) {
	// Gravity
	self->vSpeed += 0.5;

	self->hSpeed =
			(jamInputCheckKey(SDL_SCANCODE_RIGHT) + -jamInputCheckKey(SDL_SCANCODE_LEFT)) * 3;

	// Jump - just shoot the et_Player up and let gravity deal with it (only if on the ground)
	if (jamInputCheckKeyPressed(SDL_SCANCODE_UP) &&
			jamCheckEntityTileMapCollision(self, world->worldMaps[0], (int) self->x, (int) self->y + 1))
		self->vSpeed -= 10;

	// Let's not go mach speed
	if (self->vSpeed >= BLOCK_HEIGHT)
		self->vSpeed = BLOCK_HEIGHT - 1;

	if (jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x + self->hSpeed, self->y)) {
		self->x -= sign(self->hSpeed);
		self->x = round(self->x);
		while (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x + sign(self->hSpeed), self->y))
			self->x += sign(self->hSpeed);
		self->hSpeed = 0;
	}
	if (jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + self->vSpeed)) {
		self->y -= sign(self->vSpeed);
		self->y = round(self->y);
		while (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + sign(self->vSpeed)))
			self->y += sign(self->vSpeed);
		self->vSpeed = 0;
	}
	self->x += self->hSpeed;
	self->y += self->vSpeed;

	//////////////////////// Player Animations ////////////////////////
	// We must invert the et_Player if he is going left
	if (self->hSpeed > 0)
		self->scaleX = 1;
	else if (self->hSpeed < 0)
		self->scaleX = -1;
	/*
	// Walking/standing animations
	if (self->hSpeed != 0)
		self->sprite = sPlayerMove;
	else
		self->sprite = sPlayerStand;

	if (!jamCheckEntityTileMapCollision(self, currentLevel, self->x, self->y + 1))
		self->sprite = sPlayerJump;*/
}

/////////////////////////////////////// The main menu ///////////////////////////////////////
bool runMenu(JamRenderer* renderer, JamFont* font) { // Returns false if quit game
	// Menu-related variables
	bool play = false;
	bool runLoop = true;

	while (runLoop) {
		// Update the renderer and check for a quit signal
		runLoop = jamProcEvents(renderer);

		if (runLoop) {
			jamDrawFillColour(renderer, 255, 255, 255, 255);

			// Display a simple message prompting play or quit
			jamRenderFont(0, 0, "Press <ESC> to quit or <SPACE> to play.", font, renderer);

			// Now check out what the user wants to do
			if (jamInputCheckKeyPressed(SDL_SCANCODE_ESCAPE)) {
				// Just quit the game
				runLoop = false;
			} else if (jamInputCheckKeyPressed(SDL_SCANCODE_SPACE)) {
				// Play the ever-exciting test game
				runLoop = false;
				play = true;
			}

			jamProcEndFrame(renderer);
		}
	}


	return play;
}
////////////////////////////////////////// The game /////////////////////////////////////////
bool runGame(JamRenderer* renderer, JamFont* font) {
	// Core game pieces
	bool mainMenu = false; // Weather or not return to main menu
	bool runLoop = true;
	int i;

	// Create the behaviour map
	JamBehaviourMap* bMap = jamCreateBehaviourMap();
	jamAddBehaviourToMap(bMap, "PlayerBehaviour", NULL, NULL, onPlayerFrame, NULL);

	// Load the assets and create the world
	JamAssetHandler* handler = jamCreateAssetHandler();
	jamAssetLoadINI(handler, renderer, "assets/level0.ini", bMap);
	JamWorld* gameWorld = jamLoadWorldFromTMX(handler, renderer, "assets/level0.tmx");

	// Some setup
	renderer->cameraX = 50;
	renderer->cameraY = 50;

	// We don't really care what went wrong, but if something went wrong while
	// while loading assets, we cannot continue.
	if (jGetError() == 0) {
		while (runLoop) {
			// Update the renderer and check for a quit signal
			runLoop = jamProcEvents(renderer);

			if (runLoop) {
				jamDrawFillColour(renderer, 0, 0, 0, 255);

				// Mess around with the renderer reset
				if (jamInputCheckKeyPressed(SDL_SCANCODE_F)) {
					jamResetRenderer(renderer, 0, 0, RENDERER_BORDERLESS_FULLSCREEN);
					jamIntegerMaximizeScreenBuffer(renderer);
				}
				if (jamInputCheckKeyPressed(SDL_SCANCODE_G)) {
					jamResetRenderer(renderer, GAME_WIDTH, GAME_HEIGHT, RENDERER_WINDOWED);
					jamIntegerMaximizeScreenBuffer(renderer);
				}

				/////////////////////////// DRAWING THINGS //////////////////////////
				// Draw the game world
				for (i = 0; i < MAX_TILEMAPS; i++)
					if (gameWorld->worldMaps[i] != NULL)
						jamDrawTileMap(renderer, gameWorld->worldMaps[i], 0, 0, 0, 0, 0, 0);
				jamWorldProcFrame(gameWorld);

				// Draw a border around the screen
				jamDrawSetColour(renderer, 255, 255, 255, 255);
				jamDrawRectangle(renderer, 0, 0, GAME_WIDTH, GAME_HEIGHT);
				jamDrawSetColour(renderer, 0, 0, 0, 255);

				// Debug
				jamRenderFontExt(16, 16, "FPS: %f", font, renderer, 999, round(renderer->framerate));
				/////////////////////////////////////////////////////////////////////

				jamProcEndFrame(renderer);
			}
		}
	}

	// Free up the resources
	jamFreeWorld(gameWorld);
	jamFreeBehaviourMap(bMap);
	jamFreeAssetHandler(handler);

	return mainMenu;
}
/////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	JamRenderer* renderer = jamCreateRenderer("JamEngine", SCREEN_WIDTH, SCREEN_HEIGHT, 60);
	jamSetAA(renderer, false);
	JamFont* font = jamCreateFont(renderer, "assets/standardlatinwhitebg.png", NULL);
	font->characterHeight = 16;
	font->characterWidth = 8;
	bool run = true;

	// Setup the screen
	jamConfigScreenBuffer(renderer, GAME_WIDTH, GAME_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);

	// A very simple loop that allows the et_Player to bounce between
	// the menu and the game infinitely
	while (run) {
		run = runMenu(renderer, font);
		if (run)
			run = runGame(renderer, font);
	}

	jamQuitInput();
	jamFreeFont(font);
	jamFreeRenderer(renderer);
	return 0;
}

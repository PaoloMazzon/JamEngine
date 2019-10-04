//
// Created by lugi1 on 2018-07-04.
//

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <Renderer.h>
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

void onPlayerFrame(Renderer* renderer, World* world, Entity* self) {
	// Gravity
	self->vSpeed += 0.5;

	self->hSpeed =
			(inputCheckKey(input, SDL_SCANCODE_RIGHT) + -inputCheckKey(input, SDL_SCANCODE_LEFT)) * 3;

	// Jump - just shoot the player up and let gravity deal with it (only if on the ground)
	if (inputCheckKeyPressed(input, SDL_SCANCODE_UP) &&
		checkEntityTileMapCollision(self, world, (int) self->x, (int) self->y + 1))
		self->vSpeed -= 10;

	// Let's not go mach speed
	if (self->vSpeed >= BLOCK_HEIGHT)
		self->vSpeed = BLOCK_HEIGHT - 1;

	if (checkEntityTileMapCollision(self, currentLevel, self->x + self->hSpeed, self->y)) {
		self->x -= sign(self->hSpeed);
		self->x = round(self->x);
		while (!checkEntityTileMapCollision(self, currentLevel, self->x + sign(self->hSpeed), self->y))
			self->x += sign(self->hSpeed);
		self->hSpeed = 0;
	}
	if (checkEntityTileMapCollision(self, currentLevel, self->x, self->y + self->vSpeed)) {
		self->y -= sign(self->vSpeed);
		self->y = round(self->y);
		while (!checkEntityTileMapCollision(self, currentLevel, self->x, self->y + sign(self->vSpeed)))
			self->y += sign(self->vSpeed);
		self->vSpeed = 0;
	}
	self->x += self->hSpeed;
	self->y += self->vSpeed;

//////////////////////// Player Animations ////////////////////////
// We must invert the player if he is going left
	if (self->hSpeed > 0)
		self->scaleX = 1;
	else if (self->hSpeed < 0)
		self->scaleX = -1;

// Walking/standing animations
	if (self->hSpeed != 0)
		self->sprite = sPlayerMove;
	else
		self->sprite = sPlayerStand;

	if (!checkEntityTileMapCollision(self, currentLevel, self->x, self->y + 1))
		self->sprite = sPlayerJump;
}

/////////////////////////////////////// The main menu ///////////////////////////////////////
bool runMenu(Renderer* renderer, Input* input, Font* font) { // Returns false if quit game
	// Menu-related variables
	bool play = false;
	bool runLoop = true;

	while (runLoop) {
		// Update the renderer and check for a quit signal
		runLoop = rendererProcEvents(renderer, input);

		if (runLoop) {
			drawFillColour(renderer, 255, 255, 255, 255);

			// Display a simple message prompting play or quit
			renderFont(0, 0, "Press <ESC> to quit or <SPACE> to play.", font, renderer);

			// Now check out what the user wants to do
			if (inputCheckKeyPressed(input, SDL_SCANCODE_ESCAPE)) {
				// Just quit the game
				runLoop = false;
			} else if (inputCheckKeyPressed(input, SDL_SCANCODE_SPACE)) {
				// Play the ever-exciting test game
				runLoop = false;
				play = true;
			}

			rendererProcEndFrame(renderer);
		}
	}


	return play;
}
////////////////////////////////////////// The game /////////////////////////////////////////
bool runGame(Renderer* renderer, Input* input, Font* font) {
	// Core game pieces
	bool mainMenu = false; // Weather or not return to main menu
	bool runLoop = true;
	int i;

	// Create the behaviour map
	BehaviourMap* bMap = createBehaviourMap();

	// Load the assets and create the world
	AssetHandler* handler = createAssetHandler();
	assetLoadINI(handler, renderer, "assets/level0.ini", NULL);
	World* gameWorld = loadWorldFromTMX(handler, renderer, "assets/level0.tmx");

	// We don't really care what went wrong, but if something went wrong while
	// while loading assets, we cannot continue.
	if (jGetError() == 0) {
		while (runLoop) {
			// Update the renderer and check for a quit signal
			runLoop = rendererProcEvents(renderer, input);

			if (runLoop) {
				drawFillColour(renderer, 0, 0, 0, 255);

				/////////////////////////// DRAWING THINGS //////////////////////////
				renderer->cameraX = 50;
				renderer->cameraY = 50;
				for (i = 0; i < MAX_TILEMAPS; i++)
					if (gameWorld->worldMaps[i] != NULL)
						drawTileMap(renderer, gameWorld->worldMaps[i], 0, 0, 0, 0, 0, 0);
				worldProcFrame(gameWorld);
				renderFontExt(16, 16, "FPS: %f", font, renderer, 999, round(renderer->framerate));
				/////////////////////////////////////////////////////////////////////

				rendererProcEndFrame(renderer);
			}
		}
	}

	// Free up the resources
	freeWorld(gameWorld);
	freeBehaviourMap(bMap);
	freeAssetHandler(handler);

	return mainMenu;
}
/////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	Renderer* renderer = createRenderer("Jam Engine", SCREEN_WIDTH, SCREEN_HEIGHT, 60);
	Input* input = createInput();
	Font* font = createFont(renderer, "assets/standardlatinwhitebg.png", NULL);
	font->characterHeight = 16;
	font->characterWidth = 8;
	bool run = true;

	// Setup the screen
	configScreenBuffer(renderer, GAME_WIDTH, GAME_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);

	// A very simple loop that allows the player to bounce between
	// the menu and the game infinitely
	while (run) {
		run = runMenu(renderer, input, font);
		if (run)
			run = runGame(renderer, input, font);
	}

	freeInput(input);
	freeFont(font);
	freeRenderer(renderer);
	return 0;
}

//
// Created by lugi1 on 2018-07-04.
//
#include <stdio.h>
#include "Renderer.h"
#include "Entity.h"
#include "Font.h"
#include "Drawing.h"
#include <time.h>

/////////////////// Constants ///////////////////
#define GAME_WIDTH 480
#define GAME_HEIGHT 360
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 720
#define BLOCK_WIDTH 16
#define BLOCK_HEIGHT 16

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
	int x, y;

	// Core game pieces
	bool mainMenu = false; // Weather or not return to main menu
	int mx, my; // Mouse coordinates in game-world
	bool runLoop = true;

	// All the textures we need ...
	Texture* playerTex = loadTexture(renderer, "brick.png");
	Texture* wallTex = loadTexture(renderer, "walltiles.png");
	Texture* roomWalls = createTexture(renderer, GAME_WIDTH, GAME_HEIGHT);

	// ... Followed by all the sprites/entities
	Sprite* playerSprite = loadSpriteFromSheet(playerTex, 1, 0, 0, 16, 16, 0, 0, 0, 0, 0);
	Entity* playerEntity = createEntity(playerSprite, createHitbox(hitRectangle, 0, 16, 16), 196, 16);

	// The wall sprite is a weird one so gotta make it all special like
	Sprite* wallSprite = loadSpriteFromSheet(wallTex, 48, 0, 0, 24, 24, 0, 0, 0, 0, 0);
	wallSprite->originY = 8;

	// The grid the player will bounce around on
	//TileMap* collisionGrid = createTileMap(GAME_WIDTH / BLOCK_WIDTH, GAME_HEIGHT / BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT);

	TileMap* collisionGrid = loadTileMap("level.level", GAME_WIDTH / BLOCK_WIDTH, GAME_HEIGHT / BLOCK_HEIGHT, BLOCK_WIDTH, BLOCK_HEIGHT);

	// Put all the collisions onto a surface to draw
	setRenderTarget(renderer, roomWalls);
	drawFillColour(renderer, 0, 0, 0, 255);
	drawSortedMap(renderer, wallSprite, collisionGrid, 0, 0, 0, 0);
	setRenderTarget(renderer, NULL);

	while (runLoop) {
		// Update the renderer and check for a quit signal
		runLoop = rendererProcEvents(renderer, input);

		if (runLoop) {
			drawFillColour(renderer, 255, 255, 255, 255);

			// Draw the walls
			drawTexture(renderer, roomWalls, 0, 0);

			// Draw the player, of course
			drawEntity(renderer, playerEntity);

			rendererProcEndFrame(renderer);
		}
	}

	// Free up the resources
	// TODO: This (ya ik but windows does it anyway lmao)

	return mainMenu;
}
/////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	Renderer* renderer = createRenderer("Jam Game", SCREEN_WIDTH, SCREEN_HEIGHT, 60);
	Input* input = createInput();
	Font* font = createFont(renderer, "standardlatin.png", NULL);
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
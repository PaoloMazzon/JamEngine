//
// Created by lugi1 on 2018-07-04.
//

#include <stdio.h>
#include "Renderer.h"
#include "Entity.h"
#include "Font.h"
#include "Drawing.h"
#include <time.h>
#include "File.h"
#include <math.h>
#include <Entity.h>
#include <Sprite.h>
#include <Hitbox.h>
#include <Renderer.h>
#include <Input.h>
#include "AssetHandler.h"
#include <JamError.h>
#include <Vector.h>
#include <TileMap.h>

/////////////////// Constants ///////////////////
#define GAME_WIDTH 480
#define GAME_HEIGHT 360
#define VIEW_MULTIPLIER 2
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

	// Load the asset handler
	AssetHandler* handler = createAssetHandler();
	assetLoadINI(handler, renderer, "assets/level0.ini");

	// Load all the game assets from the handler
	Sprite* sWallSet = assetGetSprite(handler, "WallTilesetSprite");
	Sprite* sPlayerMove = assetGetSprite(handler, "PlayerMovingSprite");
	Sprite* sPlayerStand = assetGetSprite(handler, "PlayerStandingSprite");
	Sprite* sPlayerJump = assetGetSprite(handler, "PlayerJumpingSprite");
	Texture *tBackground = assetGetTexture(handler, "BackgroundTexture");
	Entity *ePlayer = assetGetEntity(handler, "PlayerEntity");
	TileMap *tmLevel1 = assetGetTileMap(handler, "Level0Tilemap");
	TileMap *currentLevel = tmLevel1;

	// Instead of drawing the grid everyframe, just draw it once to this
	Texture *rtRoom = createTexture(renderer, GAME_WIDTH, GAME_HEIGHT);

	// We don't really care what went wrong, but if something went wrong while
	// while loading assets, we cannot continue.
	if (jGetError() == 0) {

		/////////// CREATE THE CURRENT ROOM'S BACKGROUND ///////////
		/// This beautiful function "drawSortedMap" allows us
		/// to cook the ingredients "level1.txt" and "walltiles.png"
		/// into a very good looking set of walls that are all
		/// properly formatted based on adjacent walls.
		setRenderTarget(renderer, rtRoom);
		drawTexture(renderer, tBackground, 0, 0);
		drawSortedMap(renderer, sWallSet, currentLevel, 0, 0, 0, 0);
		setRenderTarget(renderer, NULL);

		while (runLoop) {
			// Update the renderer and check for a quit signal
			runLoop = rendererProcEvents(renderer, input);

			if (runLoop) {
				//drawTexture(renderer, rtRoom, 0, 0);
				drawTexture(renderer, tBackground, (sint32)renderer->cameraX, (sint32)renderer->cameraY);
				drawTileMap(renderer, currentLevel, 0, 0, 0, 0, 0, 0);

				////////////////////// PLAYER MOVEMENT/PHYSICS //////////////////////
				// Gravity
				ePlayer->vSpeed += 0.5;

				// Move left/right
				/* inputCheckKey returns either 0 or 1, so we add input for the right
				 * key and negative input for the right key together and multiply it
				 * by the player's speed. This will either be -speed for left key,
				 * +speed for the right key, or 0 if neither or both are pressed.
				 */
				ePlayer->hSpeed =
						(inputCheckKey(input, SDL_SCANCODE_RIGHT) + -inputCheckKey(input, SDL_SCANCODE_LEFT)) * 3;

				// Jump - just shoot the player up and let gravity deal with it (only if on the ground)
				if (inputCheckKeyPressed(input, SDL_SCANCODE_UP) &&
					checkEntityTileMapCollision(ePlayer, currentLevel, (int) ePlayer->x, (int) ePlayer->y + 1))
					ePlayer->vSpeed -= 10;

				// Let's not go mach speed
				if (ePlayer->vSpeed >= BLOCK_HEIGHT)
					ePlayer->vSpeed = BLOCK_HEIGHT - 1;

				/*******Player Collisions
				 * The essential idea to a collision here is
				 * 1. Check if we are going to collide with a wall
				 * 2. If so, continually move a pixel towards it until you're touching it
				 * 3. After we inch towards the wall, we correct the decimal place if there is one
				*/
				if (checkEntityTileMapCollision(ePlayer, currentLevel, ePlayer->x + ePlayer->hSpeed, ePlayer->y)) {
					ePlayer->x -= sign(ePlayer->hSpeed);
					ePlayer->x = round(ePlayer->x);
					while (!checkEntityTileMapCollision(ePlayer, currentLevel, ePlayer->x + sign(ePlayer->hSpeed), ePlayer->y))
						ePlayer->x += sign(ePlayer->hSpeed);
					ePlayer->hSpeed = 0;
				}
				if (checkEntityTileMapCollision(ePlayer, currentLevel, ePlayer->x, ePlayer->y + ePlayer->vSpeed)) {
					ePlayer->y -= sign(ePlayer->vSpeed);
					ePlayer->y = round(ePlayer->y);
					while (!checkEntityTileMapCollision(ePlayer, currentLevel, ePlayer->x, ePlayer->y + sign(ePlayer->vSpeed)))
						ePlayer->y += sign(ePlayer->vSpeed);
					ePlayer->vSpeed = 0;
				}
				ePlayer->x += ePlayer->hSpeed;
				ePlayer->y += ePlayer->vSpeed;

				//////////////////////// Player Animations ////////////////////////
				// We must invert the player if he is going left
				if (ePlayer->hSpeed > 0)
					ePlayer->scaleX = 1;
				else if (ePlayer->hSpeed < 0)
					ePlayer->scaleX = -1;

				// Walking/standing animations
				if (ePlayer->hSpeed != 0)
					ePlayer->sprite = sPlayerMove;
				else
					ePlayer->sprite = sPlayerStand;

				if (!checkEntityTileMapCollision(ePlayer, currentLevel, ePlayer->x, ePlayer->y + 1))
					ePlayer->sprite = sPlayerJump;

				/////////////////////////// DRAWING THINGS //////////////////////////
				renderer->cameraX = clamp(ePlayer->x - GAME_WIDTH / 2 + 8, 0, currentLevel->width * currentLevel->cellWidth - GAME_WIDTH);
				renderer->cameraY = clamp(ePlayer->y - GAME_HEIGHT / 2 + 16, 0, currentLevel->height * currentLevel->cellHeight - GAME_HEIGHT);
				drawEntity(renderer, ePlayer);
				renderFontExt(16, 16, "FPS: %f\n(%f,%f)", font, renderer, 999, round(renderer->framerate), ePlayer->x, ePlayer->y);
				/////////////////////////////////////////////////////////////////////

				rendererProcEndFrame(renderer);
			}
		}
	}

	// Free up the resources
	freeTexture(rtRoom);
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
//
// Created by lugi1 on 2018-07-04.
//

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <Audio.h>
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

void onPlayerFrame(JamWorld* world, JamEntity* self) {
	// Gravity
	self->vSpeed += 0.5;

	self->hSpeed =
			(jamInputCheckKey(JAM_KB_RIGHT) + -jamInputCheckKey(JAM_KB_LEFT)) * 3;

	// Jump - just shoot the et_Player up and let gravity deal with it (only if on the ground)
	if (jamInputCheckKeyPressed(JAM_KB_UP) &&
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

	jamAudioSetListenerPosition((float)self->x, (float)self->y, 0);

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
bool runMenu(JamFont* font) { // Returns false if quit game
	// Menu-related variables
	bool play = false;
	bool runLoop = true;

	while (runLoop) {
		// Update the renderer and check for a quit signal
		runLoop = jamProcEvents();

		if (runLoop) {
			jamDrawFillColour(255, 255, 255, 255);

			// Display a simple message prompting play or quit
			jamRenderFont(0, 0, "Press <ESC> to quit or <SPACE> to play.", font);

			// Now check out what the user wants to do
			if (jamInputCheckKeyPressed(JAM_KB_ESCAPE)) {
				// Just quit the game
				runLoop = false;
			} else if (jamInputCheckKeyPressed(JAM_KB_SPACE)) {
				// Play the ever-exciting test game
				runLoop = false;
				play = true;
			}

			jamProcEndFrame();
		}
	}


	return play;
}
////////////////////////////////////////// The game /////////////////////////////////////////
bool runGame(JamFont* font) {
	// Core game pieces
	bool mainMenu = false; // Weather or not return to main menu
	bool runLoop = true;
	int i;

	// Create the behaviour map
	JamBehaviourMap* bMap = jamCreateBehaviourMap();
	jamAddBehaviourToMap(bMap, "PlayerBehaviour", NULL, NULL, onPlayerFrame, NULL);

	// Load the assets and create the world
	JamAssetHandler* handler = jamCreateAssetHandler();
	jamAssetLoadINI(handler, "assets/level0.ini", bMap);
	JamWorld* gameWorld = jamLoadWorldFromTMX(handler, "assets/level0.tmx");

	// Some setup
	jamRendererSetCameraPos(50, 50);
	JamAudioBuffer* sound = jamLoadAudioBufferFromWAV("assets/pop.wav");
	JamAudioSource* source = jamCreateAudioSource();

	// We don't really care what went wrong, but if something went wrong while
	// while loading assets, we cannot continue.
	if (jGetError() == 0) {
		while (runLoop) {
			// Update the renderer and check for a quit signal
			runLoop = jamProcEvents();

			if (runLoop) {
				jamDrawFillColour(0, 0, 0, 255);

				// Testing suite
				if (jamInputCheckKeyPressed(JAM_KB_F)) {
					jamResetRenderer(0, 0, RENDERER_BORDERLESS_FULLSCREEN);
					jamIntegerMaximizeScreenBuffer();
				}
				if (jamInputCheckKeyPressed(JAM_KB_G)) {
					jamResetRenderer(GAME_WIDTH, GAME_HEIGHT, RENDERER_WINDOWED);
					jamIntegerMaximizeScreenBuffer();
				}
				if (jamInputCheckKeyPressed(JAM_KB_P)) {
					source->xPosition = jamInputGetMouseX();
					source->yPosition = jamInputGetMouseY();
					jamPlayAudio(sound, source, false);
				}

				/////////////////////////// DRAWING THINGS //////////////////////////
				// Draw the game world
				for (i = 0; i < MAX_TILEMAPS; i++)
					if (gameWorld->worldMaps[i] != NULL)
						jamDrawTileMap(gameWorld->worldMaps[i], 0, 0, 0, 0, 0, 0);
				jamWorldProcFrame(gameWorld);

				// Draw a border around the screen
				jamDrawSetColour(255, 255, 255, 255);
				jamDrawRectangle(0, 0, GAME_WIDTH, GAME_HEIGHT);
				jamDrawSetColour(0, 0, 0, 255);

				// Debug
				jamRenderFontExt(16, 16, "FPS: %f", font, 999, jamRendererGetFramerate());
				/////////////////////////////////////////////////////////////////////

				jamProcEndFrame();
			}
		}
	}

	// Free up the resources
	jamFreeWorld(gameWorld);
	jamFreeBehaviourMap(bMap);
	jamFreeAssetHandler(handler);

	// Test suite
	jamFreeAudioSource(source);
	jamFreeAudioBuffer(sound);

	return mainMenu;
}
/////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
	jamInitRenderer(&argc, argv, "JamEngine", SCREEN_WIDTH, SCREEN_HEIGHT, 60);
	jamSetAA(false);
	JamFont* font = jamCreateFont("assets/standardlatinwhitebg.png", NULL);
	font->characterHeight = 16;
	font->characterWidth = 8;
	bool run = true;

	// Setup the screen
	jamConfigScreenBuffer(GAME_WIDTH, GAME_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);

	// A very simple loop that allows the et_Player to bounce between
	// the menu and the game infinitely
	while (run) {
		run = runMenu(font);
		if (run)
			run = runGame(font);
	}

	jamFreeFont(font);
	jamRendererQuit();
	return 0;
}

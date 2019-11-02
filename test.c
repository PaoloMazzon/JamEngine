//
// Created by lugi1 on 2018-07-04.
//

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <Audio.h>
#include <Tweening.h>
#include <EntityList.h>
#include "JamEngine.h"

/////////////////// Constants ///////////////////
#define GAME_WIDTH 480
#define GAME_HEIGHT 360
#define VIEW_MULTIPLIER 1
#define SCREEN_WIDTH GAME_WIDTH * VIEW_MULTIPLIER
#define SCREEN_HEIGHT GAME_HEIGHT * VIEW_MULTIPLIER
#define BLOCK_WIDTH 16
#define BLOCK_HEIGHT 16

JamAssetHandler* gHandler;

// Takes a number and returns either -1, 1, or 0 depending on weather its positive negative or neither
static inline double sign(double number) {
	if (number > 0)
		return 1.0;
	else if (number < 0)
		return -1.0;
	else
		return 0.0;
}

void onEnemyCreate(JamWorld* world, JamEntity* self) {
	self->hSpeed = 3;
}

void onEnemyFrame(JamWorld* world, JamEntity* self) {
	// We change direction when there is a wall in our way or a cliff in front of us
	if (jamCheckEntityTileMapCollision(self, world->worldMaps[0], (int)self->x +self-> hSpeed, (int)self->y) ||
		!jamCheckEntityTileMapCollision(self, world->worldMaps[0], (int)self->x + sign(self->hSpeed) * 16, (int)self->y + 17))
		self->hSpeed = -self->hSpeed;

	// Make them face the direction they are walking in
	self->scaleX = sign(self->hSpeed);

	self->x += self->hSpeed;
}

void onPlayerFrame(JamWorld* world, JamEntity* self) {
	int i;

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

	// Walking/standing animations
	if (self->hSpeed != 0)
		self->sprite = jamGetSpriteFromHandler(gHandler, "PlayerMovingSprite");
	else
		self->sprite = jamGetSpriteFromHandler(gHandler, "PlayerStandingSprite");

	if (!jamCheckEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1))
		self->sprite = jamGetSpriteFromHandler(gHandler, "PlayerJumpingSprite");
}

bool runGame(JamFont* font);

/////////////////////////////////////// The main menu ///////////////////////////////////////
bool runMenu(JamFont* font) { // Returns false if quit game
	// Menu-related variables
	bool play = false;
	bool runLoop = true;
	JamTexture* logoTex = jamLoadTexture("assets/jamengine.png");

	// The menu buttons
	JamTexture* playTex = jamLoadTexture("assets/playbutton.png");
	JamTexture* exitTex = jamLoadTexture("assets/exitbutton.png");
	JamTweeningState playTween;
	JamTweeningState exitTween;
	jamInitTween(&playTween, 0.05, GAME_WIDTH - 172, GAME_WIDTH - 96);
	jamInitTween(&exitTween, 0.05, GAME_WIDTH - 172, GAME_WIDTH - 96);
	bool inPlayButtonPreviousFrame = false;
	bool inExitButtonPreviousFrame = false;
	bool inPlayButton = false;
	bool inExitButton = false;
	int playButtonY = GAME_HEIGHT - 128;
	int exitButtonY = GAME_HEIGHT - 96;

	while (runLoop) {
		// Update the renderer and check for a quit signal
		runLoop = jamProcEvents();

		if (runLoop) {
			jamDrawFillColour(255, 255, 255, 255);

			// Check if the mouse is in a button
			inPlayButton = pointInRectangle(jamInputGetMouseX(), jamInputGetMouseY(), GAME_WIDTH - 172, playButtonY, 172, 32);
			inExitButton = pointInRectangle(jamInputGetMouseX(), jamInputGetMouseY(), GAME_WIDTH - 172, exitButtonY, 172, 32);

			// Handle tweening changes
			if (inPlayButton != inPlayButtonPreviousFrame)
				jamReverseTween(&playTween);
			if (inExitButton != inExitButtonPreviousFrame)
				jamReverseTween(&exitTween);

			// And button presses
			if ((jamInputCheckMouseButtonPressed(MOUSE_LEFT_BUTTON) && inPlayButton) || jamInputCheckKeyPressed(JAM_KB_SPACE))
				runLoop = runGame(font);
			if ((jamInputCheckMouseButtonPressed(MOUSE_LEFT_BUTTON) && inExitButton) || jamInputCheckKeyPressed(JAM_KB_ESCAPE))
				runLoop = false;

			// Draw the buttons
			jamDrawTexture(playTex, (sint32)jamUpdateTweenParabolic(&playTween), playButtonY);
			jamDrawTexture(exitTex, (sint32)jamUpdateTweenParabolic(&exitTween), exitButtonY);

			jamDrawTexture(logoTex, 0, 0);

			inPlayButtonPreviousFrame = inPlayButton;
			inExitButtonPreviousFrame = inExitButton;
			jamProcEndFrame();
		}
	}

	jamFreeTexture(playTex);
	jamFreeTexture(exitTex);
	jamFreeTexture(logoTex);
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
	jamAddBehaviourToMap(bMap, "EnemyBehaviour", onEnemyCreate, NULL, onEnemyFrame, NULL);

	// Load the assets and create the world
	gHandler = jamCreateAssetHandler();
	jamAssetLoadINI(gHandler, "assets/level0.ini", bMap);
	JamWorld* gameWorld = jamGetWorldFromHandler(gHandler, "GameWorld");

	// Some setup
	jamRendererSetCameraPos(50, 50);
	JamAudioBuffer* sound = jamGetAudioBufferFromHandler(gHandler, "PopSound");
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
				// Draw the background
				jamDrawTexture(jamGetTextureFromHandler(gHandler, "BackgroundTexture"), 0, 0);

				// Draw the game world
				for (i = 0; i < MAX_TILEMAPS; i++)
					if (gameWorld->worldMaps[i] != NULL)
						jamDrawTileMap(gameWorld->worldMaps[i], 0, 0, 0, 0, 0, 0);
				jamWorldProcFrame(gameWorld);

				// Debug
				jamRenderFontExt(16, 16, "FPS: %f\nDelta: %f", font, 999, jamRendererGetFramerate(), jamRendererGetDelta());
				/////////////////////////////////////////////////////////////////////

				jamProcEndFrame();
			}
		}
	}

	// Free up the resources
	jamFreeAssetHandler(gHandler);
	jamFreeBehaviourMap(bMap);

	// Test stuff
	jamFreeAudioSource(source);

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

	////////////// Testing buffers //////////////
	JamBuffer* buffer = jamLoadBuffer("buffer.bin");
    printf("%i\n", jamReadByte4(buffer, 0));
	jamFreeBuffer(buffer);

	jamFreeFont(font);
	jamRendererQuit();
	return 0;
}

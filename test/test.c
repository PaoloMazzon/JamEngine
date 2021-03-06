//
// Created by lugi1 on 2018-07-04.
//

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <Audio.h>
#include <Tweening.h>
#include <EntityList.h>
#include <memory.h>
#include "JamEngine.h"
#include <stdlib.h>
#include <string.h>

/////////////////// Constants ///////////////////
#define GAME_WIDTH 480
#define GAME_HEIGHT 360
#define VIEW_MULTIPLIER 1
#define SCREEN_WIDTH GAME_WIDTH * VIEW_MULTIPLIER
#define SCREEN_HEIGHT GAME_HEIGHT * VIEW_MULTIPLIER
#define BLOCK_WIDTH 16
#define BLOCK_HEIGHT 16

JamAssetHandler* gHandler;

void onEnemyCreate(JamWorld* world, JamEntity* self) {
	self->hSpeed = 3;
}

void onEnemyFrame(JamWorld* world, JamEntity* self) {
	// We change direction when there is a wall in our way or a cliff in front of us
	if (jamEntityTileMapCollision(self, world->worldMaps[0], (int) self->x + self->hSpeed, (int) self->y) ||
		!jamEntityTileMapCollision(self, world->worldMaps[0], (int) self->x + sign(self->hSpeed) * 16,
								   (int) self->y + 17))
		self->hSpeed = -self->hSpeed;

	// Make them face the direction they are walking in
	self->scaleX = (float)sign(self->hSpeed);

	self->x += self->hSpeed * jamRendererGetDelta();
}

void onPlayerFrame(JamWorld* world, JamEntity* self) {
	// Gravity
	self->vSpeed += 0.5 * jamRendererGetDelta();

	self->hSpeed =
			(jamInputCheckKey(JAM_KB_RIGHT) + -jamInputCheckKey(JAM_KB_LEFT)) * 3;
	

	// Jump - just shoot the et_Player up and let gravity deal with it (only if on the ground)
	if (jamInputCheckKeyPressed(JAM_KB_UP) &&
			jamEntityTileMapCollision(self, world->worldMaps[0], (int) self->x, (int) self->y + 1))
			self->vSpeed -= 10;

	// Let's not go mach speed
	if (self->vSpeed >= BLOCK_HEIGHT)
		self->vSpeed = BLOCK_HEIGHT - 1;

	// TESTING - check for world collisions and draw the entity should there be one
	JamEntity* collEnt = jamWorldEntityCollision(world, self, self->x, self->y);

	if (jamEntityTileMapCollision(self, world->worldMaps[0], self->x + self->hSpeed, self->y)) {
		self->x -= sign(self->hSpeed);
		self->x = round(self->x);
		while (!jamEntityTileMapCollision(self, world->worldMaps[0], self->x + sign(self->hSpeed), self->y))
			self->x += sign(self->hSpeed);
		self->hSpeed = 0;
	}
	if (jamEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + self->vSpeed)) {
		self->y -= sign(self->vSpeed);
		self->y = round(self->y);
		while (!jamEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + sign(self->vSpeed)))
			self->y += sign(self->vSpeed);
		self->vSpeed = 0;
	}
	self->x += self->hSpeed * jamRendererGetDelta();
	self->y += self->vSpeed * jamRendererGetDelta();

	jamAudioSetListenerPosition((float)self->x, (float)self->y, 0);

	/////////////// Testing ///////////////
	if (jamInputCheckMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		self->x = jamInputGetMouseX() + jamRendererGetCameraX();
		self->y = jamInputGetMouseY() + jamRendererGetCameraY();
	}

	//////////////////////// Player Animations ////////////////////////
	// We must invert the et_Player if he is going left
	if (self->hSpeed > 0)
		self->scaleX = 1;
	else if (self->hSpeed < 0)
		self->scaleX = -1;

	// Walking/standing animations
	if (self->hSpeed != 0)
		jamEntitySetSprite(self, jamAssetHandlerGetSprite(gHandler, "PlayerMovingSprite"));
	else
		jamEntitySetSprite(self, jamAssetHandlerGetSprite(gHandler, "PlayerStandingSprite"));

	if (!jamEntityTileMapCollision(self, world->worldMaps[0], self->x, self->y + 1))
		jamEntitySetSprite(self, jamAssetHandlerGetSprite(gHandler, "PlayerJumpingSprite"));
}

/////////////////////////////////////// The main menu ///////////////////////////////////////
bool runGame();
bool runMenu() { // Returns false if quit game
	// Menu-related variables
	bool play = false;
	bool runLoop = true;
	JamTexture* logoTex = jamTextureLoad("assets/jamengine.png");

	// The menu buttons
	JamTexture* playTex = jamTextureLoad("assets/playbutton.png");
	JamTexture* exitTex = jamTextureLoad("assets/exitbutton.png");
	JamTweeningState playTween;
	JamTweeningState exitTween;
	jamTweenInit(&playTween, 0.05, GAME_WIDTH - 172, GAME_WIDTH - 96);
	jamTweenInit(&exitTween, 0.05, GAME_WIDTH - 172, GAME_WIDTH - 96);
	bool inPlayButtonPreviousFrame = false;
	bool inExitButtonPreviousFrame = false;
	bool inPlayButton = false;
	bool inExitButton = false;
	int playButtonY = GAME_HEIGHT - 128;
	int exitButtonY = GAME_HEIGHT - 96;

	while (runLoop) {
		// Update the renderer and check for a quit signal
		runLoop = jamRendererProcEvents();

		if (runLoop) {
			jamDrawFillColour(255, 255, 255, 255);

			// Check if the mouse is in a button
			inPlayButton = pointInRectangle(jamInputGetMouseX(), jamInputGetMouseY(), GAME_WIDTH - 172, playButtonY, 172, 32);
			inExitButton = pointInRectangle(jamInputGetMouseX(), jamInputGetMouseY(), GAME_WIDTH - 172, exitButtonY, 172, 32);

			// Handle tweening changes
			if (inPlayButton != inPlayButtonPreviousFrame)
				jamTweenReverse(&playTween);
			if (inExitButton != inExitButtonPreviousFrame)
				jamTweenReverse(&exitTween);

			// And button presses
			if ((jamInputCheckMouseButtonPressed(MOUSE_LEFT_BUTTON) && inPlayButton) || jamInputCheckKeyPressed(JAM_KB_SPACE))
				runLoop = runGame();
			if ((jamInputCheckMouseButtonPressed(MOUSE_LEFT_BUTTON) && inExitButton) || jamInputCheckKeyPressed(JAM_KB_ESCAPE))
				runLoop = false;

			// Draw the buttons
			jamDrawTexture(playTex, (sint32) jamTweenUpdateParabolic(&playTween), playButtonY);
			jamDrawTexture(exitTex, (sint32) jamTweenUpdateParabolic(&exitTween), exitButtonY);

			jamDrawTexture(logoTex, 0, 0);

			inPlayButtonPreviousFrame = inPlayButton;
			inExitButtonPreviousFrame = inExitButton;
			jamRendererProcEndFrame();
		}
	}

	jamTextureFree(playTex);
	jamTextureFree(exitTex);
	jamTextureFree(logoTex);
	return play;
}
////////////////////////////////////////// The game /////////////////////////////////////////
bool runGame() {
	// Core game pieces
	bool mainMenu = false; // Weather or not return to main menu
	bool runLoop = true;
	int i;

	// Create the behaviour map
	JamBehaviourMap* bMap = jamBehaviourMapCreate();
	jamBehaviourMapAdd(bMap, "PlayerBehaviour", NULL, NULL, onPlayerFrame, NULL);
	jamBehaviourMapAdd(bMap, "EnemyBehaviour", onEnemyCreate, NULL, onEnemyFrame, NULL);

	// Load the assets and create the world
	gHandler = jamAssetHandlerCreate(1000);
	jamAssetHandlerLoadINI(gHandler, "assets/level0.ini", bMap);
	JamWorld* gameWorld = jamAssetHandlerGetWorld(gHandler, "GameWorld");
	JamFont* font = jamAssetHandlerGetFont(gHandler, "GameFont");

	// Some setup
	jamRendererSetCameraPos(25, 25);
	JamAudioBuffer* sound = jamAssetHandlerGetAudioBuffer(gHandler, "PopSound");
	JamAudioSource* source = jamAudioCreateSource();

	// We don't really care what went wrong, but if something went wrong while
	// while loading assets, we cannot continue.
	if (jGetError() == 0) {
		while (runLoop) {
			// Update the renderer and check for a quit signal
			runLoop = jamRendererProcEvents();

			if (runLoop) {
				jamDrawFillColour(0, 0, 0, 255);

				// Testing suite
				if (jamInputCheckKeyPressed(JAM_KB_F)) {
					jamRendererReset(0, 0, RENDERER_BORDERLESS_FULLSCREEN);
					jamRendererIntegerScale();
				}
				if (jamInputCheckKeyPressed(JAM_KB_G)) {
					jamRendererReset(GAME_WIDTH, GAME_HEIGHT, RENDERER_WINDOWED);
					jamRendererIntegerScale();
				}
				if (jamInputCheckKeyPressed(JAM_KB_P)) {
					source->xPosition = jamInputGetMouseX();
					source->yPosition = jamInputGetMouseY();
					jamAudioPlay(sound, source, false);
				}

				/////////////////////////// DRAWING THINGS //////////////////////////
				// Draw the background
				jamDrawTexture(jamAssetHandlerGetTexture(gHandler, "BackgroundTexture"), (sint32)jamRendererGetCameraX(), (sint32)jamRendererGetCameraX());

				// Draw the game world
				for (i = 0; i < MAX_TILEMAPS; i++)
					if (gameWorld->worldMaps[i] != NULL)
						jamDrawTileMap(gameWorld->worldMaps[i], 0, 0, 0, 0, 0, 0);
				jamWorldProcFrame(gameWorld);

				// Debug
				sint32 debugX = (sint32)jamRendererGetCameraX() + 16;
				sint32 debugY = (sint32)jamRendererGetCameraY() + 16;
				jamDrawRectangleFilled(debugX - 2, debugY + 2, jamFontWidth(font, "FPS: 60") + 4, 16);
				JamSprite* sprite = jamAssetHandlerGetSprite(gHandler, "EnemySprite");
				jamFontRender(font, debugX, debugY, "FPS: %f", round(jamRendererGetFramerate()), sprite);
				/////////////////////////////////////////////////////////////////////

				jamRendererProcEndFrame();
			}
		}
	}

	// Free up the resources
	jamAssetHandlerFree(gHandler);
	jamBehaviourMapFree(bMap);

	// Test stuff
	jamAudioFreeSource(source);

	return mainMenu;
}

void onFrame(JamWorld* world, JamEntity* self) {
	self->x += 4.0f - ((double)rand() / RAND_MAX * 8.0f);
	self->y += 4.0f - ((double)rand() / RAND_MAX * 8.0f);
}

void onDraw(JamWorld* world, JamEntity* self) {
	jamDrawEntity(self);
}

/////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	// Decide if we're in testing suite mode or not
	bool testingSuite = false;
	if (argc > 1 && strcmp("--testing-suite", argv[1]) == 0)
		testingSuite = true;

	// Initialize JamEngine
	jamRendererInit(&argc, argv, "JamEngine", SCREEN_WIDTH, SCREEN_HEIGHT, 60);
	jamRendererSetAA(false);
	jamRendererSetIcon("assets/icon.png");
	bool run = true;

	// Setup the screen
	jamRendererConfig(GAME_WIDTH, GAME_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT);

	if (!testingSuite) { // Run normally
		while (run) {
			run = runMenu();
			if (run)
				run = runGame();
		}
	} else { // Test specific functionality of JamEngine
		
	}

	jamRendererQuit();
	return 0;
}
#include "Audio.h"
#include <al.h>
#include <alc.h>
#include <alut.h>
#include "JamError.h"
#include <malloc.h>
#include <Audio.h>

static JamAudioPlayer* gAudioPlayer;
static JamAudioSource* gDefaultSource;

///////////////////////////////////////////////////////////////////////
void jamInitAudioPlayer() {
	if (gAudioPlayer != NULL) {
		gAudioPlayer = (JamAudioPlayer*)malloc(sizeof(JamAudioPlayer));
		gDefaultSource = (JamAudioSource*)calloc(1, sizeof(JamAudioSource));

		if (gAudioPlayer != NULL && gDefaultSource != NULL) {
			// Setup OpenAL device/context
			gAudioPlayer->audioDevice = alcOpenDevice(NULL);
			gAudioPlayer->audioContext = alcCreateContext(gAudioPlayer->audioDevice, NULL);

			if (gAudioPlayer->audioDevice != NULL && gAudioPlayer->audioContext != NULL) {
				// Setup the default source
				alGenSources(1, &gDefaultSource->soundID);
				gDefaultSource->gain = 1;
				gDefaultSource->pitch = 1;

				if (alGetError() != AL_NO_ERROR) {
					alcDestroyContext(gAudioPlayer->audioContext);
					alcCloseDevice(gAudioPlayer->audioDevice);
					free(gDefaultSource);
					free(gAudioPlayer);
					gDefaultSource = NULL;
					gAudioPlayer = NULL;
					jSetError(ERROR_OPENAL_ERROR, "Failed to create audio source");
				}
			} else {
				alcDestroyContext(gAudioPlayer->audioContext);
				alcCloseDevice(gAudioPlayer->audioDevice);
				free(gDefaultSource);
				free(gAudioPlayer);
				gDefaultSource = NULL;
				gAudioPlayer = NULL;
				jSetError(ERROR_OPENAL_ERROR, "Failed to initialize OpenAL");
			}
		} else {
			if (gAudioPlayer == NULL)
				jSetError(ERROR_ALLOC_FAILED, "Failed to allocate audio player");
			if (gDefaultSource == NULL)
				jSetError(ERROR_ALLOC_FAILED, "Failed to allocate default audio source");
			free(gDefaultSource);
			free(gAudioPlayer);
			gDefaultSource = NULL;
			gAudioPlayer = NULL;
		}
	}
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void jamFreeAudioPlayer() {
	// TODO: This
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
bool jamAudioIsInitialized() {
	return gAudioPlayer != NULL;
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
JamAudioSource* jamCreateAudioSource() {
	return NULL; // TODO: This
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void jamFreeAudioSource(JamAudioSource* source) {
	// TODO: This
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
JamAudioBuffer* jamLoadAudioBuffer(const char* filename) {
	return NULL; // TODO: This
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void jamFreeAudioBuffer(JamAudioBuffer* buffer) {
	// TODO: This
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void jamPlayAudio(JamAudioBuffer* buffer, JamAudioSource* source, bool looping) {
	// TODO: This
}
///////////////////////////////////////////////////////////////////////
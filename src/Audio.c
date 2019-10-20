#include "Audio.h"
#include <al.h>
#include <alc.h>
#include <alut.h>
#include "JamError.h"
#include <malloc.h>
#include <Audio.h>
#include <string.h>

static JamAudioPlayer* gAudioPlayer;
static JamAudioSource* gDefaultSource;
static float gGainMultiplier;

///////////////////////////////////////////////////////////////////////
void jamInitAudioPlayer() {
	if (gAudioPlayer == NULL) {
		gAudioPlayer = (JamAudioPlayer*)malloc(sizeof(JamAudioPlayer));
		gDefaultSource = (JamAudioSource*)calloc(1, sizeof(JamAudioSource));
		alutInit(NULL, NULL);
		gGainMultiplier = 1;

		if (gAudioPlayer != NULL && gDefaultSource != NULL) {
			// Setup OpenAL device/context
			gAudioPlayer->audioDevice = alcOpenDevice(NULL);
			gAudioPlayer->audioContext = alcCreateContext(gAudioPlayer->audioDevice, NULL);

			if (alGetError() == AL_NO_ERROR) {
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
					alutExit();
				}
			} else {
				alcDestroyContext(gAudioPlayer->audioContext);
				alcCloseDevice(gAudioPlayer->audioDevice);
				free(gDefaultSource);
				free(gAudioPlayer);
				gDefaultSource = NULL;
				gAudioPlayer = NULL;
				jSetError(ERROR_OPENAL_ERROR, "Failed to initialize OpenAL");
				alutExit();
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
			alutExit();
		}
	}
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void jamFreeAudioPlayer() {
	if (gDefaultSource != NULL)
		alDeleteSources(1, &gDefaultSource->soundID);
	if (gAudioPlayer != NULL) {
		alcDestroyContext(gAudioPlayer->audioContext);
		alcCloseDevice(gAudioPlayer->audioDevice);
	}
	alutExit();
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
bool jamAudioIsInitialized() {
	return gAudioPlayer != NULL;
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void jamAudioSetGlobalGain(float volume) {
	if (volume <= 1 && volume >= 0)
		gGainMultiplier = volume;
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
float jamAudioGetGlobalGain() {
	return gGainMultiplier;
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
JamAudioSource* jamCreateAudioSource() {
	JamAudioSource* src = (JamAudioSource*)calloc(1, sizeof(JamAudioSource));

	if (src != NULL) {
		alGenSources(1, &src->soundID);
		gDefaultSource->gain = 1;
		gDefaultSource->pitch = 1;

		if (alGetError() != AL_NO_ERROR) {
			free(src);
			src = NULL;
			jSetError(ERROR_OPENAL_ERROR, "Failed to create audio source");
		}
	} else {
		jSetError(ERROR_ALLOC_FAILED, "Failed to allocate audio source");
	}

	return src;
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void jamFreeAudioSource(JamAudioSource* source) {
	if (source != NULL) {
		alDeleteSources(1, &source->soundID);
		free(source);
	}
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
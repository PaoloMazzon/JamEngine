#include "Audio.h"
#include <al.h>
#include <alc.h>
#include <alut.h>
#include "JamError.h"
#include <malloc.h>
#include <Audio.h>
#include <string.h>

static JamAudioPlayer* gAudioPlayer;
static uint32 gDefaultSource;
static float gGainMultiplier;

///////////////////////////////////////////////////////////////////////
void jamInitAudioPlayer() {
	if (gAudioPlayer == NULL) {
		gAudioPlayer = (JamAudioPlayer*)malloc(sizeof(JamAudioPlayer));
		alutInit(NULL, NULL);
		gGainMultiplier = 1;

		if (gAudioPlayer != NULL) {
			// Setup OpenAL device/context
			gAudioPlayer->audioDevice = alcOpenDevice(NULL);
			gAudioPlayer->audioContext = alcCreateContext(gAudioPlayer->audioDevice, NULL);

			if (alGetError() == AL_NO_ERROR) {
				// Setup the default source
				alGenSources(1, &gDefaultSource);

				if (alGetError() == AL_NO_ERROR) {
					// Initialize the default source
					alSourcef(gDefaultSource, AL_PITCH, 1);
					alSource3f(gDefaultSource, AL_POSITION, 0, 0, 0);
					alSource3f(gDefaultSource, AL_VELOCITY, 0, 0, 0);
				} else {
					alcDestroyContext(gAudioPlayer->audioContext);
					alcCloseDevice(gAudioPlayer->audioDevice);
					free(gAudioPlayer);
					gAudioPlayer = NULL;
					jSetError(ERROR_OPENAL_ERROR, "Failed to create audio source");
					alutExit();
				}
			} else {
				alcDestroyContext(gAudioPlayer->audioContext);
				alcCloseDevice(gAudioPlayer->audioDevice);
				free(gAudioPlayer);
				gAudioPlayer = NULL;
				jSetError(ERROR_OPENAL_ERROR, "Failed to initialize OpenAL");
				alutExit();
			}
		} else {
			if (gAudioPlayer == NULL)
				jSetError(ERROR_ALLOC_FAILED, "Failed to allocate audio player");
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
		alDeleteSources(1, &gDefaultSource);
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
	JamAudioSource* src = NULL;

	if (gAudioPlayer != NULL) {
		src = (JamAudioSource *) calloc(1, sizeof(JamAudioSource));

		if (src != NULL) {
			alGenSources(1, &src->soundID);
			src->gain = 1;
			src->pitch = 1;

			if (alGetError() != AL_NO_ERROR) {
				free(src);
				src = NULL;
				jSetError(ERROR_OPENAL_ERROR, "Failed to create audio source");
			}
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate audio source");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Audio player has not been initialized");
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
	JamAudioBuffer* buf = NULL;

	if (gAudioPlayer != NULL) {
		buf = (JamAudioBuffer *) calloc(1, sizeof(JamAudioSource));

		if (buf != NULL) {
			buf->bufferID = alutCreateBufferFromFile(filename);

			if (alGetError() != AL_NO_ERROR) {
				free(buf);
				buf = NULL;
				jSetError(ERROR_OPENAL_ERROR, "Failed to create audio source");
			}
		} else {
			jSetError(ERROR_ALLOC_FAILED, "Failed to allocate audio source");
		}
	} else {
		jSetError(ERROR_NULL_POINTER, "Audio player has not been initialized");
	}

	return buf;
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void jamFreeAudioBuffer(JamAudioBuffer* buffer) {
	if (buffer != NULL) {
		alDeleteBuffers(1, &buffer->bufferID);
		free(buffer);
	}
}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void jamPlayAudio(JamAudioBuffer* buffer, JamAudioSource* source, bool looping) {
	if (buffer != NULL && gAudioPlayer != NULL) {
		// Setup the surrounding audio playback parameters
		if (source != NULL) {
			alSourcef(source->soundID, AL_PITCH, source->pitch);
			alSourcef(source->soundID, AL_GAIN, source->gain * gGainMultiplier);
			alSource3f(source->soundID, AL_POSITION, source->xPosition, source->yPosition, source->zPosition);
			alSource3f(source->soundID, AL_VELOCITY, source->xVelocity, source->yVelocity, source->zVelocity);
			alSourcei(source->soundID, AL_LOOPING, source->looping);

			// Bind the sound and play it
			alSourcei(source->soundID, AL_BUFFER, buffer->bufferID);
			alSourcePlay(source->soundID);
		} else {
			alSourcei(gDefaultSource, AL_LOOPING, looping);
			alSourcef(gDefaultSource, AL_GAIN, gGainMultiplier);

			// Bind the sound and play it
			alSourcei(gDefaultSource, AL_BUFFER, buffer->bufferID);
			alSourcePlay(gDefaultSource);
		}
	} else {
		if (buffer == NULL)
			jSetError(ERROR_NULL_POINTER, "Buffer does not exist");
		if (gAudioPlayer == NULL)
			jSetError(ERROR_NULL_POINTER, "Audio player has not been initialized");
	}
}
///////////////////////////////////////////////////////////////////////
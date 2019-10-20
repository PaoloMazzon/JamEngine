/// \file Audio.h
/// \author plo
/// \brief A (somewhat) simple audio player for JamEngine
#pragma once
#include "Constants.h"

/// \brief An audio playback system for JamEngine
typedef struct {
	void* audioDevice; ///< The OpenAL audio device, internally ALCdevice*
	void* audioContext; ///< OpenAL audio context, internally ALCcontext*
	float listenerOrientation[6]; ///< The orientation of the listener
} JamAudioPlayer;

/// \brief A sound
typedef struct {

} JamAudioBuffer;

/// \brief A source from which to play sound
///
/// There are a few other options that OpenAL supports that
/// aren't listed here because they're very edge case for a 2D
/// game. If you do want access to things like the sound cone and
/// rolloff, it wouldn't be too much trouble to add.
typedef struct {
	uint32 soundID; ///< The internal id of the sound (for OpenAL)
	float pitch; ///< The pitch of the audio from 0 - 1
	float gain; ///< The gain of the velocity from 0 - 1
	float xVelocity; ///< The x velocity at which this source is travelling
	float yVelocity; ///< The y velocity at which this source is travelling
	float zVelocity; ///< The z velocity at which this source is travelling
	float xPosition; ///< The x position of this source
	float yPosition; ///< The y position of this source
	float zPosition; ///< The z position of this source
	bool looping; ///< Weather or not to loop this audio
} JamAudioSource;

/// \brief Initializes the audio system
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPENAL_ERROR
void jamInitAudioPlayer();

/// \brief Quits the audio system
void jamFreeAudioPlayer();

/// \brief Creates an audio source
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPENAL_ERROR
JamAudioSource* jamCreateAudioSource();

/// \brief Frees an audio source
void jamFreeAudioSource(JamAudioSource* source);

/// \brief Loads audio from a file
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPENAL_ERROR
JamAudioBuffer* jamLoadAudioBuffer(const char* filename);

/// \brief Frees an audio buffer
void jamFreeAudioBuffer(JamAudioBuffer* buffer);
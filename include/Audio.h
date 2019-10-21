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

/// \brief A sound in memory
typedef struct {
	uint32 bufferID; ///< The internal id of the buffer (for OpenAL)
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
void jamInitAudioPlayer(int* argc, char** argv);

/// \brief Quits the audio system
void jamFreeAudioPlayer();

/// \brief Returns weather or not the audio player was successfully initialized
bool jamAudioIsInitialized();

/// \brief Sets the global gain multiplier
///
/// All source's gain is multiplied by the global gain multiplier
/// when it is played. Say for example, your user wants the game
/// to be at 38% volume. You could just set this value to 0.38
///
/// By default the global gain multiplier is 1.
void jamAudioSetGlobalGain(float volume);

/// \brief Gets the global gain multiplier
///
/// All source's gain is multiplied by the global gain multiplier
/// when it is played. Say for example, your user wants the game
/// to be at 38% volume. You could just set this value to 0.38
///
/// By default the global gain multiplier is 1.
float jamAudioGetGlobalGain();

/// \brief Creates an audio source
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPENAL_ERROR
JamAudioSource* jamCreateAudioSource();

/// \brief Frees an audio source
void jamFreeAudioSource(JamAudioSource* source);

/// \brief Loads audio from a .wav file
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPENAL_ERROR
JamAudioBuffer* jamLoadAudioBufferFromWAV(const char *filename);

/// \brief Frees an audio buffer
void jamFreeAudioBuffer(JamAudioBuffer* buffer);

/// \brief Plays some audio
///
/// Source can be null, in which case the default source will be
/// used (basically the sound will be played right beside the
/// listener)
///
/// The looping argument only applies if you don't specify a source.
///
/// \throws ERROR_OPENAL_ERROR
/// \throws ERROR_NULL_POINTER
void jamPlayAudio(JamAudioBuffer* buffer, JamAudioSource* source, bool looping);
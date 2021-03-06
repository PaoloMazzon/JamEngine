/// \file Audio.h
/// \author plo
/// \brief A (somewhat) simple audio player for JamEngine
#pragma once
#include "Constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief An audio playback system for JamEngine
typedef struct {
	void *audioDevice; ///< The OpenAL audio device, internally ALCdevice*
	void *audioContext; ///< OpenAL audio context, internally ALCcontext*
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
///
/// This is for 3D audio, and if you don't need 3D audio you'll
/// never need to use this struct (just pass NULL to jamPlayAudio)
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
void jamAudioInit(int *argc, char **argv);

/// \brief Quits the audio system
void jamAudioQuit();

/// \brief Returns weather or not the audio player was successfully initialized
bool jamAudioIsInitialized();

/// \brief Sets the listener's position in the game world
///
/// This is for 3D audio, and if you don't need 3D audio you'll
/// never need to call this function so don't fret about it.
///
/// \throws ERROR_NULL_POINTER
void jamAudioSetListenerPosition(float x, float y, float z);

/// \brief Sets the listener's velocity in the game world
///
/// This is for 3D audio, and if you don't need 3D audio you'll
/// never need to call this function so don't fret about it.
///
/// \throws ERROR_NULL_POINTER
void jamAudioSetListenerVelocity(float x, float y, float z);

/// \brief Sets the listener's prespective in the game world
///
/// This is for 3D audio, and if you don't need 3D audio you'll
/// never need to call this function so don't fret about it.
///
/// \throws ERROR_NULL_POINTER
void jamAudioSetListenerOrientation(float atX, float atY, float atZ, float upX, float upY, float upZ);

/// \brief Sets the listener's prespective in the game world
///
/// This is for 3D audio, and if you don't need 3D audio you'll
/// never need to call this function so don't fret about it.
///
/// \throws ERROR_NULL_POINTER
void jamAudioSetListenerGain(float gain);

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

/// \brief Pauses an audio source
void jamAudioSourcePause(JamAudioSource* source);

/// \brief Rewinds an audio source back to the start
void jamAudioSourceRewind(JamAudioSource* source);

/// \brief Creates an audio source
///
/// This is for 3D audio, and if you don't need 3D audio you'll
/// never need to call this function so don't fret about it.
///
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPENAL_ERROR
JamAudioSource* jamAudioCreateSource();

/// \brief Frees an audio source
void jamAudioFreeSource(JamAudioSource *source);

/// \brief Updates internal values of the source
///
/// The source's internal OpenAL values are only updated when
/// you call jamPlayAudio or this function to save performance.
/// Usually, jamPlayAudio updating things is good enough, but
/// if you have something looping for example, you may need to
/// call this every time you update the source.
///
/// This is for 3D audio, and if you don't need 3D audio you'll
/// never need to call this function so don't fret about it.
void jamAudioUpdateSource(JamAudioSource *source);

/// \brief Loads audio from a file
///
/// Currently, JamEngine supports .ogg files and .wav files.
///
/// \throws ERROR_ALLOC_FAILED
/// \throws ERROR_OPENAL_ERROR
JamAudioBuffer *jamAudioLoadBuffer(const char *filename);

/// \brief Frees an audio buffer
void jamAudioFreeBuffer(JamAudioBuffer *buffer);

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
void jamAudioPlay(JamAudioBuffer *buffer, JamAudioSource *source, bool looping);

#ifdef __cplusplus
}
#endif
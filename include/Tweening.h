/// \file Tweening.h
/// \author plo
/// \brief Deals with animation in-betweening on a low level
/// \warning The functions in this file all expect a struct reference to be
/// passed into them and as such are unsafe (they don't check if the pointers
/// aren't null). If you don't pass by reference you may get segfaults that
/// JamEngine doesn't report an error for making debugging very hard.

#pragma once
#include "Constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief The information on a tween, which is manipulated by jamUpdateTween* functions
///
/// Typically, you create this struct not as a pointer and just pass by reference to the
/// functions (`jamUpdateTweenLinearForward(&myState);`). You can update the internal values
/// all you want but it is recommended you stick to these functions.
/// Currently, there are only three different kinds of tweening animations
/// supported, but it is really simple to add more. Internally, the tweening update functions
/// just plot a graph using JamTweeningState.progress as x.
typedef struct {
	float progress; ///< The total progress so far in the animation from 0 to 1 (start this at 0)
	float progressPerStep; ///< The progress to increment each time a tweening function is called in percentage
	double initialValue; ///< The value the tween should start at (in whatever unit you're using)
	double finalValue; ///< The final value the tween should end at (in whatever unit you're using)
	double tempValue; ///< Temporary value that is used to reverse the tween smoothly
	bool usingTempVal; ///< Weather or not its currently using the temp value
} JamTweeningState;

/// \brief Sets all the values automatically
void jamTweenInit(JamTweeningState *state, float progressPerStep, double initialValue, double finalValue);

/// \brief Updates a tweening state using a positive-slope linear graph
/// \return The current value the tween is at in the animation
///
/// Once a tweening animation is finished, this will continually return final value
double jamTweenUpdateLinear(JamTweeningState *state);

/// \brief Updates a tweening state using a parabolic curve for a smooth finish
/// \return The current value the tween is at in the animation
///
/// Once a tweening animation is finished, this will continually return final value
double jamTweenUpdateParabolic(JamTweeningState *state);

/// \brief Updates a tweening state using a parabolic curve with a jump over 1 before reaching 1 for some "punch"
/// \return The current value the tween is at in the animation
///
/// Once a tweening animation is finished, this will continually return final value
double jamTweenUpdateParabolicJump(JamTweeningState *state);

/// \brief Swaps the initial and final state then inverses the progress
void jamTweenReverse(JamTweeningState *state);

/// \brief This essentially just says if progress is at 1 or not, made into a function for clarity in code
bool jamTweenFinished(JamTweeningState *state);

#ifdef __cplusplus
}
#endif
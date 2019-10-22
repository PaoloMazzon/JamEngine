/// \file Tweening.h
/// \author plo
/// \brief Deals with animation in-betweening on a low level

#pragma once
#include "Constants.h"

/// \brief The information on a tween, which is manipulated by jamUpdateTween* functions
///
/// Typically, you create this struct not as a pointer and just pass by reference to the
/// functions (`jamUpdateTweenLinearForward(&myState);`). This means you must set the
/// values yourself.
typedef struct {
	float progress; ///< The total progress so far in the animation from 0 to 1 (start this at 0)
	float progressPerStep; ///< The progress to increment each time a tweening function is called in percentage
	double initialValue; ///< The value the tween should start at (in whatever unit you're using)
	double finalValue; ///< The final value the tween should end at (in whatever unit you're using)
} JamTweeningState;

/// \brief Updates a tweening state using a positive-slope linear graph
double jamUpdateTweenLinear(JamTweeningState* state);

/// \brief Updates a tweening state using a parabolic curve for a smooth finish
double jamUpdateTweenParabolic(JamTweeningState* state);

/// \brief Updates a tweening state using a parabolic curve with a jump over 1 before reaching 1 for some "punch"
double jamUpdateTweenParabolicJump(JamTweeningState* state);

/// \brief This essentially just says if progress is at 1 or not, made into a function for clarity in code
bool jamIsTweenFinished(JamTweeningState* state);
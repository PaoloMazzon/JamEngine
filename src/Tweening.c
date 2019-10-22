//
// Created by plo on 10/21/19.
//
#include <math.h>
#include <Tweening.h>
#include "Tweening.h"

static inline float _updatePos(float pos, float increment) {
	if (pos + increment > 1)
		return 1;
	else
		return pos + increment;
}

////////////////////////////////////////////////////////////////////
double jamUpdateTweenLinear(JamTweeningState* state) {
	state->progress = _updatePos(state->progress, state->progressPerStep);
	return state->initialValue + (state->progress * (state->finalValue - state->initialValue));
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double jamUpdateTweenParabolic(JamTweeningState* state) {
	state->progress = _updatePos(state->progress, state->progressPerStep);
	return state->initialValue + ((-pow(state->progress - 1, 2) + 1) * (state->finalValue - state->initialValue));
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double jamUpdateTweenParabolicJump(JamTweeningState* state) {
	state->progress = _updatePos(state->progress, state->progressPerStep);
	return state->initialValue + ((1.168056 - (2.2222222 * pow(state->progress - 0.725, 2))) * (state->finalValue - state->initialValue));
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void jamReverseTween(JamTweeningState* state) {
	double x = state->initialValue;
	state->initialValue = state->finalValue;
	state->finalValue = x;
	state->progress = 0;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
bool jamIsTweenFinished(JamTweeningState* state) {
	return state->progress == 1;
}
////////////////////////////////////////////////////////////////////
//
// Created by plo on 10/21/19.
//
#include <math.h>
#include <Tweening.h>
#include "Tweening.h"

static inline void _updatePos(float* pos, float increment) {
	(*pos) += increment;
	if (*pos > 1) (*pos) == 1;
}

////////////////////////////////////////////////////////////////////
double jamUpdateTweenLinear(JamTweeningState* state) {
	_updatePos(&state->progress, state->progressPerStep);
	return state->initialValue + (state->progress * (state->finalValue - state->initialValue));
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double jamUpdateTweenParabolic(JamTweeningState* state) {
	_updatePos(&state->progress, state->progressPerStep);
	return state->initialValue + ((-pow(state->progress - 1, 2) + 1) * (state->finalValue - state->initialValue));
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double jamUpdateTweenParabolicJump(JamTweeningState* state) {
	_updatePos(&state->progress, state->progressPerStep);
	return state->initialValue + (((841/720) - ((20/9) * pow(state->progress - (29/40), 2))) * (state->finalValue - state->initialValue));
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
bool jamIsTweenFinished(JamTweeningState* state) {
	return state->progress == 1;
}
////////////////////////////////////////////////////////////////////
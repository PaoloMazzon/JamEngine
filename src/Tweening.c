//
// Created by plo on 10/21/19.
//
#include <math.h>
#include <Tweening.h>
#include "Tweening.h"

static void _updatePos(JamTweeningState* state) {
	if (state->progress + state->progressPerStep > 1) {
		state->progress = 1;

		if (state->usingTempVal) {
			state->initialValue = state->tempValue;
			state->usingTempVal = false;
		}
	} else {
		state->progress += state->progressPerStep;
	}
}

////////////////////////////////////////////////////////////////////
void jamTweenInit(JamTweeningState *state, float progressPerStep, double initialValue, double finalValue) {
	state->tempValue = 0;
	state->progress = 0;
	state->progressPerStep = progressPerStep;
	state->initialValue = initialValue;
	state->finalValue = finalValue;
	state->usingTempVal = false;
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double jamTweenUpdateLinear(JamTweeningState *state) {
	_updatePos(state);
	return state->initialValue + (state->progress * (state->finalValue - state->initialValue));
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double jamTweenUpdateParabolic(JamTweeningState *state) {
	_updatePos(state);
	return state->initialValue + ((-pow(state->progress - 1, 2) + 1) * (state->finalValue - state->initialValue));
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
double jamTweenUpdateParabolicJump(JamTweeningState *state) {
	_updatePos(state);
	return state->initialValue + ((1.168056 - (2.2222222 * pow(state->progress - 0.725, 2))) * (state->finalValue - state->initialValue));
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
void jamTweenReverse(JamTweeningState *state) {
	double x;
	if (state->progress == 1 || state->progress == 0) {
		x = state->initialValue;
		state->initialValue = state->finalValue;
		state->finalValue = x;
		state->progress = 1 - state->progress;
	} else if (!state->usingTempVal) {
		x = state->initialValue;
		state->tempValue = state->finalValue;
		state->initialValue = state->initialValue + (state->progress * (state->finalValue - state->initialValue));
		state->finalValue = x;
		state->usingTempVal = true;
		state->progress = 0;
	} else {
		x = state->tempValue;
		state->initialValue = state->initialValue + (state->progress * (state->finalValue - state->initialValue));
		state->tempValue = state->finalValue;
		state->finalValue = x;
		state->progress = 0;
	}
}
////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////
bool jamTweenFinished(JamTweeningState *state) {
	return state->progress == 1;
}
////////////////////////////////////////////////////////////////////
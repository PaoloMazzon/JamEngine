#include "Profiler.h"
#include <SDL.h>

/////////////////////////////////////////////////////////////////
JamProfile jamProfileStart() {
	JamProfile prof;
	prof.iterations = 0;
	prof.totalTicks = 0;
	prof.previousTime = SDL_GetPerformanceCounter();
	return prof;
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void jamProfileSetPreviousTime(JamProfile* profile) {
	profile->previousTime = SDL_GetPerformanceCounter();
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
void jamProfileTick(JamProfile* profile) {
	profile->totalTicks += SDL_GetPerformanceCounter() - profile->previousTime;
	profile->iterations++;
	profile->previousTime = SDL_GetPerformanceCounter();
}
/////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
double jamProfileGetMilliseconds(JamProfile* profile) {
	return (((double)profile->totalTicks / (double)profile->iterations) / (double)SDL_GetPerformanceFrequency()) * 1000;
}
/////////////////////////////////////////////////////////////////
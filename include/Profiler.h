/// \file Profiler.h
/// \brief A simple tool for profiling code (testing performance)

#pragma once
#include "Constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Holds data for profiling a few different ways
///
/// To properly measure how long it takes to execute some bit of code,
/// you are best off testing that code a very large amount of times.
/// To be more specific, you should first call `jamProfileStart`, then
/// run your code repeatedly many times (how many depends on your code,
/// 1000 is generally a good starting point), and at the end of each iteration
/// of your code, call `jamProfileTick`. Once your done, call `jamProfileGetMilliseconds`
/// to get the average amount of time the code took to execute in milliseconds.
///
/// If you have some large project and you want to profile a specific portion
/// of it and can't really just stick it in a for loop, you can use jamProfileSetPreviousTime
/// before you call said function then call jamProfileTick after it and only
/// that specific function will be profiled.
typedef struct {
	uint64 previousTime; ///< Previous recorded time
	uint64 totalTicks;   ///< The total time recorded
	uint64 iterations;   ///< Number of iterations recorded
} JamProfile;

/// \brief Starts a performance profile
JamProfile jamProfileStart();

/// \brief Records the current time as the previous time for profiling specific things
void jamProfileSetPreviousTime(JamProfile* profile);

/// \brief Calculates the average amount of time over x iterations in seconds
void jamProfileTick(JamProfile *profile);

/// \brief Returns the average amount of time in milliseconds for a profiler
///
/// This function is not perfect as there is a slight overhead cost from just
/// calling `jamProfileTick` so don't take this as any sort of scientific value,
/// its really just a fairly accurate estimate of the average time some code takes
/// to execute. It is best for checking weather or not you are actually improving
/// performance with some optimizations (and by how much).
double jamProfileGetMilliseconds(JamProfile *profile);

#ifdef __cplusplus
}
#endif
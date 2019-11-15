/// \file Clock.h
/// \author lugi1
/// \brief This is a very nice tool that makes precise timing easy
#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// \brief Returns time in nanoseconds
///
/// The value returned is not guaranteed to be anything but
/// accurate relative to the previous call. This is to say you
/// can't reliably use this to get the current time across platforms,
/// this is strictly for testing how long it takes blocks of
/// code to execute (The renderer uses this to calculate the
/// time between frames and measure delta time).
uint64_t ns();

/// \brief Sleeps for a given amount of time (in nanoseconds)
///
/// This function is not precise at high values since it won't
/// sleep away interrupted values on unix machines.
void jamSleep(uint64_t nsTime);

#ifdef __cplusplus
}
#endif
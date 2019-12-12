/// \file Profiler.h
/// \brief A library for profiling code (testing performance)

#include "Constants.h"

/// \brief Starts a performance profile
uint64 jamStartProfile();

/// \brief Calculates the average amount of time over x iterations in seconds
double jamGetProfileTimeSeconds(uint64 profile, double iterations);

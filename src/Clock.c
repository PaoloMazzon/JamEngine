/// The code between the slash blocks is credit to @roxlu on Twitter
/// or http://roxlu.com/
#include "Clock.h"
#include <stdio.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////////
#if defined(__linux)
#	define HAVE_POSIX_TIMER
#	include <time.h>
#	ifdef CLOCK_MONOTONIC
#		define CLOCKID CLOCK_MONOTONIC
#	else
#		define CLOCKID CLOCK_REALTIME
#	endif
#elif defined(__APPLE__)
#	define HAVE_MACH_TIMER
#	include <mach/mach_time.h>
#elif defined(_WIN32)
#	include <windows.h>
#endif

/// \brief Grabs current time in nanoseconds
uint64_t ns() {
	static uint64_t is_init = 0;
#if defined(__APPLE__)
	static mach_timebase_info_data_t info;
		if (is_init == 0) {
			mach_timebase_info(&info);
			is_init = 1;
		}
		uint64_t now = mach_absolute_time();
		now *= info.numer;
		now /= info.denom;
		return now;
#elif defined(__linux)
	static struct timespec linux_rate;
		if (is_init == 0) {
			clock_getres(CLOCKID, &linux_rate);
			is_init = 1;
		}
		uint64_t now;
		clock_gettime(CLOCKID, &linux_rate);
		now = (uint64_t)(linux_rate.tv_sec * 1.0e9 + linux_rate.tv_nsec);
		return now;
#elif defined(_WIN32)
	static LARGE_INTEGER win_frequency;
	if (is_init == 0) {
		QueryPerformanceFrequency(&win_frequency);
		is_init = 1;
	}
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (uint64_t) ((1e9 * now.QuadPart) / win_frequency.QuadPart);
#endif
}
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
void jamSleep(uint64_t nsTime) {
#if defined(__linux)
	struct timespec t;
	t.tv_nsec = nsTime;
	nanosleep(&t, NULL);
#elif defined(_WIN32)
	Sleep(nsTime / 1000000);
#endif
}
/////////////////////////////////////////////////////////

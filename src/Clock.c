#include "Clock.h"
#include <stdio.h>

/////////////////////////////////////////////////////////
void jamSleep(uint64_t nsTime) {
#if defined(__linux)
	struct timespec t;
	t.tv_sec = (time_t)(int)(nsTime / 1000000000);
	t.tv_nsec = nsTime;
	nanosleep(&t, NULL);
#elif defined(_WIN32)
	Sleep(nsTime / 1000000);
#endif
}
/////////////////////////////////////////////////////////

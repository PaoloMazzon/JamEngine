// Code from roxlu.com

#include "Clock.h"
#include <stdio.h>

/////////////////////////////////////////////////////////
void sleep(uint64_t nsTime) {
	uint64_t startTime = ns();
	uint64_t newTime;

	do {
		newTime = ns();
	} while (newTime - startTime < nsTime);
}
/////////////////////////////////////////////////////////

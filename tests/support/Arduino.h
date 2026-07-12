#ifndef COUNTIMER_TEST_ARDUINO_H
#define COUNTIMER_TEST_ARDUINO_H

#include <stdint.h>

extern uint32_t g_fakeMillis;

inline uint32_t millis()
{
	return g_fakeMillis;
}

#endif
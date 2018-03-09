#ifndef MOCK_MICROS_H_INCLUDED
#define MOCK_MICROS_H_INCLUDED

#include "BaseMock.h"

static int microsCallCount = 0;
static uint32_t microsReading[8] = {};
MOCK uint32_t micros(void) {
	return microsReading[microsCallCount++];
}

static uint16_t delayMicroseconds_last_delay = 0;
MOCK void delayMicroseconds(uint16_t delay) {
	delayMicroseconds_last_delay = delay;
}

static void set_micros_return_value(uint32_t v0) {
	microsReading[0] = v0;
	microsCallCount = 0;
}

static void set_micros_return_values(uint32_t v0, uint32_t v1) {
	microsReading[0] = v0;
	microsReading[1] = v1;
	microsCallCount = 0;
}

#include "EndMocks.h"

#endif

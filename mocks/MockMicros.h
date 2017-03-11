#ifndef MOCK_MICROS_H_INCLUDED
#define MOCK_MICROS_H_INCLUDED

#include "BaseMock.h"

static uint8_t SREG = 0;

static int microsCallCount = 0;
static uint32_t microsReading[8] = {};
MOCK uint32_t micros(void) {
	return microsReading[microsCallCount++];
}

#include "EndMocks.h"

#endif

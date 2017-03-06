#ifndef MOCK_SREG_H_INCLUDED
#define MOCK_SREG_H_INCLUDED

#include "BaseMock.h"

static uint8_t SREG = 0;

static int cliCallCount = 0;
MOCK void cli(void) {
	++ cliCallCount;
	SREG = 0;
}

#endif

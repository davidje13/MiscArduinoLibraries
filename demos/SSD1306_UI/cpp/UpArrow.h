#ifndef UP_ARROW_H_INCLUDED
#define UP_ARROW_H_INCLUDED

// Auto-generated from UpArrow.h.gen

#include <ProgMem.h>

static PROGMEM const uint8_t UP_ARROW_PM[] = {
	0x84, 0x86, 0xc7, 0xc6, 0xc4
};

#define UP_ARROW MakeProgMem(UP_ARROW_PM)

#define UP_ARROW_WIDTH 5
#define UP_ARROW_HEIGHT 3

#define UP_ARROW_STEP UP_ARROW_WIDTH
#define MakeUpArrowBitmask() MakeStaticBitmask18( \
	UP_ARROW, \
	UP_ARROW_WIDTH, \
	UP_ARROW_HEIGHT \
)

#endif

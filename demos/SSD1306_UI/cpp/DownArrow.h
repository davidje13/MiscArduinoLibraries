#ifndef DOWN_ARROW_H_INCLUDED
#define DOWN_ARROW_H_INCLUDED

// Auto-generated from DownArrow.h.gen

#include <ProgMem.h>

static PROGMEM const uint8_t DOWN_ARROW_PM[] = {
	0x01, 0x03, 0x07, 0x03, 0x01
};

#define DOWN_ARROW MakeProgMem(DOWN_ARROW_PM)

#define DOWN_ARROW_WIDTH 5
#define DOWN_ARROW_HEIGHT 3

#define DOWN_ARROW_STEP DOWN_ARROW_WIDTH
#define MakeDownArrowBitmask() MakeStaticBitmask18( \
	DOWN_ARROW, \
	DOWN_ARROW_WIDTH, \
	DOWN_ARROW_HEIGHT \
)

#endif

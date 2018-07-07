#ifndef FONT_FIXED_H_INCLUDED
#define FONT_FIXED_H_INCLUDED

// Auto-generated from FontFixed.h.gen

#include <ProgMem.h>

// Fixed-width (5x8) font
static PROGMEM const uint8_t FONT_FIXED_DATA[] = {
	// data section defines:
	//  FONTHEIGHT
	//  LINEHEIGHT
	//  ASCII START CHAR (inclusive)
	//  ASCII END CHAR (inclusive)
	//  DATA LAYOUT:
	//    0x00 = 1 bitmap image, chars from left-to-right
	//    0x01 = 1 bitmap image per character
	//    0x02 = 1 2D bitmap image, chars left-to-right, top-to-bottom, with width given below
	//  WIDTHS:
	//    layout 0x00: one int per char
	//    layout 0x01: one int for all chars
	//    layout 0x02: one int for all chars then one for # chars per line
	8, 8, 0x20, 0x7E, 0x01,
	5,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f,
	0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x14,
	0x7f, 0x14, 0x7f, 0x14, 0x04, 0x2a, 0x7f, 0x2a,
	0x10, 0x43, 0x33, 0x08, 0x66, 0x61, 0x36, 0x49,
	0x55, 0x22, 0x50, 0x00, 0x00, 0x03, 0x00, 0x00,
	0x00, 0x1c, 0x22, 0x41, 0x00, 0x00, 0x41, 0x22,
	0x1c, 0x00, 0x2a, 0x1c, 0x3e, 0x1c, 0x2a, 0x08,
	0x08, 0x3e, 0x08, 0x08, 0x00, 0x80, 0x40, 0x00,
	0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x40, 0x00, 0x00, 0x40, 0x30, 0x08, 0x06, 0x01,
	0x3e, 0x51, 0x49, 0x45, 0x3e, 0x44, 0x42, 0x7f,
	0x40, 0x40, 0x42, 0x61, 0x51, 0x49, 0x46, 0x22,
	0x41, 0x49, 0x49, 0x36, 0x0f, 0x08, 0x7e, 0x08,
	0x08, 0x4f, 0x49, 0x49, 0x49, 0x31, 0x3e, 0x49,
	0x49, 0x49, 0x32, 0x01, 0x01, 0x71, 0x0d, 0x03,
	0x36, 0x49, 0x49, 0x49, 0x36, 0x26, 0x49, 0x49,
	0x49, 0x3e, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00,
	0x80, 0x44, 0x00, 0x00, 0x00, 0x08, 0x14, 0x22,
	0x00, 0x00, 0x14, 0x14, 0x14, 0x00, 0x00, 0x22,
	0x14, 0x08, 0x00, 0x02, 0x01, 0x51, 0x09, 0x06,
	0x3e, 0x41, 0x5d, 0x55, 0x5e, 0x7e, 0x09, 0x09,
	0x09, 0x7e, 0x7f, 0x49, 0x49, 0x49, 0x36, 0x3e,
	0x41, 0x41, 0x41, 0x22, 0x7f, 0x41, 0x41, 0x41,
	0x3e, 0x7f, 0x49, 0x49, 0x41, 0x41, 0x7f, 0x09,
	0x09, 0x01, 0x01, 0x3e, 0x41, 0x49, 0x49, 0x7a,
	0x7f, 0x08, 0x08, 0x08, 0x7f, 0x41, 0x41, 0x7f,
	0x41, 0x41, 0x41, 0x41, 0x3f, 0x01, 0x01, 0x7f,
	0x08, 0x14, 0x22, 0x41, 0x7f, 0x40, 0x40, 0x40,
	0x40, 0x7f, 0x02, 0x04, 0x02, 0x7f, 0x7f, 0x02,
	0x04, 0x08, 0x7f, 0x3e, 0x41, 0x41, 0x41, 0x3e,
	0x7f, 0x09, 0x09, 0x09, 0x06, 0x3e, 0x41, 0x51,
	0x61, 0x7e, 0x7f, 0x09, 0x09, 0x09, 0x76, 0x26,
	0x49, 0x49, 0x49, 0x32, 0x01, 0x01, 0x7f, 0x01,
	0x01, 0x3f, 0x40, 0x40, 0x40, 0x3f, 0x03, 0x1c,
	0x60, 0x1c, 0x03, 0x7f, 0x20, 0x10, 0x20, 0x7f,
	0x63, 0x14, 0x08, 0x14, 0x63, 0x03, 0x04, 0x78,
	0x04, 0x03, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00,
	0x00, 0x7f, 0x41, 0x00, 0x01, 0x06, 0x08, 0x30,
	0x40, 0x00, 0x41, 0x7f, 0x00, 0x00, 0x04, 0x02,
	0x01, 0x02, 0x04, 0x40, 0x40, 0x40, 0x40, 0x40,
	0x00, 0x01, 0x02, 0x00, 0x00, 0x38, 0x44, 0x44,
	0x44, 0x7c, 0x7f, 0x44, 0x44, 0x44, 0x38, 0x38,
	0x44, 0x44, 0x44, 0x28, 0x38, 0x44, 0x44, 0x44,
	0x7f, 0x38, 0x54, 0x54, 0x54, 0x58, 0xf8, 0x24,
	0x24, 0x24, 0x04, 0x18, 0xa4, 0xa4, 0xa4, 0x7c,
	0x7f, 0x04, 0x04, 0x04, 0x78, 0x00, 0x00, 0x7d,
	0x00, 0x00, 0x00, 0x80, 0x7d, 0x00, 0x00, 0x7f,
	0x10, 0x18, 0x24, 0x40, 0x00, 0x00, 0x3f, 0x40,
	0x00, 0x78, 0x04, 0x18, 0x04, 0x78, 0x7c, 0x04,
	0x04, 0x04, 0x78, 0x38, 0x44, 0x44, 0x44, 0x38,
	0xfc, 0x24, 0x24, 0x24, 0x18, 0x18, 0x24, 0x24,
	0x24, 0xfc, 0x00, 0x7c, 0x04, 0x04, 0x00, 0x48,
	0x54, 0x54, 0x54, 0x24, 0x00, 0x3f, 0x44, 0x44,
	0x00, 0x3c, 0x40, 0x40, 0x40, 0x3c, 0x0c, 0x10,
	0x60, 0x10, 0x0c, 0x1c, 0x60, 0x18, 0x60, 0x1c,
	0x44, 0x28, 0x10, 0x28, 0x44, 0x1c, 0xa0, 0xa0,
	0xa0, 0x7c, 0x44, 0x64, 0x54, 0x4c, 0x44, 0x00,
	0x08, 0x7f, 0x41, 0x00, 0x00, 0x00, 0x7f, 0x00,
	0x00, 0x00, 0x41, 0x7f, 0x08, 0x00, 0x08, 0x04,
	0x08, 0x04, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f
};

#define FONT_FIXED_IMG (FONT_FIXED_DATA + 5 + 1)
#define FONT_FIXED_MASK (nullptr)

#define MakeFontFixed(...) Font( \
	FONT_FIXED_DATA, \
	FONT_FIXED_IMG, \
	FONT_FIXED_MASK \
	, ## __VA_ARGS__ \
)

#endif

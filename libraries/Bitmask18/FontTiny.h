#ifndef FONT_TINY_H_INCLUDED
#define FONT_TINY_H_INCLUDED

// Auto-generated from FontTiny.h.gen

#include <ProgMem.h>

// Fixed-width (3x5) numeric font
static PROGMEM const uint8_t FONT_TINY_DATA[] = {
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
	5, 6, 0x30, 0x39, 0x01,
	3,
	0x9f, 0x91, 0x1f, 0x00, 0x1f, 0x80, 0x9d, 0x95,
	0x97, 0x95, 0x95, 0x9f, 0xc7, 0xc4, 0xdf, 0xd7,
	0xd5, 0xfd, 0xdf, 0xd5, 0xdd, 0xc1, 0xc1, 0xdf,
	0x9f, 0xb5, 0x9f, 0xd7, 0xd5, 0xdf
};

#define FONT_TINY_IMG (FONT_TINY_DATA + 5 + 1)
#define FONT_TINY_MASK (nullptr)

#define MakeFontTiny(...) Font( \
	FONT_TINY_DATA, \
	FONT_TINY_IMG, \
	FONT_TINY_MASK \
	, ## __VA_ARGS__ \
)

#endif

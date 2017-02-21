#!/bin/bash
set -e;

# Generator for the FontFixed.h file

SCRIPTDIR="$(dirname "$0")";
TOOLSDIR="$SCRIPTDIR/../../tools";
DATADIR="$SCRIPTDIR/data";

CALC="$TOOLSDIR/calcbitmap.sh";
IMG="$DATADIR/fixedfont.png";

{
cat <<EOF;
#ifndef FONTFIXED_H_INCLUDED
#define FONTFIXED_H_INCLUDED

#include <ProgMem.h>

// Auto-generated by FontFixed.gen.sh

// Fixed-width (5x8) font
static PROGMEM const uint8_t FONTFIXED_DATA[] = {
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
EOF

"$CALC" "$IMG";

cat <<EOF;
};
#define FONTFIXED_IMG (FONTFIXED_DATA + 5 + 1)
#define FONTFIXED_MASK (nullptr)

#endif
EOF
} > "$SCRIPTDIR/FontFixed.h";

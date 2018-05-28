#!/bin/bash
set -e;

# Header generator

SCRIPTDIR="$(dirname "$0")";
TOOLSDIR="$SCRIPTDIR/../../tools";
DATADIR="$SCRIPTDIR/data";

IMG="$DATADIR/variablefont.png";

FILENAME="${0##*/}";
FILENAME="${FILENAME%.gen.sh}"
DEFNAME="FONTVARIABLE";

{
cat <<EOF;
#ifndef ${DEFNAME}_H_INCLUDED
#define ${DEFNAME}_H_INCLUDED

#ifdef ARDUINO
#  include <ProgMem.h>
#else
#  include <ProgMem/ProgMem.h>
#endif

// Auto-generated by $FILENAME.gen.sh

// Variable-width (height 8) font
static PROGMEM const uint8_t ${DEFNAME}_DATA[] = {
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
	8, 8, 0x20, 0x7F, 0x00,
EOF

"$TOOLSDIR/charwidths.sh" "$IMG";

echo "	,";

"$TOOLSDIR/calcbitmap.sh" "$IMG" "18";

cat <<EOF;
};
#define ${DEFNAME}_IMG (${DEFNAME}_DATA + 5 + (0x7F - 0x20) + 1)
#define ${DEFNAME}_MASK (nullptr)

#endif
EOF
} > "$SCRIPTDIR/$FILENAME.h";

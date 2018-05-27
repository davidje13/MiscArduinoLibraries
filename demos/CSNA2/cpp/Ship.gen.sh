#!/bin/bash
set -e;

# Generator for the Ship.h file

SCRIPTDIR="$(dirname "$0")";
TOOLSDIR="$SCRIPTDIR/../../../tools";
DATADIR="$SCRIPTDIR/../data";

CALC="$TOOLSDIR/calcbitmap.sh";
IMG="$DATADIR/Burrumbeet.png";

{
cat <<EOF;
#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include <ProgMem.h>

// Auto-generated by Ship.gen.sh

static PROGMEM const uint8_t SHIP[] = {
EOF

"$CALC" "$IMG" "18";

cat <<EOF;
};

#endif
EOF
} > "$SCRIPTDIR/Ship.h";

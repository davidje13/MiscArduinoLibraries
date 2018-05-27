#!/bin/bash
set -e;

# Generator for the CompressedFace.h file

SCRIPTDIR="$(dirname "$0")";
TOOLSDIR="$SCRIPTDIR/../../../tools";
DATADIR="$SCRIPTDIR/../data";

CALC="$TOOLSDIR/calccompressedbitmap.sh";
IMG="$DATADIR/Face.png";

{
cat <<EOF;
#ifndef COMPRESSED_FACE_H_INCLUDED
#define COMPRESSED_FACE_H_INCLUDED

#include <ProgMem.h>

// Auto-generated by CompressedFace.gen.sh

static PROGMEM const uint8_t COMPRESSED_FACE[] = {
EOF

"$CALC" "$IMG" "81" 128;

cat <<EOF;
};

#endif
EOF
} > "$SCRIPTDIR/CompressedFace.h";
#!/bin/bash
set -e;

# Header generator

SCRIPTDIR="$(dirname "$0")";
TOOLSDIR="$SCRIPTDIR/../../../tools";
DATADIR="$SCRIPTDIR/../data";

IMG="$DATADIR/logo.png";

FILENAME="${0##*/}";
FILENAME="${FILENAME%.gen.sh}"
DEFNAME="LOGO";

{
cat <<EOF;
#ifndef ${DEFNAME}_H_INCLUDED
#define ${DEFNAME}_H_INCLUDED

#include <ProgMem.h>

// Auto-generated by $FILENAME.gen.sh

static PROGMEM const uint8_t $DEFNAME[] = {
EOF

"$TOOLSDIR/calcbitmap.sh" "$IMG" "18";

cat <<EOF;
};

#endif
EOF
} > "$SCRIPTDIR/$FILENAME.h";

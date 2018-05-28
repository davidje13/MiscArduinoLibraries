#!/bin/bash
set -e;

# Header generator

SCRIPTDIR="$(dirname "$0")";
TOOLSDIR="$SCRIPTDIR/../../../tools";

CONTENT="This is a moderately long test message which will be compressed and decompressed. Hopefully the compression will actually save some bytes.";

FILENAME="${0##*/}";
FILENAME="${FILENAME%.gen.sh}"
DEFNAME="COMPRESSED_TEST";

{
cat <<EOF;
#ifndef ${DEFNAME}_H_INCLUDED
#define ${DEFNAME}_H_INCLUDED

#include <ProgMem.h>

// Auto-generated by $FILENAME.gen.sh

static PROGMEM const uint8_t $DEFNAME[] = {
EOF

"$TOOLSDIR/compressor.sh" "32" <<<"$CONTENT";

cat <<EOF;
};

#endif
EOF
} > "$SCRIPTDIR/$FILENAME.h";
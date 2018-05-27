#!/bin/bash
set -e;

# Wrapper script for the Compressor program.
# Builds the program if required, and formats the output with line-wrapping
# and indentation.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/Compressor";
EXEC_SRC="$SCRIPTDIR/src/Compressor.cpp";
WINDOW="$1";
RAW="$2";

if [[ -z "$WINDOW" ]]; then
	cat >&2 <<EOF;

  Usage: $0 <window_size>

Computes a DEFLATE-like compression of the data from stdin, with a focus on low
RAM requirements when decompressing.

EOF
	exit 1;
fi;

if [[ ! -x "$EXEC" ]]; then
	mkdir -p "$SCRIPTDIR/bin";
	g++ --std=c++11 \
		-Wall -Wextra --pedantic \
		-O3 "$EXEC_SRC" \
		-o "$EXEC";
fi;

"$EXEC" "$WINDOW" | "$SCRIPTDIR/tohex.sh" "$RAW";

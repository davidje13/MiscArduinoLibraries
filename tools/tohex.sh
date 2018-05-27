#!/bin/bash
set -e;

# Wrapper script for the ToHex program.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/ToHex";
EXEC_SRC="$SCRIPTDIR/src/ToHex.cpp";

if [[ ! -x "$EXEC" ]]; then
	mkdir -p "$SCRIPTDIR/bin";
	g++ --std=c++11 \
		-Wall -Wextra --pedantic \
		-O3 "$EXEC_SRC" \
		-o "$EXEC";
fi;

if [[ " $* " == *" raw "* ]]; then
	cat;
else
	"$EXEC" "$DATA" | \
		sed 's/, $//' | \
		fold -w 48 | \
		sed 's/ $//' | \
		sed -e 's/\(.*\)/	\1/g';
fi;

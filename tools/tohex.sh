#!/bin/bash
set -eo pipefail;

# Wrapper script for the ToHex program.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/ToHex";
EXEC_SRC="$SCRIPTDIR/src/ToHex.cpp";

if [[ ! -x "$EXEC" ]]; then
	g++ --std=c++11 \
		$TOOL_BUILD_FLAGS \
		"$EXEC_SRC" \
		-o "$EXEC";
fi;

if [[ " $* " == *" raw "* ]]; then
	cat;
else
	$TOOL_RUNNER "$EXEC" "$DATA" | \
		sed 's/, $//' | \
		fold -w 48 | \
		sed 's/ $//' | \
		sed -e 's/\(.*\)/	\1/g';
fi;

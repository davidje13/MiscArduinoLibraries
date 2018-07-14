#!/bin/bash
set -eo pipefail;

# Wrapper script for the Character Widths program.
# Builds the program if required, and formats the output with line-wrapping
# and indentation.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/CharacterWidths";
EXEC_SRC="$SCRIPTDIR/src/CharacterWidths.cpp";
DATA="$1";
RAW="$2";

if [[ -z "$DATA" ]]; then
	cat >&2 <<EOF;

  Usage: $0 <image_path>

Computes the widths of all characters in the input font image and prints out a
C byte array defining them.

EOF
	exit 1;
fi;

if [[ ! -x "$EXEC" ]]; then
	g++ --std=c++11 \
		$TOOL_BUILD_FLAGS \
		-lpng \
		"$EXEC_SRC" \
		-o "$EXEC";
fi;

$TOOL_RUNNER "$EXEC" "$DATA" | "$SCRIPTDIR/tohex.sh" "$RAW";

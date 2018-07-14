#!/bin/bash
set -eo pipefail;

# Wrapper script for the Bitmap Converter program.
# Builds the program if required, and formats the output with line-wrapping
# and indentation.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/BitmapConverter";
EXEC_SRC="$SCRIPTDIR/src/BitmapConverter.cpp";
DATA="$1";
FORMAT="$2";
RAW="$3";

if [[ -z "$DATA" ]] || [[ -z "$FORMAT" ]]; then
	cat >&2 <<EOF;

  Usage: $0 <image_path> <18/81>

Computes a Bitmask-friendly form of the input image (18- or 81- format) and
prints out a C byte array defining it.

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

$TOOL_RUNNER "$EXEC" "$DATA" "$FORMAT" \
	| "$SCRIPTDIR/tohex.sh" "$RAW";

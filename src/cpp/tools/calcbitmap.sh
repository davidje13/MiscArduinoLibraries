#!/bin/bash
set -e;

# Wrapper script for the Bitmap Converter program.
# Builds the program if required, and formats the output with line-wrapping
# and indentation.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/BitmapConverter";
EXEC_SRC="$SCRIPTDIR/src/BitmapConverter.cpp";
DATA="$1";

if [[ -z "$DATA" ]]; then
	cat >&2 <<EOF;

  Usage: $0 <image_path>

Computes a Bitmask18-friendly form of the input image and prints out a C byte
array defining it.

EOF
	exit 1;
fi;

if [[ ! -x "$EXEC" ]]; then
	g++ --std=c++11 -isystem/opt/local/include -L/opt/local/lib -lpng -Wall -Wextra --pedantic -O3 "$EXEC_SRC" -o "$EXEC";
fi;

"$EXEC" "$DATA" | \
	sed 's/, $//' | \
	fold -w 48 | \
	sed 's/ $//' | \
	sed -e 's/\(.*\)/	\1/g';

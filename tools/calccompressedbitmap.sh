#!/bin/bash
set -eo pipefail;

# Wrapper script for the Bitmap Converter and Compression programs.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

DATA="$1";
FORMAT="$2";
WINDOW="$3";
RAW="$4";

if [[ -z "$DATA" ]] || [[ -z "$FORMAT" ]] || [[ -z "$WINDOW" ]]; then
	cat >&2 <<EOF;

  Usage: $0 <image_path> <18/81> <window_size>

Computes a CompressedBitmask-friendly form of the input image (18- or 81-
format) and prints out a C byte array defining it.

EOF
	exit 1;
fi;

"$SCRIPTDIR/calcbitmap.sh" "$DATA" "$FORMAT" "raw" \
	| "$SCRIPTDIR/compressor.sh" "$WINDOW" "raw" \
	| "$SCRIPTDIR/tohex.sh" "$RAW";

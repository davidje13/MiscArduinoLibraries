#!/bin/bash
set -eo pipefail;

# Wrapper script for the Compressor program when working with multiple strings.
# Builds the program if required.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/Compressor";
EXEC_SRC="$SCRIPTDIR/src/Compressor.cpp";
STRINGS_FILE="$1";
DEF_NAME="$2";
DECOMPRESSOR_NAME="$3";
WINDOW="$4";
TEMP_FILE="$DEF_NAME.tmp";

if [[ -z "$STRINGS_FILE" || -z "$DEF_NAME" || -z "$DECOMPRESSOR_NAME" || -z "$WINDOW" ]]; then
	cat >&2 <<EOF;

  Usage: $0 <strings_file> <const_var_name> <decompressor_var_name> <window>

Packs all given values into a single compressed bit-stream and defines pointers
to each part. The strings file should contain one KEY=value definition on each
line. Newlines can be represented with \\n.

EOF
	exit 1;
fi;

if [[ ! -x "$EXEC" ]]; then
	g++ --std=c++11 \
		$TOOL_BUILD_FLAGS \
		"$EXEC_SRC" \
		-o "$EXEC";
fi;

echo "static PROGMEM const uint8_t ${DEF_NAME}[] = {";
$TOOL_RUNNER "$EXEC" "$WINDOW" --strings "$TEMP_FILE" < "$STRINGS_FILE" | "$SCRIPTDIR/tohex.sh";
echo "};";
echo;
echo "static auto ${DECOMPRESSOR_NAME} = MakeSegmentedDecompressor<$WINDOW>(MakeProgMem($DEF_NAME));";
echo "#define ${DECOMPRESSOR_NAME}Ptr(p) MakeSegmentedDecompressorPointer($DECOMPRESSOR_NAME, p, char)";

awk "{ print \"#define\", \$1, \"${DECOMPRESSOR_NAME}Ptr(\", \$2, \")\" }" "$TEMP_FILE";
rm "$TEMP_FILE";

echo;

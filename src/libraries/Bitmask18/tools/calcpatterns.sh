#!/bin/bash
set -e;

# Generator script for pattern data.
# Builds the program if required, formats the output with line-wrapping and
# indentation, and adds #define lines for each named section.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/WordCompounder";
EXEC_SRC="$SCRIPTDIR/src/WordCompounder.cpp";
DATA="$1";

if [[ -z "$DATA" ]]; then
	cat >&2 <<EOF;

  Usage: $0 <data_file> [<extra_name_files>]

Computes a compressed form of the input data by identifying overlapping parts.
The output is not necessarily optimal, but will find the most obvious savings.

The input file is of the form:

  01EF Name
  1234 Other_name

  EF12 Extra_name

With this file, 0x00 0x11 0xEE 0xFF will be generated for "Name",
0x11 0x22 0x33 0x44 will be generated for "Other_name", and while
EF12 will not explicitly be generated (since it is after a blank
line), if it happens to appear it will be labelled as "Extra_name".

EOF
	exit 1;
fi;

if [[ ! -x "$EXEC" ]]; then
	g++ --std=c++11 -Wall -Wextra --pedantic -O3 "$EXEC_SRC" -o "$EXEC";
fi;

DAT="$(cut -f1 -d' ' "$DATA" | "$EXEC" 2> /dev/null)";

echo;
echo "static PROGMEM const uint8_t PATTERNS[] = {";
echo "$DAT" | \
	sed -e 's/\([0-9A-F]\)/0x\1\1, /g' | \
	sed 's/, $//' | \
	fold -w 48 | \
	sed 's/ $//' | \
	sed -e 's/\(.*\)/	\1/g';
echo "};"
echo;

grep --no-filename ' ' "$@" | while read LN; do
	WORD="${LN%% *}";
	CROP="${DAT%%$WORD*}";
	if [[ "$CROP" != "$DAT" ]]; then
		echo "#define PATTERN_${LN#* }|(PATTERNS + ${#CROP})";
	fi;
done | column -s '|' -t | sort | uniq;
echo;

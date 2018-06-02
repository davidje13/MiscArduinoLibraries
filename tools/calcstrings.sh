#!/bin/bash
set -eo pipefail;

# Wrapper script for the Strings program.
# Builds the program if required.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/Strings";
EXEC_SRC="$SCRIPTDIR/src/Strings.cpp";
STRINGS_FILE="$1";
DEF_NAME="$2";
TEMP_FILE="$DEF_NAME.tmp";

if [[ -z "$STRINGS_FILE" || -z "$DEF_NAME" ]]; then
	cat >&2 <<EOF;

  Usage: $0 <strings_file> <const_var_name>

Packs all given values into a single uncompressed bit-stream and defines
pointers to each part. The strings file should contain one KEY=value definition
on each line. Newlines can be represented with \\n.

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

echo "static PROGMEM const char ${DEF_NAME}[] = {";
"$EXEC" "$TEMP_FILE" < "$STRINGS_FILE" | "$SCRIPTDIR/tohex.sh";
echo "};";
echo;

awk "{ print \"#define\", \$1, \"ProgMem<char>(${DEF_NAME} +\", \$2, \")\" }" "$TEMP_FILE";
rm "$TEMP_FILE";

echo;

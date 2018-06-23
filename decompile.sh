#!/bin/bash
set -eo pipefail;

SCRIPTDIR="$(dirname "$0")";

DEMO="$1";
if [[ -z "$DEMO" ]]; then
	exit 1;
fi;

OUTPUT="$SCRIPTDIR/demos/$DEMO/out";

APP="Arduino.app";
if [[ -d "/Applications/$APP" ]]; then
	IDE_PACKAGE="/Applications/$APP";
elif [[ -d ~/"Applications/$APP" ]]; then
	IDE_PACKAGE=~/"Applications/$APP";
fi;

DECOMPILER="avr-objdump";
BASEDIR="$IDE_PACKAGE/Contents/Java";
HWDIR="$BASEDIR/hardware";

if ! which "$DECOMPILER"; then
	DECOMPILER="$HWDIR/tools/avr/bin/$DECOMPILER";
fi;

if [[ " $* " == *" --noaddr "* ]]; then
	# In this form, it is easy to perform diffs between compiled versions
	"$DECOMPILER" -z -S "$OUTPUT/"*".ino.elf" \
		| cut -c10- \
		| sed -E 's/([^0-9a-qs-zA-Z_]|^)[0-9a-fA-F]{4}([^0-9a-zA-Z]|$)/\1----\2/g' \
		| sed -E 's/([^0-9a-qs-zA-Z_]|^)[0-9a-fA-F]{4}([^0-9a-zA-Z]|$)/\1----\2/g' \
		| sed -E 's/([^0-9a-qs-zA-Z_]|^)[0-9a-fA-F]{3}([^0-9a-zA-Z]|$)/\1---\2/g' \
		| sed -E 's/([^0-9a-qs-zA-Z_]|^)[0-9a-fA-F]{3}([^0-9a-zA-Z]|$)/\1---\2/g' \
		| sed -E 's/([^0-9a-qs-zA-Z_]|^)[0-9a-fA-F]{2}([^0-9a-zA-Z]|$)/\1--\2/g' \
		| sed -E 's/([^0-9a-qs-zA-Z_]|^)[0-9a-fA-F]{2}([^0-9a-zA-Z]|$)/\1--\2/g' \
		| sed -E 's/0x[0-9a-fA-F]{2,}/0x?/g';
else
	"$DECOMPILER" -z -S "$OUTPUT/"*".ino.elf";
fi;

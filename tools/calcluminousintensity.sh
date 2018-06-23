#!/bin/bash
set -eo pipefail;

# Wrapper script for the Luminous Intensity program.
# Builds the program if required, and formats the output with line-wrapping
# and indentation.

# Author: David Evans
# License: Public Domain

SCRIPTDIR="$(dirname "$0")";

EXEC="$SCRIPTDIR/bin/LuminousIntensity";
EXEC_SRC="$SCRIPTDIR/src/LuminousIntensity.cpp";
WAVELENGTH_NM="$1";
INTENSITY_MCD="$2";
RAW="$3";

if [[ -z "$WAVELENGTH_NM" ]] || [[ -z "$INTENSITY_MCD" ]]; then
	cat >&2 <<EOF;

  Usage: $0 <wavelength_nm> <peak_intensity_mcd>

Computes a lookup table converting perceived intensity into LED duty cycle.

EOF
	exit 1;
fi;

if [[ ! -x "$EXEC" ]]; then
	mkdir -p "$SCRIPTDIR/bin";
	g++ --std=c++11 \
		-isystem/opt/local/include \
		-isystem/usr/local/include \
		-L/opt/local/lib \
		-lpng \
		-Wall -Wextra --pedantic \
		-O3 "$EXEC_SRC" \
		-o "$EXEC";
fi;

"$EXEC" "$WAVELENGTH_NM" "$INTENSITY_MCD" \
	| "$SCRIPTDIR/tohex.sh" "$RAW";

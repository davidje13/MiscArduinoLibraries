#!/bin/bash
set -e;

SCRIPTDIR="$(dirname "$0")";

ABSDIR="$(cd "$SCRIPTDIR" && pwd)";

# Pro/Pro Mini, ATmega328 8MHz 3.3V (matches 8MHz chip sold by HT)
BOARD="arduino:avr:pro:cpu=8MHzatmega328";
COMPORT="/dev/cu.usbserial-A104W1DT";

# Board format:
# arduino:avr:<board>:<flags>
# For a list of boards & flags, see:
#  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/boards.txt

# To change optimisation level, replace the 3 -Os occurrences in:
#  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/platform.txt

SOURCE="$SCRIPTDIR/src/wrapper/Main.ino";
LOCAL_SRCDIR="$SCRIPTDIR/src";
LOCAL_LIBDIR="$SCRIPTDIR/src/libraries";
OUTPUT="$ABSDIR/out"; # must be absolute (bug in arduino-builder)

MODE="upload";

if [[ "$1" == "--preprocess" ]]; then
	MODE="preprocess";
fi;

if [[ "$1" == "--check" ]]; then
	MODE="check";
fi;

# Invoke all generator scripts
find "./src" -iname '*.gen.sh' -type f | while read LN; do
	"$LN";
done;

if [[ "$MODE" == "preprocess" ]]; then
	exit 0;
fi;

APP="Arduino.app";
if [[ -d "/Applications/$APP" ]]; then
	IDE_PACKAGE="/Applications/$APP";
elif [[ -d ~/"Applications/$APP" ]]; then
	IDE_PACKAGE=~/"Applications/$APP";
fi;

BUILDER="arduino-builder";
BASEDIR="$IDE_PACKAGE/Contents/Java";
HWDIR="$BASEDIR/hardware";
LIBDIR="$BASEDIR/libraries";

# Most of these config values come from boards.txt for the chosen $BOARD
UPLOADER="$HWDIR/tools/avr/bin/avrdude";
UPLOADER_CONF="$HWDIR/tools/avr/etc/avrdude.conf";
UPLOADER_DEVICE="atmega328p";
UPLOADER_PROGRAMMER="arduino";
UPLOADER_BAUDRATE="57600";

mkdir -p "$OUTPUT";

if ! which "$BUILDER"; then
	IDE="$IDE_PACKAGE/Contents/MacOS/Arduino";
	BUILDER="$IDE_PACKAGE/Contents/Java/$BUILDER";
fi;

if [[ -x "$BUILDER" ]]; then
	# Use arduino-builder to build
	"$BUILDER" \
		-hardware "$HWDIR" \
		-tools "$BASEDIR/tools-builder" \
		-tools "$HWDIR/tools" \
		-built-in-libraries "$LIBDIR" \
		-libraries "$LOCAL_LIBDIR" \
		-libraries "$LOCAL_SRCDIR" \
		-fqbn "$BOARD" \
		-warnings "all" \
		-build-path "$OUTPUT" \
		"$SOURCE";

	set -- "$OUTPUT"/*.ino.with_bootloader.hex;
	FLASHFILE="$1";
	if [[ "$MODE" == "upload" ]]; then
		if [[ ! -f "$FLASHFILE" || ! -x "$UPLOADER" ]]; then
			echo "Cannot upload $FLASHFILE to device at $COMPORT." >&2;
			false;
		else
			"$UPLOADER" \
				-C"$UPLOADER_CONF" \
				-p"$UPLOADER_DEVICE" \
				-c"$UPLOADER_PROGRAMMER" \
				-P"$COMPORT" \
				-b"$UPLOADER_BAUDRATE" \
				-D \
				-Uflash:w:"$FLASHFILE":i;
		fi;
	fi;
elif [[ -x "$IDE" ]]; then
	# Use IDE to build
	echo "Using Arduino IDE to build. Functionality may be limited." >&2;

	if [[ "$MODE" == "check" ]]; then
		"$IDE" --verify --board "$BOARD" "$SOURCE";
	elif [[ "$MODE" == "upload" ]]; then
		"$IDE" --upload --board "$BOARD" --port "$COMPORT" "$SOURCE";
	fi;
else
	echo "No arduino compilers found." >&2;
	false;
fi;

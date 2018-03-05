#!/bin/bash
set -e;

SCRIPTDIR="$(dirname "$0")";

DEMO="$2";
if [[ -z "$DEMO" ]]; then
	DEMO="SSD1306";
fi;

# To change optimisation level, replace the 3 -Os occurrences in:
#  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/platform.txt

SOURCE="$SCRIPTDIR/wrapper/Main.ino";
SRCDIR="$SCRIPTDIR/demos/$DEMO";
LIBDIR="$SCRIPTDIR/libraries";
SHAREDIR="$SCRIPTDIR/shared";
OUTPUT="$(cd "$SRCDIR" && pwd)/out"; # must be absolute (bug in arduino-builder)
BOOSTDIR="/usr/local/include";

MODE="upload";

if [[ "$1" == "--preprocess" ]]; then
	MODE="preprocess";
fi;

if [[ "$1" == "--check" ]]; then
	MODE="check";
fi;

if [[ "$1" == "--local-test" ]]; then
	MODE="local-test";
fi;

# Invoke all generator scripts
find "$LIBDIR" "$SRCDIR" -iname '*.gen.sh' -type f | while read LN; do
	"$LN";
done;

# Copy all shared header files to each library
for D in "$LIBDIR"/*/; do
	cp "$SHAREDIR"/*'.h' "$D";
done;

if [[ "$MODE" == "preprocess" ]]; then
	exit 0;
fi;

if [[ "$MODE" == "local-test" ]]; then
	if [[ -z "$2" ]]; then
		LIB=".";
		TESTNAME="all";
	else
		LIB="libraries/$2";
		TESTNAME="$2";
	fi;

	EXTRA_PEDANTIC='
		-Weverything
		-Wno-c++98-compat -Wno-c++98-compat-pedantic
		-Wno-disabled-macro-expansion
		-Wno-global-constructors
	';

	mkdir -p "$SCRIPTDIR/out";
	find "$LIB" -path '*/test/*.cpp' -exec \
		g++ --std=c++11 -isystem"$BOOSTDIR" \
		-fno-rtti \
		-Wall -Wextra -Weffc++ -Wpedantic \
		-Wconversion -Wsign-conversion \
		-Wold-style-cast -Wmissing-variable-declarations \
		-Warray-bounds-pointer-arithmetic -Wundefined-reinterpret-cast \
		-I "$SCRIPTDIR/libraries" \
		-I "$SCRIPTDIR/mocks" \
		-o "$SCRIPTDIR/out/test-$TESTNAME" \
		"$SCRIPTDIR/test.cpp" \
		"$SCRIPTDIR/mocks/"*".cpp" \
		{} +;

	"./out/test-$TESTNAME" \
		--build_info=true \
		--color_output=true \
		--show_progress=true \
		--log_level=warning \
		--report_level=confirm;
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

. "$SCRIPTDIR/target.sh";

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
		-libraries "$LIBDIR" \
		-libraries "$SRCDIR" \
		-fqbn "$BOARD" \
		-warnings "all" \
		-build-path "$OUTPUT" \
		"$SOURCE";

	if [[ "$WITH_BOOTLOADER" == "true" ]]; then
		EXTENSION="ino.with_bootloader.hex";
	else
		EXTENSION="ino.hex";
	fi;
	set -- "$OUTPUT"/*."$EXTENSION";
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

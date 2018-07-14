#!/bin/bash
set -eo pipefail;

SCRIPT_ABS_DIR="$(cd "$(dirname "$0")"; pwd)";
TOOLS_DIR="$SCRIPT_ABS_DIR/tools";
MODE="$1";
shift;

mkdir -p "$TOOLS_DIR/bin";
if [[ "$CLEAR_TOOLBIN" == "true" ]]; then
	rm -rf "$TOOLS_DIR/bin"/*.dSYM >/dev/null || true;
	rm -f "$TOOLS_DIR/bin"/* >/dev/null || true;
fi;

TOOL_BUILD_FLAGS="-Wall -Wextra --pedantic";

if [[ -d "/opt/local/include" ]]; then
	TOOL_BUILD_FLAGS="$TOOL_BUILD_FLAGS -isystem/opt/local/include";
fi;

if [[ -d "/usr/local/include" ]]; then
	TOOL_BUILD_FLAGS="$TOOL_BUILD_FLAGS -isystem/usr/local/include";
fi;

if [[ -d "/opt/local/lib" ]]; then
	TOOL_BUILD_FLAGS="$TOOL_BUILD_FLAGS -L/opt/local/lib";
fi;

if [[ -n "$TOOL_RUNNER" ]]; then
	TOOL_BUILD_FLAGS="$TOOL_BUILD_FLAGS -g -O1";
else
	TOOL_BUILD_FLAGS="$TOOL_BUILD_FLAGS -O3";
fi;

export TOOL_BUILD_FLAGS;

generate() {
	GENERATOR_FILE="$1";
	BASE_DIR="$(dirname "$GENERATOR_FILE")";
	OUTPUT_FILE="${GENERATOR_FILE%.gen}";
	OUTPUT_NAME="${OUTPUT_FILE##*/}";
	GENERATOR_NAME="${GENERATOR_FILE##*/}";

	CAMEL_NAME="${OUTPUT_NAME%.*}";
	# Thanks, https://stackoverflow.com/a/28795550/1180785
	SNAKE_NAME="$(echo "$CAMEL_NAME" | sed -E 's/([a-z0-9])([A-Z])/\1_\2/g')";
	LOWER_SNAKE_NAME="$(echo "$SNAKE_NAME" | tr '[:upper:]' '[:lower:]')";
	UPPER_SNAKE_NAME="$(echo "$SNAKE_NAME" | tr '[:lower:]' '[:upper:]')";

	if [[ -n "$TOOL_RUNNER" ]]; then
		echo;
		echo;
		echo "Running $GENERATOR_FILE" >&2;
	fi;

	cd "$BASE_DIR";
	if ! eval > "$OUTPUT_NAME" "
			set -eo pipefail;
			TMP=\"$(cat "$GENERATOR_NAME")\";
			[[ $? == 0 ]] && echo \"\$TMP\";
	"; then
		echo "ERROR GENERATING FILE" > "$OUTPUT_NAME";
		echo "Failed to generate $OUTPUT_NAME";
		cd - >/dev/null;
		exit 1;
	fi;
	cd - >/dev/null;
};

find "$@" -iname '*.gen' -type f | while read LN; do
	if [[ "$MODE" == "all" || "$LN" != *"/test/"* ]]; then
		generate "$LN";
	fi;
done;

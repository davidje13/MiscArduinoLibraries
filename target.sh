#!/bin/bash
set -e;

# Pro/Pro Mini, ATmega328 8MHz 3.3V (matches 8MHz chip sold by HT)
BOARD="arduino:avr:pro:cpu=8MHzatmega328";
COMPORT="/dev/cu.usbserial-A104W1DT";

# Board format:
# arduino:avr:<board>:<flags>
# For a list of boards & flags, see:
#  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/boards.txt

# Most of these config values come from boards.txt for the chosen $BOARD
UPLOADER="$HWDIR/tools/avr/bin/avrdude";
UPLOADER_CONF="$HWDIR/tools/avr/etc/avrdude.conf";
UPLOADER_DEVICE="atmega328p";
UPLOADER_PROGRAMMER="arduino";
UPLOADER_BAUDRATE="57600";

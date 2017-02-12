# General Arduino Libraries

These libraries for Arduino-based platforms are written by David Evans and
where possible are available as Public Domain.

See the README.md file in each library's source directory for full details.

## Overview

All libraries are currently header-only implementations (often taking advantage
of Arduino's merge-all-source-into-one-cpp behaviour). Templating is used where
it makes sense. Code has been optimised for speed and RAM usage, and has a
reasonably small compiled footprint.

To allow the use of C++ language features, the compiled .ino file
(/wrapper/Main.ino) is a minimal implementation which simply includes a header
file (*/main.h). This is because Arduino's compilation behaviour mangles C++
constructs within .ino files, but will not mangle library code.

Compilation is performed using the arduino-builder tool (included with the
standard Arduino IDE), and has been tested on OS X (it should be possible to
run on Linux too, but modifications to the compilation script will likely be
required). Uploading to the connected Arduino (configured for an ATmega328 chip
with Arduino bootloader, running at 8MHz:
http://www.hobbytronics.co.uk/atmega328-arduino-8mhz) is performed using the
avrdude utility tool (also bundled with the Arduino IDE). Note that deployment
to other devices will require changes to the target.sh script.

### Dependencies

There are no required dependencies, but if you want to rebuild image data from
files, you will need to install Boost (http://www.boost.org/). Specifically,
Boost GIL is used to load image data. Boost is generally easy to install using
a package manager such as homebrew, macports, yum, apt, etc.

### Compilation

The demo files can be compiled via compile.sh (the included Makefile is a thin
wrapper around this script). It will attempt to locate an Arduino IDE
installation to invoke the arduino-builder compiler tool and avrdude binary
uploader.

Note that it is configured for deploying to an ATmega328 chip running at 8MHz.
To deploy to an alternative device, change `BOARD` in target.sh (the format is
explained in the file). It may also be necessary to change the `UPLOADER_*`
values; values for these are harder to find, but check boards.txt in your
Arduino installation.

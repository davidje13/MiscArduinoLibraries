# General Arduino Libraries

These libraries for Arduino-based platforms are written by David Evans and
where possible are available as Public Domain. The current focus is 2D
rendering on the SSD1306; specifically,
http://www.hobbytronics.co.uk/lcd/oled-graphic-display-128-64-wb

Each library's features are explained below.

## Overview

All libraries are currently header-only implementations (often taking advantage
of Arduino's merge-all-source-into-one-cpp behaviour). Templating is used where
it makes sense. Code has been optimised for speed and RAM usage, and has a
reasonably small compiled footprint.

To allow the use of C++ language features, the compiled .ino file
(/src/wrapper/Main.ino) is a minimal implementation which simply includes a
header file (/src/cpp/main.h). This is because Arduino's compilation behaviour
mangles C++ constructs within .ino files, but will not mangle library code.

Compilation is performed using the arduino-builder tool (included with the
standard Arduino IDE), and has been tested on OS X (it should be possible to
run on Linux too, but modifications to the compilation script will likely be
required). Uploading to the connected Arduino (specifically, an ATmega328 chip
with Arduino bootloader, running at 8MHz:
http://www.hobbytronics.co.uk/atmega328-arduino-8mhz) is performed using the
avrdude utility tool (also bundled with the Arduino IDE). Note that deployment
to other devices will require changes to the compile.sh script.

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
To deploy to an alternative device, change BOARD in compile.sh (the format is
explained in the file). It may also be necessary to change the UPLOADER_*
values further down the script; values for these are harder to find, but check
boards.txt in your Arduino installation.

## Libraries

### SSD1306

This is currently the core library for the demo. It is capable of sending
commands and data to a SSD1306 chip (typically used to drive a black-and-white
graphical display).

#### Features

This exposes almost all functionality which the chip exposes (some configuration
options are kept private as it does not make sense to change them, but you can
always make them public if you want to play).

* Send whole screens of data (fast enough to send at 100fps)
* Send partial screens of data
* Configure built-in x and y mirroring
* Screen colour inversion, full-screen-white
* Configure vertical offset and displayed height
* Configure contrast (brightness) level
* Begin and end marquee mode (continuous horizontal scrolling at configurable
  speed, with vertical multiplier)
* Soft reset, and display on/off control
* Minimal RAM usage; no buffering is performed, so only a few bytes of RAM are
  used by this class (this leaves the RAM available for image operations
  performed by other classes)
* Super fast communication, with minimal data sent
* Can be used with varying sizes of display via template parameters
* Supports SPI communication (no support for I2C communication)

#### Example

```cpp
#include <SSD1306.h>

...

int OLED_PIN_CS  = ?; // Set to any available pin
int OLED_PIN_RST = ?; // Set to any available pin
int OLED_PIN_DC  = ?; // Set to any available pin
SSD1306<128,64> myDisplay(OLED_PIN_CS, OLED_PIN_RST, OLED_PIN_DC);
// Put any pre-switch-on configuration here, e.g. to rotate the screen 180:
myDisplay.set_x_flip(true);
myDisplay.set_y_flip(true);
myDisplay.set_on(true);
myDisplay.send(myBitmask); // See Bitmask18
```

### Bitmask18

This is the companion library for SSD1306. It stores bitmask representations in
the format understood by the SSD1306 chip (array of bytes, each covering a 1x8
pixel area of the screen). It includes a number of convenient drawing operations
which have been optimised for the data storage.

#### Features

* Render outlined / filled lines / rectangles / ellipses / triangles
* Render bitmaps
* Full-screen clear
* Set individual pixels, and get raw memory access for faster algorithms
* All rendering operations support blend modes (ON, OFF, XOR, NOP)
* All rendering operations except bitmap support patterns (ON, CHECKER,
  CROSSHATCH, BRICKS, FILL75, and many more, as well as support for custom 4x8
  patterns)
* All rendering operations paint pixels at most once, allowing blend modes such
  as XOR to work correctly in all cases
* Line and triangle rendering supports sub-pixel precision (configurable)
* Ellipse rendering supports even and odd sized ellipses; just specify bounding
  box
* Fixed-width and variable-width fonts (example fonts are included in the demo)
* Text rendering with support for rudimentary line wrapping
* Basic 3D rendering (vertex projection, line rendering, triangle rendering,
  backface culling) - note: does not (yet) include depth buffer, so objects
  must be convex, or faces must be rendered back-to-front
* Full support for PROGMEM data (fonts, text, bitmaps, patterns, 3D data)
* Minimal RAM usage; size information is controlled by the template parameters
  so does not consume RAM - the only RAM used is for pixel data

#### Example

```cpp
#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>
#include <Font.h>
#include <FontRenderer.h>

...

Bitmask18<128,64> myBitmask;
myBitmask.fill_ellipse(10, 10, 60, 40, BlendMode::ON, PATTERN_CHECKER);

Font myFont(MY_FONT_DATA, MY_FONT_IMAGE, MY_FONT_MASK);
auto myRenderer = MakeFontRenderer(&myBitmask, 0, 0, myBitmask.width(), 0);
myRenderer.print(myFont, "The quick brown fox jumps over the lazy dog.");
// Note: ProgMemString can be used to avoid using RAM when storing the message)
```

### FastMath [MIT license]

This is a small library which calculates sine and cosine values using integer-
only arithmetic. It is extracted from the FastLED project, available under the
MIT license, and required by the SceneRenderer (available as part of the
Bitmask18 library).

When rendering 3D scenes on a chip without an FPU (e.g. typical Arduino chips),
using floating point arithmetic causes massive slowdown.

### ProgMem

This wrapper class makes interacting with PROGMEM (non-RAM-consuming) data much
easier. It allows the same syntax to be used as for normal data.

Note that not all operations are automatically fixed; it is still necessary to
switch calls to (e.g.) `memcpy` with `memcpy_P`. A `raw()` method is provided
to access the underlying data pointer for these cases.

#### Features

* Natively supports byte, word and long (1/2/4-byte) integer types
* Natively supports floats
* Natively supports pointer types
* Supports all other types of data array, via the use of memcpy_P (note that
  this may not be the most efficient way to access data from arrays of structs)
* Supports common operators to behave like a normal pointer
* Zero overhead compared to writing PROGMEM functions directly in almost all
  cases (no extra RAM usage, no extra CPU instructions)
* Convenience wrapper for strings, allowing them to be put into PROGMEM inline

#### Example

```cpp
#include <ProgMem.h>

static PROGMEM const uint8_t myByteData[] = {0x00, 0x11, 0x22};
auto myWrappedByteData = WrapProgMem(myByteData);
myWrappedByteData[0]; // 0x00
myWrappedByteData[1]; // 0x11
myWrappedByteData += 2;
myWrappedByteData[0]; // 0x22

struct Coordinate {
	int x;
	int y;
};
static PROGMEM const Coordinate myCoordinates[] = {
	{0, 0},
	{10, 0},
	{100, 5}
};
auto myWrappedCoordinates = WrapProgMem(myCoordinates);
myWrappedCoordinates[0]; // Coordinate(0, 0)

myTextFunction(ProgMemString("Hello; I won't use up any RAM!"));
```

### RawPin

Allows quick changing of pin values. This uses more RAM than storing a simple
pin number, but allows the pin to be changed HIGH/LOW with minimal overhead for
super-fast switching. Note that interrupts are not blocked by these operations,
so if interrupts may interfere with pin buffers, you should block them yourself.

#### Example

```cpp
#include <RawPin.h>

RawPin myPin(7, RAWPIN_OUTPUT);
myPin.high();
myPin.low();

RawPin myInputPin(8, RAWPIN_INPUT, true); // pull-up input

// Prevent interrupts while sending a series of pulses. Note that blocking
// interrupts for too long will make I2C/SPI/PWM sad. Often it's best to block
// for only a single state change at a time.
RawPin mySafePin(9, RAWPIN_OUTPUT);
uint8_t oldSREG = SREG; cli(); // Block interrupts
mySafePin.high();
mySafePin.low();
mySafePin.high();
mySafePin.low();
SREG = oldSREG; // Restore interrupts
```

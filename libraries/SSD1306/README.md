# SSD1306 (Graphical display)

This library is capable of sending commands and data to a SSD1306 chip
(typically used to drive a black-and-white graphical display). It has been
written specifically to work with this product:
http://www.hobbytronics.co.uk/lcd/oled-graphic-display-128-64-wb but should
work with any display using the same core chip (note that it enables the voltage
pump, which may not always be desirable).

## Features

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

## Example

```cpp
#include <SSD1306.h>
#include <ArduinoPin.h>
#include <ArduinoSPI.h>

...

// DAT (D1) must be connected to MOSI on the Arduino (e.g. D11 on Uno)
// CLK (D0) must be connected to SCK on the Arduino (e.g. D13 on Uno)
int OLED_PIN_CS  = ?; // Set to any available pin
int OLED_PIN_RST = ?; // Set to any available pin
int OLED_PIN_DC  = ?; // Set to any available pin
auto myDisplay = MakeSSD1306<128,64>(
	ArduinoSPI(),
	ArduinoPin(OLED_PIN_CS),
	ArduinoPin(OLED_PIN_RST),
	ArduinoPin(OLED_PIN_DC)
);
// Put any pre-switch-on configuration here, e.g. to rotate the screen 180:
myDisplay.set_x_flip(true);
myDisplay.set_y_flip(true);
myDisplay.set_on(true);
myDisplay.send(myBitmask); // See Bitmask18
```

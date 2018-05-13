# 1602 (LCD display)

This library communicates with a 1602-based LCD display.

There are several displays which conform to the 1602 interface; this
should be compatible with all of them.

It is similar to the standard Arduino LiquidCrystal library, but
supports flexible communication (e.g. via an expander)

## Features

* 4- or 8- bit data transfer
* Compatible with *Pin and *Bank implementations for flexible
  communication
* Configurable width and height
* Supports custom character data (max of 8 distinct characters)
* Character data can be specified row-wise (native) or column-wise
  (Bitmask18-compatible)

## Example

```cpp
#include <ArduinoPin.h>
#include <VoidPin.h>
#include <PinBank.h>
#include <1602.h>

auto lcd = Make1602<16, 2>(
	MakePinBank(
		ArduinoPin(2), // MSB (D7)
		ArduinoPin(3),
		ArduinoPin(4),
		ArduinoPin(5)  // LSB (D4)
	),
	ArduinoPin(11), // RS
	VoidPin(),      // RW
	ArduinoPin(12)  // EN
);

uint8_t customChar1 = lcd.set_char(1,
	0xFC,
	0x00,
	0xFF,
	0x00,
	0x3C
);

lcd.on(); // Switch on display

lcd.print("Hello!");
lcd.print(customChar1);
```

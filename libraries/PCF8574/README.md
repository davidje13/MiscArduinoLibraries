# PCF8574 (IO Expander)

This library wraps communication with a PCF8574 chip via I2C.
The chip provides 8 pins which can be used as inputs / outputs.

## Features

* I2C communication (at 100kHz)
* Configurable address bits (allowing up to 8 chips on the same
  interface)
* Aware of addresses for PCF8574 and PCF8574A
* Pin state memory allows changing individual pins
* Compatible with the pin and pinBank APIs used elsewhere

## Example

```cpp
#include <PCF8574.h>
#include <VoidPin.h>
#include <ArduinoTWIMaster.h>

...

auto extender = MakePCF8574(
	ArduinoTWIMaster(),
	VoidPin(), // optional interrupt pin
	0 // sub-address = (A2 << 2) | (A1 << 1) | A0
);

// All pins default to Output High

extender.set(0xAB);

auto pin0 = extender.pin(0);
// pin0 conforms to the pin API used throughout these libraries
pin0.high();
```

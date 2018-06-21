# ArduinoTWI

Object-oriented abstraction around the standard I2C / TWI access functions on
Arduino.

`begin()` and `end()` are automatically managed through the constructors and
destructor.

## Example

```cpp
#include <ArduinoTWI.h>

ArduinoTWIMaster myTWI;

// Each device on the wire should specify its maximum speed once;
// the highest speed which all devices (and the host) support will
// be used for communication with any device.
myTWI.set_max_clock(100000); // 100kHz

myTWI.begin_transmission(
	0x01 // 7-bit target address
);

myTWI.write(0x50);

const uint8_t buffer[] = {1, 2, 3};
myTWI.write(buffer, sizeof(buffer));

myTWI.end_transmission();
```

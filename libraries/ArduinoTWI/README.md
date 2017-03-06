# ArduinoTWI

Object-oriented abstraction around the standard I2C / TWI access functions on
Arduino.

`begin()` and `end()` are automatically managed through the constructors and
destructor.

## Example

```cpp
#include <ArduinoTWI.h>

ArduinoTWIMaster myTWI;

myTWI.begin_transmission(
	0x01,  // 7-bit target address
	100000 // 100kHz
);

myTWI.write(0x50);

const uint8_t buffer[] = {1, 2, 3};
myTWI.write(buffer, sizeof(buffer));

myTWI.end_transmission();
```

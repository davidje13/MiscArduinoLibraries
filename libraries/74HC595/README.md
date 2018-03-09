# 74HC595 (8-bit Shift Register)

Sends data to one or more chained 74HC595 registers.

## Features

* Fast pin communication
* Stable data on rising edge of SH_cp
* Most Significant Byte and Least Significant Byte operations
* Automatic latch pin management (or use a VoidPin to disable)

## Example

```cpp
#include <74HC595.h>
#include <ArduinoPin.h>

auto ST_CP_PIN = ArduinoPin(7); // latch
auto SH_CP_PIN = ArduinoPin(8); // clock
auto DS_PIN    = ArduinoPin(9); // data

auto register = Make74HC595(ST_CP_PIN, SH_CP_PIN, DS_PIN);

...

register.send8_msb(0xC4);

register.send1(true);

char bits[] = {0x97, 0x11, 0xF8, 0xC4};
register.send_msb<32>(bits);
```


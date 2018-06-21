# MCP23017 / MCP23S17 (IO Expander)

## Features

* I2C communication
* 16 individually-configurable input/output pins
* Input pins (optionally pullup)
* Output pins
* Fast-access API (outputs only)
* Pin API compatible

## Example

```cpp
#include <MCP23017.h>

...

auto io = MakeMCP23017(
	ArduinoTWIMaster(),
	0x0 // sub-address (0-7), matching A0/A1/A2 on chip
);

io.set_pin_high(MCP23017::Pin::A_0);

io.set(0x1234); // set all pins

// set pin bank B to 0xF0 and leave A unchanged
io.set(0xF000, 0xFF00);

// read all input pins at once:
uint16_t state = io.get();

// read a specific pin (note: for reading multiple pins it is faster
// to fetch all pins in one call, as above)
bool pinA7 = io.get_port(MCP23017::Pin::A_7);

// Compatible with pin API
auto pA5 = io.pin(MCP23017::Pin::A_5);
pA5.high();
pA5.low();

auto pA6 = io.pin<MCP23017::Pin::A_6>();
pA6.set_input();
bool on = pA6.read_digital();

...

// The fast API can be used to perform quick output operations.
// Note that no non-output operations can be performed while this
// is active, and nothing else can use the same TWI interface.
auto fast_out = io.fast_output();

// Fast API supports many of the same methods:

fast_out.set_pin_high(MCP23017::Pin::A_1);

fast_out.set(0x1234); // set all pins

// set pin bank B to 0xF0 and leave A unchanged
fast_out.set(0xF000, 0xFF00);

// fast mode stops when fast_out is destructed
// (i.e. when the variable goes out of scope)
```

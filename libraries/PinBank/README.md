# PinBank

This provides a bridge for using arbitrary pins with APIs requiring pin
banks. This is not an efficient implementation, as it will set each pin
in turn (though it will enable batching for APIs which support it). For
better performance, use the available bespoke pin bank implementations
where available.

## Features

* Supports 2-, 4-, 6- and 8- pin banks
* Low or no overhead
* Uses `begin_batch` and `send_batch` to batch changes to external pins

## Example

```cpp
#include <ArduinoPin.h>
#include <VoidPin.h>
#include <PinBank.h>

auto bank = MakePinBank(
	ArduinoPin(2),
	ArduinoPin(3),
	VoidPin(),
	ArduinoPin(8)
);

bank.set_output(); // switch all pins to output mode

...

bank.set(0xC); // set bits to 1100 (MSB first)

// for the constructor above, this is equivalent to:
// ArduinoPin(2).high();
// ArduinoPin(3).high();
// VoidPin().low();
// ArduinoPin(8).low();

...

bank.set_input(); // switch all pins to input mode
uint8_t value = bank.get();
```

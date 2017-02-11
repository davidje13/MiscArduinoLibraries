# RawPin

Allows quick changing of pin values. This uses more RAM than storing a simple
pin number, but allows the pin to be changed HIGH/LOW with minimal overhead for
super-fast switching. Note that interrupts are not blocked by these operations,
so if interrupts may interfere with pin buffers, you should block them yourself.

## Example

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

# RotaryEncoder

Checks the output of digital rotary encoders.

## Features

* Detect directed movement
* Detects all 4 state transitions in both directions
* Low instruction overhead (minimal branching)
* Can use pin interrupts for better accuracy
* Optionally enable pullup resistors on input pins
* Tolerates both channels changing simultaneously
  (always returns delta = 2)
* Supports a single interrupt-enabled pin, combined with a
  non-interrupt pin (figures out direction of both channels changing
  simultaneously based on which pin had interrupt capability)

## Example

### Without Interrupts

```cpp
#include <RotaryEncoder.h>
#include <ArduinoPin.h>

auto encoder = MakeRotaryEncoder(
	ArduinoPin(12),
	ArduinoPin(13)
	// to use pullup resisors, add "true" as a third argument
);

...

// if position within the states is important (e.g. threshold every 4
// notches), you can seed your raw value with:
int raw = encoder.fraction();

while(true) {
	int delta = encoder.delta();
	raw += delta;
	...
}
```

### Using Interrupts (on one or both pins)

```cpp
#include <RotaryEncoder.h>
#include <ArduinoPin.h>

// Pins 2 and 3 both support interrupts
auto encoder = MakeInterruptRotaryEncoder(
	ArduinoPin(2),
	ArduinoPin(3)
	// to use pullup resisors, add "true" as a third argument
);

// If only 1 pin supports interrupts, the same call will work:
// MakeInterruptRotaryEncoder(ArduinoPin(2), ArduinoPin(4));

...

int raw = encoder.fraction();

while(true) {
	int delta = encoder.delta();
	raw += delta;
	...
}
```

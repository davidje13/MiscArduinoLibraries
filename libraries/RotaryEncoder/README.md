# RotaryEncoder

Checks the output of digital rotary encoders.

## Features

* Detect directed movement
* Detects all 4 state transitions in both directions
* Low instruction overhead (no branching)
* Can use pin interrupts for better accuracy
* Tolerates both channels changing simultaneously
  (always returns delta = 2)

## Example

### Without Interrupts

```cpp
#include <RotaryEncoder.h>
#include <ArduinoPin.h>

auto encoder = MakeRotaryEncoder(
	ArduinoPin(12),
	ArduinoPin(13)
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

### Using Interrupts

```cpp
#include <RotaryEncoder.h>
#include <ArduinoPin.h>

auto encoder = MakeInterruptRotaryEncoder(
	ArduinoPin(2),
	ArduinoPin(3)
);

...

int raw = encoder.fraction();

while(true) {
	int delta = encoder.delta();
	raw += delta;
	...
}
```

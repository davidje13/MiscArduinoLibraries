# WS2812 ("NeoPixel")

## Features

* Individual pixel RGB control
* Send arrays, progmem data, or lambdas
* Minimal interference for interrupt-driven code
* Works with any data pin (does not require PWM or interrupts)

## Example

```cpp
#include <WS2812.h>
#include <ArduinoPin.h>

...

FixedArduinoPin<6> stripPin;
auto strip = MakeWS2812(stripPin);

...

// Channel order is: Green, Red, Blue
uint8_t data[] = {0, 1, 2, 3, 4, 5};
strip.send(6, data);

// To avoid using RAM, pixel values can be calculated on-the-fly
// (but note that there are strict timing constraints)
strip.send_fn(6, [] (uint16_t index) {
	// This lambda is allowed up to 8 microseconds of processing time
	return 32;
});

// RGB compatibility:

strip.send_rgb_fn(2, [] (uint16_t index, uint8_t *r, uint8_t *g, uint8_t *b) {
	// This lambda is allowed up to 8 microseconds of processing time
	*r = 32;
	*g = 32;
	*b = 32;
});

// For more processing time per pixel, the channels can be broken up:
// (each individual call is permitted 8 microseconds)
strip.send_rgb_fn(
	2,
	[] (uint16_t index, uint8_t *r) {
		*r = 32;
	},
	[] (uint16_t index, uint8_t *g) {
		*g = 32;
	},
	[] (uint16_t index, uint8_t *b) {
		*b = 32;
	},
);
```

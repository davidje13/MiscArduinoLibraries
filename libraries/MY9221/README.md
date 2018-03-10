# MY9221 (12-channel grayscale LED controller)

Sends data to a MY9221 controller, which can in turn control up to 12
LED channels using Pulse Width Modulation or Pulse Density Modulation.

## Features

* Fast pin communication
* 12 channels at 8, 12, 14 or 16 bits per channel
* Configurable clock speed
* PWM or PDM output mode

## Gotchyas

There is a brief flicker when changing from a lower bit-mode (e.g.
8-bit) to a higher mode (e.g. 16-bit). This flicker is proportional to
the current frequency and is not present when switching from a higher
mode to a lower one.

High bit modes will strobe noticeably when using lower frequencies.

## Example

```cpp
#include <MY9221.h>
#include <ArduinoPin.h>

auto DCKI_PIN = ArduinoPin(7); // clock
auto DI_PIN   = ArduinoPin(8); // data

auto bar = MakeMY9221(DCKI_PIN, DI_PIN);

...

//bar.set_modulation(MY9221::Modulation::PULSE_WIDTH);
bar.set_modulation(MY9221::Modulation::PULSE_DENSITY);

//bar.set_frequency(MY9221::Frequency::EXTERNAL_FREE_RUNNING);
bar.set_frequency(MY9221::Frequency::F8600000Hz);

uint8_t values[] = {
	0x00, 0x11, 0x22,
	0x44, 0x55, 0x66,
	0x88, 0xAA, 0xBB,
	0xDD, 0xEE, 0xFF
};
//bar.set(values); // Set raw values
bar.set_norm(values); // Set values which are perceptually consistent
// (you can also use MY9221::normalise(v) to make values consistent)

uint16_t values[] = {
	0x0000, 0x1111, 0x2222,
	0x4444, 0x5555, 0x6666,
	0x8888, 0xAAAA, 0xBBBB,
	0xDDDD, 0xEEEE, 0xFFFF
};
bar.set_norm(values);

uint16_t values[] = {
	0x000, 0x111, 0x222,
	0x444, 0x555, 0x666,
	0x888, 0xAAA, 0xBBB,
	0xDDD, 0xEEE, 0xFFF
};
bar.set_norm<12>(values);
```


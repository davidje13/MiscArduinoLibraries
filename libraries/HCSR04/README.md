# HC-SR04 (Echolocator)

This library wraps communication with the HC-SR04 echolocator.

The HC-SR04 works by sending a series of untrasound pulses when
triggered, measuring the time to reflection, then reporting this back
by holding ECHO high for the same duration.

This wrapper library supports 2 modes of operation; synchronous and
asynchronous.

Requests to measure distance should be separated by at least 50ms to
avoid interference from earlier reflections.

## Features

* Synchronous operation to get current measured distance
* Recognises "no response" signal and avoids waiting longer than
  necessary
* Asynchronous operation uses pin interrupts to allow code to continue
  while distance is measured
* Device measures from 26mm (150us) -- ~4000mm (25ms)

## Example

### Synchronous

```cpp
#include <HCSR04.h>
#include <ArduinoPin.h>

auto echo = MakeSynchronousHCSR04(
	ArduinoPin(7), // output pin
	ArduinoPin(8)  // input pin
);

...

while(true) {
	unsigned long distance = echo.measure_millimetres();
	if(distance == HCSR04::no_reflection()) {
		...
	} else {
		...
	}

	delay(50);
}
```

### Using Interrupts

```cpp
#include <HCSR04.h>
#include <ArduinoPin.h>

auto echo = MakeAsynchronousHCSR04(
	ArduinoPin(7), // output pin
	ArduinoPin(3)  // input pin (must be interrupt capable)
);

...

while(true) {
	// returns the most recent completed measurement and triggers a
	// new measurement (if no measurement is ongoing)
	unsigned long lastDistance = echo.measure_millimetres();

	// can force a synchronous wait for latest result
	// unsigned long distance = echo.await_result();

	if(lastDistance == HCSR04::no_reflection()) {
		...
	} else {
		...
	}

	delay(50);
}
```

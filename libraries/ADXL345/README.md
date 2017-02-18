# ADXL345 (Accelerometer)

This library wraps communication with an ADXL345 chip via I2C. The chip is able
to detect acceleration in 3 orthogonal axes. It also has features for detecting
tap, double-tap, inactivity, and free-fall events.

## Features

* I2C communication (at 400kHz), but currently SPI communication is not
  supported
* Connection diagnostics
* Supports most functionality of the chip, including:
  * Choice of two I2C addresses
  * Fetching XYZ acceleration samples
  * Configurable 0-offset
  * Configurable sample buffering (up to 32 samples in the buffer + 1 in the
    register), including support for interrupt-driven snapshots
  * Configurable sample rate
  * Configurable tap / double-tap detection
  * Configurable activity / inactivity detection
  * Configurable free-fall detection
  * Per-axis event configuration and reporting
  * Supports sleep mode, auto-sleep mode and low-power mode to save power when
    measurement accuracy is not critical
  * Supports standby mode to save power when not in use
  * Self-test functionality
* Normalises input quantities into milli-g's, milliseconds, microseconds (no
  need to multiply by 12.5, 6.25 or other random multipliers!)
* Optional normalisation of output into milli-g's via `*_millig()` methods

## Example

```cpp
#include <ADXL345.h>

...

bool usingAlternateAddress = true;
ADXL345 Accelerometer(usingAlternateAddress, /*optional INT1, INT2 pins here*/);

accelerometer.configure_fifo_bypass();
accelerometer.configure_double_tap_detection(
	3000,   // 3g    (tap threshold)
	50000,  // 50ms  (max tap duration)
	100000, // 100ms (min gap between taps in double-tap)
	500000  // 500ms (max gap between taps in double-tap)
);
accelerometer.set_on(true);

// Sampling values and responding to actions
while(true) {
	auto reading = accelerometer.next_reading();
	if(reading.is_valid()) {
		int x = reading.x_millig();
		int y = reading.y_millig();
		int z = reading.z_millig();
	}

	if(accelerometer.has_detected_tap()) {
		// ...
	}
}

// Just responding to actions
while(true) {
	accelerometer.poll_device();
	if(accelerometer.has_detected_tap()) {
		// ...
	}
}
```

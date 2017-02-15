# ITG3200 (Gyroscope)

This library wraps communication with a ITG3200 chip via I2C. The chip is able
to detect rotational velocity around 3 orthogonal axes, as well as on-board
temperature (which can be used to improve accuracy).

## Features

* I2C communication (at 400kHz)
* Connection diagnostics
* Supports most chip operations, including
  * Configurable AD0 value (allowing 2 chips on the same I2C bus)
  * Access to gyroscope readings as raw values (divide by 14.375 for deg/s)
  * Access to temperature readings as raw values and milli-degrees-celsius
  * Customisable on-board low-pass filter
  * Customisable frequency via frequency divider
  * Customisable interrupt pin behaviour
  * Customisable clock source for improved accuracy
  * Soft reset
  * Disable individual gyroscopes to save power
  * Sleep state when not in use to save power
* Synchronous and asynchronous API
* GyroAccumulator class for easy sample accumulation (note: currently
  accumulates each axis independently)
* Calibration stage, allowing zero-point detection and correction

## Example

```cpp
#include <ITG3200.h>
#include <GyroAccumulator.h>

...

bool AD0 = false;
ITG3200 gyroscope(AD0, /*optional interrupt pin here*/);

gyroscope.set_filter_bandwidth(ITG3200::LowPassBandwidth::L188_HZ); // 1kHz
gyroscope.set_sample_rate_divider(16); // New sample every 60 ms

// More reliable clock source according to datasheet
gyroscope.set_clock_source_sync(ITG3200::ClockSource::X_GYRO);

GyroAccumulator<ITG3200::reading> accumulator;

// Calibrate
accumulator.begin_calibration();
for(int n = 0; n < 128; ++ n) {
	auto reading = gyroscope.next_reading();
	if(reading.is_valid()) {
		accumulator.accumulate(reading);
	}
}
accumulator.complete_calibration();

while(true) {
	auto reading = gyroscope.next_reading();
	if(reading.is_valid()) {
		accumulator.accumulate(reading);

		... = accumulator.x_millideg();
		... = accumulator.y_millideg();
		... = accumulator.z_millideg();
	}
}

...

gyroscope.sleep(); // enter low-power mode

...

gyroscope.awake(); // leave low-power mode
gyroscope.disable_gyros(true, false, false); // disable the X gyroscope
```

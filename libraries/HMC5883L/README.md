# HMC5883L (Compass)

This library wraps communication with a HMC5883L chip via I2C. The chip is able
to detect magnetic field strength in 3 orthogonal axes.

Note that for finding north, this should be combined with an accelerometer to
determine the direction of gravity. Magnetic field lines run at steep angles
when away from the equator (e.g. approximately 60 degrees in the UK and USA),
so without normalisation, results can vary wildly from slight tilting of the
device.

## Features

* I2C communication (at 400kHz)
* Connection diagnostics
* Optional support for DRDY (interupt) pin - if not provided, will fall-back to
  polling over I2C
* Optional support for configuring DRDY pin as interrupt - if a pin is given
  but it does not support interrupts, will fall-back to querying the pin
* Supports full functionality of the chip, including:
  * Single-sample and continuous-sample modes
  * Fastest sample rate (approx. 160Hz)
  * Customisable sample averaging and gain
  * Self-test functionality and calibration (via `HMC5883L::calibrator`)
  * Idle state when not in use to save power
* Synchronous and asynchronous API

## Example

```cpp
#include <HMC5883L.h>
#include <VoidPin.h>

...

auto compass = MakeHMC5883L(VoidPin()); // optional DRDY pin

compass.set_averaging(HMC5883L::Averaging::A4); // 4 samples for each reading
compass.set_gain(HMC5883L::Gain::G1370);
compass.read_continuous_async(HMC5883L::Rate::R75_HZ);

while(true) {
	auto reading = compass.next_reading();
	if(reading.is_saturated()) {
		// Try a different gain with a wider range, e.g.:
		compass.set_gain(HMC5883L::Gain::G230); // Max range, worst resolution
		continue;
	}
	int x = reading.x();
	int y = reading.y();
	int z = reading.z();
	int gain = reading.gain();
	// value in gauss = x / gain, y / gain, z / gain
}
```

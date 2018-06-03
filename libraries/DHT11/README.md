# DHT11 (Temp. / Humidity sensor)

This library wraps communication with the DHT11 temperature / humidity
sensor.

## Features

* Data in integer or float representations
* Error checking
* Automatically retry readings until hardware is ready
* Optional internal pull-up

## Notes

Due to the hardware, samples will be given at most once every 2 seconds
or so. If you request samples more frequently, the request will block
until a sample is ready, or will return a `TIMEOUT` error.

## Example

```cpp
#include <ArduinoPin.h>
#include <DHT11.h>

// Specify the data pin when constructing
auto sensor = MakeDHT11(ArduinoPin(4));

// If you have an external pullup, disable the internal pullup with:
auto sensor = MakeDHT11ExternalPullup(ArduinoPin(4));

// ...

auto result = sensor.read(); // wait up to 2.1 seconds for a reading

// result.state() ->
//    DHT11::State::OK
//    DHT11::State::TIMEOUT
//    DHT11::State::CHECKSUM_MISMATCH
// result.valid() -> true if state is OK

// result.temperature() -> float
// result.temperature_raw() -> unsigned int (16 bits)

// result.humidity() -> float
// result.humidity_raw() -> unsigned int (16 bits)

// to customise the timeout:
// auto result = sensor.read(0); // 0ms timeout (don't retry)
```

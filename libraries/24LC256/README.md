# 24LC256 (external EEPROM)

## Features

* Read and write arbitrary memory addresses of any size
* Automatic retry if device is busy
* Efficient write behaviour for large data

## Warning

EEProm memory has a finite lifetime based on the number of erase-write
cycles (per page). For this chip, the manufacturer guarantees 1 million
write cycles.

This means that to maximise the lifetime of the chip, and to get best
performance, it is always good to write page-aligned data
simultaneously (writing any byte within a page will cause the entire
page to be refreshed). For this reason, it is best to avoid writing
individual bytes one-at-a-time.

Also note that if this is used with the standard Arduino TWI library,
it will perform 3 writes per page. This is because the Arduino TWI
library is limited to a buffer of 32 bytes. 2 bytes are used for the
address, and 64 bytes must be sent per page. The result of this is that
sending large amounts of data will take slightly longer than expected,
and will wear the chip faster than expected (333k total write cycles
per page). This problem can be avoided by using a better TWI
implementation.

## Example

```cpp
#include <24LC256.h>
#include <ArduinoTWIMaster.h>

...

ArduinoTWIMaster twi;

auto eeprom = Make24LC256(
	twi,
	0x0 // sub-address must match A0-A2 pins on device (0x0 -- 0x07)
);

...

const uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
auto err = eeprom.write_bytes(128, data, 10);
// err is one of ArduinoTWIMaster::Error

uint8_t outData[10];
bool success = eeprom.read_bytes(128, outData, 10);

uint16_t size = eeprom.size_bytes();
constexpr uint8_t pageSize = eeprom.page_size_bytes();

const uint8_t pageData[pageSize];
eeprom.write_page(1, pageData);
```

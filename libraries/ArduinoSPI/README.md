# ArduinoSPI

Object-oriented abstraction around the standard SPI access functions on Arduino.

`begin()` and `end()` are automatically managed through the constructors and
destructor.

## Example

```cpp
#include <ArduinoSPI.h>

ArduinoSPI mySPI;

mySPI.begin_transaction(
	4000000, // Talking to a slave with maximum frequency of 4MHz
	ArduinoSPI::ByteOrder::MSB_FIRST,
	ArduinoSPI::DataMode::MODE1 // = SPI_MODE1
);

uint8_t result8 = mySPI.transfer(0x50);

uint16_t result16 = mySPI.transfer16(0x1234);

uint8_t buffer[] = {1, 2, 3};
mySPI.transfer(buffer, sizeof(buffer));
// buffer now contains response

mySPI.end_transaction();
```

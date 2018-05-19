# ArduinoSerial

Consistent abstraction around the serial API provided by Arduino.

## Example

```cpp
#include <ArduinoSerial.h>
#include <ArduinoPin.h>
#include <VoidPin.h>
#include <ArduinoSoftwareSerial.h>

auto mySerial = MakeArduinoSerial(&Serial);

ArduinoPin RX(5);
ArduinoPin TX(5);
auto mySerial2 = MakeArduinoSoftwareSerial(TX, RX);

auto writeOnlySerial = MakeArduinoSoftwareSerial(TX, VoidPin());
auto writeOnlySerial2 = MakeArduinoSoftwareSerial(TX);

mySerial.begin(9600);

mySerial.write(0x23);

if(mySerial.can_listen()) {
	mySerial.listen();
	mySerial.write(0x45);
	while(mySerial.available()) {
		uint8_t v = mySerial.read();
		// ...
	}
}
```

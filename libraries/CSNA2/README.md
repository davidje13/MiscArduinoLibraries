# CSNA2 (Thermal Printer)

This library wraps communication with a CSNA2 thermal printer via a
single or two-wire serial connection.

## Features

* Serial communication with configurable baud rate (check printer test
  sheet)
* Optional TX-only mode (leave RX unconnected)
* Exposes power-saving configuration options such as sleep and thermal
  settings
* Exposes font options (emphasis, underline, invert, etc.)
* Simple character rotation (0, 90, 180 and 270 degrees)
* Works with PROGMEM or non-PROGMEM strings everywhere

## Example

```cpp
#include <CSNA2.h>
#include <ProgMem.h>
#include <ArduinoSoftwareSerial.h>
#include <ArduinoPin.h>

...
ArduinoPin TX(6);
ArduinoPin RX(5);

auto printer = MakeCSNA2(
	MakeArduinoSoftwareSerial(TX, RX),
	19200 // baud rate
);

printer.configure(CSNA2::Configuration::FACTORY);
printer.set_sleep_delay(1);

printer.print(ProgMemString("Hello!\n\n\n"));

delay(2000); // Printer will go to sleep after 1 second

printer.awake();
printer.print(ProgMemString("Still here!\n\n\n"));
```

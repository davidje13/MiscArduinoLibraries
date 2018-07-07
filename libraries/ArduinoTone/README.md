# ArduinoTone

Consistent abstraction around the tone API provided by Arduino.

## Example

```cpp
#include <ArduinoTone.h>

ArduinoTone<5> myBleeper;

// Or for runtime flexibility in pin used:
// DynamicArduinoTone myBleeper(5);

// ...

// Play a sound with a particular frequency (Hz)
myBleeper.play(40000);

delay(1000);

myBleeper.stop();

delay(500);

myBleeper.play(50000);

delay(1000);

// Optional second parameter is duration in milliseconds
myBleeper.play(40000, 500);
```

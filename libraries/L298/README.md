# L298 (Dual full-bridge driver)

This library wraps communication with a L298 bridge driver. It also
includes support for the Arduino Motor Shield Rev3, which uses the
L298.

## Features

* Variable speed control using PWM pins
* Free-wheeling and variable-strength braking
* Current detection

## Example

### Arduino Motor Shield

```cpp
#include <ArduinoMotorShield.h>

// Shield uses pins 12, 9, 3 and A0 for channelA
auto channelA = MakeArduinoMotorShieldChannelA();

// and pins 13, 8, 11, A1 for channelB
auto channelB = MakeArduinoMotorShieldChannelB();

// Use MakeArduinoMotorShieldChannelA_nobrake if you do not want to
// use the brake pin (forward, reverse and freewheel will still work)
// Removing the brake pin frees up pin 9 or 8 depending on the channel
// (note that you will also need to physically cut the jumper on the
// shield)

...

channelA.forward(); // same as forward(255) or set(255)
channelB.set(-128); // range: [-255 255]

delay(1000);

channelA.brake();
channelB.freewheel();

delay(1000);

channelA.reverse(192);
delay(500);
channelA.brake(128); // 50% brake, 50% freewheel

...

float currentA = channelA.current();
```

### Raw chip access

```cpp
#include <L298.h>
#include <ArduinoPin.h>
#include <ArduinoAnalogPin.h>

ArduinoPin fwd(5);
ArduinoPin rev(6);
ArduinoPin en(7); // for variable speed control, must be PWM
ArduinoAnalogPin sense(0);

auto channel = MakeL298Channel(fwd, rev, en, sense);

// Same interface as ArduinoMotorShield, except:

float currentA = channelA.current(0.15f); // specify resistor value
```

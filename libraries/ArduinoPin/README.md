# ArduinoPin

Object-oriented abstraction around the standard pin access functions on Arduino.

The standard classes (`ArduinoPin` and `ArduinoAnalogPin`) have no runtime
overhead compared to using standard Arduino function calls, and in fact should
compile to exactly the same bytecode.

If pin numbers are known at compile time, the `Fixed*` variants can be used to
remove all RAM requirements (but see note below). These will effectively hard-
code the pin number at all call sites, and will typically produce smaller
program sizes with no performance penalty (the downside is that the pin numbers
cannot be changed dynamically).

Alternatively, the `Raw*` variant can be used if faster switching is desired
and RAM is not a concern.

All classes are compatible with the general Pin interface, allowing templating
and easy swapping of implementations.

## Standard digital pins

The `ArduinoPin` class is a simple wrapper around the standard operations for
the digital (`D*`) input/output pins.

### Example

```cpp
#include <ArduinoPin.h>

ArduinoPin myPin(7);

bool exists = myPin.exists();  // always true
myPin.supports_input();        // true
myPin.supports_analog_input(); // false
myPin.supports_output();       // true
myPin.supports_pwm_output();   // true for some pins
myPin.supports_interrupts();   // true for some pins

myPin.set_output();    // switch to output mode
myPin.high();          // set HIGH
myPin.low();           // set LOW
myPin.pwm(88);         // set PWM mode (88/255 duty cycle)

myPin.set_input(true); // switch to pull-up input mode (false for floating)

// on supported pins, these set the interrupt function (no-op for other pins)
// note that only one interrupt can be set at a time (so for rising & falling,
// use change and check the current state, etc.)
myPin.set_interrupt_on_low(&myFunction);
myPin.set_interrupt_on_rising(&myFunction);
myPin.set_interrupt_on_falling(&myFunction);
myPin.set_interrupt_on_change(&myFunction);
myPin.set_interrupt_on_high(&myFunction); // only supported on some boards
myPin.remove_interrupt();

bool isHigh = myPin.read_digital();
```

## Standard analog pins

The `ArduinoAnalogPin` class is a simple wrapper around standard operations for
the analog (`A*`) input pins.

### Example

```cpp
#include <ArduinoAnalogPin.h>

ArduinoAnalogPin myAnalogPin(7);

bool exists = myAnalogPin.exists();  // always true
myAnalogPin.supports_input();        // true
myAnalogPin.supports_analog_input(); // true
myAnalogPin.supports_output();       // false
myAnalogPin.supports_pwm_output();   // false
myAnalogPin.supports_interrupts();   // false

myAnalogPin.set_input(); // does nothing; provided for compatibility

bool isHigh = myAnalogPin.read_digital(); // true if analog value is >= 512
uint16_t value = myAnalogPin.read_analog(); // [0-1023]
uint32_t rangedValue = myAnalogPin.read_analog(100); // [0-99]
```

## Fixed pins

`FixedArduinoPin` and `FixedArduinoAnalogPin` can be used if the pin number is
known at compile-time. These have identical APIs to their non-fixed
counterparts, but do not consume any RAM (the pin number is hard-coded at all
call sites, so only consumes program memory). The constructors for these are
slightly different (see the example). Note that these will only show any
benefit if you are calling functions or creating objects which store pins. For
typical use, the standard classes are recommended.

Note that if you are writing a class which stores pin references, you will not
get the zero-RAM benefit from these classes by default. C++ defines that all
structs must have unique locations in memory, which means that zero-size structs
always consume at least 1 byte. There are various tricks to get around this.

### Example

```cpp
#include <ArduinoPin.h>
#include <ArduinoAnalogPin.h>

auto myPin = FixedArduinoPin<7>(); // Template parameter for pin number
FixedArduinoPin<7> myPin; // Alternative construction style

myPin.set_output();
myPin.high();
myPin.low();

auto myAnalogPin = FixedArduinoAnalogPin<3>();
uint16_t value = myAnalogPin.read_analog();
```

## Raw pin access

If speed of switching is a concern, but memory is not, you can use
`RawArduinoPin`. This class uses 5 bytes of RAM per pin, but allows minimal-
overhead switching. It is fully API compliant with `ArduinoPin`, but some of
the methods (e.g. PWM access, configuring interrupts) will not be any faster.

### Example

```cpp
#include <RawArduinoPin.h>

RawArduinoPin myPin(7);
myPin.set_output();
myPin.high();
myPin.low();

// For even faster access, you can use the _fast variants which do not block
// interrupts internally. Note that unless you know for sure that nothing else
// will touch pins in the same bank as the ones you are changing (check for I2C
// and SPI!), you should block the interrupts yourself.
RawArduinoPin myFastPin(9);
uint8_t oldSREG = SREG; cli(); // Block interrupts
myFastPin.high_fast();
myFastPin.low_fast();
myFastPin.high_fast();
myFastPin.low_fast();
SREG = oldSREG; // Restore interrupts
```

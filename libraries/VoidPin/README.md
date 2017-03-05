# VoidPin

A simple object which can be provided to APIs which require a pin. Does not
support any pin operations and will no-op if requested to perform them.

This class conforms to the general Pin interface, allowing templating and easy
swapping of implementations.

## Example

```cpp
#include <VoidPin.h>

VoidPin myPin;

bool exists = myPin.exists();  // always false
myPin.supports_input();        // false
myPin.supports_analog_input(); // false
myPin.supports_output();       // false
myPin.supports_pwm_output();   // false
myPin.supports_interrupts();   // false

myPin.set_output();    // does nothing
myPin.high();          // also nothing
myPin.low();           // still nothing
myPin.pwm(88);         // guess what this does?
myPin.set_input(true); // that's right: not a thing

// none of these do anything either
myPin.set_interrupt_on_low(&myFunction);
myPin.set_interrupt_on_rising(&myFunction);
myPin.set_interrupt_on_falling(&myFunction);
myPin.set_interrupt_on_change(&myFunction);
myPin.set_interrupt_on_high(&myFunction);
myPin.remove_interrupt();

bool isHigh = myPin.read_digital();      // always false
uint16_t value = myPin.read_analog();    // always 0
uint32_t value = myPin.read_analog(100); // always 0
```

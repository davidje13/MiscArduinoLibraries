# Rotary Encoder Demo

This demo shows reading a rotary encoder's state.

## Hardware requirements

This demo was written to work with 2-wire digital rotary encoders.
The speed of the built-in LED (on pin 13) will change depending on the
selected value.

```
A -- D2 (or any available pin; change ROT_PIN_A in main.h)
B -- D3 (or any available pin; change ROT_PIN_B in main.h)
```

Pins 2 and 3 are chosen because on the Uno these can have interrupts
registered, which means that changes will not be missed while
processing other tasks. Any other pins can be used, but without
interrupts it is possible that some transitions will be missed.

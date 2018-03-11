# Infrared Utilities

Records and interprets received IR signals.

## Features

* Synchronous operation waits for a command up to a specified timeout
* Asynchronous operation uses pin interrupts to capture commands in the
  background
* Pause/resume asynchronous listening with `stop()` / `start()`
* Recognises active-high or active-low (default)
* Generic signal parsing works with most vendor implementations

## Example

### Synchronous

```cpp
#include <IRReceiver.h>
#include <ArduinoPin.h>

auto IR_PIN = ArduinoPin(7);

auto ir = MakeSynchronousIRReceiver(
	IR_PIN,
	IRReceiver::Activation::ACTIVE_LOW
);

// Usually long headers are expected, but for remotes which do not use
// these, the check can be disabled
// ir.set_strict_header(false);

...

while(true) {
	const IRCommand *recorded = ir.await_command(1000); // Wait 1s max
	// const IRCommand *recorded = ir.await_command();  // Wait forever

	// the return value (recorded) is valid until the next call to
	// await_command, or until the destruction of ir.

	if(recorded) {
		uint16_t hlen = recorded->header_duration();
		uint8_t bits = recorded->length();
		uint64_t data = recorded->decode_data();
		// process command
	}
}
```

### Asynchronous

```cpp
#include <IRReceiver.h>
#include <ArduinoPin.h>

auto IR_PIN = ArduinoPin(2);

auto ir = MakeAsynchronousIRReceiver(
	IR_PIN,
	IRReceiver::Activation::ACTIVE_LOW
);
ir.start();

...

while(true) {
	const IRCommand *recorded = ir->next_command();
	if(recorded) {
		uint16_t hlen = recorded->header_duration();
		uint8_t bits = recorded->length();
		uint64_t data = recorded->decode_data();

		// Invalidate recorded ASAP so that new commands can be stored
		// as they are received
		ir.invalidate(recorded);

		// process command
	}

	// synchronous waiting is also supported
	// const IRCommand *recorded = ir.await_command(1000);
}
```

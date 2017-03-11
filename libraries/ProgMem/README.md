# ProgMem

This wrapper class makes interacting with PROGMEM (non-RAM-consuming) data much
easier. It allows the same syntax to be used as for normal data.

Note that not all operations are automatically fixed; it is still necessary to
switch calls to (e.g.) `memcpy` with `memcpy_P`. A `raw()` method is provided
to access the underlying data pointer for these cases.

## Features

* Natively supports byte, word and long (1/2/4-byte) integer types
* Natively supports floats
* Natively supports pointer types
* Supports all other types of data array, via the use of memcpy_P (note that
  this may not be the most efficient way to access data from arrays of structs)
* Supports common operators to behave like a normal pointer
* Zero overhead compared to writing PROGMEM functions directly in almost all
  cases (no extra RAM usage, no extra CPU instructions)
* Convenience wrapper for strings, allowing them to be put into PROGMEM inline

## Example

```cpp
#include <ProgMem.h>

static PROGMEM const uint8_t myByteData[] = {0x00, 0x11, 0x22};
auto myWrappedByteData = MakeProgMem(myByteData);
myWrappedByteData[0]; // 0x00
myWrappedByteData[1]; // 0x11
myWrappedByteData += 2;
myWrappedByteData[0]; // 0x22

struct Coordinate {
	int x;
	int y;
};
static PROGMEM const Coordinate myCoordinates[] = {
	{0, 0},
	{10, 0},
	{100, 5}
};
auto myWrappedCoordinates = MakeProgMem(myCoordinates);
myWrappedCoordinates[0]; // Coordinate(0, 0)

myTextFunction(ProgMemString("Hello; I won't use up any RAM!"));
```

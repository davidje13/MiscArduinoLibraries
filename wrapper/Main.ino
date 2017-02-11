// This wrapper provides a way to interface with Arduino's compiler,
// which would normally make quite a mess out of C++ code, but will
// play nicely if it lives inside a library folder. The actual "cpp"
// folder contains the real implementations, and is seen by the
// compiler as a library.

#include <main.h>

void loop(void) {}

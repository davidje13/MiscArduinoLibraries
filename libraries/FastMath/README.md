# FastMath

This is a small library which calculates sine and cosine values using integer-
only arithmetic. It is extracted from the FastLED project, available under the
MIT license, and required by the SceneRenderer (available as part of the
Bitmask18 library).

When rendering 3D scenes on a chip without an FPU (e.g. typical Arduino chips),
using floating point arithmetic causes massive slowdown.

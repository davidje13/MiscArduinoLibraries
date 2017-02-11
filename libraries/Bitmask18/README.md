# Bitmask18

This is the companion library for SSD1306. It stores bitmask representations in
the format understood by the SSD1306 chip (array of bytes, each covering a 1x8
pixel area of the screen). It includes a number of convenient drawing operations
which have been optimised for the data storage.

## Features

* Render outlined / filled lines / rectangles / ellipses / triangles
* Render bitmaps
* Full-screen clear
* Set individual pixels, and get raw memory access for faster algorithms
* All rendering operations support blend modes (ON, OFF, XOR, NOP)
* All rendering operations except bitmap support patterns (ON, CHECKER,
  CROSSHATCH, BRICKS, FILL75, and many more, as well as support for custom 4x8
  patterns)
* All rendering operations paint pixels at most once, allowing blend modes such
  as XOR to work correctly in all cases
* Line and triangle rendering supports sub-pixel precision (configurable)
* Ellipse rendering supports even and odd sized ellipses; just specify bounding
  box
* Fixed-width and variable-width fonts (example fonts are included in the
  SSD1306 demo)
* Text rendering with support for rudimentary line wrapping
* Basic 3D rendering (vertex projection, line rendering, triangle rendering,
  backface culling) - note: does not (yet) include depth buffer, so objects
  must be convex, or faces must be rendered back-to-front
* Full support for PROGMEM data (fonts, text, bitmaps, patterns, 3D data)
* Minimal RAM usage; size information is controlled by the template parameters
  so does not consume RAM - the only RAM used is for pixel data

## Dependencies

* The SceneRenderer tools use the FastMath library when generating quaternions.
  Note that the FastMath library is not public domain, but MIT licensed.
* The library can be used with the ProgMem library to avoid storing constant
  data in RAM (relevant functions are templated). This is an optional
  dependency.

## Example

```cpp
#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>
#include <Font.h>
#include <FontRenderer.h>

...

Bitmask18<128,64> myBitmask;
myBitmask.fill_ellipse(10, 10, 60, 40, BlendMode::ON, PATTERN_CHECKER);

Font myFont(MY_FONT_DATA, MY_FONT_IMAGE, MY_FONT_MASK);
auto myRenderer = MakeFontRenderer(&myBitmask, 0, 0, myBitmask.width(), 0);
myRenderer.print(myFont, "The quick brown fox jumps over the lazy dog.");
// Note: ProgMemString can be used to avoid using RAM when storing the message)
```

# SSD1306 Demo

These demo sources are based on work from Hobbytronics, in turn based on work
from Sparkfun. The sources have been significantly re-written to work with the
alternative SSD1306 library and implement general improvements in structure and
capability.

## Hardware requirements

This demo was written specifically to work with
http://www.hobbytronics.co.uk/lcd/oled-graphic-display-128-64-wb but should
also work with similar displays. It requires the following connections:

```
GND -- GND
 +V -- 3.3V
DAT -- D11 (MOSI)
CLK -- D13 (SCK)
 DC -- D9  (or any available pin; change OLED_PIN_DC in main.h)
RST -- D7  (or any available pin; change OLED_PIN_RST in main.h)
 CS -- D10 (or any available pin; change OLED_PIN_CS in main.h)
```

If you want to play the included PONG game, you should also connect
potentiometers to A0 and A1 (or any available A* pins; change PONG_POT_L/R in
main.h).

## Included demonstrations

1. Full-screen bitmap loading
1. Various commands (e.g. x/y flip, contrast changes, marquee)
1. Individual pixel access
1. Line rendering
1. Interactive PONG game (attach potentiometers to A0 and A1 to play)
1. Patterns (including using alternating patterns for crude shading)
1. Text (font support, line wrapping, numeric display)
1. Clock (sub-pixel line rendering, font measurement, blend modes)
1. 3D (vertex projection and scene rendering, quaternions, triangle
   rendering with patterns)

(then demo loops)

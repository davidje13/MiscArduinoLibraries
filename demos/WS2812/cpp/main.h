/*
 * Written in 2018 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

/*
 * Hardware connections:
 *
 *  In -- D6 (any available pin)
 */

// This demo is designed primarily for a 12-pixel ring,
// but should work with any WS2812-based display

#include <WS2812.h>
#include <ArduinoPin.h>
#include <ProgMem.h>
#include <FastMath.h>

FixedArduinoPin<6> stripPin;

// Green Red Blue
const PROGMEM uint8_t values[] = {
	0, 64, 0,
	96, 96, 0,
	64, 0, 0,
	96, 0, 96,
	0, 0, 64,
	0, 96, 96,
	0, 0, 0,
	1, 1, 1,
	32, 32, 32,
	64, 64, 64,
	128, 128, 128,
	255, 255, 255
};

const PROGMEM uint8_t values_zeros[] = {
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	0, 0, 255
};

void setup(void) {
	auto strip = MakeWS2812(stripPin);

	// Display rainbow / brightness pattern
	strip.send(12 * 3, MakeProgMem(values));

	delay(3000);

	// Display test pattern (all off except 1 bright blue pixel)
	strip.send(12 * 3, MakeProgMem(values_zeros));

	delay(1000);

	// Display moving rainbow
	uint8_t x = 0;
	while(true) {
		strip.send_rgb_fn(
			12,
			[x] (uint16_t i, uint8_t *r) {
				*r = uint8_t(sin8(i * 21 + x) + 127) / 4 + 1;
			},
			[x] (uint16_t i, uint8_t *g) {
				*g = uint8_t(sin8(i * 21 + 85 + x) + 127) / 8 + 1;
			},
			[x] (uint16_t i, uint8_t *b) {
				*b = uint8_t(sin8(i * 21 + 171 + x) + 127) / 16 + 1;
			}
		);
		x += 2;
		delay(10);
	}
}

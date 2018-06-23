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
#include <HumanLuminosity.h>

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

void setup(void) {
	auto strip = MakeWS2812(stripPin);
	const uint8_t count = 12;

	for(uint16_t n = 0; n < count; ++ n) {
		// Display test pattern (all off except 1 bright blue pixel)
		strip.send_fn(count * 3, [n] (uint16_t i) {
			if(i == n * 3 + 2) {
				return 255;
			} else {
				return 0;
			}
		});
		delay(100);
	}

	// Display rainbow / brightness pattern
	strip.send(12 * 3, MakeProgMem(values));
	delay(2000);

	// Display moving rainbow
	const uint16_t period_r = 190;
	const uint16_t period_g = 220;
	const uint16_t period_b = 250;
	const uint8_t step = 256 / count;
	uint16_t xr = 0;
	uint16_t xg = 85 * period_g;
	uint16_t xb = 171 * period_b;
	while(true) {
		strip.send_rgb_fn(
			count,
			[xr] (uint16_t i, uint8_t *r) {
				*r = HUMAN_LUMINOSITY_R[
					uint8_t(sin8(i * step + xr / period_r) + 127) / 2 + 1
				];
			},
			[xg] (uint16_t i, uint8_t *g) {
				*g = HUMAN_LUMINOSITY_G[
					uint8_t(sin8(i * step + xg / period_g) + 127) / 2 + 1
				];
			},
			[xb] (uint16_t i, uint8_t *b) {
				*b = HUMAN_LUMINOSITY_B[
					uint8_t(sin8(i * step + xb / period_b) + 127) / 2 + 1
				];
			}
		);
		xr += 256;
		xg += 256;
		xb += 256;
		if(xr >= period_r * 256) {
			xr -= period_r * 256;
		}
		if(xg >= period_g * 256) {
			xg -= period_g * 256;
		}
		if(xb >= period_b * 256) {
			xb -= period_b * 256;
		}
		delay(10);
	}
}

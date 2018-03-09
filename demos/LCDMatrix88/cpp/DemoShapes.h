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

#ifndef DEMOSHAPES_H_INCLUDED
#define DEMOSHAPES_H_INCLUDED

#include <Bitmask18.h>
#include <BlendMode.h>
#include <FastMath.h>

#define BAND_WIDTH 2

template <typename Display>
void demoShapes(Display &display) {
	// Render at high resolution. When displaying, the extra data will appear
	// as grayscale (provides antialiasing)
	constexpr uint8_t res = 4;
	constexpr uint8_t w = display.width() * res;
	constexpr uint8_t h = display.height() * res;

	Bitmask18<w,h> bitmask;

	for(uint8_t f = 0; f < 100; ++ f) {
		int8_t r = w + f % (res * BAND_WIDTH * 2);
		while(r > 0) {
			bitmask.fill_ellipse(
				w / 2 - r, h / 2 - r,
				r * 2, r * 2,
				BlendMode::ON, PATTERN_ON
			);
			r -= BAND_WIDTH * res;
			if(r > 0) {
				bitmask.fill_ellipse(
					w / 2 - r, h / 2 - r,
					r * 2, r * 2,
					BlendMode::OFF, PATTERN_ON
				);
				r -= BAND_WIDTH * res;
			}
		}
		display.send(bitmask, 0, 0, 32);
	}
}

#endif

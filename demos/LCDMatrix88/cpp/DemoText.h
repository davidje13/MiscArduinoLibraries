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

#ifndef DEMOTEXT_H_INCLUDED
#define DEMOTEXT_H_INCLUDED

#include <Bitmask18.h>
#include <Font.h>
#include <FontVariable.h>
#include <FontRenderer.h>

template <typename Display>
void demoText(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;

	Font font(FONTVARIABLE_DATA, FONTVARIABLE_IMG, FONTVARIABLE_MASK);
	auto r = MakeFontRenderer(&bitmask, 0, 0);
	auto message = ProgMemString(
		"Hello LED array!"
	);
	int16_t fullWidth = int16_t(font.measure(message));

	for(int16_t x = display.width(); x >= -fullWidth; -- x) {
		bitmask.clear();
		r.move(x, 0);
		r.print(font, message);
		display.send(bitmask);
	}

	delay(200);
}

#endif

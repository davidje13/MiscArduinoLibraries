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

#ifndef TEXTFIELD_INCLUDED
#define TEXTFIELD_INCLUDED

#include "UpArrow.h"
#include "DownArrow.h"

#include <BlendMode.h>
#include <FontRenderer.h>

template <typename CharT>
class TextField {
	char *target;
	uint8_t length;
	CharT chars;
	uint8_t charCount;
	uint8_t pos;
	int8_t posFraction;
	int8_t spinFraction;
	int8_t spinLean;

public:
	void reset_fractions(void) {
		posFraction = 0;
		spinFraction = 0;
		spinLean = 0;
	}

	void navigate(int16_t delta) {
		pos = ext::min2(ext::max2(pos + delta, 0), int16_t(length - 1));
	}

	void set_instantaneous_navigate_fraction(int8_t fraction) {
		posFraction = fraction;
	}

	template <typename ScrollerT>
	[[gnu::always_inline]]
	inline void navigate(ScrollerT &scroller, bool invert = false) {
		int8_t dir = (invert ? -1 : 1);
		navigate(scroller.delta() * dir);
		set_instantaneous_navigate_fraction(scroller.fraction() * dir);
	}

	void spin(int16_t delta) {
		target[pos] = ext::posmod(target[pos] + delta, int16_t(charCount));
	}

	void set_instantaneous_spin_fraction(int8_t fraction) {
		spinFraction = fraction;
	}

	void set_instantaneous_spin_lean(int8_t lean) {
		spinLean = lean;
	}

	template <typename ScrollerT>
	[[gnu::always_inline]]
	inline void spin(ScrollerT &scroller, bool invert = false) {
		int8_t dir = (invert ? -1 : 1);
		spin(scroller.delta() * dir);
		set_instantaneous_spin_fraction(scroller.fraction() * dir);
		set_instantaneous_spin_lean(scroller.lean() * dir);
	}

	template <typename Bitmask, typename Font>
	void render(
		Bitmask &bitmask,
		const Font &font,
		const bool showArrows,
		const uint8_t x,
		const uint8_t y,
		const uint8_t cw = 9,
		const uint8_t ch = 8,
		const uint8_t gap = 0,
		const uint8_t boxPadX = 0,
		const uint8_t boxPadY = 3,
		const uint8_t arrowMarginY = 2
	) {
		auto r = MakeFontRenderer(&bitmask, 0, 0);

		uint8_t px = x;
		for(uint8_t i = 0; i < length; ++ i) {
			char c = chars[target[i]];
			uint8_t w = font.measure(c);
			uint8_t py = y;
			if(i == pos) {
				py -= spinFraction;
			}
			r.move(px + (cw - w) / 2, py);
			r.print(font, c);
			px += cw + gap;
		}

		uint8_t cursorX = x + pos * (cw + gap);

		bitmask.fill_rect(
			cursorX + posFraction - boxPadX,
			y - boxPadY,
			cw + boxPadX * 2,
			ch + boxPadY * 2,
			BlendMode::XOR
		);

		if(showArrows) {
			bitmask.render_bitmap(
				UP_ARROW,
				UP_ARROW,
				cursorX + (cw - UP_ARROW_WIDTH) / 2,
				y - UP_ARROW_HEIGHT - boxPadY - arrowMarginY + ext::min2(spinLean, int8_t(0)),
				UP_ARROW_WIDTH,
				UP_ARROW_HEIGHT,
				UP_ARROW_STEP
			);

			bitmask.render_bitmap(
				DOWN_ARROW,
				DOWN_ARROW,
				cursorX + (cw - DOWN_ARROW_WIDTH) / 2,
				y + ch + boxPadY + arrowMarginY + ext::max2(spinLean, int8_t(0)),
				DOWN_ARROW_WIDTH,
				DOWN_ARROW_HEIGHT,
				DOWN_ARROW_STEP
			);
		}
	}

	TextField(char *target, uint8_t length, CharT chars)
		: target(target)
		, length(length)
		, chars(chars)
		, charCount(0)
		, pos(0)
		, posFraction(0)
		, spinFraction(0)
		, spinLean(0)
	{
		while(chars[charCount]) {
			++ charCount;
		}
	}
};

template <typename CharT>
[[gnu::always_inline,nodiscard]]
inline TextField<CharT> MakeTextField(
	char *target,
	uint8_t length,
	CharT chars
) {
	return TextField<CharT>(target, length, chars);
}

#endif


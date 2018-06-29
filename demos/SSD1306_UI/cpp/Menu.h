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

#ifndef MENU_INCLUDED
#define MENU_INCLUDED

#include <BlendMode.h>
#include <FontRenderer.h>

template <typename ItemsT>
class Menu {
	ItemsT items;
	int16_t pos;
	int8_t scrollFraction;
	int16_t smoothY;

public:
	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline int16_t index(void) const {
		return pos;
	}

	void jump(int16_t newPos) {
		pos = newPos;
	}

	void scroll(int16_t delta) {
		if(delta <= 0) {
			pos = ext::max2(pos + delta, 0);
		} else {
			while(delta && items(pos + 1)) {
				++ pos;
				-- delta;
			}
		}
	}

	void set_instantaneous_scroll_fraction(int8_t fraction) {
		scrollFraction = fraction;
	}

	template <typename ScrollerT>
	[[gnu::always_inline]]
	inline void scroll(ScrollerT &scroller, bool invert = false) {
		int8_t dir = (invert ? -1 : 1);
		scroll(scroller.delta() * dir);
		set_instantaneous_scroll_fraction(scroller.fraction() * dir);
	}

	template <typename Bitmask, typename Font>
	void render(
		Bitmask &bitmask,
		const Font &font,
		const uint8_t x,
		const uint8_t y,
		const uint8_t w,
		const uint8_t h = 8,
		const uint8_t gap = 6,
		const uint8_t barPadX = 5,
		const uint8_t barPadY = 3
	) {
		const uint8_t lh = h + gap;
		const int instantY = pos * lh;
		if(instantY > smoothY - 5 && instantY < smoothY + 5) {
			smoothY = instantY;
		} else {
			smoothY = (smoothY + instantY * 2) / 3;
		}
		const int16_t py0 = y - smoothY - scrollFraction;

		auto r = MakeFontRenderer(&bitmask, 0, 0);
		int16_t firstVis = -(py0 + h) / lh;
		int16_t py = py0 + lh * firstVis;
		for(int16_t i = firstVis; py < bitmask.height(); ++ i, py += lh) {
			auto label = items(i);
			if(label) {
				r.move(x, py);
				r.print(font, label);
			}
		}

		bitmask.fill_rect(
			x - barPadX, y - barPadY, w + barPadX * 2, h + barPadY * 2,
			BlendMode::XOR
		);
	}

	Menu(ItemsT items)
		: items(items)
		, pos(0)
		, scrollFraction(0)
		, smoothY(0)
	{}
};

template <typename ItemsT>
[[gnu::always_inline,nodiscard]]
inline Menu<ItemsT> MakeMenu(ItemsT items) {
	return Menu<ItemsT>(items);
}

#endif


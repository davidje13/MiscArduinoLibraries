/*
 * Written in 2017 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#include "BlendMode.h"

#include <ProgMem.h>

#include "ext.h"

#define FONT_HEADER_SIZE 5

class Font {
	ProgMem<uint8_t> data;
	ProgMem<uint8_t> image;
	ProgMem<uint8_t> mask;
	uint8_t tabsize;
	uint8_t space;
	uint8_t h;
	uint8_t lh;
	uint8_t asciiStart;
	uint8_t asciiEnd;
	uint8_t layoutMode;
	int16_t step;
	BlendMode mode;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t char_pos(uint8_t c) const {
		return c - asciiStart;
	}

public:
//	[[gnu::nonnull(1)]]
	Font(
		ProgMem<uint8_t> d,
		ProgMem<uint8_t> img, // nullable
		ProgMem<uint8_t> msk, // nullable
		BlendMode m = BlendMode::ON
	)
		: data(d + FONT_HEADER_SIZE)
		, image(img)
		, mask(msk)
		, tabsize(10)
		, space(1)
		, h(d[0])
		, lh(d[1])
		, asciiStart(d[2])
		, asciiEnd(d[3])
		, layoutMode(d[4])
		, step(0)
		, mode(m)
	{
		switch(layoutMode) {
		case 0x00:
			for(uint16_t i = 0, e = asciiEnd - asciiStart; i <= e; ++ i) {
				step += data[i];
			}
			break;
		case 0x01:
			step = data[0];
			break;
		case 0x02:
			step = data[0] * data[1];
			break;
		default:
			__builtin_unreachable();
		}
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool char_supported(uint8_t c) const {
		return c >= asciiStart && c <= asciiEnd;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t first_supported_char(void) const {
		return asciiStart;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t last_supported_char(void) const {
		return asciiEnd;
	}

	[[gnu::pure,nodiscard]]
	uint8_t measure(uint8_t c) const {
		if(!char_supported(c)) {
			return 0;
		}
		switch(layoutMode) {
		case 0x00:
			return data[char_pos(c)];
		case 0x01:
			return uint8_t(step);
		case 0x02:
			return data[0];
		default:
			__builtin_unreachable();
			return 0;
		}
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t measure(char c) const {
		return measure(uint8_t(c));
	}

	template <typename T>
	[[gnu::pure,nodiscard]]
	uint16_t measure(T message) const {
		if(!message) {
			return 0;
		}

		uint16_t w = 0;
		uint8_t c;
		for(T p = message; (c = uint8_t(p[0])) != 0; p = p + 1) {
			if(char_supported(c)) {
				w += measure(c) + spacing();
			}
		}
		return uint16_t(w - (w > 0 ? spacing() : 0));
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t spacing(void) const {
		return space;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t height(void) const {
		return h;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t line_height(void) const {
		return lh;
	}

	[[gnu::pure,nodiscard]]
	inline int16_t next_tabstop(int16_t x) const {
		return (x / tabsize + 1) * tabsize;
	}

	template <typename Bitmask>
	uint8_t render(
		Bitmask &bitmask,
		uint8_t c,
		int16_t x,
		int16_t y
	) const {
		if(!char_supported(c)) {
			return 0;
		}
		uint8_t l = char_pos(c);
		uint8_t w = measure(c);
		uint16_t p;
		switch(layoutMode) {
		case 0x00:
			p = 0;
			for(uint8_t i = 0; i < l; ++ i) {
				p += data[i];
			}
			break;
		case 0x01:
			p = l * w * ((h + 7) >> 3);
			break;
		case 0x02:
			p = l * w;
			p = uint16_t((p / step) * step * ((h + 7) >> 3) + (p % step));
			break;
		default:
			__builtin_unreachable();
		}
		bitmask.render_bitmap(
			safe_subdata(image, p),
			safe_subdata(mask, p),
			x, y, w, h, step, mode
		);
		return w;
	}

	template <typename Bitmask>
	[[gnu::always_inline]]
	inline uint8_t render(
		Bitmask &bitmask,
		char c,
		int16_t x,
		int16_t y
	) const {
		return render(bitmask, uint8_t(c), x, y);
	}

	template <typename Bitmask, typename T>
	uint16_t render(
		Bitmask &bitmask,
		T message,
		int16_t x,
		int16_t y
	) const {
		if(!message) {
			return 0;
		}

		uint16_t w = 0;
		uint8_t c;
		for(T p = message; (c = uint8_t(p[0])) != 0; p = p + 1) {
			if(char_supported(c)) {
				w += render(bitmask, c, x + w, y) + spacing();
			}
		}
		return uint16_t(w - (w > 0 ? spacing() : 0));
	}
};

#endif

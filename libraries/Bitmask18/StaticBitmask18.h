/*
 * StaticBitmask18 - Immutable bitmask storage.
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

#ifndef STATICBITMASK18_H_INCLUDED
#define STATICBITMASK18_H_INCLUDED

#include "ext.h"

template <typename T> // T = ProgMem<uint8_t> / const uint8_t*
class StaticBitmask18 {
	T d;
	int16_t s;
	uint16_t w;
	uint16_t h;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool in_bounds(int16_t x, int16_t y) const {
		return x >= 0 && y >= 0 && uint16_t(x) < w && uint16_t(y) < h;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint16_t pixel_byte(uint16_t x, uint16_t y) const {
		return (y >> 3) * s + x;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t pixel_bit(uint8_t y) {
		return y & 7;
	}

public:
	[[gnu::always_inline]]
	inline StaticBitmask18(
		T data,
		uint16_t width,
		uint16_t height,
		int16_t step
	)
		: d(data)
		, s(step)
		, w(width)
		, h(height)
	{}

	[[gnu::always_inline]]
	inline StaticBitmask18(T data, uint16_t width, uint16_t height)
		: d(data)
		, s(width)
		, w(width)
		, h(height)
	{}

	[[gnu::always_inline]]
	inline StaticBitmask18(const StaticBitmask18&) = default;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint16_t width(void) const {
		return w;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint16_t height(void) const {
		return h;
	}

	[[gnu::pure,nodiscard]]
	inline bool get_pixel(int16_t x, int16_t y) const {
		if(!in_bounds(x, y)) {
			return false;
		}
		return (d[pixel_byte(x, y)] >> pixel_bit(y)) & 1;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline T raw(void) const {
		return d;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline int16_t raw_step(void) const {
		return s;
	}
};

template <typename T>
[[gnu::always_inline,nodiscard]]
static inline StaticBitmask18<T> MakeStaticBitmask18(
	T data,
	uint16_t width,
	uint16_t height,
	int16_t step
) {
	return StaticBitmask18<T>(data, width, height, step);
}

template <typename T>
[[gnu::always_inline,nodiscard]]
static inline StaticBitmask18<T> MakeStaticBitmask18(
	T data,
	uint16_t width,
	uint16_t height
) {
	return StaticBitmask18<T>(data, width, height);
}

#endif

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

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

template <typename T> // T = ProgMem<uint8_t> / const uint8*
class StaticBitmask18 {
	T d;
	int s;
	uint8_t w;
	uint8_t h;

public:
	[[gnu::always_inline]]
	inline StaticBitmask18(T data, uint8_t width, uint8_t height, int step)
		: d(data)
		, s(step)
		, w(width)
		, h(height)
	{}

	[[gnu::always_inline]]
	inline StaticBitmask18(T data, uint8_t width, uint8_t height)
		: d(data)
		, s(width)
		, w(width)
		, h(height)
	{}

	[[gnu::always_inline]]
	inline StaticBitmask18(const StaticBitmask18&) = default;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t width(void) const {
		return w;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t height(void) const {
		return h;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline T raw(void) const {
		return d;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline int raw_step(void) const {
		return s;
	}
};

template <typename T>
[[gnu::always_inline]]
static inline StaticBitmask18<T> WrapStaticBitmask18(T data, uint8_t width, uint8_t height, int step) {
	return StaticBitmask18<T>(data, width, height, step);
}

template <typename T>
[[gnu::always_inline]]
static inline StaticBitmask18<T> WrapStaticBitmask18(T data, uint8_t width, uint8_t height) {
	return StaticBitmask18<T>(data, width, height);
}

#endif

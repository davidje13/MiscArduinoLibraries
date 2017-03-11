/*
 * SolidFill - Infinite array which always returns the same value.
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

#ifndef SOLIDFILL_H_INCLUDED
#define SOLIDFILL_H_INCLUDED

#include "ext.h"

class SolidFill {
	const uint8_t v;

public:
	[[gnu::always_inline]]
	inline SolidFill(void) : v(0) {}

	[[gnu::always_inline]]
	inline explicit SolidFill(uint8_t value) : v(value) {}

	[[gnu::always_inline]]
	inline SolidFill(bool value) : v(value ? 0xFF : 0x00) {}

	[[gnu::always_inline]]
	inline SolidFill(const SolidFill&) = default;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t operator[](int) const {
		return v;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	inline operator bool(void) const {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	inline SolidFill operator+(int) const {
		return *this;
	}
};

#endif

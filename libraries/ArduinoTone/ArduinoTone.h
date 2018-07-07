/*
 * ArduinoTone - Wrapper of standard Arduino Tone access.
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

#ifndef ARDUINOTONE_H_INCLUDED
#define ARDUINOTONE_H_INCLUDED

#include "ext.h"

template <uint8_t p>
class ArduinoTone {
public:
	[[gnu::always_inline]]
	inline void play(uint16_t hz) {
		tone(p, hz);
	}

	[[gnu::always_inline]]
	inline void play(uint16_t hz, uint32_t durationMillis) {
		tone(p, hz, durationMillis);
	}

	[[gnu::always_inline]]
	inline void stop(void) {
		noTone(p);
	}
};

class DynamicArduinoTone {
	uint8_t p;

public:
	[[gnu::always_inline]]
	inline void play(uint16_t hz) {
		tone(p, hz);
	}

	[[gnu::always_inline]]
	inline void play(uint16_t hz, uint32_t durationMillis) {
		tone(p, hz, durationMillis);
	}

	[[gnu::always_inline]]
	inline void stop(void) {
		noTone(p);
	}

	inline constexpr DynamicArduinoTone(uint8_t pin) : p(pin) {}
};

#endif

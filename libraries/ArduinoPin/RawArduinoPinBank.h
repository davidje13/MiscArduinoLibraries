/*
 * RawArduinoPinBank - Zero-overhead wrapper of standard pin bank access.
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

#ifndef RAWARDUINOPINBANK_H_INCLUDED
#define RAWARDUINOPINBANK_H_INCLUDED

#include "ext.h"

template <
	uint8_t port,
	uint8_t start = 0,
	uint8_t end = 8,
	uint8_t mask = ((1 << end) - 1) ^ ((1 << start) - 1)
>
class RawArduinoPinBank {
	volatile uint8_t *outvar;
	volatile uint8_t *invar;

public:
	[[gnu::always_inline]]
	static constexpr inline uint8_t size(void) {
		return end - start;
	}

	[[gnu::always_inline]]
	inline RawArduinoPinBank(void)
		: outvar(portOutputRegister(port))
		, invar(portInputRegister(port))
	{}

	[[gnu::always_inline]]
	inline void set_fast(uint8_t value) {
		if(mask == 0xFF) {
			*outvar = value;
		} else {
			*outvar = ((*outvar) & ~mask) | ((value << start) & mask);
		}
	}

	[[gnu::always_inline]]
	inline void set(uint8_t value) {
		uint8_t oldSREG = SREG;
		cli();
		set_fast(value);
		SREG = oldSREG;
	}

	[[gnu::always_inline]]
	inline void set_input(bool pullup = false) {
		volatile uint8_t *mode = portModeRegister(port);

		uint8_t oldSREG = SREG;
		cli();
		set_fast(mask * pullup);
		if(mask == 0xFF) {
			*mode = 0x00;
		} else {
			*mode &= ~mask;
		}
		SREG = oldSREG;
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		volatile uint8_t *mode = portModeRegister(port);

		uint8_t oldSREG = SREG;
		cli();
		if(mask == 0xFF) {
			*mode = 0xFF;
		} else {
			*mode |= mask;
		}
		SREG = oldSREG;
	}

	[[nodiscard,gnu::always_inline]]
	inline uint8_t get(void) const {
		if(mask == 0xFF) {
			return *invar;
		} else {
			return (*invar & mask) >> start;
		}
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
	}

	[[gnu::always_inline]]
	inline void send_batch(void) {
	}

	[[nodiscard,gnu::always_inline]]
	inline RawArduinoPinBank fast(void) {
		return *this;
	}
};

#endif

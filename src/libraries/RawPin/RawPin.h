/*
 * RawPin - Fast pin access for Arduino.
 * Written in 2017 by David Evans
 *
 * Pre-calculated, fast pin modifications. Uses more memory than storing the
 * pin number, but allows much faster changes to values. Note that interrupts
 * are not blocked while changing, which can be problematic when interupts are
 * used. These should be blocked manually if necessary.
 *
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef RAWPIN_H_INCLUDED
#define RAWPIN_H_INCLUDED

enum RawPinMode : uint8_t {
	RAWPIN_INPUT = 0,
	RAWPIN_OUTPUT = 1
};

class RawPin {
	volatile uint8_t *outvar;
	uint8_t port;
	uint8_t pinmask;

public:
	[[gnu::always_inline]]
	inline RawPin(uint8_t pin)
		: outvar(portOutputRegister(digitalPinToPort(pin)))
		, port(digitalPinToPort(pin))
		, pinmask(digitalPinToBitMask(pin))
	{}

	[[gnu::always_inline]]
	inline RawPin(uint8_t pin, RawPinMode mode, bool pullup = false)
		: RawPin(pin)
	{
		configure(mode, pullup);
	}

	[[gnu::always_inline]]
	inline RawPin(uint8_t pin, RawPinMode mode, int state) : RawPin(pin) {
		configure(mode, state != LOW);
	}

	[[gnu::always_inline]]
	inline void high(void) const {
		*outvar |= pinmask;
	}

	[[gnu::always_inline]]
	inline void low(void) const {
		*outvar &= ~pinmask;
	}

	void configure(RawPinMode mode, bool pullup) const {
		volatile uint8_t *mode = portModeRegister(port);

		uint8_t oldSREG = SREG;
		cli(); // Block interrupts

		*outvar = ((*outvar) & ~pinmask) | (pinmask * pullup);
		*mode = ((*mode) & ~pinmask) | (pinmask * mode);

		SREG = oldSREG; // Restore interrupts
	}
};

#endif

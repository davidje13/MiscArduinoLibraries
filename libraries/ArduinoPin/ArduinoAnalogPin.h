/*
 * ArduinoAnalogPin - Zero-overhead wrapper of standard Arduino pin access.
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

#ifndef ARDUINOANALOGPIN_H_INCLUDED
#define ARDUINOANALOGPIN_H_INCLUDED

#include "ext.h"

class ArduinoAnalogPin {
	uint8_t p;

public:
	[[gnu::always_inline]]
	inline ArduinoAnalogPin(uint8_t pin) : p(pin) {}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool exists(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_input(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_analog_input(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_output(void) {
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_pwm_output(void) {
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_interrupts(void) {
		return false;
	}

	[[gnu::always_inline]]
	inline void set_input(bool = false) {
		// analog pins are always input
	}

	[[gnu::always_inline]]
	inline uint16_t read_analog(void) const {
		return analogRead(p);
	}

	[[gnu::always_inline]]
	inline uint32_t read_analog(uint32_t range) const {
		return (read_analog() * range) / 1024;
	}

	[[gnu::always_inline]]
	inline bool read_digital(void) const {
		return read_analog() >= 512;
	}
};

template <uint8_t p>
class FixedArduinoAnalogPin {
public:
	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool exists(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_input(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_analog_input(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_output(void) {
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_pwm_output(void) {
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_interrupts(void) {
		return false;
	}

	[[gnu::always_inline]]
	inline void set_input(bool = false) {
		// analog pins are always input
	}

	[[gnu::always_inline]]
	inline uint16_t read_analog(void) const {
		return analogRead(p);
	}

	[[gnu::always_inline]]
	inline uint32_t read_analog(uint32_t range) const {
		return (read_analog() * range) / 1024;
	}

	[[gnu::always_inline]]
	inline bool read_digital(void) const {
		return read_analog() >= 512;
	}
};

#endif

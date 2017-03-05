/*
 * VoidPin - A pin which doesn't exist.
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

#ifndef VOIDPIN_H_INCLUDED
#define VOIDPIN_H_INCLUDED

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

class VoidPin {
public:
	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool exists(void) {
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_input(void) {
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_analog_input(void) {
		return false;
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
	inline void high(void) {
	}

	[[gnu::always_inline]]
	inline void low(void) {
	}

	[[gnu::always_inline]]
	inline void pwm(uint8_t) {
	}

	[[gnu::always_inline]]
	inline void set_input(bool = false) {
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
	}

	[[gnu::always_inline]]
	inline uint16_t read_analog(void) const {
		return 0;
	}

	[[gnu::always_inline]]
	inline uint32_t read_analog(uint32_t) const {
		return 0;
	}

	[[nodiscard,gnu::always_inline]]
	inline bool read_digital(void) const {
		return false;
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_low(void (*)(void)) {
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_rising(void (*)(void)) {
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_falling(void (*)(void)) {
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_change(void (*)(void)) {
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_high(void (*)(void)) {
	}

	[[gnu::always_inline]]
	inline void remove_interrupt(void) {
	}
};

#endif
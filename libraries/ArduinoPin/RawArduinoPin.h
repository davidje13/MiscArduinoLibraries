/*
 * RawArduinoPin - Fast pin access for Arduino.
 * Written in 2017 by David Evans
 *
 * Pre-calculated, fast pin modifications. Uses more memory than storing the
 * pin number, but allows much faster changes to values.
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

#ifndef RAWARDUINOPIN_H_INCLUDED
#define RAWARDUINOPIN_H_INCLUDED

#include "ext.h"

class RawArduinoPin {
	volatile uint8_t *outvar;
	uint8_t p;
	uint8_t pinmask;

public:
	[[gnu::always_inline]]
	inline RawArduinoPin(uint8_t pin)
		: outvar(portOutputRegister(digitalPinToPort(pin)))
		, p(pin)
		, pinmask(digitalPinToBitMask(pin))
	{}

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
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_output(void) {
		return true;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool supports_pwm_output(void) const {
		return digitalPinToTimer(p) != NOT_ON_TIMER;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool supports_interrupts(void) const {
		return digitalPinToInterrupt(p) != NOT_AN_INTERRUPT;
	}

	[[gnu::always_inline]]
	inline void high_fast(void) {
		*outvar |= pinmask;
	}

	[[gnu::always_inline]]
	inline void low_fast(void) {
		*outvar &= ~pinmask;
	}

	[[gnu::always_inline]]
	inline void set_fast(uint8_t high) {
		*outvar = ((*outvar) & ~pinmask) | (pinmask * high);
	}

	[[gnu::always_inline]]
	inline void high(void) {
		uint8_t oldSREG = SREG;
		cli(); // Block interrupts
		high_fast();
		SREG = oldSREG; // Restore interrupts
	}

	[[gnu::always_inline]]
	inline void low(void) {
		uint8_t oldSREG = SREG;
		cli();
		low_fast();
		SREG = oldSREG;
	}

	[[gnu::always_inline]]
	inline void set(bool high) {
		uint8_t oldSREG = SREG;
		cli();
		set_fast(high);
		SREG = oldSREG;
	}

	[[gnu::always_inline]]
	inline void pwm(uint8_t value) {
		// Too many hardware-specific cases for us to write our own; use the
		// built-in method
		analogWrite(p, value);
	}

	[[gnu::always_inline]]
	inline void set_input(bool pullup = false) {
		volatile uint8_t *mode = portModeRegister(digitalPinToPort(p));

		uint8_t oldSREG = SREG;
		cli();
		set_fast(pullup);
		*mode = (*mode) & ~pinmask;
		SREG = oldSREG;
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		volatile uint8_t *mode = portModeRegister(digitalPinToPort(p));

		uint8_t oldSREG = SREG;
		cli();
		*mode = (*mode) | pinmask;
		SREG = oldSREG;
	}

	[[nodiscard,gnu::always_inline]]
	inline bool read_digital(void) const {
		return ((*portInputRegister(digitalPinToPort(p))) & pinmask) != 0;
	}

	[[nodiscard,gnu::always_inline]]
	inline int16_t read_analog(void) const {
		return read_digital() ? 1023 : 0;
	}

	[[nodiscard,gnu::always_inline]]
	inline int32_t read_analog(int32_t range) const {
		if(range <= 1) {
			return 0;
		}
		return read_digital() ? (range - 1) : 0;
	}

	[[nodiscard,gnu::always_inline]]
	inline uint32_t measure_high_pulse(uint32_t timeout = 1000000) const {
		return pulseIn(p, HIGH, timeout);
	}

	[[nodiscard,gnu::always_inline]]
	inline uint32_t measure_low_pulse(uint32_t timeout = 1000000) const {
		return pulseIn(p, LOW, timeout);
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_low(void (*callback)(void)) {
		attachInterrupt(digitalPinToInterrupt(p), callback, LOW);
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_rising(void (*callback)(void)) {
		attachInterrupt(digitalPinToInterrupt(p), callback, RISING);
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_falling(void (*callback)(void)) {
		attachInterrupt(digitalPinToInterrupt(p), callback, FALLING);
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_change(void (*callback)(void)) {
		attachInterrupt(digitalPinToInterrupt(p), callback, CHANGE);
	}

#if CHANGE != HIGH
	// Only supported on Due, Zero, MKR1000
	[[gnu::always_inline]]
	inline void set_interrupt_on_high(void (*callback)(void)) {
		attachInterrupt(digitalPinToInterrupt(p), callback, HIGH);
	}
#endif

	[[gnu::always_inline]]
	inline void remove_interrupt(void) {
		detachInterrupt(digitalPinToInterrupt(p));
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
	}

	[[gnu::always_inline]]
	inline void send_batch(void) {
	}

	[[nodiscard,gnu::always_inline]]
	inline RawArduinoPin fast(void) {
		return *this;
	}
};

#endif

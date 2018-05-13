/*
 * ArduinoPin - Zero-overhead wrapper of standard Arduino pin access.
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

#ifndef ARDUINOPIN_H_INCLUDED
#define ARDUINOPIN_H_INCLUDED

#include "ext.h"
#include "RawArduinoPin.h"

class ArduinoPin {
	uint8_t p;

public:
	[[gnu::always_inline]]
	inline ArduinoPin(uint8_t pin) : p(pin) {}

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
	inline void high(void) {
		digitalWrite(p, HIGH);
	}

	[[gnu::always_inline]]
	inline void low(void) {
		digitalWrite(p, LOW);
	}

	[[gnu::always_inline]]
	inline void set(bool high) {
		digitalWrite(p, high ? HIGH : LOW);
	}

	[[gnu::always_inline]]
	inline void pwm(uint8_t value) {
		analogWrite(p, value);
	}

	[[gnu::always_inline]]
	inline void set_input(bool pullup = false) {
		pinMode(p, pullup ? INPUT_PULLUP : INPUT);
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		pinMode(p, OUTPUT);
	}

	[[nodiscard,gnu::always_inline]]
	inline bool read_digital(void) const {
		return digitalRead(p) != LOW;
	}

	[[nodiscard,gnu::always_inline]]
	inline uint8_t get(void) const {
		return digitalRead(p);
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
		return RawArduinoPin(p);
	}
};

template <uint8_t p>
class FixedArduinoPin {
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
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_output(void) {
		return true;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	static inline bool supports_pwm_output(void) {
		// digitalPinToTimer uses a lookup table, so this cannot be constexpr
		return digitalPinToTimer(p) != NOT_ON_TIMER;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr bool supports_interrupts(void) {
		return digitalPinToInterrupt(p) != NOT_AN_INTERRUPT;
	}

	[[gnu::always_inline]]
	inline void high(void) {
		digitalWrite(p, HIGH);
	}

	[[gnu::always_inline]]
	inline void low(void) {
		digitalWrite(p, LOW);
	}

	[[gnu::always_inline]]
	inline void set(bool high) {
		digitalWrite(p, high ? HIGH : LOW);
	}

	[[gnu::always_inline]]
	inline void pwm(uint8_t value) {
		analogWrite(p, value);
	}

	[[gnu::always_inline]]
	inline void set_input(bool pullup = false) {
		pinMode(p, pullup ? INPUT_PULLUP : INPUT);
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		pinMode(p, OUTPUT);
	}

	[[nodiscard,gnu::always_inline]]
	inline bool read_digital(void) const {
		return digitalRead(p) != LOW;
	}

	[[nodiscard,gnu::always_inline]]
	inline uint8_t get(void) const {
		return digitalRead(p);
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
		return RawArduinoPin(p);
	}
};

#endif

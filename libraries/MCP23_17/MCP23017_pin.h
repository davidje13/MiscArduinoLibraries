/*
 * MCP23017_pin - Pin compatible API for MCP23017.
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

#ifndef MCP23017_PIN_H_INCLUDED
#define MCP23017_PIN_H_INCLUDED

#include "ext.h"

template <typename T>
class MCP23017_pin {
	T *device;
	typename T::Pin port;

public:
	MCP23017_pin(void) = delete;
	constexpr MCP23017_pin(T *device, typename T::Pin port)
		: device(device)
		, port(port)
	{}

	MCP23017_pin(const MCP23017_pin&) = default;
	MCP23017_pin(MCP23017_pin&&) = default;

	MCP23017_pin &operator=(const MCP23017_pin&) = default;
	MCP23017_pin &operator=(MCP23017_pin&&) = default;

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool exists(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool supports_input(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool supports_analog_input(void) {
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool supports_output(void) {
		return true;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool supports_pwm_output(void) const {
		return false;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool supports_interrupts(void) const {
		return false; // TODO
	}

	[[gnu::always_inline]]
	inline void high(void) {
		device->set_port_high(port);
	}

	[[gnu::always_inline]]
	inline void low(void) {
		device->set_port_low(port);
	}

	[[gnu::always_inline]]
	inline void set(bool high) {
		device->set_port(port, high);
	}

	[[gnu::always_inline]]
	inline void pwm(uint8_t value) {
		if(value >= 128) {
			device->set_port_high(port);
		} else {
			device->set_port_low(port);
		}
	}

	[[gnu::always_inline]]
	inline void set_input(bool pullup = false) {
		device->set_input(port, pullup);
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		device->set_output(port);
	}

	[[nodiscard,gnu::always_inline]]
	inline bool read_digital(void) const {
		return device->get_port(port);
	}

	[[nodiscard,gnu::always_inline]]
	inline uint8_t get(void) const {
		return read_digital();
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
	inline uint32_t measure_high_pulse(uint32_t = 1000000) const {
		return 0; // TODO
	}

	[[nodiscard,gnu::always_inline]]
	inline uint32_t measure_low_pulse(uint32_t = 1000000) const {
		return 0; // TODO
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_rising(void (*callback)(void)) {
		// TODO
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_falling(void (*callback)(void)) {
		// TODO
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_change(void (*callback)(void)) {
		// TODO
	}

	[[gnu::always_inline]]
	inline void remove_interrupt(void) {
		// TODO
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
		device->begin_batch();
	}

	[[gnu::always_inline]]
	inline void send_batch(void) {
		device->send_batch();
	}

	MCP23017_pin fast(void) {
		return *this;
	}
};

template <typename T, typename T::Pin port>
class MCP23017_fixedpin {
	T *device;

public:
	MCP23017_fixedpin(void) = delete;
	constexpr MCP23017_fixedpin(T *device)
		: device(device)
	{}

	MCP23017_fixedpin(const MCP23017_fixedpin&) = default;
	MCP23017_fixedpin(MCP23017_fixedpin&&) = default;

	MCP23017_fixedpin &operator=(const MCP23017_fixedpin&) = default;
	MCP23017_fixedpin &operator=(MCP23017_fixedpin&&) = default;

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool exists(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool supports_input(void) {
		return true;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool supports_analog_input(void) {
		return false;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool supports_output(void) {
		return true;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool supports_pwm_output(void) const {
		return false;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool supports_interrupts(void) const {
		return false; // TODO
	}

	[[gnu::always_inline]]
	inline void high(void) {
		device->set_port_high(port);
	}

	[[gnu::always_inline]]
	inline void low(void) {
		device->set_port_low(port);
	}

	[[gnu::always_inline]]
	inline void set(bool high) {
		device->set_port(port, high);
	}

	[[gnu::always_inline]]
	inline void pwm(uint8_t value) {
		if(value >= 128) {
			device->set_port_high(port);
		} else {
			device->set_port_low(port);
		}
	}

	[[gnu::always_inline]]
	inline void set_input(bool pullup = false) {
		device->set_input(port, pullup);
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		device->set_output(port);
	}

	[[nodiscard,gnu::always_inline]]
	inline bool read_digital(void) const {
		return device->get_port(port);
	}

	[[nodiscard,gnu::always_inline]]
	inline uint8_t get(void) const {
		return read_digital();
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
	inline uint32_t measure_high_pulse(uint32_t = 1000000) const {
		return 0; // TODO
	}

	[[nodiscard,gnu::always_inline]]
	inline uint32_t measure_low_pulse(uint32_t = 1000000) const {
		return 0; // TODO
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_rising(void (*callback)(void)) {
		// TODO
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_falling(void (*callback)(void)) {
		// TODO
	}

	[[gnu::always_inline]]
	inline void set_interrupt_on_change(void (*callback)(void)) {
		// TODO
	}

	[[gnu::always_inline]]
	inline void remove_interrupt(void) {
		// TODO
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
		device->begin_batch();
	}

	[[gnu::always_inline]]
	inline void send_batch(void) {
		device->send_batch();
	}

	MCP23017_fixedpin fast(void) {
		return *this;
	}
};

#endif

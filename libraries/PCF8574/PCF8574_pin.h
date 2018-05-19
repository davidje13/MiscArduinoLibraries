/*
 * PCF8574_pin
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

#ifndef PCF8574_PIN_H_INCLUDED
#define PCF8574_PIN_H_INCLUDED

template <typename T>
class PCF8574_pin {
	T *device;
	uint8_t port;

public:
	PCF8574_pin(void) = delete;
	constexpr PCF8574_pin(T *device, uint8_t port)
		: device(device)
		, port(port)
	{}

	PCF8574_pin(const PCF8574_pin&) = default;
	PCF8574_pin(PCF8574_pin&&) = default;

	PCF8574_pin &operator=(const PCF8574_pin&) = default;
	PCF8574_pin &operator=(PCF8574_pin&&) = default;

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
	inline void set_input(bool = false) {
		device->set_input(port);
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		// no-op
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

	PCF8574_pin fast(void) {
		return *this;
	}
};

template <typename T, uint8_t port>
class PCF8574_fixedpin {
	T *device;

public:
	PCF8574_fixedpin(void) = delete;
	constexpr PCF8574_fixedpin(T *device)
		: device(device)
	{}

	PCF8574_fixedpin(const PCF8574_fixedpin&) = default;
	PCF8574_fixedpin(PCF8574_fixedpin&&) = default;

	PCF8574_fixedpin &operator=(const PCF8574_fixedpin&) = default;
	PCF8574_fixedpin &operator=(PCF8574_fixedpin&&) = default;

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
	inline void set_input(bool = false) {
		device->set_input(port);
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		// no-op
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

	PCF8574_fixedpin fast(void) {
		return *this;
	}
};

#endif

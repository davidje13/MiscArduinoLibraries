#ifndef MOCK_PIN_H_INCLUDED
#define MOCK_PIN_H_INCLUDED

#include "BaseMock.h"

#include <cstdint>

class Pin {
public:
	class Mock {
	public:
		bool exists_return_value = false;
		bool supports_input_return_value = false;
		bool supports_analog_input_return_value = false;
		bool supports_output_return_value = false;
		bool supports_pwm_output_return_value = false;
		bool supports_interrupts_return_value = false;
		uint8_t high_call_count = 0;
		uint8_t low_call_count = 0;
		uint8_t pwm_call_count = 0;
		uint8_t pwm_last_value = 0;
		uint8_t set_input_call_count = 0;
		bool set_input_last_pullup = false;
		uint8_t set_output_call_count = 0;
		uint16_t read_analog_return_value = 0;
		uint32_t read_analog_range_last_range = 0;
		uint32_t read_analog_range_return_value = 0;
		uint32_t measure_high_pulse_return_value = 0;
		uint32_t measure_high_pulse_last_timeout = 0;
		uint32_t measure_low_pulse_return_value = 0;
		uint32_t measure_low_pulse_last_timeout = 0;
		uint32_t begin_batch_call_count = 0;
		uint32_t send_batch_call_count = 0;
		bool read_digital_return_value = false;
		void (*set_interrupt_on_low_last_func)(void) = nullptr;
		void (*set_interrupt_on_rising_last_func)(void) = nullptr;
		void (*set_interrupt_on_falling_last_func)(void) = nullptr;
		void (*set_interrupt_on_change_last_func)(void) = nullptr;
		void (*set_interrupt_on_high_last_func)(void) = nullptr;
		uint8_t remove_interrupt_call_count = 0;

		void reset(void) {
			*this = Mock();
		}

		Pin implementation(void);
	};

private:
	Mock *mock;
	Pin(Mock *m) : mock(m) {}

	friend class Mock;

public:
	bool exists(void) const {
		return mock->exists_return_value;
	}

	bool supports_input(void) const {
		return mock->supports_input_return_value;
	}

	bool supports_analog_input(void) const {
		return mock->supports_analog_input_return_value;
	}

	bool supports_output(void) const {
		return mock->supports_output_return_value;
	}

	bool supports_pwm_output(void) const {
		return mock->supports_pwm_output_return_value;
	}

	bool supports_interrupts(void) const {
		return mock->supports_interrupts_return_value;
	}

	void high(void) {
		++ mock->high_call_count;
	}

	void low(void) {
		++ mock->low_call_count;
	}

	void set(bool high) {
		if(high) {
			++ mock->high_call_count;
		} else {
			++ mock->low_call_count;
		}
	}

	void pwm(uint8_t value) {
		++ mock->pwm_call_count;
		mock->pwm_last_value = value;
	}

	void set_input(bool pullup = false) {
		++ mock->set_input_call_count;
		mock->set_input_last_pullup = pullup;
	}

	void set_output(void) {
		++ mock->set_output_call_count;
	}

	uint16_t read_analog(void) const {
		return mock->read_analog_return_value;
	}

	uint32_t read_analog(uint32_t range) const {
		mock->read_analog_range_last_range = range;
		return mock->read_analog_range_return_value;
	}

	uint32_t measure_high_pulse(uint32_t timeout = 1000000) const {
		mock->measure_high_pulse_last_timeout = timeout;
		return mock->measure_high_pulse_return_value;
	}

	uint32_t measure_low_pulse(uint32_t timeout = 1000000) const {
		mock->measure_low_pulse_last_timeout = timeout;
		return mock->measure_low_pulse_return_value;
	}

	bool read_digital(void) const {
		return mock->read_digital_return_value;
	}

	inline uint8_t get(void) const {
		return read_digital();
	}

	void set_interrupt_on_low(void (*func)(void)) {
		mock->set_interrupt_on_low_last_func = func;
	}

	void set_interrupt_on_rising(void (*func)(void)) {
		mock->set_interrupt_on_rising_last_func = func;
	}

	void set_interrupt_on_falling(void (*func)(void)) {
		mock->set_interrupt_on_falling_last_func = func;
	}

	void set_interrupt_on_change(void (*func)(void)) {
		mock->set_interrupt_on_change_last_func = func;
	}

	void set_interrupt_on_high(void (*func)(void)) {
		mock->set_interrupt_on_high_last_func = func;
	}

	void remove_interrupt(void) {
		++ mock->remove_interrupt_call_count;
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
		++ mock->begin_batch_call_count;
	}

	[[gnu::always_inline]]
	inline void send_batch(void) {
		++ mock->send_batch_call_count;
	}
};

#include "EndMocks.h"

#endif

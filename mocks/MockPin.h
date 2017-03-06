#ifndef MOCK_PIN_H_INCLUDED
#define MOCK_PIN_H_INCLUDED

#include "BaseMock.h"

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
		bool read_digital_return_value = false;
		void (*set_interrupt_on_low_last_func)(void) = nullptr;
		void (*set_interrupt_on_rising_last_func)(void) = nullptr;
		void (*set_interrupt_on_falling_last_func)(void) = nullptr;
		void (*set_interrupt_on_change_last_func)(void) = nullptr;
		void (*set_interrupt_on_high_last_func)(void) = nullptr;
		uint8_t remove_interrupt_call_count = 0;

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

	bool read_digital(void) const {
		return mock->read_digital_return_value;
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
};

Pin Pin::Mock::implementation(void) {
	return Pin(this);
}

#endif

#include <boost/test/unit_test.hpp>

#include "arduino_mock.h"

static void voidFunc(void) {
}

template <int v>
struct ConstExprChecker { static constexpr bool value(void) { return true; } };

#include "../RawArduinoPin.h"

BOOST_AUTO_TEST_SUITE(RawArduinoPin_test)

BOOST_AUTO_TEST_CASE(exists_is_true) {
	BOOST_CHECK_EQUAL(RawArduinoPin(1).exists(), true);
}

BOOST_AUTO_TEST_CASE(exists_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<RawArduinoPin::exists()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_true) {
	BOOST_CHECK_EQUAL(RawArduinoPin(1).supports_input(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<RawArduinoPin::supports_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_false) {
	BOOST_CHECK_EQUAL(RawArduinoPin(1).supports_analog_input(), false);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<RawArduinoPin::supports_analog_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_output_is_true) {
	BOOST_CHECK_EQUAL(RawArduinoPin(1).supports_output(), true);
}

BOOST_AUTO_TEST_CASE(supports_output_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<RawArduinoPin::supports_output()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_pwm_output_checks_timer) {
	BOOST_CHECK_EQUAL(RawArduinoPin(1).supports_pwm_output(), false);
	BOOST_CHECK_EQUAL(RawArduinoPin(2).supports_pwm_output(), true);
	BOOST_CHECK_EQUAL(RawArduinoPin(5).supports_pwm_output(), false);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_checks_interrupts) {
	BOOST_CHECK_EQUAL(RawArduinoPin(1).supports_interrupts(), false);
	BOOST_CHECK_EQUAL(RawArduinoPin(2).supports_interrupts(), false);
	BOOST_CHECK_EQUAL(RawArduinoPin(5).supports_interrupts(), true);
}

BOOST_AUTO_TEST_CASE(high_sets_output_register_bit) {
	portOutputRegisterValues[digitalPinToPort(4)] = 0x02;
	RawArduinoPin(4).high();
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(4)], 0x12);
	RawArduinoPin(4).high(); // stays high
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(4)], 0x12);
}

BOOST_AUTO_TEST_CASE(low_clears_output_register_bit) {
	portOutputRegisterValues[digitalPinToPort(5)] = 0x31;
	RawArduinoPin(5).low();
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(5)], 0x11);
	RawArduinoPin(5).low(); // stays low
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(5)], 0x11);
}

BOOST_AUTO_TEST_CASE(high_disables_interrupts) {
	SREG = 123;
	cliCallCount = 0;
	RawArduinoPin(1).high();
	BOOST_CHECK_EQUAL(cliCallCount, 1);
	BOOST_CHECK_EQUAL(SREG, 123);
}

BOOST_AUTO_TEST_CASE(low_disables_interrupts) {
	SREG = 123;
	cliCallCount = 0;
	RawArduinoPin(1).low();
	BOOST_CHECK_EQUAL(cliCallCount, 1);
	BOOST_CHECK_EQUAL(SREG, 123);
}

BOOST_AUTO_TEST_CASE(high_fast_sets_output_register_bit_without_cli) {
	cliCallCount = 0;
	portOutputRegisterValues[digitalPinToPort(4)] = 0x02;
	RawArduinoPin(4).high_fast();
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(4)], 0x12);
	BOOST_CHECK_EQUAL(cliCallCount, 0);
}

BOOST_AUTO_TEST_CASE(low_fast_clears_output_register_bit_without_cli) {
	cliCallCount = 0;
	portOutputRegisterValues[digitalPinToPort(5)] = 0x31;
	RawArduinoPin(5).low_fast();
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(5)], 0x11);
	BOOST_CHECK_EQUAL(cliCallCount, 0);
}

BOOST_AUTO_TEST_CASE(pwm_calls_analogWrite) {
	RawArduinoPin(92).pwm(80);
	BOOST_CHECK_EQUAL(analogWriteLastPin, 92);
	BOOST_CHECK_EQUAL(analogWriteLastValue, 80);
}

BOOST_AUTO_TEST_CASE(set_input_sets_registers) {
	portOutputRegisterValues[digitalPinToPort(6)] = 0x46;
	portModeRegisterValues[digitalPinToPort(6)] = 0x41;
	RawArduinoPin(6).set_input();
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(6)], 0x06);
	BOOST_CHECK_EQUAL(portModeRegisterValues[digitalPinToPort(6)], 0x01);
}

BOOST_AUTO_TEST_CASE(set_input_true_sets_registers) {
	portOutputRegisterValues[digitalPinToPort(6)] = 0x06;
	portModeRegisterValues[digitalPinToPort(6)] = 0x41;
	RawArduinoPin(6).set_input(true);
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(6)], 0x46);
	BOOST_CHECK_EQUAL(portModeRegisterValues[digitalPinToPort(6)], 0x01);
}

BOOST_AUTO_TEST_CASE(set_input_false_sets_registers) {
	portOutputRegisterValues[digitalPinToPort(6)] = 0x46;
	portModeRegisterValues[digitalPinToPort(6)] = 0x41;
	RawArduinoPin(6).set_input(false);
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(6)], 0x06);
	BOOST_CHECK_EQUAL(portModeRegisterValues[digitalPinToPort(6)], 0x01);
}

BOOST_AUTO_TEST_CASE(set_output_sets_registers) {
	portOutputRegisterValues[digitalPinToPort(6)] = 0x46;
	portModeRegisterValues[digitalPinToPort(6)] = 0x01;
	RawArduinoPin(6).set_output();
	BOOST_CHECK_EQUAL(portOutputRegisterValues[digitalPinToPort(6)], 0x46);
	BOOST_CHECK_EQUAL(portModeRegisterValues[digitalPinToPort(6)], 0x41);
}

BOOST_AUTO_TEST_CASE(set_input_disables_interrupts) {
	SREG = 123;
	cliCallCount = 0;
	RawArduinoPin(1).set_input();
	BOOST_CHECK_EQUAL(cliCallCount, 1);
	BOOST_CHECK_EQUAL(SREG, 123);
}

BOOST_AUTO_TEST_CASE(set_output_disables_interrupts) {
	SREG = 123;
	cliCallCount = 0;
	RawArduinoPin(1).set_output();
	BOOST_CHECK_EQUAL(cliCallCount, 1);
	BOOST_CHECK_EQUAL(SREG, 123);
}

BOOST_AUTO_TEST_CASE(read_digital_checks_output_bit) {
	portOutputRegisterValues[digitalPinToPort(5)] = 0x31;
	BOOST_CHECK_EQUAL(RawArduinoPin(5).read_digital(), true);
	portOutputRegisterValues[digitalPinToPort(5)] = 0x11;
	BOOST_CHECK_EQUAL(RawArduinoPin(5).read_digital(), false);
}

BOOST_AUTO_TEST_CASE(read_analog_checks_output_bit) {
	portOutputRegisterValues[digitalPinToPort(5)] = 0x31;
	BOOST_CHECK_EQUAL(RawArduinoPin(5).read_analog(), 1023);
	portOutputRegisterValues[digitalPinToPort(5)] = 0x11;
	BOOST_CHECK_EQUAL(RawArduinoPin(5).read_analog(), 0);
}

BOOST_AUTO_TEST_CASE(read_analog_small_range_returns_0) {
	BOOST_CHECK_EQUAL(RawArduinoPin(5).read_analog(0), 0);
	BOOST_CHECK_EQUAL(RawArduinoPin(5).read_analog(1), 0);
}

BOOST_AUTO_TEST_CASE(read_analog_range_checks_output_bit) {
	portOutputRegisterValues[digitalPinToPort(5)] = 0x31;
	BOOST_CHECK_EQUAL(RawArduinoPin(5).read_analog(10), 9);
	portOutputRegisterValues[digitalPinToPort(5)] = 0x11;
	BOOST_CHECK_EQUAL(RawArduinoPin(5).read_analog(10), 0);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_low_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	RawArduinoPin(5).set_interrupt_on_low(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, LOW);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_rising_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	RawArduinoPin(5).set_interrupt_on_rising(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, RISING);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_falling_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	RawArduinoPin(5).set_interrupt_on_falling(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, FALLING);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_change_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	RawArduinoPin(5).set_interrupt_on_change(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, CHANGE);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_high_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	RawArduinoPin(5).set_interrupt_on_high(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, HIGH);
}

BOOST_AUTO_TEST_CASE(remove_interrupt_calls_detachInterrupt) {
	RawArduinoPin(5).remove_interrupt();
	BOOST_CHECK_EQUAL(detachInterruptLastInt, 3);
}

BOOST_AUTO_TEST_SUITE_END()

#include <boost/test/unit_test.hpp>

#include "MockArduinoPin.h"

template <int v>
struct ConstExprChecker { static constexpr bool value(void) { return true; } };

#include "../ArduinoAnalogPin.h"

BOOST_AUTO_TEST_SUITE(ArduinoAnalogPin_test)

BOOST_AUTO_TEST_CASE(exists_is_true) {
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).exists(), true);
}

BOOST_AUTO_TEST_CASE(exists_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoAnalogPin::exists()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_true) {
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).supports_input(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoAnalogPin::supports_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_true) {
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).supports_analog_input(), true);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoAnalogPin::supports_analog_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_output_is_false) {
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).supports_output(), false);
}

BOOST_AUTO_TEST_CASE(supports_output_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoAnalogPin::supports_output()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_pwm_output_is_false) {
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).supports_pwm_output(), false);
}

BOOST_AUTO_TEST_CASE(supports_pwm_output_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoAnalogPin::supports_pwm_output()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_is_false) {
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).supports_interrupts(), false);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoAnalogPin::supports_interrupts()>::value(), true);
}

BOOST_AUTO_TEST_CASE(set_input_does_nothing) {
	ArduinoAnalogPin(93).set_input();
}

BOOST_AUTO_TEST_CASE(set_input_true_does_nothing) {
	ArduinoAnalogPin(94).set_input(true);
}

BOOST_AUTO_TEST_CASE(set_input_false_does_nothing) {
	ArduinoAnalogPin(95).set_input(false);
}

BOOST_AUTO_TEST_CASE(read_analog_calls_analogRead) {
	analogReadReturnValue = 1000;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(99).read_analog(), 1000);
	BOOST_CHECK_EQUAL(analogReadLastPin, 99);

	analogReadReturnValue = 2;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(100).read_analog(), 2);
}

BOOST_AUTO_TEST_CASE(read_analog_range_calls_analogRead) {
	analogReadReturnValue = 0;
	ArduinoAnalogPin(102).read_analog(16);
	BOOST_CHECK_EQUAL(analogReadLastPin, 102);
}

BOOST_AUTO_TEST_CASE(read_analog_range_scales_result) {
	analogReadReturnValue = 0;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).read_analog(16), 0);

	analogReadReturnValue = 1023;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).read_analog(16), 15);

	analogReadReturnValue = 256;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).read_analog(16), 4);

	analogReadReturnValue = 512;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).read_analog(16), 8);
}

BOOST_AUTO_TEST_CASE(read_digital_calls_analogRead) {
	analogReadReturnValue = 0;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(103).read_digital(), false);
	BOOST_CHECK_EQUAL(analogReadLastPin, 103);

	analogReadReturnValue = 1023;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(104).read_digital(), true);
}

BOOST_AUTO_TEST_CASE(read_digital_thresholds_at_512) {
	analogReadReturnValue = 511;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).read_digital(), false);

	analogReadReturnValue = 512;
	BOOST_CHECK_EQUAL(ArduinoAnalogPin(1).read_digital(), true);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(FixedArduinoAnalogPin_test)

BOOST_AUTO_TEST_CASE(exists_is_true) {
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().exists(), true);
}

BOOST_AUTO_TEST_CASE(exists_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoAnalogPin<1>::exists()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_true) {
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().supports_input(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoAnalogPin<1>::supports_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_true) {
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().supports_analog_input(), true);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoAnalogPin<1>::supports_analog_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_output_is_false) {
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().supports_output(), false);
}

BOOST_AUTO_TEST_CASE(supports_output_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoAnalogPin<1>::supports_output()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_pwm_output_is_false) {
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().supports_pwm_output(), false);
}

BOOST_AUTO_TEST_CASE(supports_pwm_output_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoAnalogPin<1>::supports_pwm_output()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_is_false) {
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().supports_interrupts(), false);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoAnalogPin<1>::supports_interrupts()>::value(), true);
}

BOOST_AUTO_TEST_CASE(set_input_does_nothing) {
	FixedArduinoAnalogPin<93>().set_input();
}

BOOST_AUTO_TEST_CASE(set_input_true_does_nothing) {
	FixedArduinoAnalogPin<94>().set_input(true);
}

BOOST_AUTO_TEST_CASE(set_input_false_does_nothing) {
	FixedArduinoAnalogPin<95>().set_input(false);
}

BOOST_AUTO_TEST_CASE(read_analog_calls_analogRead) {
	analogReadReturnValue = 1000;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<99>().read_analog(), 1000);
	BOOST_CHECK_EQUAL(analogReadLastPin, 99);

	analogReadReturnValue = 2;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<100>().read_analog(), 2);
}

BOOST_AUTO_TEST_CASE(read_analog_range_calls_analogRead) {
	analogReadReturnValue = 0;
	FixedArduinoAnalogPin<102>().read_analog(16);
	BOOST_CHECK_EQUAL(analogReadLastPin, 102);
}

BOOST_AUTO_TEST_CASE(read_analog_range_scales_result) {
	analogReadReturnValue = 0;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().read_analog(16), 0);

	analogReadReturnValue = 1023;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().read_analog(16), 15);

	analogReadReturnValue = 256;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().read_analog(16), 4);

	analogReadReturnValue = 512;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().read_analog(16), 8);
}

BOOST_AUTO_TEST_CASE(read_digital_calls_analogRead) {
	analogReadReturnValue = 0;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<103>().read_digital(), false);
	BOOST_CHECK_EQUAL(analogReadLastPin, 103);

	analogReadReturnValue = 1023;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<104>().read_digital(), true);
}

BOOST_AUTO_TEST_CASE(read_digital_thresholds_at_512) {
	analogReadReturnValue = 511;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().read_digital(), false);

	analogReadReturnValue = 512;
	BOOST_CHECK_EQUAL(FixedArduinoAnalogPin<1>().read_digital(), true);
}

BOOST_AUTO_TEST_SUITE_END()

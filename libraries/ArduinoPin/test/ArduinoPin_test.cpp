#include <boost/test/unit_test.hpp>

#include "MockArduinoPin.h"

#define digitalPinToTimer(x) (((x) == 2) ? 9 : NOT_ON_TIMER)
#define digitalPinToInterrupt(x) (((x) == 5) ? 3 : NOT_AN_INTERRUPT)

static void voidFunc(void) {
}

template <int v>
struct ConstExprChecker { static constexpr bool value(void) { return true; } };

#include "../ArduinoPin.h"

BOOST_AUTO_TEST_SUITE(ArduinoPin_test)

BOOST_AUTO_TEST_CASE(exists_is_true) {
	BOOST_CHECK_EQUAL(ArduinoPin(1).exists(), true);
}

BOOST_AUTO_TEST_CASE(exists_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoPin::exists()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_true) {
	BOOST_CHECK_EQUAL(ArduinoPin(1).supports_input(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoPin::supports_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_false) {
	BOOST_CHECK_EQUAL(ArduinoPin(1).supports_analog_input(), false);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoPin::supports_analog_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_output_is_true) {
	BOOST_CHECK_EQUAL(ArduinoPin(1).supports_output(), true);
}

BOOST_AUTO_TEST_CASE(supports_output_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<ArduinoPin::supports_output()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_pwm_output_checks_timer) {
	BOOST_CHECK_EQUAL(ArduinoPin(1).supports_pwm_output(), false);
	BOOST_CHECK_EQUAL(ArduinoPin(2).supports_pwm_output(), true);
	BOOST_CHECK_EQUAL(ArduinoPin(5).supports_pwm_output(), false);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_checks_interrupts) {
	BOOST_CHECK_EQUAL(ArduinoPin(1).supports_interrupts(), false);
	BOOST_CHECK_EQUAL(ArduinoPin(2).supports_interrupts(), false);
	BOOST_CHECK_EQUAL(ArduinoPin(5).supports_interrupts(), true);
}

BOOST_AUTO_TEST_CASE(high_calls_digitalWrite) {
	ArduinoPin(90).high();
	BOOST_CHECK_EQUAL(digitalWriteLastPin, 90);
	BOOST_CHECK_EQUAL(digitalWriteLastValue, HIGH);
}

BOOST_AUTO_TEST_CASE(low_calls_digitalWrite) {
	ArduinoPin(91).low();
	BOOST_CHECK_EQUAL(digitalWriteLastPin, 91);
	BOOST_CHECK_EQUAL(digitalWriteLastValue, LOW);
}

BOOST_AUTO_TEST_CASE(pwm_calls_analogWrite) {
	ArduinoPin(92).pwm(80);
	BOOST_CHECK_EQUAL(analogWriteLastPin, 92);
	BOOST_CHECK_EQUAL(analogWriteLastValue, 80);
}

BOOST_AUTO_TEST_CASE(set_input_calls_pinMode) {
	ArduinoPin(93).set_input();
	BOOST_CHECK_EQUAL(pinModeLastPin, 93);
	BOOST_CHECK_EQUAL(pinModeLastMode, INPUT);
}

BOOST_AUTO_TEST_CASE(set_input_true_calls_pinMode) {
	ArduinoPin(94).set_input(true);
	BOOST_CHECK_EQUAL(pinModeLastPin, 94);
	BOOST_CHECK_EQUAL(pinModeLastMode, INPUT_PULLUP);
}

BOOST_AUTO_TEST_CASE(set_input_false_calls_pinMode) {
	ArduinoPin(95).set_input(false);
	BOOST_CHECK_EQUAL(pinModeLastPin, 95);
	BOOST_CHECK_EQUAL(pinModeLastMode, INPUT);
}

BOOST_AUTO_TEST_CASE(set_output_calls_pinMode) {
	ArduinoPin(96).set_output();
	BOOST_CHECK_EQUAL(pinModeLastPin, 96);
	BOOST_CHECK_EQUAL(pinModeLastMode, OUTPUT);
}

BOOST_AUTO_TEST_CASE(read_digital_calls_digitalRead) {
	digitalReadReturnValue = LOW;
	BOOST_CHECK_EQUAL(ArduinoPin(97).read_digital(), false);
	BOOST_CHECK_EQUAL(digitalReadLastPin, 97);

	digitalReadReturnValue = HIGH;
	BOOST_CHECK_EQUAL(ArduinoPin(98).read_digital(), true);
}

BOOST_AUTO_TEST_CASE(read_analog_calls_digitalRead) {
	digitalReadReturnValue = LOW;
	BOOST_CHECK_EQUAL(ArduinoPin(99).read_analog(), 0);
	BOOST_CHECK_EQUAL(digitalReadLastPin, 99);

	digitalReadReturnValue = HIGH;
	BOOST_CHECK_EQUAL(ArduinoPin(100).read_analog(), 1023);
}

BOOST_AUTO_TEST_CASE(read_analog_small_range_returns_0) {
	BOOST_CHECK_EQUAL(ArduinoPin(101).read_analog(0), 0);
	BOOST_CHECK_EQUAL(ArduinoPin(101).read_analog(1), 0);
	BOOST_CHECK(digitalReadLastPin != 101);
}

BOOST_AUTO_TEST_CASE(read_analog_range_calls_digitalRead) {
	digitalReadReturnValue = LOW;
	BOOST_CHECK_EQUAL(ArduinoPin(102).read_analog(10), 0);
	BOOST_CHECK_EQUAL(digitalReadLastPin, 102);

	digitalReadReturnValue = HIGH;
	BOOST_CHECK_EQUAL(ArduinoPin(103).read_analog(10), 9);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_low_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	ArduinoPin(5).set_interrupt_on_low(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, LOW);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_rising_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	ArduinoPin(5).set_interrupt_on_rising(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, RISING);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_falling_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	ArduinoPin(5).set_interrupt_on_falling(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, FALLING);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_change_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	ArduinoPin(5).set_interrupt_on_change(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, CHANGE);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_high_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	ArduinoPin(5).set_interrupt_on_high(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, HIGH);
}

BOOST_AUTO_TEST_CASE(remove_interrupt_calls_detachInterrupt) {
	ArduinoPin(5).remove_interrupt();
	BOOST_CHECK_EQUAL(detachInterruptLastInt, 3);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(FixedArduinoPin_test)

BOOST_AUTO_TEST_CASE(exists_is_true) {
	BOOST_CHECK_EQUAL(FixedArduinoPin<1>().exists(), true);
}

BOOST_AUTO_TEST_CASE(exists_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoPin<1>::exists()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_true) {
	BOOST_CHECK_EQUAL(FixedArduinoPin<1>().supports_input(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoPin<1>::supports_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_false) {
	BOOST_CHECK_EQUAL(FixedArduinoPin<1>().supports_analog_input(), false);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoPin<1>::supports_analog_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_output_is_true) {
	BOOST_CHECK_EQUAL(FixedArduinoPin<1>().supports_output(), true);
}

BOOST_AUTO_TEST_CASE(supports_output_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoPin<1>::supports_output()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_pwm_output_checks_timer) {
	BOOST_CHECK_EQUAL(FixedArduinoPin<1>().supports_pwm_output(), false);
	BOOST_CHECK_EQUAL(FixedArduinoPin<2>().supports_pwm_output(), true);
	BOOST_CHECK_EQUAL(FixedArduinoPin<5>().supports_pwm_output(), false);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_checks_interrupts) {
	BOOST_CHECK_EQUAL(FixedArduinoPin<1>().supports_interrupts(), false);
	BOOST_CHECK_EQUAL(FixedArduinoPin<2>().supports_interrupts(), false);
	BOOST_CHECK_EQUAL(FixedArduinoPin<5>().supports_interrupts(), true);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<FixedArduinoPin<1>::supports_interrupts()>::value(), true);
}

BOOST_AUTO_TEST_CASE(high_calls_digitalWrite) {
	FixedArduinoPin<90>().high();
	BOOST_CHECK_EQUAL(digitalWriteLastPin, 90);
	BOOST_CHECK_EQUAL(digitalWriteLastValue, HIGH);
}

BOOST_AUTO_TEST_CASE(low_calls_digitalWrite) {
	FixedArduinoPin<91>().low();
	BOOST_CHECK_EQUAL(digitalWriteLastPin, 91);
	BOOST_CHECK_EQUAL(digitalWriteLastValue, LOW);
}

BOOST_AUTO_TEST_CASE(pwm_calls_analogWrite) {
	FixedArduinoPin<92>().pwm(80);
	BOOST_CHECK_EQUAL(analogWriteLastPin, 92);
	BOOST_CHECK_EQUAL(analogWriteLastValue, 80);
}

BOOST_AUTO_TEST_CASE(set_input_calls_pinMode) {
	FixedArduinoPin<93>().set_input();
	BOOST_CHECK_EQUAL(pinModeLastPin, 93);
	BOOST_CHECK_EQUAL(pinModeLastMode, INPUT);
}

BOOST_AUTO_TEST_CASE(set_input_true_calls_pinMode) {
	FixedArduinoPin<94>().set_input(true);
	BOOST_CHECK_EQUAL(pinModeLastPin, 94);
	BOOST_CHECK_EQUAL(pinModeLastMode, INPUT_PULLUP);
}

BOOST_AUTO_TEST_CASE(set_input_false_calls_pinMode) {
	FixedArduinoPin<95>().set_input(false);
	BOOST_CHECK_EQUAL(pinModeLastPin, 95);
	BOOST_CHECK_EQUAL(pinModeLastMode, INPUT);
}

BOOST_AUTO_TEST_CASE(set_output_calls_pinMode) {
	FixedArduinoPin<96>().set_output();
	BOOST_CHECK_EQUAL(pinModeLastPin, 96);
	BOOST_CHECK_EQUAL(pinModeLastMode, OUTPUT);
}

BOOST_AUTO_TEST_CASE(read_digital_calls_digitalRead) {
	digitalReadReturnValue = LOW;
	BOOST_CHECK_EQUAL(FixedArduinoPin<97>().read_digital(), false);
	BOOST_CHECK_EQUAL(digitalReadLastPin, 97);

	digitalReadReturnValue = HIGH;
	BOOST_CHECK_EQUAL(FixedArduinoPin<98>().read_digital(), true);
}

BOOST_AUTO_TEST_CASE(read_analog_calls_digitalRead) {
	digitalReadReturnValue = LOW;
	BOOST_CHECK_EQUAL(FixedArduinoPin<99>().read_analog(), 0);
	BOOST_CHECK_EQUAL(digitalReadLastPin, 99);

	digitalReadReturnValue = HIGH;
	BOOST_CHECK_EQUAL(FixedArduinoPin<100>().read_analog(), 1023);
}

BOOST_AUTO_TEST_CASE(read_analog_small_range_returns_0) {
	BOOST_CHECK_EQUAL(FixedArduinoPin<101>().read_analog(0), 0);
	BOOST_CHECK_EQUAL(FixedArduinoPin<101>().read_analog(1), 0);
	BOOST_CHECK(digitalReadLastPin != 101);
}

BOOST_AUTO_TEST_CASE(read_analog_range_calls_digitalRead) {
	digitalReadReturnValue = LOW;
	BOOST_CHECK_EQUAL(FixedArduinoPin<102>().read_analog(10), 0);
	BOOST_CHECK_EQUAL(digitalReadLastPin, 102);

	digitalReadReturnValue = HIGH;
	BOOST_CHECK_EQUAL(FixedArduinoPin<103>().read_analog(10), 9);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_low_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	FixedArduinoPin<5>().set_interrupt_on_low(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, LOW);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_rising_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	FixedArduinoPin<5>().set_interrupt_on_rising(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, RISING);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_falling_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	FixedArduinoPin<5>().set_interrupt_on_falling(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, FALLING);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_change_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	FixedArduinoPin<5>().set_interrupt_on_change(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, CHANGE);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_high_calls_attachInterrupt) {
	attachInterruptLastFunc = nullptr;
	FixedArduinoPin<5>().set_interrupt_on_high(&voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastInt, 3);
	BOOST_CHECK_EQUAL(attachInterruptLastFunc, &voidFunc);
	BOOST_CHECK_EQUAL(attachInterruptLastMode, HIGH);
}

BOOST_AUTO_TEST_CASE(remove_interrupt_calls_detachInterrupt) {
	FixedArduinoPin<5>().remove_interrupt();
	BOOST_CHECK_EQUAL(detachInterruptLastInt, 3);
}

BOOST_AUTO_TEST_SUITE_END()

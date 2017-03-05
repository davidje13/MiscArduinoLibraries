#include <boost/test/unit_test.hpp>

#include "../VoidPin.h"

template <int v>
struct ConstExprChecker { static constexpr bool value(void) { return true; } };

BOOST_AUTO_TEST_SUITE(VoidPin_test)

BOOST_AUTO_TEST_CASE(exists_is_false) {
	BOOST_CHECK_EQUAL(VoidPin().exists(), false);
}

BOOST_AUTO_TEST_CASE(exists_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<VoidPin::exists()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_input_is_false) {
	BOOST_CHECK_EQUAL(VoidPin().supports_input(), false);
}

BOOST_AUTO_TEST_CASE(supports_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<VoidPin::supports_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_false) {
	BOOST_CHECK_EQUAL(VoidPin().supports_analog_input(), false);
}

BOOST_AUTO_TEST_CASE(supports_analog_input_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<VoidPin::supports_analog_input()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_output_is_false) {
	BOOST_CHECK_EQUAL(VoidPin().supports_output(), false);
}

BOOST_AUTO_TEST_CASE(supports_output_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<VoidPin::supports_output()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_pwm_output_is_false) {
	BOOST_CHECK_EQUAL(VoidPin().supports_pwm_output(), false);
}

BOOST_AUTO_TEST_CASE(supports_pwm_output_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<VoidPin::supports_pwm_output()>::value(), true);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_is_false) {
	BOOST_CHECK_EQUAL(VoidPin().supports_interrupts(), false);
}

BOOST_AUTO_TEST_CASE(supports_interrupts_is_constexpr) {
	BOOST_CHECK_EQUAL(ConstExprChecker<VoidPin::supports_interrupts()>::value(), true);
}

BOOST_AUTO_TEST_CASE(high_does_nothing) {
	VoidPin().high();
}

BOOST_AUTO_TEST_CASE(low_does_nothing) {
	VoidPin().low();
}

BOOST_AUTO_TEST_CASE(pwm_does_nothing) {
	VoidPin().pwm(80);
}

BOOST_AUTO_TEST_CASE(set_input_does_nothing) {
	VoidPin().set_input();
}

BOOST_AUTO_TEST_CASE(set_input_true_does_nothing) {
	VoidPin().set_input(true);
}

BOOST_AUTO_TEST_CASE(set_input_false_does_nothing) {
	VoidPin().set_input(false);
}

BOOST_AUTO_TEST_CASE(set_output_does_nothing) {
	VoidPin().set_output();
}

BOOST_AUTO_TEST_CASE(read_digital_returns_false) {
	BOOST_CHECK_EQUAL(VoidPin().read_digital(), false);
}

BOOST_AUTO_TEST_CASE(read_analog_returns_0) {
	BOOST_CHECK_EQUAL(VoidPin().read_analog(), 0);
}

BOOST_AUTO_TEST_CASE(read_analog_range_returns_0) {
	BOOST_CHECK_EQUAL(VoidPin().read_analog(10), 0);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_low_does_nothing) {
	VoidPin().set_interrupt_on_low(nullptr);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_rising_does_nothing) {
	VoidPin().set_interrupt_on_rising(nullptr);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_falling_does_nothing) {
	VoidPin().set_interrupt_on_falling(nullptr);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_change_does_nothing) {
	VoidPin().set_interrupt_on_change(nullptr);
}

BOOST_AUTO_TEST_CASE(set_interrupt_on_high_does_nothing) {
	VoidPin().set_interrupt_on_high(nullptr);
}

BOOST_AUTO_TEST_CASE(remove_interrupt_does_nothing) {
	VoidPin().remove_interrupt();
}

BOOST_AUTO_TEST_SUITE_END()

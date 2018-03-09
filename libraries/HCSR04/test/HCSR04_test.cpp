#include <boost/test/unit_test.hpp>

#include <MockPin.h>
#include <MockMicros.h>

#include "../HCSR04.h"

BOOST_AUTO_TEST_SUITE(SynchronousHCSR04_test)

BOOST_AUTO_TEST_CASE(sends_trigger_pulse_when_measuring) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeSynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	BOOST_CHECK_EQUAL(outPin.set_output_call_count, 1);
	BOOST_CHECK_EQUAL(inPin.set_input_call_count, 1);
	BOOST_CHECK_EQUAL(outPin.low_call_count, 1);

	echo.measure_micros();

	BOOST_CHECK_EQUAL(outPin.high_call_count, 1);
	// outPin should be held high for at least 10 microseconds
	BOOST_CHECK_EQUAL(delayMicroseconds_last_delay, 10);
	BOOST_CHECK_EQUAL(outPin.low_call_count, 2);
}

BOOST_AUTO_TEST_CASE(returns_width_of_echo_pulse) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeSynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	inPin.measure_high_pulse_return_value = 50;
	uint16_t duration = echo.measure_micros();

	BOOST_CHECK_EQUAL(duration, 50);
}

BOOST_AUTO_TEST_CASE(returns_no_echo_if_too_long) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeSynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	inPin.measure_high_pulse_return_value = 32000;
	uint16_t duration = echo.measure_micros();

	BOOST_CHECK_EQUAL(duration, HCSR04::no_reflection());
}

BOOST_AUTO_TEST_CASE(returns_distance_in_mm) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeSynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	inPin.measure_high_pulse_return_value = 50;
	uint16_t distance = echo.measure_millimetres();

	BOOST_CHECK_EQUAL(distance, 8);
}

BOOST_AUTO_TEST_CASE(returns_no_echo_if_too_far) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeSynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	inPin.measure_high_pulse_return_value = 32000;
	uint16_t distance = echo.measure_millimetres();

	BOOST_CHECK_EQUAL(distance, HCSR04::no_reflection());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(AsynchronousHCSR04_test)

BOOST_AUTO_TEST_CASE(sends_trigger_pulse_when_measuring) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeAsynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	BOOST_CHECK_EQUAL(outPin.set_output_call_count, 1);
	BOOST_CHECK_EQUAL(inPin.set_input_call_count, 1);
	BOOST_CHECK_EQUAL(outPin.low_call_count, 1);
	BOOST_CHECK(inPin.set_interrupt_on_change_last_func);

	echo.measure_micros();

	BOOST_CHECK_EQUAL(outPin.high_call_count, 1);
	// outPin should be held high for at least 10 microseconds
	BOOST_CHECK_EQUAL(delayMicroseconds_last_delay, 10);
	BOOST_CHECK_EQUAL(outPin.low_call_count, 2);
}

BOOST_AUTO_TEST_CASE(returns_width_of_previous_echo_pulse) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeAsynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	set_micros_return_values(1000, 1050);

	uint16_t duration0 = echo.measure_micros();
	BOOST_CHECK_EQUAL(duration0, HCSR04::no_reflection());

	inPin.read_digital_return_value = true;
	inPin.set_interrupt_on_change_last_func();

	inPin.read_digital_return_value = false;
	inPin.set_interrupt_on_change_last_func();

	uint16_t duration1 = echo.measure_micros();
	BOOST_CHECK_EQUAL(duration1, 50);
}

BOOST_AUTO_TEST_CASE(returns_no_echo_if_too_long) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeAsynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	set_micros_return_values(1000, 41000);

	uint16_t duration0 = echo.measure_micros();
	BOOST_CHECK_EQUAL(duration0, HCSR04::no_reflection());

	inPin.read_digital_return_value = true;
	inPin.set_interrupt_on_change_last_func();

	inPin.read_digital_return_value = false;
	inPin.set_interrupt_on_change_last_func();

	uint16_t duration1 = echo.measure_micros();
	BOOST_CHECK_EQUAL(duration1, HCSR04::no_reflection());
}

BOOST_AUTO_TEST_CASE(returns_distance_in_mm) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeAsynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	set_micros_return_values(1000, 1050);

	uint16_t distance0 = echo.measure_millimetres();
	BOOST_CHECK_EQUAL(distance0, HCSR04::no_reflection());

	inPin.read_digital_return_value = true;
	inPin.set_interrupt_on_change_last_func();

	inPin.read_digital_return_value = false;
	inPin.set_interrupt_on_change_last_func();

	uint16_t distance1 = echo.measure_millimetres();
	BOOST_CHECK_EQUAL(distance1, 8);
}

BOOST_AUTO_TEST_CASE(returns_no_echo_if_too_far) {
	auto outPin = Pin::Mock();
	auto inPin = Pin::Mock();

	auto echo = MakeAsynchronousHCSR04(
		outPin.implementation(),
		inPin.implementation()
	);

	set_micros_return_values(1000, 41000);

	uint16_t duration0 = echo.measure_millimetres();
	BOOST_CHECK_EQUAL(duration0, HCSR04::no_reflection());

	inPin.read_digital_return_value = true;
	inPin.set_interrupt_on_change_last_func();

	inPin.read_digital_return_value = false;
	inPin.set_interrupt_on_change_last_func();

	uint16_t duration1 = echo.measure_millimetres();
	BOOST_CHECK_EQUAL(duration1, HCSR04::no_reflection());
}

BOOST_AUTO_TEST_SUITE_END()

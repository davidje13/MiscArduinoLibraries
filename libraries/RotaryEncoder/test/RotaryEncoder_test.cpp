#include <boost/test/unit_test.hpp>

#include <MockPin.h>
#include <MockSREG.h>

#include "../RotaryEncoder.h"

BOOST_AUTO_TEST_SUITE(RotaryEncoder_test)

BOOST_AUTO_TEST_CASE(returns_0_while_unchanging) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	auto encoder = MakeRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	encoder.reset();
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = false;
	encoder.reset();
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = true;
	encoder.reset();
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = true;
	encoder.reset();
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(starts_with_current_state) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = false;

	auto encoder = MakeRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(returns_1_while_incrementing) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	auto encoder = MakeRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	encoder.reset();

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = true;
	BOOST_CHECK_EQUAL(int(encoder.delta()), 1);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = true;
	BOOST_CHECK_EQUAL(int(encoder.delta()), 1);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = false;
	BOOST_CHECK_EQUAL(int(encoder.delta()), 1);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	BOOST_CHECK_EQUAL(int(encoder.delta()), 1);
}

BOOST_AUTO_TEST_CASE(returns_minus_1_while_decrementing) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	auto encoder = MakeRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	encoder.reset();

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = false;
	BOOST_CHECK_EQUAL(int(encoder.delta()), -1);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = true;
	BOOST_CHECK_EQUAL(int(encoder.delta()), -1);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = true;
	BOOST_CHECK_EQUAL(int(encoder.delta()), -1);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	BOOST_CHECK_EQUAL(int(encoder.delta()), -1);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(InterruptRotaryEncoder_test)

BOOST_AUTO_TEST_CASE(returns_0_while_unchanging) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	encoder.reset();
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = false;
	encoder.reset();
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = true;
	encoder.reset();
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = true;
	encoder.reset();
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(starts_with_current_state) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(returns_1_while_incrementing) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	encoder.reset();

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = true;
	BOOST_CHECK_EQUAL(int(encoder.delta()), 1);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = true;
	BOOST_CHECK_EQUAL(int(encoder.delta()), 1);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = false;
	BOOST_CHECK_EQUAL(int(encoder.delta()), 1);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	BOOST_CHECK_EQUAL(int(encoder.delta()), 1);
}

BOOST_AUTO_TEST_CASE(returns_minus_1_while_decrementing) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	encoder.reset();

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = false;
	BOOST_CHECK_EQUAL(int(encoder.delta()), -1);

	aPin.read_digital_return_value = true;
	bPin.read_digital_return_value = true;
	BOOST_CHECK_EQUAL(int(encoder.delta()), -1);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = true;
	BOOST_CHECK_EQUAL(int(encoder.delta()), -1);

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;
	BOOST_CHECK_EQUAL(int(encoder.delta()), -1);
}

BOOST_AUTO_TEST_CASE(registers_interrupts_if_available) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = true;
	bPin.supports_interrupts_return_value = true;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	BOOST_CHECK(aPin.set_interrupt_on_change_last_func);
	BOOST_CHECK(bPin.set_interrupt_on_change_last_func);
}

BOOST_AUTO_TEST_CASE(accumulates_positive_deltas_during_interrupts) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = true;
	bPin.supports_interrupts_return_value = true;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	bPin.read_digital_return_value = true;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = true;
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = false;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = false;
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = true;
	bPin.set_interrupt_on_change_last_func();

	BOOST_CHECK_EQUAL(int(encoder.delta()), 5);
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(accumulates_negative_deltas_during_interrupts) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = true;
	bPin.supports_interrupts_return_value = true;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	aPin.read_digital_return_value = true;
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = true;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = false;
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = false;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = true;
	aPin.set_interrupt_on_change_last_func();

	BOOST_CHECK_EQUAL(int(encoder.delta()), -5);
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(resets_delta_after_reading) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = true;
	bPin.supports_interrupts_return_value = true;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	bPin.read_digital_return_value = true;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = true;
	aPin.set_interrupt_on_change_last_func();

	BOOST_CHECK_EQUAL(int(encoder.delta()), 2);

	bPin.read_digital_return_value = false;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = false;
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = true;
	bPin.set_interrupt_on_change_last_func();

	BOOST_CHECK_EQUAL(int(encoder.delta()), 3);
}

BOOST_AUTO_TEST_CASE(accumulates_mixed_deltas_during_interrupts) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = true;
	bPin.supports_interrupts_return_value = true;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	bPin.read_digital_return_value = true; // +
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = true; // +
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = false; // +
	bPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = true; // -
	bPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = false; // +
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = false; // +
	aPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = true; // -
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = true; // -
	bPin.set_interrupt_on_change_last_func();

	BOOST_CHECK_EQUAL(int(encoder.delta()), 2);
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(registers_partial_interrupts_if_available_on_a) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = true;
	bPin.supports_interrupts_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	BOOST_CHECK(aPin.set_interrupt_on_change_last_func);
	BOOST_CHECK_EQUAL(bPin.set_interrupt_on_change_last_func, nullptr);
}

BOOST_AUTO_TEST_CASE(accumulates_positive_deltas_during_partial_interrupts_a) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = true;
	bPin.supports_interrupts_return_value = false;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	bPin.read_digital_return_value = true;

	aPin.read_digital_return_value = true;
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = false;

	aPin.read_digital_return_value = false;
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = true;

	BOOST_CHECK_EQUAL(int(encoder.delta()), 5);
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(accumulates_negative_deltas_during_partial_interrupts_a) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = true;
	bPin.supports_interrupts_return_value = false;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	aPin.read_digital_return_value = true;
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = true;

	aPin.read_digital_return_value = false;
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = false;

	aPin.read_digital_return_value = true;
	aPin.set_interrupt_on_change_last_func();

	BOOST_CHECK_EQUAL(int(encoder.delta()), -5);
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(accumulates_mixed_deltas_during_partial_interrupts_a) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = true;
	bPin.supports_interrupts_return_value = false;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	bPin.read_digital_return_value = true; // +

	aPin.read_digital_return_value = true; // +
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = false; // +

	bPin.read_digital_return_value = true; // -

	bPin.read_digital_return_value = false; // +

	aPin.read_digital_return_value = false; // +
	aPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = true; // -
	aPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = true; // -

	BOOST_CHECK_EQUAL(int(encoder.delta()), 2);
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(registers_partial_interrupts_if_available_on_b) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = false;
	bPin.supports_interrupts_return_value = true;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	BOOST_CHECK_EQUAL(aPin.set_interrupt_on_change_last_func, nullptr);
	BOOST_CHECK(bPin.set_interrupt_on_change_last_func);
}

BOOST_AUTO_TEST_CASE(accumulates_positive_deltas_during_partial_interrupts_b) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = false;
	bPin.supports_interrupts_return_value = true;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	bPin.read_digital_return_value = true;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = true;

	bPin.read_digital_return_value = false;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = false;

	bPin.read_digital_return_value = true;
	bPin.set_interrupt_on_change_last_func();

	BOOST_CHECK_EQUAL(int(encoder.delta()), 5);
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(accumulates_negative_deltas_during_partial_interrupts_b) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = false;
	bPin.supports_interrupts_return_value = true;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	aPin.read_digital_return_value = true;

	bPin.read_digital_return_value = true;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = false;

	bPin.read_digital_return_value = false;
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = true;

	BOOST_CHECK_EQUAL(int(encoder.delta()), -5);
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_CASE(accumulates_mixed_deltas_during_partial_interrupts_b) {
	auto aPin = Pin::Mock();
	auto bPin = Pin::Mock();

	aPin.supports_interrupts_return_value = false;
	bPin.supports_interrupts_return_value = true;

	aPin.read_digital_return_value = false;
	bPin.read_digital_return_value = false;

	auto encoder = MakeInterruptRotaryEncoder(
		aPin.implementation(),
		bPin.implementation()
	);

	bPin.read_digital_return_value = true; // +
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = true; // +

	bPin.read_digital_return_value = false; // +
	bPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = true; // -
	bPin.set_interrupt_on_change_last_func();

	bPin.read_digital_return_value = false; // +
	bPin.set_interrupt_on_change_last_func();

	aPin.read_digital_return_value = false; // +

	aPin.read_digital_return_value = true; // -

	bPin.read_digital_return_value = true; // -
	bPin.set_interrupt_on_change_last_func();

	BOOST_CHECK_EQUAL(int(encoder.delta()), 2);
	BOOST_CHECK_EQUAL(int(encoder.delta()), 0);
}

BOOST_AUTO_TEST_SUITE_END()

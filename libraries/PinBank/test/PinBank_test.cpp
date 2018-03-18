#include <boost/test/unit_test.hpp>

#include <MockPin.h>

#include "../PinBank.h"

template <int v>
struct ConstExprChecker { static constexpr bool value(void) { return true; } };

BOOST_AUTO_TEST_SUITE(PinBank_test)

BOOST_AUTO_TEST_CASE(size_is_constexpr) {
	auto bank = MakePinBank(
		Pin::Mock().implementation(),
		Pin::Mock().implementation()
	);
	BOOST_CHECK_EQUAL(ConstExprChecker<bank.size()>::value(), true);
}

BOOST_AUTO_TEST_CASE(sets_pins_msb_first) {
	auto p1 = Pin::Mock();
	auto p2 = Pin::Mock();

	auto bank = MakePinBank(
		p1.implementation(),
		p2.implementation()
	);

	bank.set(0x02);
	BOOST_CHECK_EQUAL(p1.low_call_count + p1.high_call_count, 1);
	BOOST_CHECK_EQUAL(p2.low_call_count + p2.high_call_count, 1);
	BOOST_CHECK_EQUAL(
		(p1.high_call_count << 1) |
		p2.high_call_count,
		0x02
	);
}

BOOST_AUTO_TEST_CASE(handles_many_pins) {
	auto p1 = Pin::Mock();
	auto p2 = Pin::Mock();
	auto p3 = Pin::Mock();
	auto p4 = Pin::Mock();

	auto bank = MakePinBank(
		p1.implementation(),
		p2.implementation(),
		p3.implementation(),
		p4.implementation()
	);

	for(uint8_t v = 0; v < 0x10; ++ v) {
		p1.reset();
		p2.reset();
		p3.reset();
		p4.reset();

		bank.set(v);

		BOOST_CHECK_EQUAL(p1.low_call_count + p1.high_call_count, 1);
		BOOST_CHECK_EQUAL(p2.low_call_count + p2.high_call_count, 1);
		BOOST_CHECK_EQUAL(p3.low_call_count + p3.high_call_count, 1);
		BOOST_CHECK_EQUAL(p4.low_call_count + p4.high_call_count, 1);
		int actual = (
			(p1.high_call_count << 3) |
			(p2.high_call_count << 2) |
			(p3.high_call_count << 1) |
			p4.high_call_count
		);
		BOOST_CHECK_EQUAL(actual, v);
	}
}

BOOST_AUTO_TEST_CASE(batches_requests) {
	auto p1 = Pin::Mock();
	auto p2 = Pin::Mock();
	auto p3 = Pin::Mock();
	auto p4 = Pin::Mock();

	auto bank = MakePinBank(
		p1.implementation(),
		p2.implementation(),
		p3.implementation(),
		p4.implementation()
	);

	bank.set(0x01);

	BOOST_CHECK_EQUAL(p1.begin_batch_call_count, 1);
	BOOST_CHECK_EQUAL(p1.send_batch_call_count, p1.begin_batch_call_count);
	BOOST_CHECK_EQUAL(p2.begin_batch_call_count, 1);
	BOOST_CHECK_EQUAL(p2.send_batch_call_count, p2.begin_batch_call_count);
	BOOST_CHECK_EQUAL(p3.begin_batch_call_count, 1);
	BOOST_CHECK_EQUAL(p3.send_batch_call_count, p3.begin_batch_call_count);
	BOOST_CHECK_EQUAL(p4.begin_batch_call_count, 1);
	BOOST_CHECK_EQUAL(p4.send_batch_call_count, p4.begin_batch_call_count);
}

BOOST_AUTO_TEST_SUITE_END()

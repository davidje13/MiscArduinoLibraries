#include <boost/test/unit_test.hpp>

#include "MockArduinoPin.h"
#include <MockSREG.h>

#include "../ArduinoPinBank.h"

template <int v>
struct ConstExprChecker { static constexpr bool value(void) { return true; } };

#define MY_PORT 2

BOOST_AUTO_TEST_SUITE(ArduinoPinBank_test)

BOOST_AUTO_TEST_CASE(size_returns_pin_count) {
	ArduinoPinBank<MY_PORT> bank1;
	ArduinoPinBank<MY_PORT, 2, 6> bank2;
	BOOST_CHECK_EQUAL(bank1.size(), 8);
	BOOST_CHECK_EQUAL(bank2.size(), 4);
}

BOOST_AUTO_TEST_CASE(size_is_constexpr) {
	ArduinoPinBank<MY_PORT> bank;
	BOOST_CHECK_EQUAL(ConstExprChecker<bank.size()>::value(), true);
}

BOOST_AUTO_TEST_CASE(full_width_set_replaces_output_register_bits) {
	portOutputRegisterValues[MY_PORT] = 0x02;
	ArduinoPinBank<MY_PORT>().set(0xAB);
	BOOST_CHECK_EQUAL(portOutputRegisterValues[MY_PORT], 0xAB);
}

BOOST_AUTO_TEST_CASE(part_width_set_modifies_output_register_bits) {
	portOutputRegisterValues[MY_PORT] = 0x00;
	ArduinoPinBank<MY_PORT, 2, 7>().set(0xFF);
	BOOST_CHECK_EQUAL(portOutputRegisterValues[MY_PORT], 0x7C);

	portOutputRegisterValues[MY_PORT] = 0xFF;
	ArduinoPinBank<MY_PORT, 2, 7>().set(0xFF);
	BOOST_CHECK_EQUAL(portOutputRegisterValues[MY_PORT], 0xFF);

	portOutputRegisterValues[MY_PORT] = 0xFF;
	ArduinoPinBank<MY_PORT, 2, 7>().set(0x00);
	BOOST_CHECK_EQUAL(portOutputRegisterValues[MY_PORT], 0x83);
}

BOOST_AUTO_TEST_SUITE_END()

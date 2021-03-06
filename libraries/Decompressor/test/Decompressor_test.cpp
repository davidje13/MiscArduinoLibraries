#include <boost/test/unit_test.hpp>

#include "../Decompressor.h"
#include "CompressedTest.h"
#include "CompressedShip.h"
#include "Ship.h"

BOOST_AUTO_TEST_SUITE(Decompressor_test)

BOOST_AUTO_TEST_CASE(decompresses_to_original_string) {
	auto test = MakeTestDecompressor();
	const char *expected = (
		"This is a moderately long test message which will be"
		" compressed and decompressed. Hopefully the compression"
		" will actually save some bytes."
	);
	for(uint16_t i = 0; expected[i] != '\0'; ++ i) {
		char actual = char(test.get(i));
		BOOST_CHECK_EQUAL(actual, expected[i]);
	}
}

BOOST_AUTO_TEST_CASE(uses_minimal_ram) {
	auto test = MakeDecompressor<1>(COMPRESSED_TEST);
	BOOST_CHECK_LT(sizeof(test), 50);
}

BOOST_AUTO_TEST_CASE(decompresses_complex_data) {
	const uint16_t size = 10752; // 384x224

	auto test = MakeShipDecompressor();
	uint8_t *actual = static_cast<uint8_t*>(malloc(size));

	for(uint16_t i = 0; i < size; ++ i) {
		actual[i] = test.get(i);
	}
	BOOST_CHECK_EQUAL_COLLECTIONS(
		actual, actual + size,
		SHIP_PM, SHIP_PM + size
	);
}

BOOST_AUTO_TEST_SUITE_END()

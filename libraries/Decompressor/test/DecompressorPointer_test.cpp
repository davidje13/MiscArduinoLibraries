#include <boost/test/unit_test.hpp>

#include "../Decompressor.h"
#include "../DecompressorPointer.h"
#include "CompressedTest.h"

BOOST_AUTO_TEST_SUITE(Decompressor_test)

BOOST_AUTO_TEST_CASE(supports_bracket_syntax) {
	auto test = MakeDecompressor<32>(COMPRESSED_TEST);
	auto ptr = MakeDecompressorPointer(&test);
	const char *expected = (
		"This is a moderately long test message which will be"
		" compressed and decompressed. Hopefully the compression"
		" will actually save some bytes."
	);
	for(int i = 0; expected[i] != '\0'; ++ i) {
		char actual = char(ptr[i]);
		BOOST_CHECK_EQUAL(actual, expected[i]);
	}
}

BOOST_AUTO_TEST_CASE(supports_incrementing) {
	auto test = MakeDecompressor<32>(COMPRESSED_TEST);
	auto ptr = MakeDecompressorPointer(&test);
	const char *expected = (
		"This is a moderately long test message which will be"
		" compressed and decompressed. Hopefully the compression"
		" will actually save some bytes."
	);
	for(int i = 0; expected[i] != '\0'; ++ i) {
		char actual = char(ptr[0]);
		BOOST_CHECK_EQUAL(actual, expected[i]);
		ptr += 1;
	}
}

BOOST_AUTO_TEST_SUITE_END()

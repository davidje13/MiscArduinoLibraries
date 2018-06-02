#include <boost/test/unit_test.hpp>

#include "CompressedStrings.h"

BOOST_AUTO_TEST_SUITE(SegmentedDecompressor_test)

BOOST_AUTO_TEST_CASE(decompresses_each_string) {
	auto test1 = TEST_1;
	const char *expected1 = "test 1";
	for(uint16_t i = 0; expected1[i] != '\0'; ++ i) {
		BOOST_CHECK_EQUAL(test1[i], expected1[i]);
	}

	auto test2 = ANOTHER_TEST;
	const char *expected2 = "another test with repetition repetition repetition";
	for(uint16_t i = 0; expected2[i] != '\0'; ++ i) {
		BOOST_CHECK_EQUAL(test2[i], expected2[i]);
	}
}

BOOST_AUTO_TEST_CASE(ends_messages_with_nil) {
	auto test = TEST_1;
	BOOST_CHECK_EQUAL(test[6], 0);
}

BOOST_AUTO_TEST_CASE(switches_between_strings) {
	auto test1 = TEST_1;
	auto test2 = ANOTHER_TEST;
	BOOST_CHECK_EQUAL(test1[0], 't');
	BOOST_CHECK_EQUAL(test2[0], 'a');
	BOOST_CHECK_EQUAL(test1[1], 'e');
	BOOST_CHECK_EQUAL(test2[1], 'n');

	BOOST_CHECK_EQUAL(test1[4], ' ');
	BOOST_CHECK_EQUAL(test2[3], 't');
}

BOOST_AUTO_TEST_SUITE_END()

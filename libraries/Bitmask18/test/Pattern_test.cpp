#include <boost/test/unit_test.hpp>

#include "../Pattern.h"

BOOST_AUTO_TEST_SUITE(Pattern_test)

BOOST_AUTO_TEST_CASE(pattern_getter) {
	static PROGMEM const uint8_t testPattern[] = {0x01, 0x02, 0x03, 0x04};
	BOOST_CHECK_EQUAL(PatternCol(testPattern, 0), 0x01);
	BOOST_CHECK_EQUAL(PatternCol(testPattern, 1), 0x02);
	BOOST_CHECK_EQUAL(PatternCol(testPattern, 2), 0x03);
	BOOST_CHECK_EQUAL(PatternCol(testPattern, 3), 0x04);
	BOOST_CHECK_EQUAL(PatternCol(testPattern, 4), 0x01);
	BOOST_CHECK_EQUAL(PatternCol(testPattern, 5), 0x02);
}

BOOST_AUTO_TEST_CASE(pattern_on) {
	Pattern p = PATTERN_ON;
	BOOST_CHECK_EQUAL(PatternCol(p, 0), 0xFF);
	BOOST_CHECK_EQUAL(PatternCol(p, 1), 0xFF);
	BOOST_CHECK_EQUAL(PatternCol(p, 2), 0xFF);
	BOOST_CHECK_EQUAL(PatternCol(p, 3), 0xFF);
}

BOOST_AUTO_TEST_CASE(pattern_checker) {
	Pattern p = PATTERN_CHECKER;
	BOOST_CHECK_EQUAL(PatternCol(p, 0), 0x55);
	BOOST_CHECK_EQUAL(PatternCol(p, 1), 0xAA);
	BOOST_CHECK_EQUAL(PatternCol(p, 2), 0x55);
	BOOST_CHECK_EQUAL(PatternCol(p, 3), 0xAA);
}

BOOST_AUTO_TEST_CASE(pattern_grid_sm_11) {
	Pattern p = PATTERN_GRID_SM_11;
	BOOST_CHECK_EQUAL(PatternCol(p, 0), 0xFF);
	BOOST_CHECK_EQUAL(PatternCol(p, 1), 0x55);
	BOOST_CHECK_EQUAL(PatternCol(p, 2), 0xFF);
	BOOST_CHECK_EQUAL(PatternCol(p, 3), 0x55);
}

BOOST_AUTO_TEST_SUITE_END()

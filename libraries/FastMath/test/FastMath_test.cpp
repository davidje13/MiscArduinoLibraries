#include <boost/test/unit_test.hpp>

#define PROGMEM
#define pgm_read_word_near(x) (*static_cast<const uint16_t*>(x))
#define pgm_read_byte_near(x) (*static_cast<const uint8_t*>(x))

#include "../FastMath.h"
#include <cmath>

BOOST_AUTO_TEST_SUITE(FastMath_test)

BOOST_AUTO_TEST_CASE(sin16_is_within_0_69_percent) {
	double accuracy = 0.0069;
	for(uint32_t i = 0; i < 0x10000; ++ i) {
		double v = double(sin16(uint16_t(i)));
		double actual = std::sin(i * 2.0 * M_PI / 65536.0) * 32767.0;
		BOOST_CHECK_GT(v, actual - 65536 * accuracy);
		BOOST_CHECK_LT(v, actual + 65536 * accuracy);
	}
}

BOOST_AUTO_TEST_CASE(cos16_is_within_0_69_percent) {
	double accuracy = 0.0069;
	for(uint32_t i = 0; i < 0x10000; ++ i) {
		double v = double(cos16(uint16_t(i)));
		double actual = std::cos(i * 2.0 * M_PI / 65536.0) * 32767.0;
		BOOST_CHECK_GT(v, actual - 65536 * accuracy);
		BOOST_CHECK_LT(v, actual + 65536 * accuracy);
	}
}

BOOST_AUTO_TEST_CASE(sin8_is_within_2_percent) {
	double accuracy = 0.02;
	for(uint16_t i = 0; i < 0x100; ++ i) {
		double v = double(sin8(uint8_t(i)));
		double actual = std::sin(i * 2.0 * M_PI / 256.0) * 128.0;
		BOOST_CHECK_GT(v, actual - 256 * accuracy);
		BOOST_CHECK_LT(v, actual + 256 * accuracy);
	}
}

BOOST_AUTO_TEST_CASE(cos8_is_within_2_percent) {
	double accuracy = 0.02;
	for(uint16_t i = 0; i < 0x100; ++ i) {
		double v = double(cos8(uint8_t(i)));
		double actual = std::cos(i * 2.0 * M_PI / 256.0) * 128.0;
		BOOST_CHECK_GT(v, actual - 256 * accuracy);
		BOOST_CHECK_LT(v, actual + 256 * accuracy);
	}
}

BOOST_AUTO_TEST_CASE(sin8_fine_is_within_0_69_percent) {
	double accuracy = 0.0069;
	for(uint16_t i = 0; i < 0x100; ++ i) {
		double v = double(sin8_fine(uint8_t(i)));
		double actual = std::sin(i * 2.0 * M_PI / 256.0) * 128.0;
		BOOST_CHECK_GT(v, actual - 256 * accuracy);
		BOOST_CHECK_LT(v, actual + 256 * accuracy);
	}
}

BOOST_AUTO_TEST_CASE(cos8_fine_is_within_0_69_percent) {
	double accuracy = 0.0069;
	for(uint16_t i = 0; i < 0x100; ++ i) {
		double v = double(cos8_fine(uint8_t(i)));
		double actual = std::cos(i * 2.0 * M_PI / 256.0) * 128.0;
		BOOST_CHECK_GT(v, actual - 256 * accuracy);
		BOOST_CHECK_LT(v, actual + 256 * accuracy);
	}
}

BOOST_AUTO_TEST_SUITE_END()

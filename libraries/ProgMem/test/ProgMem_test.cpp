#include <boost/test/unit_test.hpp>

#include "../ProgMem.h"

BOOST_AUTO_TEST_SUITE(ProgMem_test)

static PROGMEM const char data[] = "foo";

BOOST_AUTO_TEST_CASE(bool_conversion_checks_nullptr) {
	BOOST_CHECK_EQUAL(bool(ProgMem<char>(data)), true);
	BOOST_CHECK_EQUAL(bool(ProgMem<char>(nullptr)), false);
	BOOST_CHECK_EQUAL(bool(ProgMem<char>()), false);
}

BOOST_AUTO_TEST_CASE(raw) {
	BOOST_CHECK_EQUAL(ProgMem<char>(data).raw(), data);
}

BOOST_AUTO_TEST_CASE(copy_constructor) {
	ProgMem<char> a(data);
	ProgMem<char> b(a);
	BOOST_CHECK_EQUAL(b.raw(), data);
}

BOOST_AUTO_TEST_CASE(indexing) {
	BOOST_CHECK_EQUAL(ProgMem<char>(data)[0], 'f');
	BOOST_CHECK_EQUAL(ProgMem<char>(data)[1], 'o');
}

BOOST_AUTO_TEST_CASE(addition) {
	BOOST_CHECK_EQUAL((ProgMem<char>(data + 1) + 1).raw(), data + 2);
	BOOST_CHECK_EQUAL((ProgMem<char>(data + 1) + (-1)).raw(), data);
}

BOOST_AUTO_TEST_CASE(addition_assignment) {
	BOOST_CHECK_EQUAL((ProgMem<char>(data + 1) += 1).raw(), data + 2);
	BOOST_CHECK_EQUAL((ProgMem<char>(data + 1) += (-1)).raw(), data);
}

BOOST_AUTO_TEST_CASE(wrapping) {
	BOOST_CHECK_EQUAL(MakeProgMem(data)[0], 'f');
}

BOOST_AUTO_TEST_CASE(string_wrapping) {
	BOOST_CHECK_EQUAL(ProgMemString("hello")[0], 'h');
}

static PROGMEM const unsigned char uchar_data[] = {130, 7};

BOOST_AUTO_TEST_CASE(type_uchar) {
	BOOST_CHECK_EQUAL(MakeProgMem(uchar_data)[0], 130);
}

static PROGMEM const signed char schar_data[] = {-8, 7};

BOOST_AUTO_TEST_CASE(type_schar) {
	BOOST_CHECK_EQUAL(MakeProgMem(schar_data)[0], -8);
}

static PROGMEM const uint16_t uword_data[] = {50000, 7};

BOOST_AUTO_TEST_CASE(type_uword) {
	BOOST_CHECK_EQUAL(MakeProgMem(uword_data)[0], 50000);
}

static PROGMEM const int16_t sword_data[] = {-1000, 7};

BOOST_AUTO_TEST_CASE(type_sword) {
	BOOST_CHECK_EQUAL(MakeProgMem(sword_data)[0], -1000);
}

static PROGMEM const uint32_t uint_data[] = {3000000000, 7};

BOOST_AUTO_TEST_CASE(type_uint) {
	BOOST_CHECK_EQUAL(MakeProgMem(uint_data)[0], 3000000000);
}

static PROGMEM const int32_t sint_data[] = {-1000000000, 7};

BOOST_AUTO_TEST_CASE(type_sint) {
	BOOST_CHECK_EQUAL(MakeProgMem(sint_data)[0], -1000000000);
}

static PROGMEM const float float_data[] = {4.5f, 2.0f};

BOOST_AUTO_TEST_CASE(type_float) {
	BOOST_CHECK_EQUAL(MakeProgMem(float_data)[0], 4.5f);
}

struct obj {
	char a;
	char b;
	char c;
};

static PROGMEM const obj complex_data[] = {{5, 7, 11}, {10, 9, 8}};

BOOST_AUTO_TEST_CASE(type_complex) {
	BOOST_CHECK_EQUAL(MakeProgMem(complex_data)[0].b, 7);
}

static const float fptr_a = 1.0f;
static const float fptr_b = 2.0f;
static PROGMEM const float *const float_pointer_data[] = {&fptr_a, &fptr_b};

BOOST_AUTO_TEST_CASE(type_float_ptr) {
	BOOST_CHECK_EQUAL(MakeProgMem(float_pointer_data)[0], &fptr_a);
	BOOST_CHECK_EQUAL(MakeProgMem(float_pointer_data)[0][0], 1.0f);
}

static float fmptr_a = 1.0f;
static float fmptr_b = 2.0f;
static PROGMEM float *const malleable_float_pointer_data[] = {&fmptr_a, &fmptr_b};

BOOST_AUTO_TEST_CASE(type_float_ptr_malleable) {
	MakeProgMem(malleable_float_pointer_data)[0][0] = 1.5f;
	BOOST_CHECK_EQUAL(MakeProgMem(malleable_float_pointer_data)[0][0], 1.5f);
}

static const char cptr_a = 'j';
static const char cptr_b = 'w';
static PROGMEM const char *const char_pointer_data[] = {&cptr_a, &cptr_b};

BOOST_AUTO_TEST_CASE(type_char_ptr) {
	BOOST_CHECK_EQUAL(MakeProgMem(char_pointer_data)[0], &cptr_a);
	BOOST_CHECK_EQUAL(MakeProgMem(char_pointer_data)[0][0], 'j');
}

BOOST_AUTO_TEST_SUITE_END()

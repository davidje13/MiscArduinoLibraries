#include <boost/test/unit_test.hpp>

#include "../FontRenderer.h"
#include "MockFont.h"

BOOST_AUTO_TEST_SUITE(FontRenderer_test)

BOOST_AUTO_TEST_CASE(cursor_positioning) {
	int fakeBitmask;
	auto fontRenderer = MakeFontRenderer(&fakeBitmask, 5, 7, 11, 3);
	BOOST_CHECK_EQUAL(fontRenderer.cursor_x(), 5 + 3);
	BOOST_CHECK_EQUAL(fontRenderer.cursor_y(), 7);
}

BOOST_AUTO_TEST_CASE(print_char) {
	int fakeBitmask;
	FakeFont fakeFont;
	auto fontRenderer = MakeFontRenderer(&fakeBitmask, 5, 7, 11, 3);
	fontRenderer.print(fakeFont, 'f');
	BOOST_CHECK_EQUAL(fontRenderer.cursor_x(), 5 + 3 + 2 + 1);
	BOOST_CHECK_EQUAL(fontRenderer.cursor_y(), 7);

	BOOST_CHECK_EQUAL(fakeFont.captor->nCaptured, 1);
	BOOST_CHECK_EQUAL(fakeFont.captor->captured[0].bitmask, &fakeBitmask);
	BOOST_CHECK_EQUAL(fakeFont.captor->captured[0].c, 'f');
	BOOST_CHECK_EQUAL(fakeFont.captor->captured[0].x, 5 + 3);
	BOOST_CHECK_EQUAL(fakeFont.captor->captured[0].y, 7);
}

// TODO: test messages, line wrapping, tabs, newlines, etc.

BOOST_AUTO_TEST_SUITE_END()

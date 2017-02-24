#include <boost/test/unit_test.hpp>

#include "../Font.h"
#include "../FontVariable.h"
#include "../FontFixed.h"

BOOST_AUTO_TEST_SUITE(Font_test)

BOOST_AUTO_TEST_CASE(char_supported) {
	Font font(FONTVARIABLE_DATA, FONTVARIABLE_IMG, FONTVARIABLE_MASK);
	BOOST_CHECK_EQUAL(font.char_supported('\0'), false);
	BOOST_CHECK_EQUAL(font.char_supported('a'), true);
	BOOST_CHECK_EQUAL(font.char_supported(font.first_supported_char()), true);
	BOOST_CHECK_EQUAL(font.char_supported(font.first_supported_char() - 1), false);
	BOOST_CHECK_EQUAL(font.char_supported(font.last_supported_char()), true);
	BOOST_CHECK_EQUAL(font.char_supported(font.last_supported_char() + 1), false);
}

BOOST_AUTO_TEST_CASE(font_attributes) {
	Font font(FONTVARIABLE_DATA, FONTVARIABLE_IMG, FONTVARIABLE_MASK);
	BOOST_CHECK_EQUAL(font.spacing(), 1);
	BOOST_CHECK_EQUAL(font.height(), 8);
	BOOST_CHECK_EQUAL(font.line_height(), 8);
}

BOOST_AUTO_TEST_CASE(tabstop) {
	Font font(FONTVARIABLE_DATA, FONTVARIABLE_IMG, FONTVARIABLE_MASK);
	BOOST_CHECK_EQUAL(font.next_tabstop(0), 10);
	BOOST_CHECK_EQUAL(font.next_tabstop(9), 10);
	BOOST_CHECK_EQUAL(font.next_tabstop(10), 20);
	BOOST_CHECK_EQUAL(font.next_tabstop(11), 20);
}

BOOST_AUTO_TEST_CASE(measure_char_variable) {
	Font font(FONTVARIABLE_DATA, FONTVARIABLE_IMG, FONTVARIABLE_MASK);
	BOOST_CHECK_EQUAL(font.measure('a'), 4);
	BOOST_CHECK_EQUAL(font.measure('i'), 1);
	BOOST_CHECK_EQUAL(font.measure('\0'), 0);
}

BOOST_AUTO_TEST_CASE(measure_char_fixed) {
	Font font(FONTFIXED_DATA, FONTFIXED_IMG, FONTFIXED_MASK);
	BOOST_CHECK_EQUAL(font.measure('a'), 5);
	BOOST_CHECK_EQUAL(font.measure('i'), 5);
	BOOST_CHECK_EQUAL(font.measure('\0'), 0);
}

BOOST_AUTO_TEST_CASE(measure_message_variable) {
	Font font(FONTVARIABLE_DATA, FONTVARIABLE_IMG, FONTVARIABLE_MASK);
	BOOST_CHECK_EQUAL(font.measure("ai"), 4 + 1 + 1);
}

BOOST_AUTO_TEST_CASE(measure_message_fixed) {
	Font font(FONTFIXED_DATA, FONTFIXED_IMG, FONTFIXED_MASK);
	BOOST_CHECK_EQUAL(font.measure("ai"), 5 + 1 + 5);
}

class FakeBitmask {
public:
	struct Captured {
		int x;
		int y;
		int w;
		int h;
		int step;
		BlendMode mode;
	};

	Captured captured[8];
	int nCaptured;

	FakeBitmask(void) : captured(), nCaptured(0) {}

	template <typename T>
	void render_bitmap(T img, T mask, int x, int y, int w, int h, int step, BlendMode mode) {
		(void) img;
		(void) mask; // TODO
		captured[nCaptured].x = x;
		captured[nCaptured].y = y;
		captured[nCaptured].w = w;
		captured[nCaptured].h = h;
		captured[nCaptured].step = step;
		captured[nCaptured].mode = mode;
		++ nCaptured;
	}
};

BOOST_AUTO_TEST_CASE(render_char_variable) {
	Font font(FONTVARIABLE_DATA, FONTVARIABLE_IMG, FONTVARIABLE_MASK);
	FakeBitmask fakeBitmask;
	BOOST_CHECK_EQUAL(font.render(fakeBitmask, "ai", 5, 7), font.measure("ai"));
	BOOST_CHECK_EQUAL(fakeBitmask.nCaptured, 2);

	BOOST_CHECK_EQUAL(fakeBitmask.captured[0].x, 5);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[0].y, 7);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[0].w, 4);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[0].h, 8);
	BOOST_CHECK_EQUAL(int(fakeBitmask.captured[0].mode), int(BlendMode::ON));

	BOOST_CHECK_EQUAL(fakeBitmask.captured[1].x, 10);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[1].y, 7);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[1].w, 1);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[1].h, 8);
	BOOST_CHECK_EQUAL(int(fakeBitmask.captured[1].mode), int(BlendMode::ON));
}

BOOST_AUTO_TEST_CASE(render_char_fixed) {
	Font font(FONTFIXED_DATA, FONTFIXED_IMG, FONTFIXED_MASK);
	FakeBitmask fakeBitmask;
	BOOST_CHECK_EQUAL(font.render(fakeBitmask, "ai", 5, 7), font.measure("ai"));
	BOOST_CHECK_EQUAL(fakeBitmask.nCaptured, 2);

	BOOST_CHECK_EQUAL(fakeBitmask.captured[0].x, 5);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[0].y, 7);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[0].w, 5);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[0].h, 8);
	BOOST_CHECK_EQUAL(int(fakeBitmask.captured[0].mode), int(BlendMode::ON));

	BOOST_CHECK_EQUAL(fakeBitmask.captured[1].x, 11);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[1].y, 7);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[1].w, 5);
	BOOST_CHECK_EQUAL(fakeBitmask.captured[1].h, 8);
	BOOST_CHECK_EQUAL(int(fakeBitmask.captured[1].mode), int(BlendMode::ON));
}

BOOST_AUTO_TEST_SUITE_END()

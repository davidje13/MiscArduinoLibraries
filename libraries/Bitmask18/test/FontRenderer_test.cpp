#include <boost/test/unit_test.hpp>

#include "../FontRenderer.h"

BOOST_AUTO_TEST_SUITE(FontRenderer_test)

class FakeFont {
public:
	class FakeFontCaptor {
	public:
		struct Captured {
			int *bitmask;
			char c;
			int x;
			int y;
		};

		Captured captured[8];
		int nCaptured;

		FakeFontCaptor(void) : captured(), nCaptured(0) {}

		void capture(int &bitmask, char c, int x, int y) {
			captured[nCaptured].bitmask = &bitmask;
			captured[nCaptured].c = c;
			captured[nCaptured].x = x;
			captured[nCaptured].y = y;
			++ nCaptured;
		}
	};

	FakeFontCaptor *captor;

	FakeFont(void) : captor(new FakeFontCaptor()) {}
	~FakeFont(void) {
		delete captor;
	}

	bool char_supported(char) const {
		return true;
	}

	int measure(char) const {
		return 2;
	}

	int spacing(void) const {
		return 1;
	}

	int line_height(void) const {
		return 13;
	}

	int next_tabstop(int) const {
		return 40;
	}

	int render(int &bitmask, char c, int x, int y) const {
		captor->capture(bitmask, c, x, y);
		return measure(c);
	}
};

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

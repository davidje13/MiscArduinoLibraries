#ifndef MOCK_FONT_H_INCLUDED
#define MOCK_FONT_H_INCLUDED

#include <BaseMock.h>

class FakeFont {
public:
	class FakeFontCaptor {
	public:
		struct Captured {
			int *bitmask;
			uint8_t c;
			int16_t x;
			int16_t y;
		};

		Captured captured[8];
		int nCaptured;

		FakeFontCaptor(void) : captured(), nCaptured(0) {}

		void capture(int &bitmask, uint8_t c, int16_t x, int16_t y) {
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

	bool char_supported(uint8_t) const {
		return true;
	}

	uint8_t measure(uint8_t) const {
		return 2;
	}

	uint8_t spacing(void) const {
		return 1;
	}

	uint8_t line_height(void) const {
		return 13;
	}

	int16_t next_tabstop(int16_t) const {
		return 40;
	}

	uint16_t render(int &bitmask, uint8_t c, int16_t x, int16_t y) const {
		captor->capture(bitmask, c, x, y);
		return measure(c);
	}
};

#include <EndMocks.h>

#endif

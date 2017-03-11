#ifndef MOCK_BITMASK_H_INCLUDED
#define MOCK_BITMASK_H_INCLUDED

#include <BaseMock.h>

class FakeBitmask {
public:
	struct Captured {
		int16_t x;
		int16_t y;
		int16_t w;
		int16_t h;
		int16_t step;
		BlendMode mode;
	};

	Captured captured[8];
	int nCaptured;

	FakeBitmask(void) : captured(), nCaptured(0) {}

	template <typename T>
	void render_bitmap(
		T img, T mask,
		int16_t x, int16_t y,
		int16_t w, int16_t h,
		int16_t step,
		BlendMode mode
	) {
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

#include <EndMocks.h>

#endif

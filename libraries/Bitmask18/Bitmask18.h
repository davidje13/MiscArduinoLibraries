/*
 * Bitmask18 - Bitmask storage and drawing operations.
 * Written in 2017 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 *
 * Attributions / Acknowledgements: (also noted in source)
 * - Ellipse rendering algorithm based on a whitepaper from John Kennedy,
 *   Santa Monica College
 * - Filled-triangle rendering algorithm benefited from public domain blog
 *   posts by Fabian Giesen
 */

#ifndef BITMASK18_H_INCLUDED
#define BITMASK18_H_INCLUDED

#include "BlendMode.h"
#include "Pattern.h"

#ifdef ARDUINO
#  include <ProgMem.h>
#else
#  include <ProgMem/ProgMem.h>
#endif

#include "ext.h"

#ifdef WIDE_DATA
	typedef uint16_t ubitsz_t;
	typedef int16_t bitsz_t;
	typedef int16_t bitshiftsz_t;
	typedef int32_t bitshift2sz_t;
	typedef int32_t bitlsz_t;
#else
	typedef uint8_t ubitsz_t;
	typedef int16_t bitsz_t;
	typedef int16_t bitshiftsz_t;
	typedef int16_t bitshift2sz_t;
	typedef int32_t bitlsz_t;
#endif

template <
	ubitsz_t WIDTH,
	ubitsz_t HEIGHT,
	ubitsz_t HEIGHT_BYTES = (HEIGHT + 7) / 8
>
class Bitmask18 {
	uint8_t buffer[WIDTH * HEIGHT_BYTES];

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool in_bounds(bitsz_t x, bitsz_t y) {
		return x >= 0 && y >= 0 && ubitsz_t(x) < WIDTH && ubitsz_t(y) < HEIGHT;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline ubitsz_t pixel_byte(ubitsz_t x, ubitsz_t y) {
		return (y >> 3) * WIDTH + x;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t pixel_bit(ubitsz_t y) {
		return y & 7;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool check_tl(
		bitshiftsz_t x0, bitshiftsz_t y0,
		bitshiftsz_t x1, bitshiftsz_t y1
	) {
		return y1 > y0 || (y1 == y0 && x1 < x0);
	}

	template <typename T>
	[[gnu::pure,gnu::always_inline,gnu::nonnull]]
	static inline uint8_t read_shifted(T r0, T r1, ubitsz_t x, uint8_t shift) {
		return uint8_t((r0[x] >> (8 - shift)) | (r1[x] << shift));
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t precalc_block_A(uint8_t mask, BlendMode m) {
		return ~uint8_t((uint8_t(m) >> 1) * mask);
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t precalc_block_B(uint8_t mask, BlendMode m) {
		return uint8_t((uint8_t(m) & 1) * mask);
	}

	inline void set_pixel_block_fast(
		uint8_t A, uint8_t B,
		ubitsz_t x, ubitsz_t yBlock,
		uint8_t maskedValue
	) {
		ubitsz_t s = yBlock * WIDTH + x;
		buffer[s] = (buffer[s] & A) ^ B ^ maskedValue;
	}

	template <typename T>
	void render_image_row(
		T r0data, // nullable
		T r0mask, // nullable
		T r1data, // ignored if r0data is null
		T r1mask, // ignored if r0mask is null
		ubitsz_t x0, ubitsz_t w, ubitsz_t yBlock,
		uint8_t shift, uint8_t mask,
		BlendMode m
	) {
		if(!r0data) {
			for(ubitsz_t x = 0; x < w; ++ x) {
				uint8_t M = mask & read_shifted(r0mask, r1mask, x, shift);
				uint8_t A = precalc_block_A(M, m);
				uint8_t B = precalc_block_B(M, m);
				set_pixel_block_fast(A, B, x + x0, yBlock, mask);
			}
		} else if(!r0mask) {
			uint8_t A = precalc_block_A(mask, m);
			uint8_t B = precalc_block_B(mask, m);
			for(ubitsz_t x = 0; x < w; ++ x) {
				uint8_t v = read_shifted(r0data, r1data, x, shift);
				set_pixel_block_fast(A, B, x + x0, yBlock, v & mask);
			}
		} else {
			for(ubitsz_t x = 0; x < w; ++ x) {
				uint8_t M = mask & read_shifted(r0mask, r1mask, x, shift);
				uint8_t A = precalc_block_A(M, m);
				uint8_t B = precalc_block_B(M, m);
				uint8_t v = read_shifted(r0data, r1data, x, shift);
				// Do not mask v with M; use mask instead (allows XOR)
				set_pixel_block_fast(A, B, x + x0, yBlock, v & mask);
			}
		}
	}

	// TODO: rendering with BlendMode::OFF seems to set outside mask area white
	template <typename T> // T = ProgMem<uint8_t> / const uint8_t*
	void render_bitmap_b(
		T data,
		T maskImg,
		bitsz_t x, bitsz_t y, bitsz_t w, bitsz_t h, bitsz_t step,
		BlendMode m
	) {
		if(!data && !maskImg) {
			fill_rect(x, y, w, h, m, PATTERN_ON);
			return;
		}

		bitsz_t s = (step == 0) ? w : step;

		if(w <= 0 || h <= 0 || x + w <= 0 || y + h <= 0 || x >= WIDTH || y >= HEIGHT) {
			return;
		}

		T bd = data;
		T bm = maskImg;

		ubitsz_t x0;
		ubitsz_t x1 = ubitsz_t(ext::min2(bitsz_t(x + w), bitsz_t(WIDTH)));
		ubitsz_t y1 = ubitsz_t(ext::min2(bitsz_t(y + h), bitsz_t(HEIGHT)));
		uint8_t shift = y & 7;

		if(x < 0) {
			bd = safe_subdata(bd, -x);
			bm = safe_subdata(bm, -x);
			x0 = 0;
		} else {
			x0 = ubitsz_t(x);
		}
		ubitsz_t ww = x1 - x0;
		ubitsz_t yBlock1 = y1 / 8;
		ubitsz_t yBlock0;
		ubitsz_t yBlock;

		if(y < 0) {
			bitsz_t adjust = ((7 - y) / 8) * step;
			bd = safe_subdata(bd, adjust);
			bm = safe_subdata(bm, adjust);
			yBlock0 = 0;
			yBlock = 0;
		} else {
			yBlock0 = ubitsz_t(y / 8);

			uint8_t mask = uint8_t(0xFF << shift);
			if(yBlock0 == yBlock1) {
				mask ^= 0xFF << (y1 & 7);
			}
			// Don't care about row duplication; mask will mask it out
			render_image_row(
				bd, bm,
				bd, bm,
				x0, ww, yBlock0, shift, mask, m
			);

			if(yBlock0 == yBlock1) {
				return;
			}

			yBlock = yBlock0 + 1;
		}

		for(; yBlock != yBlock1; ++ yBlock) {
			bitsz_t locn = (yBlock - yBlock0) * s;
			render_image_row(
				safe_subdata(bd, locn - s),
				safe_subdata(bm, locn - s),
				bd + locn,
				bm + locn,
				x0, ww, yBlock, shift, 0xFF, m
			);
		}

		if(y1 & 7) {
			uint8_t mask = ~uint8_t(0xFF << (y1 & 7));
			bitsz_t locn = (yBlock - yBlock0) * s;
			// On the Arduino, this check isn't strictly needed, since it
			// doesn't care about invalid memory reads and the data will get
			// masked out anyway, but it's not a costly operation so it's here
			// to be safe & portable.
			bool beyondMemory = ((yBlock - yBlock0) * 8 >= h);
			render_image_row(
				safe_subdata(bd, locn - s),
				safe_subdata(bm, locn - s),
				bd + (locn - s * beyondMemory),
				bm + (locn - s * beyondMemory),
				x0, ww, yBlock, shift, mask, m
			);
		}
	}

public:
	[[gnu::always_inline]]
	inline Bitmask18(void) : buffer() {}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline ubitsz_t width(void) {
		return WIDTH;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline ubitsz_t height(void) {
		return HEIGHT;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline ubitsz_t height_bytes(void) {
		return HEIGHT_BYTES;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline ubitsz_t raw_step(void) {
		return WIDTH;
	}

	[[gnu::pure,nodiscard,gnu::always_inline,gnu::returns_nonnull]]
	inline const uint8_t *raw(void) const {
		return buffer;
	}

	[[gnu::pure,nodiscard,gnu::always_inline,gnu::returns_nonnull]]
	inline uint8_t *raw(void) {
		return buffer;
	}

	inline void clear(uint8_t value = 0) {
		memset(buffer, value, sizeof(buffer));
	}

	[[gnu::always_inline]]
	inline void clear(BlendMode value) {
		clear((value == BlendMode::ON) ? 0xFF : 0x00);
	}

	[[gnu::always_inline]]
	inline void pixel_on_fast(ubitsz_t x, ubitsz_t y) {
		buffer[pixel_byte(x, y)] |= 1 << pixel_bit(y);
	}

	[[gnu::always_inline]]
	inline void pixel_off_fast(ubitsz_t x, ubitsz_t y) {
		buffer[pixel_byte(x, y)] &= ~(1 << pixel_bit(y));
	}

	[[gnu::always_inline]]
	inline void pixel_toggle_fast(ubitsz_t x, ubitsz_t y) {
		buffer[pixel_byte(x, y)] ^= 1 << pixel_bit(y);
	}

	inline void set_pixel_fast(
		ubitsz_t x, ubitsz_t y,
		BlendMode m, Pattern p = PATTERN_ON
	) {
		uint8_t &v = buffer[pixel_byte(x, y)];
		uint8_t n = pixel_bit(y);
		v = (
			(v & ~((uint8_t(m) >> 1) << n)) ^
			((((uint8_t(m) & 1) * 0xFF) ^ PatternCol(p, x)) & (1 << n))
		);
	}

	inline void set_pixel_block_fast(
		ubitsz_t x, ubitsz_t yBlock,
		uint8_t mask,
		BlendMode m, Pattern p
	) {
		uint8_t A = precalc_block_A(mask, m);
		uint8_t B = precalc_block_B(0xFF, m);
		ubitsz_t s = yBlock * WIDTH + x;
		buffer[s] = (buffer[s] & A) ^ ((B ^ PatternCol(p, x)) & mask);
	}

	void set_pixel_blocks_fast(
		ubitsz_t x, ubitsz_t w, ubitsz_t yBlock,
		uint8_t mask,
		BlendMode m, Pattern p
	) {
		uint8_t A = precalc_block_A(mask, m);
		uint8_t B = precalc_block_B(0xFF, m);
		ubitsz_t s = yBlock * WIDTH + x;
		for(ubitsz_t i = 0; i < w; ++ i) {
			buffer[s+i] = (buffer[s+i] & A) ^ ((B ^ PatternCol(p, x + i)) & mask);
		}
	}

	inline void pixel_on(bitsz_t x, bitsz_t y) {
		if(in_bounds(x, y)) {
			pixel_on_fast(ubitsz_t(x), ubitsz_t(y));
		}
	}

	inline void pixel_off(bitsz_t x, bitsz_t y) {
		if(in_bounds(x, y)) {
			pixel_off_fast(ubitsz_t(x), ubitsz_t(y));
		}
	}

	inline void pixel_toggle(bitsz_t x, bitsz_t y) {
		if(in_bounds(x, y)) {
			pixel_toggle_fast(ubitsz_t(x), ubitsz_t(y));
		}
	}

	inline void set_pixel(
		bitsz_t x,
		bitsz_t y,
		BlendMode m,
		Pattern p = PATTERN_ON
	) {
		if(in_bounds(x, y)) {
			set_pixel_fast(ubitsz_t(x), ubitsz_t(y), m, p);
		}
	}

	[[gnu::pure,nodiscard]]
	inline bool get_pixel(bitsz_t x, bitsz_t y) const {
		if(!in_bounds(x, y)) {
			return false;
		}
		return (buffer[pixel_byte(x, y)] >> pixel_bit(y)) & 1;
	}

	void fill_rect_fast(
		ubitsz_t x0, ubitsz_t y0,
		ubitsz_t x1, ubitsz_t y1,
		BlendMode m,
		Pattern p
	) {
		ubitsz_t w = x1 - x0;

		ubitsz_t yBlock0 = y0 >> 3;
		ubitsz_t yBlock1 = y1 >> 3;
		if(yBlock0 == yBlock1) {
			uint8_t mask = (
				uint8_t(0xFF << (y0 & 7)) ^
				uint8_t(0xFF << (y1 & 7))
			);
			set_pixel_blocks_fast(x0, w, yBlock0, mask, m, p);
			return;
		}

		set_pixel_blocks_fast(x0, w, yBlock0, uint8_t(0xFF << (y0 & 7)), m, p);
		for(ubitsz_t yBlock = yBlock0 + 1; yBlock != yBlock1; ++ yBlock) {
			set_pixel_blocks_fast(x0, w, yBlock, 0xFF, m, p);
		}
		if(y1 & 7) {
			set_pixel_blocks_fast(x0, w, yBlock1, ~uint8_t(0xFF << (y1 & 7)), m, p);
		}
	}

	void fill_rect(
		bitsz_t x, bitsz_t y,
		bitsz_t w, bitsz_t h,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON
	) {
		if(
			w <= 0 || h <= 0 ||
			x + w <= 0 || y + h <= 0 ||
			x >= bitsz_t(WIDTH) || y >= bitsz_t(HEIGHT)
		) {
			return;
		}

		fill_rect_fast(
			ubitsz_t(ext::max2(x, bitsz_t(0))),
			ubitsz_t(ext::max2(y, bitsz_t(0))),
			ubitsz_t(ext::min2(bitsz_t(x + w), bitsz_t(WIDTH))),
			ubitsz_t(ext::min2(bitsz_t(y + h), bitsz_t(HEIGHT))),
			m, p
		);
	}

	void outline_rect(
		bitsz_t x, bitsz_t y,
		bitsz_t w, bitsz_t h,
		ubitsz_t thickness = 1,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON
	) {
		if(w <= 0 || h <= 0 || thickness <= 0) {
			return;
		}
		bitsz_t t2 = thickness * 2;
		if(w <= t2 || h <= t2) {
			fill_rect(x, y, w, h, m, p);
			return;
		}

		fill_rect(x, y, thickness, h, m, p);
		fill_rect(x + thickness, y, w - t2, thickness, m, p);
		fill_rect(x + thickness, y + h - thickness, w - t2, thickness, m, p);
		fill_rect(x + w - thickness, y, thickness, h, m, p);
	}

	[[gnu::always_inline]]
	inline void hline(
		bitsz_t x0,
		bitsz_t y,
		bitsz_t x1,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON
	) {
		bitsz_t l = ext::min2(x0, x1);
		bitsz_t r = x0 + x1 - l + 1;
		fill_rect(l, y, r - l, 1, m, p);
	}

	[[gnu::always_inline]]
	inline void vline(
		bitsz_t x,
		bitsz_t y0,
		bitsz_t y1,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON
	) {
		bitsz_t t = ext::min2(y0, y1);
		bitsz_t b = y0 + y1 - t + 1;
		fill_rect(x, t, 1, b - t, m, p);
	}

	void line(
		bitshiftsz_t x0, bitshiftsz_t y0,
		bitshiftsz_t x1, bitshiftsz_t y1,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON,
		uint8_t shift = 0
	) {
		bitshiftsz_t lowx = ext::min2(x0, x1);
		bitshiftsz_t lowy = ext::min2(y0, y1);

		if(
			(x0 + x1 - lowx) < 0 ||
			(y0 + y1 - lowy) < 0 ||
			lowx >= (bitshiftsz_t(WIDTH) << shift) ||
			lowy >= (bitshiftsz_t(HEIGHT) << shift)
		) {
			return;
		}

		// Bresenham's line algorithm

		bitshiftsz_t dx = x1 - x0;
		bitshiftsz_t dy = y1 - y0;
		bitshiftsz_t dA;
		bitshiftsz_t dB;
		bitshiftsz_t a;
		bitshiftsz_t b;
		bitshiftsz_t lA;
		bitshiftsz_t lB;
		bool yDominant = abs(dy) > abs(dx);
		if(yDominant) {
			dA = dy;
			dB = dx;
			a = lowy;
			b = ((dA < 0) ? x1 : x0);
			lA = ext::min2(
				bitshiftsz_t(((y0 + y1 - lowy) >> shift) + 1),
				bitshiftsz_t(HEIGHT)
			);
			lB = WIDTH;
		} else {
			dA = dx;
			dB = dy;
			a = lowx;
			b = ((dA < 0) ? y1 : y0);
			lA = ext::min2(
				bitshiftsz_t(((x0 + x1 - lowx) >> shift) + 1),
				bitshiftsz_t(WIDTH)
			);
			lB = HEIGHT;
		}
		if(dA < 0) {
			dA = -dA;
			dB = -dB;
		}

		bitshiftsz_t D = dA * ((dB < 0) ? 1 : -1);
		if(shift > 0) {
			bitshiftsz_t mask = bitshiftsz_t((bitshiftsz_t(1) << shift) - 1);
			D += (dB - dA
				+ (((b & mask) * dA) >> (shift - 1))
				- (((a & mask) * dB) >> (shift - 1))
			);
		}
		a >>= shift;
		b >>= shift;

		dA <<= 1;
		dB <<= 1;

		if(a < 0) {
			// TODO: convert this to 16-bit operations
			bitlsz_t D2 = D - bitlsz_t(dB) * a;
			a = 0;
			b += D2 / dA;
			D = D2 % dA;
			if((dB >= 0 && b >= lB) || (dB < 0 && b < 0)) {
				return;
			}
		}

		if(dB >= 0) {
			for(; a < lA; ++ a) {
				if(D >= 0) {
					D -= dA;
					if((++ b) == lB) { // TODO: pre-calculate matching lA for speed
						break;
					}
				}
				if(b >= 0) { // TODO: pre-calculate matching initial a for speed
					if(yDominant) {
						set_pixel_fast(b, a, m, p);
					} else {
						set_pixel_fast(a, b, m, p);
					}
				}
				D += dB;
			}
		} else {
			for(; a < lA; ++ a) {
				if(D < 0) {
					D += dA;
					if((-- b) == -1) { // TODO: pre-calculate matching lA for speed
						break;
					}
				}
				if(b < lB) { // TODO: pre-calculate matching initial a for speed
					if(yDominant) {
						set_pixel_fast(b, a, m, p);
					} else {
						set_pixel_fast(a, b, m, p);
					}
				}
				D += dB;
			}
		}
	}

	void fill_ellipse(
		bitsz_t x, bitsz_t y,
		bitsz_t w, bitsz_t h,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON
	) {
		if(
			w <= 0 || h <= 0 ||
			x + w <= 0 || y + h <= 0 ||
			x >= WIDTH || y >= HEIGHT
		) {
			return;
		}

		if(w <= 2 || h <= 2) {
			fill_rect(x, y, w, h, m, p);
			return;
		}

		// Thanks, http://dai.fmph.uniba.sk/upload/0/01/Ellipse.pdf

		bitsz_t W = w - 1;
		bitsz_t H = h - 1;
		bitsz_t cx = x + (W >> 1);
		bitsz_t cy = y + (H >> 1);
		uint8_t wodd = W & 1;
		uint8_t yinc = (H & 1) + 1;

		fill_rect(cx, y, 1 + wodd, h, m, p);

		ubitsz_t X = 0;
		ubitsz_t Y = ubitsz_t(H >> 1);
		bitlsz_t xc = H * bitlsz_t(H);
		bitlsz_t yc = W * bitlsz_t(W) * (1 - H);
		bitlsz_t ww2 = W * bitlsz_t(W) * 2;
		bitlsz_t hh2 = H * bitlsz_t(H) * 2;
		bitlsz_t error = 0;
		ubitsz_t xlim = ubitsz_t(W >> 1);

		while(true) {
			++ X;
			error += xc;
			if(error * 2 + yc > 0 && Y != 0) {
				-- Y;
				error += yc;
				yc += ww2;
				if(error * 2 + yc > 0 && Y != 0) {
					break;
				}
			}
			fill_rect(cx - X, cy - Y, 1, bitsz_t(Y * 2 + yinc), m, p);
			fill_rect(cx + X + wodd, cy - Y, 1, bitsz_t(Y * 2 + yinc), m, p);
			if(X == xlim) {
				return;
			}
			xc += hh2;
		}

		while(Y != 0) {
			-- Y;
			error += yc;
			yc += ww2;
			if(error * 2 - xc < 0) {
				fill_rect(cx - X, cy - Y, 1, bitsz_t(Y * 2 + yinc), m, p);
				fill_rect(cx + X + wodd, cy - Y, 1, bitsz_t(Y * 2 + yinc), m, p);
				++ X;
				error += (xc += hh2);
			}
		}
	}

	void outline_ellipse(
		bitsz_t x, bitsz_t y,
		bitsz_t w, bitsz_t h,
		bitsz_t thickness = 1,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON
	) {
		if(
			w <= 0 || h <= 0 ||
			x + w <= 0 || y + h <= 0 ||
			x >= WIDTH || y >= HEIGHT ||
			thickness <= 0
		) {
			return;
		}

		bitsz_t t2 = thickness * 2;
		if(w <= t2 || h <= t2) {
			fill_ellipse(x, y, w, h, m, p);
			return;
		}

		if(thickness != 1) {
			// TODO
			return;
		}

		bitsz_t W = w - 1;
		bitsz_t H = h - 1;
		bitsz_t cx = x + (W >> 1);
		bitsz_t cy = y + (H >> 1);
		uint8_t wodd = W & 1;
		uint8_t yodd = H & 1;

		set_pixel(cx, y, m, p);
		set_pixel(cx, y + H, m, p);
		if(wodd) {
			set_pixel(cx + 1, y, m, p);
			set_pixel(cx + 1, y + H, m, p);
		}

		ubitsz_t X = 0;
		ubitsz_t Y = ubitsz_t(H >> 1);
		bitlsz_t xc = H * bitlsz_t(H);
		bitlsz_t yc = W * bitlsz_t(W) * (1 - H);
		bitlsz_t ww2 = W * bitlsz_t(W) * 2;
		bitlsz_t hh2 = H * bitlsz_t(H) * 2;
		bitlsz_t error = 0;
		ubitsz_t xlim = ubitsz_t(W >> 1);

		while(true) {
			++ X;
			error += xc;
			if(error * 2 + yc > 0 && Y != 0) {
				-- Y;
				error += yc;
				yc += ww2;
				if(error * 2 + yc > 0 && Y != 0) {
					break;
				}
			}
			if(X == xlim) {
				fill_rect(cx - X, cy - Y, 1, bitsz_t(Y * 2 + yodd + 1), m, p);
				fill_rect(cx + X + wodd, cy - Y, 1, bitsz_t(Y * 2 + yodd + 1), m, p);
				return;
			}
			set_pixel(cx - X, cy - Y, m, p);
			set_pixel(cx + X + wodd, cy - Y, m, p);
			if(Y | yodd) {
				set_pixel(cx - X, cy + Y + yodd, m, p);
				set_pixel(cx + X + wodd, cy + Y + yodd, m, p);
			}
			xc += hh2;
		}

		-- X;
		while(true) {
			set_pixel(cx - X, cy - Y, m, p);
			set_pixel(cx + X + wodd, cy - Y, m, p);
			if(Y | yodd) {
				set_pixel(cx - X, cy + Y + yodd, m, p);
				set_pixel(cx + X + wodd, cy + Y + yodd, m, p);
			}
			if(Y == 0) {
				return;
			}
			-- Y;
			error += yc;
			yc += ww2;
			if(error * 2 - xc < 0) {
				++ X;
				error += (xc += hh2);
			}
		}
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline bool is_ccw_triangle(
		bitshiftsz_t x0, bitshiftsz_t y0,
		bitshiftsz_t x1, bitshiftsz_t y1,
		bitshiftsz_t x2, bitshiftsz_t y2
	) {
		return (
			bitlsz_t(x1 - x0) * (y2 - y0) >
			bitlsz_t(y1 - y0) * (x2 - x0)
		);
	}

	void outline_triangle(
		bitshiftsz_t x0, bitshiftsz_t y0,
		bitshiftsz_t x1, bitshiftsz_t y1,
		bitshiftsz_t x2, bitshiftsz_t y2,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON,
		uint8_t shift = 0
	) {
		// TODO: line includes both end pixels, which leads to overdrawing here.
		// The line function should probably be updated to only draw up to the
		// last pixels which have their midpoints covered, with a <= vs < for
		// start and end, but this will make the typical line use less
		// intuitive.
		line(x0, y0, x1, y1, m, p, shift);
		line(x1, y1, x2, y2, m, p, shift);
		line(x2, y2, x0, y0, m, p, shift);
	}

	void outline_cw_triangle(
		bitshiftsz_t x0, bitshiftsz_t y0,
		bitshiftsz_t x1, bitshiftsz_t y1,
		bitshiftsz_t x2, bitshiftsz_t y2,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON,
		uint8_t shift = 0
	) {
		if(!is_ccw_triangle(x0, y0, x1, y1, x2, y2)) {
			outline_triangle(x0, y0, x1, y1, x2, y2, m, p, shift);
		}
	}

	void outline_ccw_triangle(
		bitshiftsz_t x0, bitshiftsz_t y0,
		bitshiftsz_t x1, bitshiftsz_t y1,
		bitshiftsz_t x2, bitshiftsz_t y2,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON,
		uint8_t shift = 0
	) {
		if(is_ccw_triangle(x0, y0, x1, y1, x2, y2)) {
			outline_triangle(x0, y0, x1, y1, x2, y2, m, p, shift);
		}
	}

	void fill_ccw_triangle(
		bitshiftsz_t x0, bitshiftsz_t y0,
		bitshiftsz_t x1, bitshiftsz_t y1,
		bitshiftsz_t x2, bitshiftsz_t y2,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON,
		uint8_t shift = 0
	) {
		// Thanks, https://fgiesen.wordpress.com/2013/02/17/optimizing-sw-occlusion-culling-index/

		// TODO: this is sloooow

		if(!is_ccw_triangle(x0, y0, x1, y1, x2, y2)) {
			return;
		}

		bitshiftsz_t minx = ext::max2(ext::min3(x0, x1, x2), bitshiftsz_t(0));
		bitshiftsz_t miny = ext::max2(ext::min3(y0, y1, y2), bitshiftsz_t(0));
		bitshiftsz_t maxx = ext::min2(
			ext::max3(x0, x1, x2),
			bitshiftsz_t((bitshiftsz_t(WIDTH) << shift) - 1)
		);
		bitshiftsz_t maxy = ext::min2(
			ext::max3(y0, y1, y2),
			bitshiftsz_t((bitshiftsz_t(HEIGHT) << shift) - 1)
		);
		if(maxx < minx || maxy < miny) {
			return;
		}

		bitsz_t yL = bitsz_t((miny >> shift) >> 3);
		bitsz_t yH = bitsz_t(((maxy >> shift) >> 3) + 1);
		bitsz_t xL = bitsz_t(minx >> shift);
		bitsz_t xH = bitsz_t((maxx >> shift) + 1);

		bitshift2sz_t sA01 = y1 - y0;
		bitshift2sz_t sB01 = x0 - x1;
		bitshift2sz_t sA12 = y2 - y1;
		bitshift2sz_t sB12 = x1 - x2;
		bitshift2sz_t sA20 = y0 - y2;
		bitshift2sz_t sB20 = x2 - x0;

		bitshiftsz_t px = bitshiftsz_t((bitshiftsz_t(xL) << shift) + (1 << shift) / 2);
		bitshiftsz_t py = bitshiftsz_t(((bitshiftsz_t(yL) << 3) << shift) + (1 << shift) / 2);
		bitshift2sz_t e010 = (
			+ sA01 * px
			+ sB01 * py
			+ bitshift2sz_t(y0) * x1
			- bitshift2sz_t(x0) * y1
			- check_tl(x0, y0, x1, y1)
		);
		bitshift2sz_t e120 = (
			+ sA12 * px
			+ sB12 * py
			+ bitshift2sz_t(y1) * x2
			- bitshift2sz_t(x1) * y2
			- check_tl(x1, y1, x2, y2)
		);
		bitshift2sz_t e200 = (
			+ sA20 * px
			+ sB20 * py
			+ bitshift2sz_t(y2) * x0
			- bitshift2sz_t(x2) * y0
			- check_tl(x2, y2, x0, y0)
		);

		sA01 <<= shift;
		sA12 <<= shift;
		sA20 <<= shift;
		sB01 <<= shift;
		sB12 <<= shift;
		sB20 <<= shift;

		e010 -= sA01 + sB01;
		e120 -= sA12 + sB12;
		e200 -= sA20 + sB20;

		for(bitsz_t x = xL; x < xH; ++ x) {
			bitshift2sz_t e01 = (e010 += sA01);
			bitshift2sz_t e12 = (e120 += sA12);
			bitshift2sz_t e20 = (e200 += sA20);
			for(bitsz_t y = yL; y < yH; ++ y) {
				// Manually unrolled loop, since at Arduino's default -Os,
				// the compiler won't unroll it for us, and this is a hot loop.
				uint8_t mask
				      = uint8_t((((e01 += sB01) & (e12 += sB12) & (e20 += sB20)) < 0) << 0);
				mask |= uint8_t((((e01 += sB01) & (e12 += sB12) & (e20 += sB20)) < 0) << 1);
				mask |= uint8_t((((e01 += sB01) & (e12 += sB12) & (e20 += sB20)) < 0) << 2);
				mask |= uint8_t((((e01 += sB01) & (e12 += sB12) & (e20 += sB20)) < 0) << 3);
				mask |= uint8_t((((e01 += sB01) & (e12 += sB12) & (e20 += sB20)) < 0) << 4);
				mask |= uint8_t((((e01 += sB01) & (e12 += sB12) & (e20 += sB20)) < 0) << 5);
				mask |= uint8_t((((e01 += sB01) & (e12 += sB12) & (e20 += sB20)) < 0) << 6);
				mask |= uint8_t((((e01 += sB01) & (e12 += sB12) & (e20 += sB20)) < 0) << 7);
				set_pixel_block_fast(x, y, mask, m, p);
			}
		}
	}

	[[gnu::always_inline]]
	inline void fill_cw_triangle(
		bitshiftsz_t x0, bitshiftsz_t y0,
		bitshiftsz_t x1, bitshiftsz_t y1,
		bitshiftsz_t x2, bitshiftsz_t y2,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON,
		uint8_t shift = 0
	) {
		fill_ccw_triangle(x0, y0, x2, y2, x1, y1, m, p, shift);
	}

	inline void fill_triangle(
		bitshiftsz_t x0, bitshiftsz_t y0,
		bitshiftsz_t x1, bitshiftsz_t y1,
		bitshiftsz_t x2, bitshiftsz_t y2,
		BlendMode m = BlendMode::ON,
		Pattern p = PATTERN_ON,
		uint8_t shift = 0
	) {
		if(is_ccw_triangle(x0, y0, x1, y1, x2, y2)) {
			fill_ccw_triangle(x0, y0, x1, y1, x2, y2, m, p, shift);
		} else {
			fill_ccw_triangle(x0, y0, x2, y2, x1, y1, m, p, shift);
		}
	}

	[[gnu::nonnull]]
	void render_fs_bitmap(const uint8_t *data) {
		memcpy(buffer, data, WIDTH * HEIGHT_BYTES);
	}

	void render_fs_bitmap(ProgMem<uint8_t> data) {
		memcpy_P(buffer, data.raw(), WIDTH * HEIGHT_BYTES);
	}

	void render_bitmap(
		const uint8_t *data, // nullable
		const uint8_t *maskImg, // nullable
		bitsz_t x, bitsz_t y, bitsz_t w, bitsz_t h, bitsz_t step = 0,
		BlendMode m = BlendMode::ON
	) {
		render_bitmap_b(data, maskImg, x, y, w, h, step, m);
	}

	void render_bitmap(
		ProgMem<uint8_t> data, // nullable
		ProgMem<uint8_t> maskImg, // nullable
		bitsz_t x, bitsz_t y, bitsz_t w, bitsz_t h, bitsz_t step = 0,
		BlendMode m = BlendMode::ON
	) {
		render_bitmap_b(data, maskImg, x, y, w, h, step, m);
	}
};

#endif

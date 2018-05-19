/*
 * LCDMatrix88 - Sends data to an 8x8 LCD matrix powered by two 74HC595 chips
 * (e.g. the Adeept 8x8 LED Matrix Module)
 * Written in 2018 by David Evans
 *
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef LCDMATRIX88_H_INCLUDED
#define LCDMATRIX88_H_INCLUDED

#ifdef ARDUINO
#  include <ProgMem.h>
#else
#  include <ProgMem/ProgMem.h>
#endif

#include "ext.h"

static PROGMEM const uint32_t FLICKER[] = {
	0x00000000, //  0/32
	0x04000000, //  1/32
	0x10101010, //  4/32
	0x8A8A8A8A, // 12/32
	0xFFFFFFFF, // 32/32

	0x00000000, //  0/32
	0x04000000, //  1/32
	0x10101010, //  4/32
	0x88888888, //  8/32
	0x8A8A8A8A, // 12/32
	0x55555555, // 16/32
	0x75757575, // 20/32
	0xDDDDDDDD, // 24/32
	0x7F7F7F7F, // 28/32
	0xFFFFFFFF, // 32/32

	0x00000000, //  0/32
	0x04000000, //  1/32
	0x00040004, //  2/32
	0x10101010, //  4/32
	0x11011101, //  6/32
	0x88888888, //  8/32
	0x8A888A88, // 10/32
	0x8A8A8A8A, // 12/32
	0xAAA8AAA8, // 14/32
	0x55555555, // 16/32
	0x57555755, // 18/32
	0x75757575, // 20/32
	0x77577757, // 22/32
	0xDDDDDDDD, // 24/32
	0xDFDDDFDD, // 26/32
	0x7F7F7F7F, // 28/32
	0xFFFFFFFF  // 32/32
};

class LCDMatrix88 {
protected:
	LCDMatrix88(void) = default;
	LCDMatrix88(const LCDMatrix88&) = delete;
	LCDMatrix88(LCDMatrix88&&) = default;

	LCDMatrix88 &operator=(const LCDMatrix88&) = delete;
	LCDMatrix88 &operator=(LCDMatrix88&&) = delete;
};

template <typename RegisterT>
class LCDMatrix88_impl : public LCDMatrix88 {
	RegisterT reg;

public:
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t width(void) {
		return 8;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t height(void) {
		return 8;
	}

	[[gnu::always_inline]]
	inline void send_mask(uint8_t colMask, uint8_t rowMask) {
		reg.send16_msb(rowMask, 0xFF ^ colMask);
	}

	[[gnu::always_inline]]
	inline void display_row(uint8_t colMask, uint8_t row) {
		send_mask(colMask, 1 << row);
	}

	[[gnu::always_inline]]
	inline void display_column(uint8_t rowMask, uint8_t col) {
		send_mask(1 << col, rowMask);
	}

	[[gnu::always_inline]]
	inline void display_hline(
		uint8_t row,
		uint8_t colStart = 0,
		uint8_t colEnd = 8
	) {
		send_mask((0xFF << colStart) ^ (0xFF << colEnd), 1 << row);
	}

	[[gnu::always_inline]]
	inline void display_vline(
		uint8_t col,
		uint8_t rowStart = 0,
		uint8_t rowEnd = 8
	) {
		send_mask(1 << col, (0xFF << rowStart) ^ (0xFF << rowEnd));
	}

	[[gnu::always_inline]]
	inline void display_box(
		uint8_t colStart,
		uint8_t rowStart,
		uint8_t colEnd,
		uint8_t rowEnd
	) {
		send_mask(
			(0xFF << colStart) ^ (0xFF << colEnd),
			(0xFF << rowStart) ^ (0xFF << rowEnd)
		);
	}

	[[gnu::always_inline]]
	inline void display_point(uint8_t x, uint8_t y) {
		send_mask(1 << x, 1 << y);
	}

	[[gnu::always_inline]]
	inline void fill(void) {
		send_mask(0xFF, 0xFF);
	}

	[[gnu::always_inline]]
	inline void blank(void) {
		send_mask(0x00, 0x00);
	}

	template <typename Bitmask>
	void flash_noblank(
		const Bitmask &bitmask,
		uint8_t sourceX = 0,
		uint8_t sourceY = 0
	) {
		int16_t step = bitmask.raw_step();
		auto raw = bitmask.raw() + (sourceX + (sourceY >> 3) * step);
		for(uint8_t x = 0; x < 8; ++ x) {
			// TODO: interpolate between y blocks when sourceY % 8 != 0
			display_column(raw[x], x);
		}
	}

	template <typename Bitmask>
	[[gnu::always_inline]]
	inline void flash(
		const Bitmask &bitmask,
		uint8_t sourceX = 0,
		uint8_t sourceY = 0
	) {
		flash_noblank(bitmask, sourceX, sourceY);
		blank();
	}

	template <uint8_t scale, uint8_t flicker0, typename Bitmask>
	void send_grayscale(
		const Bitmask &bitmask,
		uint16_t loops = 128
	) {
		int16_t step = bitmask.raw_step();
		auto raw = bitmask.raw();
		uint8_t gray[64];
		for(uint8_t x = 0; x < 8; ++ x) {
			for(uint8_t y = 0; y < 8; ++ y) {
				uint8_t count = 0;
				uint8_t p0 = step * ((y * scale) / 8) + x * scale;
				for(uint8_t p = 0; p < scale; ++ p) {
					uint8_t v = (
						(raw[p0 + p] | (raw[p0 + p + step] << 8)) >>
						((y * scale) & 7)
					);
					for(uint8_t q = 0; q < scale; ++ q) {
						count += v & 1;
						v >>= 1;
					}
				}
				gray[y*8+x] = count;
			}
		}

		auto flicker = MakeProgMem(FLICKER);
		for(uint16_t loop = 0; loop < loops; ++ loop) {
			uint8_t frame = loop % 32;
			for(uint8_t x = 0; x < 8; ++ x) {
				uint8_t col = 0;
				for(uint8_t y = 8; (y --) > 0;) {
					col <<= 1;
					col |= (flicker[flicker0+gray[y*8+x]] >> frame) & 1;
				}
				display_column(col, x);
			}
		}
	}

	template <typename Bitmask>
	void send(
		const Bitmask &bitmask,
		uint8_t xPos = 0, // not supported
		uint8_t yPos = 0, // not supported
		uint16_t loops = 128
	) {
		(void) xPos;
		(void) yPos;
		if(bitmask.width() == 16 && bitmask.height() == 16) {
			send_grayscale<2, 0>(bitmask, loops);
		} else if(bitmask.width() == 24 && bitmask.height() == 24) {
			send_grayscale<3, 5>(bitmask, loops);
		} else if(bitmask.width() == 32 && bitmask.height() == 32) {
			send_grayscale<4, 15>(bitmask, loops);
		} else {
			for(uint16_t loop = 0; loop < loops; ++ loop) {
				flash_noblank(bitmask);
			}
		}
		blank();
	}

	LCDMatrix88_impl(RegisterT &&reg)
		: reg(static_cast<RegisterT&&>(reg))
	{
		blank();
	}

	LCDMatrix88_impl(LCDMatrix88_impl &&b)
		: reg(static_cast<RegisterT&&>(b.reg))
	{}
};

template <typename RegisterT>
[[gnu::always_inline]]
inline LCDMatrix88_impl<RegisterT> MakeLCDMatrix88(RegisterT &&reg) {
	return LCDMatrix88_impl<RegisterT>(static_cast<RegisterT&&>(reg));
}

#endif

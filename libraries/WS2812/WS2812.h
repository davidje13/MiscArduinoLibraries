/*
 * WS2812 - "NeoPixel" communication library.
 * Written in 2018 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef WS2812_H_INCLUDED
#define WS2812_H_INCLUDED

#include "ext.h"

// https://cpldcpu.wordpress.com/2014/06/16/timing-of-ws2812-clones-pd9823/

#define NOP_1 __asm__ __volatile__ ("nop")
#define NOP_2 NOP_1; NOP_1
#define NOP_3 NOP_2; NOP_1
#define NOP_4 NOP_2; NOP_2
#define NOP_5 NOP_4; NOP_1
#define NOP_6 NOP_4; NOP_2
#define NOP_7 NOP_4; NOP_3
#define NOP_8 NOP_4; NOP_4

class WS2812 {
protected:
	WS2812(void) = default;
	WS2812(const WS2812&) = delete;
	WS2812(WS2812&&) = default;

	WS2812 &operator=(const WS2812&) = delete;
	WS2812 &operator=(WS2812&&) = delete;
};

template <typename InPinT>
class WS2812_impl : public WS2812 {
	InPinT pin;

	template <typename TurboPinT>
	[[gnu::always_inline]]
	inline void send_value(TurboPinT &turbo, uint8_t v) {
		// This function requires somewhat precise timing;
		// The high pulse must be:
		// 0: 62--500ns (1--8 clocks)
		// 1: >= 625ns (10+ clocks)
		// The low pulse can be longer, but not more than 9000ns (144 clocks)
		// The total period per bit must be at least 1060ns (17 clocks)

		// We assume 62.5ns per clock (16MHz)

		// We use pin functions which are almost direct register access;
		// they typically perform *bank = value (for pre-computed bank & value)
		// This is (st) or (mov, st) (?)
		// On an ATmega328 this translates to 2 or 4 clocks

		// A NOP consumes 1 clock

		for(int b = 0; b < 7; ++ b) { // loop counter uses ~4 clocks
			turbo.high(); // 2 clocks
			if(v & 0x80) { // 1--3 clocks (?) (uses SBRS)
				// Must be high for 10 clocks total
				// Seems we can rely on SBRS using 3 clocks in this branch
				// (?) if this isn't true, this needs to be 7 instead
				NOP_5;
				turbo.low(); // 2 clocks
			} else {
				turbo.low(); // 2 clocks
				NOP_5;
			}
			v <<= 1; // 1 clock (uses v += v in generated asm)
			// Loop must be at least 17 clocks total
			// Seems we can rely on the loop using some extra clocks
			// (?) if this isn't true, this needs to be 3 instead
//			NOP_3;
		}

		turbo.high();
		if(v & 0x80) {
			NOP_5; // as above, this may need to be 7
			turbo.low();
		} else {
			turbo.low();
			NOP_5;
		}

		// destructor of turbo in calling function restores interrupts
	}

public:
	// Channel order: Green Red Blue
	template <typename Fn>
	void send_fn(uint16_t count, Fn fn) {
		// Function must not take more than ~8us (~128 clocks)

		auto p = pin.fast();

		for(uint16_t i = 0; i < count; ++ i) {
			uint8_t v = fn(i);

			auto turbo = p.turbo();
			send_value(turbo, v);
		}
	}

	template <typename Fn>
	void send_rgb_fn(uint16_t count, Fn fn) {
		// Function must not take more than ~8us (~128 clocks)

		auto p = pin.fast();

		for(uint16_t i = 0; i < count; ++ i) {
			uint8_t r;
			uint8_t g;
			uint8_t b;
			fn(i, &r, &g, &b);

			auto turbo = p.turbo();
			send_value(turbo, g); // Green
			send_value(turbo, r); // Red
			send_value(turbo, b); // Blue
		}
	}

	template <typename FnR, typename FnG, typename FnB>
	void send_rgb_fn(uint16_t count, FnR fnR, FnG fnG, FnB fnB) {
		// Functions must not take more than ~8us (~128 clocks)

		auto p = pin.fast();

		for(uint16_t i = 0; i < count; ++ i) {
			uint8_t v;

			auto turbo = p.turbo();
			fnG(i, &v);
			send_value(turbo, v); // Green
			fnR(i, &v);
			send_value(turbo, v); // Red
			fnB(i, &v);
			send_value(turbo, v); // Blue
		}
	}

	template <typename T> // byte array
	void send(uint16_t count, T bytes) {
		send_fn(count, [&bytes] (uint16_t i) {
			return bytes[i];
		});
	}

	WS2812_impl(InPinT pin)
		: pin(pin)
	{
		pin.set_output();
	}
};

template <typename InPinT>
[[gnu::always_inline,nodiscard]]
inline WS2812_impl<InPinT> MakeWS2812(InPinT pin) {
	return WS2812_impl<InPinT>(pin);
}

#endif

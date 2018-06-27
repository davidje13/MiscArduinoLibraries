/*
 * RotaryEncoder - Checks the output of digital rotary encoders.
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

#ifndef ROTARYENCODER_H_INCLUDED
#define ROTARYENCODER_H_INCLUDED

#include <ProgMem.h>
#include "ext.h"

class RotaryEncoder {
protected:
	[[gnu::always_inline]]
	inline int8_t calculate_delta(uint8_t oldState, uint8_t newState) const {
		// most transitions are obvious, but if we skip a transition (e.g.
		// because only 1 pin has an interrupt, or neither do and the sampling
		// isn't fast enough), we assume that pin 1 is the last to have changed
		// (if pin 2 was last to change, negate any +/- 2 results)

		// 00 01 > 11 10 > 00    0 1 2 3 0
		// 00 > 10 11 > 01 00    0 3 2 1 0

		// A lookup table is faster and takes less program space than the
		// necessary calculations / branching
		static PROGMEM const int8_t DIRS[] = {
			+0, // 0 -> 0
			+1, // 0 -> 1
			+2, // 0 -> 2
			-1, // 0 -> 3

			-1, // 1 -> 0
			+0, // 1 -> 1
			+1, // 1 -> 2
			-2, // 1 -> 3

			+2, // 2 -> 0
			-1, // 2 -> 1
			+0, // 2 -> 2
			+1, // 2 -> 3

			+1, // 3 -> 0
			-2, // 3 -> 1
			-1, // 3 -> 2
			+0  // 3 -> 3
		};

		return MakeProgMem(DIRS)[(oldState << 2) | newState];

		// Pure code option (but does not account for +/- 2 decisions):
		// return ((((newState - oldState) & 3) + 1) & 3) - 1;
	}

	RotaryEncoder(void) = default;
	RotaryEncoder(const RotaryEncoder&) = delete;
	RotaryEncoder(RotaryEncoder&&) = default;

	RotaryEncoder &operator=(const RotaryEncoder&) = delete;
	RotaryEncoder &operator=(RotaryEncoder&&) = delete;
};

template <typename APinT, typename BPinT>
class RotaryEncoder_impl : public RotaryEncoder {
	ext::Flattener<APinT,ext::Flattener<BPinT,uint8_t>> aPin;
#define bPin aPin.flattened_value
#define lastState bPin.flattened_value

	uint8_t get_state(void) const {
		return (
			uint8_t(aPin.read_digital() * 3) ^
			bPin.read_digital()
		);
	}

public:
	int8_t delta(void) {
		uint8_t state = get_state();
		int8_t dir = calculate_delta(lastState, state);
		lastState = state;
		return dir;
	}

	int8_t fraction(void) const {
		return lastState;
	}

	RotaryEncoder_impl(APinT a, BPinT b, bool pullup = false)
		: aPin(a, ext::Flattener<BPinT, uint8_t>(b, 0))
	{
		aPin.set_input(pullup);
		bPin.set_input(pullup);
		lastState = get_state();
	}

	RotaryEncoder_impl(RotaryEncoder_impl &&b)
		: aPin(static_cast<
			ext::Flattener<APinT,
			ext::Flattener<BPinT,
			uint8_t
		>>&&>(b.aPin))
	{}

#undef bPin
#undef lastState
};

template <typename APinT, typename BPinT, typename AccumT>
class InterruptRotaryEncoder_impl : public RotaryEncoder {
#define FLAG_BITS 2

	static InterruptRotaryEncoder_impl<APinT, BPinT, AccumT> *interruptTarget;

	static void update_global_a(void) {
		interruptTarget->update_a();
	}

	static void update_global_b(void) {
		interruptTarget->update_b();
	}

	ext::Flattener<APinT,ext::Flattener<BPinT,volatile AccumT>> aPin;
#define bPin aPin.flattened_value
#define lastState bPin.flattened_value

	uint8_t get_state(void) const {
		return (
			uint8_t(aPin.read_digital() * 3) ^
			bPin.read_digital()
		);
	}

	[[gnu::always_inline]]
	inline void update_a(void) {
		uint8_t state = get_state();
		AccumT oldState = lastState; // remove volatility
		int8_t dir = calculate_delta(uint8_t(oldState & 0x3), state);
		lastState = AccumT(state | ((oldState >> FLAG_BITS) + dir) << FLAG_BITS);
	}

	[[gnu::always_inline]]
	inline void update_b(void) {
		uint8_t state = get_state();
		AccumT oldState = lastState; // remove volatility
		int8_t dir = calculate_delta(uint8_t(oldState & 0x3), state);
		if(dir == -2 || dir == 2) {
			dir = -dir;
		}
		lastState = AccumT(state | ((oldState >> FLAG_BITS) + dir) << FLAG_BITS);
	}

public:
	AccumT delta(void) {
		uint8_t oldSREG = SREG;
		cli();
		AccumT oldState = lastState; // remove volatility
		AccumT dir = oldState >> FLAG_BITS;
		uint8_t state = get_state();
		dir += calculate_delta(uint8_t(oldState & 0x3), state);
		lastState = state;
		SREG = oldSREG;
		return dir;
	}

	int8_t fraction(void) const {
		return lastState & 0x3;
	}

	InterruptRotaryEncoder_impl(APinT a, BPinT b, bool pullup = false)
		: aPin(a, ext::Flattener<BPinT, volatile AccumT>(b, 0))
	{
		aPin.set_input(pullup);
		bPin.set_input(pullup);
		lastState = get_state();

		bool intA = aPin.supports_interrupts();
		bool intB = bPin.supports_interrupts();
		if(intA || intB) {
			interruptTarget = this;
		}
		if(intA) {
			aPin.set_interrupt_on_change(&update_global_a);
		}
		if(intB) {
			bPin.set_interrupt_on_change(&update_global_b);
		}
	}

	InterruptRotaryEncoder_impl(InterruptRotaryEncoder_impl &&b)
		: aPin(static_cast<
			ext::Flattener<APinT,
			ext::Flattener<BPinT,
			volatile AccumT
		>>&&>(b.aPin))
	{
		if(interruptTarget == &b) {
			interruptTarget = this;
		}
	}

	~InterruptRotaryEncoder_impl(void) {
		if(interruptTarget == this) {
			aPin.remove_interrupt();
			bPin.remove_interrupt();
			interruptTarget = nullptr;
		}
	}

#undef bPin
#undef lastState
#undef FLAG_BITS
};

template <typename APinT, typename BPinT, typename AccumT>
InterruptRotaryEncoder_impl<APinT, BPinT, AccumT> *
InterruptRotaryEncoder_impl<APinT, BPinT, AccumT>::interruptTarget = nullptr;

template <typename APinT, typename BPinT>
[[gnu::always_inline,nodiscard]]
inline RotaryEncoder_impl<
	APinT,
	BPinT
> MakeRotaryEncoder(APinT a, BPinT b, bool pullup = false) {
	return RotaryEncoder_impl<APinT, BPinT>(a, b, pullup);
}

template <typename APinT, typename BPinT, typename AccumT = int16_t>
[[gnu::always_inline,nodiscard]]
inline InterruptRotaryEncoder_impl<
	APinT,
	BPinT,
	AccumT
> MakeInterruptRotaryEncoder(APinT a, BPinT b, bool pullup = false) {
	return InterruptRotaryEncoder_impl<APinT, BPinT, AccumT>(a, b, pullup);
}

#endif

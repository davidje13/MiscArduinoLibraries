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

#include "ext.h"

class RotaryEncoder {
protected:
	[[gnu::always_inline]]
	inline int8_t calculate_delta(uint8_t oldState, uint8_t newState) const {
		// 00 01 11 10 00    0 1 2 3 0
		// 00 10 11 01 00    0 3 2 1 0

		// 1 2 -1 0 1 2 -1

		return ((((newState - oldState) & 3) + 1) & 3) - 1;
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

	RotaryEncoder_impl(APinT a, BPinT b)
		: aPin(a, ext::Flattener<BPinT, uint8_t>(b, 0))
	{
		aPin.set_input();
		bPin.set_input();
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
#define FLAG_BITS 3
#define FLAG_HAS_INTERRUPTS 4

	static InterruptRotaryEncoder_impl<APinT, BPinT, AccumT> *interruptTarget;

	static void update_global(void) {
		if(interruptTarget != nullptr) {
			interruptTarget->update();
		}
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

	void update(void) {
		uint8_t state = get_state();
		AccumT oldState = lastState; // remove volatility
		int8_t dir = calculate_delta(uint8_t(oldState & 3), state);
		lastState = AccumT(
			state | FLAG_HAS_INTERRUPTS |
			((oldState >> FLAG_BITS) + dir) << FLAG_BITS
		);
	}

public:
	AccumT delta(void) {
		uint8_t oldSREG = SREG;
		cli();
		AccumT oldState = lastState; // remove volatility
		AccumT dir;
		if(oldState & FLAG_HAS_INTERRUPTS) {
			dir = oldState >> FLAG_BITS;
			lastState &= 7;
		} else {
			uint8_t state = get_state();
			dir = calculate_delta(uint8_t(oldState), state);
			lastState = state;
		}
		SREG = oldSREG;
		return dir;
	}

	int8_t fraction(void) const {
		return lastState & 3;
	}

	InterruptRotaryEncoder_impl(APinT a, BPinT b)
		: aPin(a, ext::Flattener<BPinT, volatile AccumT>(b, 0))
	{
		aPin.set_input();
		bPin.set_input();
		lastState = get_state();

		if(aPin.supports_interrupts() && bPin.supports_interrupts()) {
			lastState |= FLAG_HAS_INTERRUPTS;
			aPin.set_interrupt_on_change(&update_global);
			bPin.set_interrupt_on_change(&update_global);
			interruptTarget = this;
		}
	}

	InterruptRotaryEncoder_impl(InterruptRotaryEncoder_impl &&b)
		: aPin(static_cast<
			ext::Flattener<APinT,
			ext::Flattener<BPinT,
			volatile AccumT
		>>&&>(b.aPin))
	{
		if(b.lastState & FLAG_HAS_INTERRUPTS) {
			interruptTarget = this;
			b.lastState &= ~FLAG_HAS_INTERRUPTS;
		}
	}

	~InterruptRotaryEncoder_impl(void) {
		if(lastState & FLAG_HAS_INTERRUPTS) {
			interruptTarget = nullptr;
			aPin.remove_interrupt();
			bPin.remove_interrupt();
		}
	}

#undef bPin
#undef lastState
#undef FLAG_BITS
#undef FLAG_HAS_INTERRUPTS
};

template <typename APinT, typename BPinT, typename AccumT>
InterruptRotaryEncoder_impl<APinT, BPinT, AccumT> *
InterruptRotaryEncoder_impl<APinT, BPinT, AccumT>::interruptTarget = nullptr;

template <typename APinT, typename BPinT>
[[gnu::always_inline]]
inline RotaryEncoder_impl<APinT, BPinT> MakeRotaryEncoder(APinT a, BPinT b) {
	return RotaryEncoder_impl<APinT, BPinT>(a, b);
}

template <typename APinT, typename BPinT, typename AccumT = int16_t>
[[gnu::always_inline]]
inline InterruptRotaryEncoder_impl<
	APinT,
	BPinT,
	AccumT
> MakeInterruptRotaryEncoder(APinT a, BPinT b) {
	return InterruptRotaryEncoder_impl<APinT, BPinT, AccumT>(a, b);
}

#endif

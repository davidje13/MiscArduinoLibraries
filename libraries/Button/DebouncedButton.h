/*
 * DebouncedButton - ensures button presses are counted only once.
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

#ifndef DEBOUNCEDBUTTON_H_INCLUDED
#define DEBOUNCEDBUTTON_H_INCLUDED

#include "ext.h"

class DebouncedButton {
public:
	enum class State : uint8_t {
		INACTIVE,
		PRESS,
		HOLD,
		RELEASE
	};
};

template <typename PinT, bool activeHigh>
class DebouncedButton_impl : public DebouncedButton {
	PinT pin;
	bool lastState;

public:
	[[nodiscard]]
	State state(void) {
		bool state = (pin.read_digital() == activeHigh);
		if(state == lastState) {
			return state ? State::HOLD : State::INACTIVE;
		}
		lastState = state;
		return state ? State::PRESS : State::RELEASE;
	}

	[[nodiscard,gnu::always_inline]]
	inline bool pressed(void) {
		return state() == State::PRESS;
	}

	[[nodiscard,gnu::always_inline]]
	inline bool released(void) {
		return state() == State::RELEASE;
	}

	DebouncedButton_impl(PinT pin, bool pullup = false)
		: pin(pin)
		, lastState(false)
	{
		pin.set_input(pullup);
	}
};

template <bool activeHigh, typename PinT>
[[gnu::always_inline,nodiscard]]
inline DebouncedButton_impl<PinT, activeHigh> _MakeDebouncedButton(
	PinT pin,
	bool pullup = false
) {
	return DebouncedButton_impl<PinT, activeHigh>(pin, pullup);
}

#define MakeDebouncedButton(pin, activeHigh) \
	_MakeDebouncedButton<activeHigh>(pin, !activeHigh)

#endif

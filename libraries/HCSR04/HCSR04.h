/*
 * HCSR04 - wraps communication with the HC-SR04 echolocation board.
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

#ifndef HCSR04_H_INCLUDED
#define HCSR04_H_INCLUDED

#include "ext.h"

class HCSR04 {
public:
	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr uint16_t no_reflection(void) {
		return 0xFFFF;
	}

protected:
	[[gnu::always_inline]]
	inline uint16_t micros_to_mm(uint16_t micros) const {
		if(micros == no_reflection()) {
			return no_reflection();
		}
		return uint16_t(uint32_t(micros) * 5 / 29);
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr uint16_t max_duration(void) {
		// Output range is 150us - 25ms, or 38ms if no reflection
		// -> 2.6cm - ~4m
		return 26000;
	}

	HCSR04(void) = default;
	HCSR04(const HCSR04&) = delete;
	HCSR04(HCSR04&&) = default;

	HCSR04 &operator=(const HCSR04&) = delete;
	HCSR04 &operator=(HCSR04&&) = delete;
};

template <typename OutPinT, typename InPinT>
class SynchronousHCSR04_impl : public HCSR04 {
	ext::Flattener<OutPinT,InPinT> outPin;
#define inPin outPin.flattened_value

public:
	uint16_t measure_micros(void) {
		outPin.high();
		delayMicroseconds(10);
		outPin.low();
		uint32_t micros = inPin.measure_high_pulse(max_duration());
		return micros >= max_duration() ? no_reflection() : uint16_t(micros);
	}

	[[gnu::always_inline]]
	inline uint16_t measure_millimetres(void) {
		return micros_to_mm(measure_micros());
	}

	SynchronousHCSR04_impl(OutPinT o, InPinT i)
		: outPin(o, i)
	{
		outPin.set_output();
		outPin.low();
		inPin.set_input();
	}

	SynchronousHCSR04_impl(SynchronousHCSR04_impl &&b)
		: outPin(static_cast<ext::Flattener<OutPinT,InPinT>&&>(b.outPin))
	{}

#undef inPin
};

template <typename OutPinT, typename InPinT>
class AsynchronousHCSR04_impl : public HCSR04 {
	static AsynchronousHCSR04_impl<OutPinT, InPinT> *interruptTarget;

	static void update_global(void) {
		interruptTarget->update();
	}

	ext::Flattener<OutPinT,uint16_t> outPin;
#define lastBegin outPin.flattened_value
	ext::Flattener<InPinT,volatile uint16_t> inPin;
#define lastMicros inPin.flattened_value
	volatile uint8_t state;

	[[gnu::always_inline]]
	inline void update(void) {
		bool high = inPin.read_digital();

		if(state == 1 && high) {
			lastBegin = uint16_t(micros());
			state = 2;
		} else if(state == 2 && !high) {
			lastMicros = uint16_t(micros()) - lastBegin;
			state = 0;
		}
	}

public:
	uint16_t latest_result(void) const {
		uint16_t micros = lastMicros; // remove volatility
		return micros >= max_duration() ? no_reflection() : micros;
	}

	[[nodiscard,gnu::always_inline]]
	inline bool awaiting_result(void) const {
		return state != 0;
	}

	uint16_t await_result(void) const {
		while(state);
		return latest_result();
	}

	uint16_t measure_micros(void) {
		if(state == 0) {
			state = 1;
			outPin.high();
			delayMicroseconds(10);
			outPin.low();
		}
		return latest_result();
	}

	[[gnu::always_inline]]
	inline uint16_t measure_millimetres(void) {
		return micros_to_mm(measure_micros());
	}

	AsynchronousHCSR04_impl(OutPinT o, InPinT i)
		: outPin(o, 0)
		, inPin(i, no_reflection())
		, state(0)
	{
		outPin.set_output();
		outPin.low();
		inPin.set_input();
		interruptTarget = this;
		inPin.set_interrupt_on_change(&update_global);
	}

	AsynchronousHCSR04_impl(AsynchronousHCSR04_impl &&b)
		: outPin(static_cast<ext::Flattener<OutPinT,uint16_t>&&>(b.outPin))
		, inPin(static_cast<ext::Flattener<InPinT,volatile uint16_t>&&>(b.inPin))
		, state(b.state)
	{
		interruptTarget = this;
		b.state = 0xFF;
	}

	~AsynchronousHCSR04_impl(void) {
		if(state != 0xFF) {
			inPin.remove_interrupt();
			interruptTarget = nullptr;
		}
	}

#undef lastBegin
#undef lastMicros
};

template <typename OutPinT, typename InPinT>
AsynchronousHCSR04_impl<OutPinT, InPinT> *
AsynchronousHCSR04_impl<OutPinT, InPinT>::interruptTarget = nullptr;

template <typename OutPinT, typename InPinT>
[[gnu::always_inline]]
inline SynchronousHCSR04_impl<
	OutPinT,
	InPinT
> MakeSynchronousHCSR04(OutPinT o, InPinT i) {
	return SynchronousHCSR04_impl<OutPinT, InPinT>(o, i);
}

template <typename OutPinT, typename InPinT>
[[gnu::always_inline]]
inline AsynchronousHCSR04_impl<
	OutPinT,
	InPinT
> MakeAsynchronousHCSR04(OutPinT o, InPinT i) {
	return AsynchronousHCSR04_impl<OutPinT, InPinT>(o, i);
}

#endif

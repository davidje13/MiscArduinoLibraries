/*
 * PinBank - Fallback implementation of pin bank for connecting arbitrary pins
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

#ifndef PINBANK_H_INCLUDED
#define PINBANK_H_INCLUDED

#include "ext.h"

template <typename P1, typename P2>
class PinBank2 : public P1 {
	P2 extra;

public:
	[[gnu::always_inline]]
	static constexpr inline uint8_t size(void) {
		return 2;
	}

	[[gnu::always_inline]]
	inline PinBank2(P1 p1, P2 p2)
		: P1(p1)
		, extra(p2)
	{}

	[[gnu::always_inline]]
	inline void _set(uint8_t v) {
		P1::set((v >> 1) & 1);
		extra.set(v & 1);
	}

	[[gnu::always_inline]]
	inline void _set_input(bool pullup = false) {
		P1::set_input(pullup);
		extra.set_input(pullup);
	}

	[[gnu::always_inline]]
	inline void _set_output(void) {
		P1::set_output();
		extra.set_output();
	}

	[[gnu::always_inline]]
	inline void set(uint8_t v) {
		begin_batch();
		_set(v);
		send_batch();
	}

	[[gnu::always_inline]]
	inline void set_input(bool pullup = false) {
		begin_batch();
		_set_input(pullup);
		send_batch();
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		begin_batch();
		_set_output();
		send_batch();
	}

	[[nodiscard,gnu::always_inline]]
	inline uint8_t get(void) const {
		return (
			(P1::get() << 1) |
			extra.get()
		);
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
		P1::begin_batch();
		extra.begin_batch();
	}

	[[gnu::always_inline]]
	inline void send_batch(void) {
		P1::send_batch();
		extra.send_batch();
	}
};

template <typename T1, typename T2>
class PinBankCombo : public T1 {
	T2 extra;

protected:
	[[gnu::always_inline]]
	inline PinBankCombo(T1 t1, T2 t2)
		: T1(t1)
		, extra(t2)
	{}

public:
	[[gnu::always_inline]]
	static constexpr inline uint8_t size(void) {
		return T1::size() + T2::size();
	}

	[[gnu::always_inline]]
	inline void _set(uint8_t v) {
		T1::_set(v >> extra.size());
		extra._set(v);
	}

	[[gnu::always_inline]]
	inline void _set_input(bool pullup = false) {
		T1::_set_input(pullup);
		extra._set_input(pullup);
	}

	[[gnu::always_inline]]
	inline void _set_output(void) {
		T1::_set_output();
		extra._set_output();
	}

	[[gnu::always_inline]]
	inline void set(uint8_t v) {
		begin_batch();
		_set(v);
		send_batch();
	}

	[[gnu::always_inline]]
	inline void set_input(bool pullup = false) {
		begin_batch();
		_set_input(pullup);
		send_batch();
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		begin_batch();
		_set_output();
		send_batch();
	}

	[[nodiscard,gnu::always_inline]]
	inline uint8_t get(void) const {
		return (
			(T1::get() << extra.size()) |
			extra.get()
		);
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
		T1::begin_batch();
		extra.begin_batch();
	}

	[[gnu::always_inline]]
	inline void send_batch(void) {
		T1::send_batch();
		extra.send_batch();
	}
};

template <typename P1, typename P2, typename P3, typename P4>
class PinBank4 : public PinBankCombo<
	PinBank2<P1,P2>,
	PinBank2<P3,P4>
> {
public:
	[[gnu::always_inline]]
	inline PinBank4(P1 p1, P2 p2, P3 p3, P4 p4)
		: PinBankCombo<
			PinBank2<P1,P2>,
			PinBank2<P3,P4>
		>(
			PinBank2<P1,P2>(p1, p2),
			PinBank2<P3,P4>(p3, p4)
		)
	{}
};

template <
	typename P1, typename P2, typename P3, typename P4,
	typename P5, typename P6
>
class PinBank6 : public PinBankCombo<
	PinBank4<P1,P2,P3,P4>,
	PinBank2<P5,P6>
> {
public:
	[[gnu::always_inline]]
	inline PinBank6(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		: PinBankCombo<
			PinBank4<P1,P2,P3,P4>,
			PinBank2<P5,P6>
		>(
			PinBank4<P1,P2,P3,P4>(p1, p2, p3, p4),
			PinBank2<P5,P6>(p5, p6)
		)
	{}
};

template <
	typename P1, typename P2, typename P3, typename P4,
	typename P5, typename P6, typename P7, typename P8
>
class PinBank8 : public PinBankCombo<
	PinBank4<P1,P2,P3,P4>,
	PinBank4<P5,P6,P7,P8>
> {
public:
	[[gnu::always_inline]]
	inline PinBank8(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		: PinBankCombo<
			PinBank4<P1,P2,P3,P4>,
			PinBank4<P5,P6,P7,P8>
		>(
			PinBank4<P1,P2,P3,P4>(p1, p2, p3, p4),
			PinBank4<P5,P6,P7,P8>(p5, p6, p7, p8)
		)
	{}
};

template <typename P1, typename P2>
[[gnu::always_inline]]
inline PinBank2<P1,P2> MakePinBank(P1 p1, P2 p2) {
	return PinBank2<P1,P2>(p1, p2);
}

template <typename P1, typename P2, typename P3, typename P4>
[[gnu::always_inline]]
inline PinBank4<P1,P2,P3,P4> MakePinBank(P1 p1, P2 p2, P3 p3, P4 p4) {
	return PinBank4<P1,P2,P3,P4>(p1, p2, p3, p4);
}

template <
	typename P1, typename P2, typename P3, typename P4,
	typename P5, typename P6
>
[[gnu::always_inline]]
inline PinBank6<P1,P2,P3,P4,P5,P6> MakePinBank(
	P1 p1, P2 p2, P3 p3, P4 p4,
	P5 p5, P6 p6
) {
	return PinBank6<P1,P2,P3,P4,P5,P6>(p1, p2, p3, p4, p5, p6);
}

template <
	typename P1, typename P2, typename P3, typename P4,
	typename P5, typename P6, typename P7, typename P8
>
[[gnu::always_inline]]
inline PinBank8<P1,P2,P3,P4,P5,P6,P7,P8> MakePinBank(
	P1 p1, P2 p2, P3 p3, P4 p4,
	P5 p5, P6 p6, P7 p7, P8 p8
) {
	return PinBank8<P1,P2,P3,P4,P5,P6,P7,P8>(p1, p2, p3, p4, p5, p6, p7, p8);
}

#endif

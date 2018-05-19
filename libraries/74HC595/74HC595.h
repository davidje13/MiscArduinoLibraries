/*
 * 74HC595 - Sends data to a shift register.
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

#ifndef C74HC595_H_INCLUDED
#define C74HC595_H_INCLUDED

#include "ext.h"

class C74HC595 {
protected:
	C74HC595(void) = default;
	C74HC595(const C74HC595&) = delete;
	C74HC595(C74HC595&&) = default;

	C74HC595 &operator=(const C74HC595&) = delete;
	C74HC595 &operator=(C74HC595&&) = delete;
};

template <typename LatchPinT, typename ClockPinT, typename DataPinT>
class C74HC595_impl : public C74HC595 {
	ext::Flattener<LatchPinT,ext::Flattener<ClockPinT,DataPinT>> latchPin;
#define clockPin latchPin.flattened_value
#define dataPin clockPin.flattened_value

public:
	void send1(bool v) {
		auto fastData = dataPin.fast();
		auto fastClock = clockPin.fast();

		uint8_t oldSREG = SREG;
		cli();

		fastClock.low_fast();
		fastData.set_fast(v);
		fastClock.high_fast();

		SREG = oldSREG;
	}

	template <uint8_t n, bool msb = true>
	void send(const void *values) {
		auto fastLatch = latchPin.fast();
		auto fastData = dataPin.fast();
		auto fastClock = clockPin.fast();

		uint8_t oldSREG = SREG;

		cli();
		fastLatch.low_fast();

		const uint8_t *vs = static_cast<const uint8_t*>(values);
		for(uint8_t b = 0; b < (n / 8); ++ b) {
			uint8_t v = vs[b];
			cli();
			for(uint8_t i = 0; i < 8; ++ i) {
				// if clock & data are on the same bank,
				// they can change simultaneously
				fastClock.begin_batch();
				fastClock.low_fast();
				if(msb) {
					fastData.set_fast(v >> 7);
				} else {
					fastData.set_fast(v & 1);
				}
				fastClock.send_batch();
				fastClock.high_fast();
				if(msb) {
					v <<= 1;
				} else {
					v >>= 1;
				}
			}
			SREG = oldSREG;
		}
		if(n & 7) {
			uint8_t v = vs[n / 8];
			cli();
			for(uint8_t i = 0; i < (n & 7); ++ i) {
				fastClock.begin_batch();
				fastClock.low_fast();
				if(msb) {
					fastData.set_fast(v >> 7);
				} else {
					fastData.set_fast(v & 1);
				}
				fastClock.send_batch();
				fastClock.high_fast();
				if(msb) {
					v <<= 1;
				} else {
					v >>= 1;
				}
			}
			SREG = oldSREG;
		}
		fastLatch.high();
	}

	template <uint8_t n>
	[[gnu::always_inline]]
	inline void send_msb(const void *values) {
		return send<n, true>(values);
	}

	template <uint8_t bytes>
	[[gnu::always_inline]]
	inline void send_msb(const uint8_t (&values)[bytes]) {
		return send<bytes * 8, true>(values);
	}

	template <uint8_t n>
	[[gnu::always_inline]]
	inline void send_lsb(const void *values) {
		return send<n, false>(values);
	}

	template <uint8_t bytes>
	[[gnu::always_inline]]
	inline void send_lsb(const uint8_t (&values)[bytes]) {
		return send<bytes * 8, false>(values);
	}

	[[gnu::always_inline]]
	inline void send8_msb(uint8_t v) {
		send<8, true>(&v);
	}

	[[gnu::always_inline]]
	inline void send16_msb(uint8_t v0, uint8_t v1) {
		uint8_t vs[] = {v0, v1};
		send<16, true>(vs);
	}

	[[gnu::always_inline]]
	inline void send16_msb(uint16_t v) {
		uint8_t vs[] = {
			uint8_t(v >> 8),
			uint8_t(v)
		};
		send<16, true>(vs);
	}

	[[gnu::always_inline]]
	inline void send24_msb(uint8_t v0, uint8_t v1, uint8_t v2) {
		uint8_t vs[] = {v0, v1, v2};
		send<24, true>(vs);
	}

	[[gnu::always_inline]]
	inline void send24_msb(uint32_t v) {
		uint8_t vs[] = {
			uint8_t(v >> 16),
			uint8_t(v >> 8),
			uint8_t(v)
		};
		send<24, true>(vs);
	}

	[[gnu::always_inline]]
	inline void send32_msb(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3) {
		uint8_t vs[] = {v0, v1, v2, v3};
		send<32, true>(vs);
	}

	[[gnu::always_inline]]
	inline void send32_msb(uint32_t v) {
		uint8_t vs[] = {
			uint8_t(v >> 24),
			uint8_t(v >> 16),
			uint8_t(v >> 8),
			uint8_t(v)
		};
		send<32, true>(vs);
	}

	[[gnu::always_inline]]
	inline void send8_lsb(uint8_t v) {
		send<8, false>(&v);
	}

	[[gnu::always_inline]]
	inline void send16_lsb(uint8_t v0, uint8_t v1) {
		uint8_t vs[] = {v0, v1};
		send<16, false>(vs);
	}

	[[gnu::always_inline]]
	inline void send16_lsb(uint16_t v) {
		uint8_t vs[] = {
			uint8_t(v),
			uint8_t(v >> 8)
		};
		send<16, false>(vs);
	}

	[[gnu::always_inline]]
	inline void send24_lsb(uint8_t v0, uint8_t v1, uint8_t v2) {
		uint8_t vs[] = {v0, v1, v2};
		send<24, false>(vs);
	}

	[[gnu::always_inline]]
	inline void send24_lsb(uint32_t v) {
		uint8_t vs[] = {
			uint8_t(v),
			uint8_t(v >> 8),
			uint8_t(v >> 16)
		};
		send<24, false>(vs);
	}

	[[gnu::always_inline]]
	inline void send32_lsb(uint8_t v0, uint8_t v1, uint8_t v2, uint8_t v3) {
		uint8_t vs[] = {v0, v1, v2, v3};
		send<32, false>(vs);
	}

	[[gnu::always_inline]]
	inline void send32_lsb(uint32_t v) {
		uint8_t vs[] = {
			uint8_t(v),
			uint8_t(v >> 8),
			uint8_t(v >> 16),
			uint8_t(v >> 24)
		};
		send<32, false>(vs);
	}

	template <uint8_t n, bool msb = true>
	void read(void *values) {
		dataPin.set_input(true);

		auto fastLatch = latchPin.fast();
		auto fastData = dataPin.fast();
		auto fastClock = clockPin.fast();

		uint8_t oldSREG = SREG;

		cli();
		fastLatch.low_fast();

		const uint8_t *vs = static_cast<const uint8_t*>(values);
		for(uint8_t b = 0; b < (n / 8); ++ b) {
			uint8_t *v = &vs[b];
			cli();
			for(uint8_t i = 0; i < 8; ++ i) {
				if(msb) {
					v <<= 1;
				} else {
					v >>= 1;
				}
				fastClock.low_fast();
				v |= fastData.read_digital() * (msb ? 0x01 : 0x80);
				fastClock.high_fast();
			}
			SREG = oldSREG;
		}
		if(n & 7) {
			uint8_t *v = &vs[n / 8];
			cli();
			for(uint8_t i = 0; i < (n & 7); ++ i) {
				if(msb) {
					v <<= 1;
				} else {
					v >>= 1;
				}
				fastClock.low_fast();
				v |= fastData.read_digital() * (msb ? 0x01 : 0x80);
				fastClock.high_fast();
			}
			SREG = oldSREG;
		}
		fastLatch.high();

		dataPin.set_output();
		dataPin.low();
	}

	template <uint8_t n>
	[[gnu::always_inline]]
	inline void read_msb(void *values) {
		return read<n, true>(values);
	}

	template <uint8_t n>
	[[gnu::always_inline]]
	inline void read_lsb(void *values) {
		return read<n, false>(values);
	}

	[[gnu::always_inline]]
	inline uint8_t read8_msb(void) {
		uint8_t v;
		read<8, true>(&v);
		return v;
	}

	[[gnu::always_inline]]
	inline uint8_t read8_lsb(void) {
		uint8_t v;
		read<8, false>(&v);
		return v;
	}

	[[gnu::always_inline]]
	inline uint16_t read16_msb(void) {
		uint8_t vs[2];
		read<16, true>(vs);
		return (vs[0] << 8) | vs[1];
	}

	[[gnu::always_inline]]
	inline uint16_t read16_lsb(void) {
		uint8_t vs[2];
		read<16, false>(vs);
		return vs[0] | (vs[1] << 8);
	}

	[[gnu::always_inline]]
	inline uint32_t read24_msb(void) {
		uint8_t vs[3];
		read<24, true>(vs);
		return (uint32_t(vs[0]) << 16) | (uint32_t(vs[1]) << 8) | vs[2];
	}

	[[gnu::always_inline]]
	inline uint32_t read24_lsb(void) {
		uint8_t vs[3];
		read<24, false>(vs);
		return vs[0] | (uint32_t(vs[1]) << 8) | (uint32_t(vs[2]) << 16);
	}

	[[gnu::always_inline]]
	inline uint32_t read32_msb(void) {
		uint8_t vs[4];
		read<32, true>(vs);
		return (
			(uint32_t(vs[0]) << 24) |
			(uint32_t(vs[1]) << 16) |
			(uint32_t(vs[2]) << 8) |
			vs[3]
		);
	}

	[[gnu::always_inline]]
	inline uint32_t read32_lsb(void) {
		uint8_t vs[4];
		read<32, false>(vs);
		return (
			vs[0] |
			(uint32_t(vs[1]) << 8) |
			(uint32_t(vs[2]) << 16) |
			(uint32_t(vs[3]) << 24)
		);
	}

	C74HC595_impl(LatchPinT stcp, ClockPinT shcp, DataPinT ds)
		: latchPin(stcp, ext::Flattener<ClockPinT, DataPinT>(shcp, ds))
	{
		latchPin.set_output();
		clockPin.set_output();
		dataPin.set_output();
		latchPin.high();
		clockPin.high();
	}

	C74HC595_impl(C74HC595_impl &&b)
		: latchPin(static_cast<
			ext::Flattener<LatchPinT,
			ext::Flattener<ClockPinT,
			DataPinT
		>>&&>(b.latchPin))
	{}

#undef clockPin
#undef dataPin
};

template <typename LatchPinT, typename ClockPinT, typename DataPinT>
[[gnu::always_inline,nodiscard]]
inline C74HC595_impl<
	LatchPinT,
	ClockPinT,
	DataPinT
> Make74HC595(LatchPinT stcp, ClockPinT shcp, DataPinT ds) {
	return C74HC595_impl<LatchPinT, ClockPinT, DataPinT>(stcp, shcp, ds);
}

#endif

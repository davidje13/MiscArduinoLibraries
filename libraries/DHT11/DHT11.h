/*
 * DHT11 - wraps communication with the DHT11 temp. / humidity sensor.
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

#ifndef DHT11_H_INCLUDED
#define DHT11_H_INCLUDED

#include "ext.h"

class DHT11 {
protected:
	[[gnu::pure,nodiscard]]
	static bool check(const uint8_t bytes[5]) {
		return uint8_t(bytes[0] + bytes[1] + bytes[2] + bytes[3]) == bytes[4];
	}

public:
	enum class State : uint8_t {
		OK,
		TIMEOUT,
		CHECKSUM_MISMATCH
	};

	class Reading {
		uint16_t temp;
		uint16_t humid;

	public:
		constexpr Reading(uint16_t temp, uint16_t humid)
			: temp(temp)
			, humid(humid)
		{}

		constexpr Reading(State state)
			: temp(0xFFFF)
			, humid(uint16_t(state))
		{}

		[[gnu::always_inline,gnu::pure,nodiscard]]
		inline bool valid(void) const {
			return temp != 0xFFFF;
		}

		[[gnu::always_inline,gnu::pure,nodiscard]]
		inline State state(void) const {
			if(temp == 0xFFFF) {
				return State(humid);
			} else {
				return State::OK;
			}
		}

		[[gnu::always_inline,gnu::pure,nodiscard]]
		inline uint16_t temperature_raw(void) const {
			if(temp == 0xFFFF) {
				return 0;
			} else {
				return temp;
			}
		}

		[[gnu::always_inline,gnu::pure,nodiscard]]
		inline float temperature(void) const {
			if(temp == 0xFFFF) {
				return 0.0f;
			} else {
				return temp / 256.0f;
			}
		}

		[[gnu::always_inline,gnu::pure,nodiscard]]
		inline uint16_t humidity_raw(void) const {
			if(temp == 0xFFFF) {
				return 0;
			} else {
				return humid;
			}
		}

		[[gnu::always_inline,gnu::pure,nodiscard]]
		inline float humidity(void) const {
			if(temp == 0xFFFF) {
				return 0.0f;
			} else {
				return humid / 256.0f;
			}
		}
	};
};

template <typename PinT, bool requiresPullup>
class DHT11_impl : public DHT11 {
	PinT pin;

	[[gnu::always_inline]]
	inline void idle(void) {
		pin.set_output();
		pin.high();
	}

	[[gnu::always_inline]]
	inline void ping(void) {
		pin.set_output();
		pin.low();
		delay(18);
		pin.set_input(requiresPullup);
	}

	bool await_state(bool state, uint16_t timeoutMillis) const {
		uint16_t tm0 = millis();
		while(pin.read_digital() != state) {
			if(uint16_t(millis() - tm0) > timeoutMillis) {
				return false;
			}
		}
		return true;
	}

	bool read(uint8_t target[5]) const {
		// wait for data
		if(!await_state(false, 100)) {
			return false;
		}

		// low for 80us, high for 80us
		if(pin.measure_high_pulse(220) < 50) {
			return false;
		}

		// 40 bits of data
		for(uint8_t byte = 0; byte < 5; ++ byte) {
			uint8_t v = 0;
			for(uint8_t bit = 0; bit < 8; ++ bit) {
				// 50us low, then 26-28us high (0) or 70us high (1)
				bool reading = pin.measure_high_pulse(150) > 50;
				v = (v << 1) | (reading ? 1 : 0);
			}
			target[byte] = v;
		}

		return true;
	}

public:
	Reading read(uint16_t timeoutMillis = 2100) {
		uint8_t bytes[5];

		uint16_t tm0 = millis();
		bool received = false;
		while(true) {
			ping();
			received = read(bytes);
			if(received || uint16_t(millis() - tm0) > timeoutMillis) {
				break;
			}
			delay(50);
		}
		idle();

		if(!received) {
			return Reading(State::TIMEOUT);
		}

		if(!check(bytes)) {
			return Reading(State::CHECKSUM_MISMATCH);
		}

		return Reading(
			(uint16_t(bytes[2]) << 8) | bytes[3], // temp
			(uint16_t(bytes[0]) << 8) | bytes[1]  // humidity
		);
	}

	DHT11_impl(PinT pin) : pin(pin) {
		idle();
	}
};

template <typename PinT>
[[gnu::always_inline,nodiscard]]
inline DHT11_impl<PinT, true> MakeDHT11(PinT pin) {
	return DHT11_impl<PinT, true>(pin);
}

template <typename PinT>
[[gnu::always_inline,nodiscard]]
inline DHT11_impl<PinT, false> MakeDHT11ExternalPullup(PinT pin) {
	return DHT11_impl<PinT, false>(pin);
}

#endif

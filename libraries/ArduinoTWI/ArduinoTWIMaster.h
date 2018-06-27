/*
 * ArduinoTWI - Wrapper of standard Arduino TWI / I2C access.
 * Written in 2017 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef ARDUINOTWI_H_INCLUDED
#define ARDUINOTWI_H_INCLUDED

#include <Wire.h>

#include "ext.h"

// Typical supported speeds:
// - 10000 (sometimes)
// - 100000 (always)
// - 400000 (always)
// - 1000000 (sometimes)
// - 3400000 (sometimes)

class ArduinoTWIMaster {
	static uint8_t refCount;
	static uint32_t capClockHz;

	static void inc(void);
	static void dec(void);
	static void _set_max_clock(uint32_t hz, bool force);

public:
	enum Error : uint8_t {
		SUCCESS = 0,
		DATA_TOO_LONG = 1,
		NACK_ADDR = 2,
		NACK_DATA = 3,
		OTHER = 4
	};

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr uint8_t max_write_bytes(void) {
		return BUFFER_LENGTH;
	}

	class Transmission {
		Transmission(void) {
			inc();
		}

		friend class ArduinoTWIMaster;

	public:
		Transmission(const Transmission&) = delete;
		Transmission(Transmission&&) = default;

		[[gnu::always_inline]]
		inline void write(uint8_t data) {
			Wire.write(data);
		}

		[[gnu::always_inline]]
		inline void write(const uint8_t *data, uint8_t count) {
			Wire.write(data, count);
		}

		[[gnu::always_inline]]
		inline Error stop(void) {
			return Error(Wire.endTransmission(true));
		}

		[[gnu::always_inline]]
		inline Error restart(void) {
			return Error(Wire.endTransmission(false));
		}

		~Transmission(void) {
			dec();
		}
	};

	class Request {
		Request(void) {
			inc();
		}

		friend class ArduinoTWIMaster;

	public:
		Request(const Request&) = delete;
		Request(Request&&) = default;

		[[nodiscard,gnu::always_inline]]
		inline uint8_t available(void) const {
			return uint8_t(Wire.available());
		}

		[[nodiscard,gnu::always_inline]]
		inline uint8_t read(void) {
			return Wire.read();
		}

		[[nodiscard,gnu::always_inline]]
		inline uint8_t peek(void) const {
			return Wire.peek();
		}

		[[nodiscard]]
		bool read(void *buffer, uint16_t count, uint16_t maxMicros) {
			uint16_t t0 = micros();
			uint8_t *b = static_cast<uint8_t*>(buffer);
			while(true) {
				while(available()) {
					*b = read();
					++ b;
					if((-- count) == 0) {
						return true;
					}
				}
				if(uint16_t(micros() - t0) > maxMicros) {
					return false;
				}
			}
		}

		~Request(void) {
			dec();
		}
	};

	[[gnu::always_inline]]
	inline ArduinoTWIMaster(void) {
		inc();
	}

	// Nothing to copy, but make sure we increment the TWI reference counter
	[[gnu::always_inline]]
	inline ArduinoTWIMaster(const ArduinoTWIMaster&) {
		inc();
	}

	[[gnu::always_inline]]
	inline ArduinoTWIMaster(ArduinoTWIMaster&&) {
		inc();
	}

	// Assignment operator would dec() then inc(), so overall: no-op

	[[gnu::always_inline]]
	inline ~ArduinoTWIMaster(void) {
		dec();
	}

	[[gnu::always_inline]]
	inline void set_max_clock(uint32_t hz, bool force = false) {
		_set_max_clock(hz, force);
	}

	[[nodiscard,gnu::always_inline]]
	inline Transmission begin_transmission(uint8_t address) {
		Wire.beginTransmission(address);
		return Transmission();
	}

	[[nodiscard,gnu::always_inline]]
	inline Error send(uint8_t address, uint8_t value) {
		auto t = begin_transmission(address);
		t.write(value);
		return t.stop();
	}

	[[nodiscard,gnu::always_inline]]
	inline Request request_from(
		uint8_t address,
		uint8_t count,
		bool stop = true
	) {
		Wire.requestFrom(address, count, uint8_t(stop));
		return Request();
	}

	[[gnu::always_inline]]
	inline bool request_from(
		uint8_t address,
		void *buffer,
		uint16_t count,
		uint16_t maxMicros,
		bool stop = true
	) {
		return request_from(address, count, stop)
			.read(buffer, count, maxMicros);
	}
};

#endif

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

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

class ArduinoTWIMaster {
	static uint8_t refCount;
	static uint32_t lastClockHz;

	static void inc(void);
	static void dec(void);

public:
	enum Error : uint8_t {
		SUCCESS = 0,
		DATA_TOO_LONG = 1,
		NACK_ADDR = 2,
		NACK_DATA = 3,
		OTHER = 4
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
	inline void set_clock(uint32_t hz, bool force = false) {
		if(lastClockHz != hz || force) {
			Wire.setClock(hz);
			lastClockHz = hz;
		}
	}

	// TODO: ideally this would return a new object which owns the transaction
	// (with endTransmission happening implicitly when destroyed), but this
	// would add lots of overhead in places where it is used.
	[[gnu::always_inline]]
	inline void begin_transmission(uint8_t address) {
		Wire.beginTransmission(address);
	}

	[[gnu::always_inline]]
	inline void begin_transmission(uint8_t address, uint32_t hz) {
		set_clock(hz);
		begin_transmission(address);
	}

	[[gnu::always_inline]]
	inline Error end_transmission(bool stop = true) {
		return Error(Wire.endTransmission(stop));
	}

	[[gnu::always_inline]]
	inline uint8_t request_from(
		uint8_t address,
		uint8_t count,
		bool stop = true
	) {
		return Wire.requestFrom(address, count, uint8_t(stop));
	}

	[[gnu::always_inline]]
	inline uint8_t request_from(
		uint8_t address,
		uint8_t count,
		uint32_t hz,
		bool stop = true
	) {
		set_clock(hz);
		return request_from(address, count, stop);
	}

	[[gnu::always_inline]]
	inline void write(uint8_t data) {
		Wire.write(data);
	}

	[[gnu::always_inline]]
	inline void write(const uint8_t *data, uint8_t count) {
		Wire.write(data, count);
	}

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
};

#endif

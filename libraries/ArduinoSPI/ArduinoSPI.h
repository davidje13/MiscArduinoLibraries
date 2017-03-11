/*
 * ArduinoSPI - Wrapper of standard Arduino SPI access.
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

#ifndef ARDUINOSPI_H_INCLUDED
#define ARDUINOSPI_H_INCLUDED

#include <SPI.h>

#include "ext.h"

class ArduinoSPI {
public:
	enum class ByteOrder : uint8_t {
		LSB_FIRST,
		MSB_FIRST
	};

	enum class DataMode : uint8_t {
		MODE0 = SPI_MODE0,
		MODE1 = SPI_MODE1,
		MODE2 = SPI_MODE2,
		MODE3 = SPI_MODE3
	};

	ArduinoSPI(void) {
		SPI.begin();
	}

	// Nothing to copy, but make sure we increment the SPI reference counter
	ArduinoSPI(const ArduinoSPI&) {
		SPI.begin();
	}

	ArduinoSPI(ArduinoSPI&&) {
		SPI.begin();
	}

	// Assignment operator would end() then begin(), so overall: no-op

	~ArduinoSPI(void) {
		// Arduino's SPI library balances begin() and end() calls internally,
		// so just call end whenever we go out of scope
		SPI.end();
	}

	// TODO: ideally this would return a new object which owns the transaction
	// (with endTransaction happening implicitly when destroyed), but this
	// would add lots of overhead in places where it is used.
	[[gnu::always_inline]]
	inline void begin_transaction(
		uint32_t maxFrequency,
		ByteOrder byteOrder,
		DataMode dataMode
	) {
		SPI.beginTransaction(SPISettings(
			maxFrequency,
			(byteOrder == ByteOrder::LSB_FIRST) ? LSBFIRST : MSBFIRST,
			uint8_t(dataMode)
		));
	}

	[[gnu::always_inline]]
	inline void end_transaction(void) {
		SPI.endTransaction();
	}

	[[gnu::always_inline]]
	inline uint8_t transfer(uint8_t data) {
		return SPI.transfer(data);
	}

	[[gnu::always_inline]]
	inline uint16_t transfer16(uint16_t data) {
		return SPI.transfer16(data);
	}

	[[gnu::always_inline]]
	inline void transfer(uint8_t *buffer, uint8_t size) {
		SPI.transfer(buffer, size);
	}
};

#endif

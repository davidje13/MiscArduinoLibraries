/*
 * ArduinoSerial - Wrapper of standard Arduino Serial access.
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

#ifndef ARDUINOSERIAL_H_INCLUDED
#define ARDUINOSERIAL_H_INCLUDED

#include "ext.h"

template <typename SerialT>
class ArduinoSerial {
	SerialT *serial;

public:
	void begin(uint32_t baud) {
		serial->begin(baud);
	}

	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline bool can_listen(void) {
		return true;
	}

	[[gnu::always_inline]]
	inline void listen(void) {
	}

	[[nodiscard]]
	inline uint8_t read(void) {
		return serial->read();
	}

	inline void write(uint8_t byte) {
		serial->write(byte);
	}

	[[nodiscard]]
	inline uint8_t available(void) {
		return serial->available();
	}

	constexpr inline ArduinoSerial(SerialT *serial) : serial(serial) {}
};

template <typename SerialT>
[[gnu::always_inline,nodiscard]]
inline ArduinoSerial<SerialT> MakeArduinoSerial(SerialT *serial) {
	return ArduinoSerial<SerialT>(serial);
}

#endif

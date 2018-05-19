/*
 * ArduinoSoftwareSerial - Wrapper of standard Arduino Serial access.
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

#ifndef ARDUINOSOFTWARESERIAL_H_INCLUDED
#define ARDUINOSOFTWARESERIAL_H_INCLUDED

#include "ext.h"
#include <VoidPin.h>
#include <SoftwareSerial.h>

template <bool canListen>
class ArduinoSoftwareSerial {
	SoftwareSerial *serial;

public:
	[[gnu::always_inline]]
	inline void begin(uint32_t baud) {
		serial->begin(baud);
	}

	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline bool can_listen(void) {
		return canListen;
	}

	[[gnu::always_inline]]
	inline void listen(void) {
		if(canListen) {
			serial->listen();
		}
	}

	[[gnu::always_inline,nodiscard]]
	inline uint8_t read(void) {
		if(!canListen) {
			return 0;
		}
		return serial->read();
	}

	[[gnu::always_inline]]
	inline void write(uint8_t byte) {
		serial->write(byte);
	}

	[[gnu::always_inline,nodiscard]]
	inline uint8_t available(void) {
		if(!canListen) {
			return 0;
		}
		return serial->available();
	}

	constexpr inline ArduinoSoftwareSerial(SoftwareSerial *serial)
		: serial(serial)
	{}
};

template <typename TXPinT, typename RXPinT>
[[gnu::always_inline,nodiscard]]
inline ArduinoSoftwareSerial<RXPinT::exists()> MakeArduinoSoftwareSerial(
	TXPinT tx,
	RXPinT rx
) {
	// SoftwareSerial's move constructor doesn't work, so we must put it on the
	// heap to ensure it won't move around
	SoftwareSerial *serial = new SoftwareSerial(
		// default to built-in RX (0) in the hope that nothing bad will happen;
		// we will block any read functions anyway
		rx.exists() ? rx.get_raw_pin() : 0,
		tx.get_raw_pin()
	);
	return ArduinoSoftwareSerial<RXPinT::exists()>(serial);
}

template <typename TXPinT>
[[gnu::always_inline,nodiscard]]
inline ArduinoSoftwareSerial<false> MakeArduinoSoftwareSerial(TXPinT tx) {
	return MakeArduinoSoftwareSerial(tx, VoidPin());
}

#endif

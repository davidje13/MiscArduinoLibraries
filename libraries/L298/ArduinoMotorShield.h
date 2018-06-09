/*
 * ArduinoMotorShield - Dual full-bridge driver communication library.
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

#ifndef ARDUINO_MOTOR_SHIELD_H_INCLUDED
#define ARDUINO_MOTOR_SHIELD_H_INCLUDED

#include "ext.h"
#include "L298.h"
#include <ArduinoPin.h>
#include <ArduinoAnalogPin.h>

template <typename DPinT, typename BrPinT, typename EnPinT, typename SensePinT>
class ArduinoMotorShield : public L298Channel {
	ext::Flattener<DPinT,BrPinT> dPin;
#define brPin dPin.flattened_value
	ext::Flattener<EnPinT,SensePinT> enPin;
#define sensePin enPin.flattened_value

public:
	[[gnu::always_inline]]
	inline void forward(void) {
		dPin.high();
		brPin.low();
		enPin.high();
	}

	[[gnu::always_inline]]
	inline void forward(uint8_t pwm) {
		dPin.high();
		brPin.low();
		enPin.pwm(pwm);
	}

	[[gnu::always_inline]]
	inline void reverse(void) {
		dPin.low();
		brPin.low();
		enPin.high();
	}

	[[gnu::always_inline]]
	inline void reverse(uint8_t pwm) {
		dPin.low();
		brPin.low();
		enPin.pwm(pwm);
	}

	[[gnu::always_inline]]
	inline void brake(void) {
		brPin.high();
		enPin.high();
	}

	[[gnu::always_inline]]
	inline void brake(uint8_t pwm) {
		brPin.high();
		enPin.pwm(pwm);
	}

	[[gnu::always_inline]]
	inline void freewheel(void) {
		brPin.low();
		enPin.low();
	}

	void set(int16_t speed) {
		if(speed == 0) {
			freewheel();
			return;
		}

		brPin.low();
		if(speed > 0) {
			dPin.high();
			enPin.pwm(uint8_t(speed));
		} else {
			dPin.low();
			enPin.pwm(uint8_t(-speed));
		}
	}

	[[gnu::always_inline]]
	inline uint16_t sensed_voltage_raw(void) const {
		return sensePin.read_analog();
	}

	[[gnu::always_inline]]
	inline float sensed_voltage(float aRef = 5.0f) const {
		return sensed_voltage_raw() / (aRef * 1024.0f);
	}

	[[gnu::always_inline]]
	inline float current(float aRef = 5.0f) const {
		// Arduino shield uses resistors of 0R15 (0.15Ohm)
		return sensed_voltage(aRef) / 0.15f;
	}

	ArduinoMotorShield(DPinT dir, BrPinT brake, EnPinT en, SensePinT sense)
		: dPin(dir, brake)
		, enPin(en, sense)
	{
		dPin.set_output();
		brPin.set_output();
		enPin.set_output();
		sensePin.set_input();
		enPin.low();
	}

	ArduinoMotorShield(ArduinoMotorShield&&) = default;

#undef dPin
#undef sensePin
};

template <typename DPinT, typename BrPinT, typename EnPinT, typename SensePinT>
[[gnu::always_inline,nodiscard]]
inline ArduinoMotorShield<DPinT, BrPinT, EnPinT, SensePinT>
MakeArduinoMotorShieldChannel(
	DPinT dir,
	BrPinT brake,
	EnPinT en,
	SensePinT sense
) {
	return ArduinoMotorShield<DPinT, BrPinT, EnPinT, SensePinT>(
		dir,
		brake,
		en,
		sense
	);
}

#define MakeArduinoMotorShieldChannelA() MakeArduinoMotorShieldChannel( \
	FixedArduinoPin<12>(), \
	FixedArduinoPin<9>(), \
	FixedArduinoPin<3>(), \
	FixedArduinoAnalogPin<0>() \
)

#define MakeArduinoMotorShieldChannelA_nobrake() MakeArduinoMotorShieldChannel( \
	FixedArduinoPin<12>(), \
	VoidPin(), \
	FixedArduinoPin<3>(), \
	FixedArduinoAnalogPin<0>() \
)

#define MakeArduinoMotorShieldChannelB() MakeArduinoMotorShieldChannel( \
	FixedArduinoPin<13>(), \
	FixedArduinoPin<8>(), \
	FixedArduinoPin<11>(), \
	FixedArduinoAnalogPin<1>() \
)

#define MakeArduinoMotorShieldChannelB_nobrake() MakeArduinoMotorShieldChannel( \
	FixedArduinoPin<13>(), \
	VoidPin(), \
	FixedArduinoPin<11>(), \
	FixedArduinoAnalogPin<1>() \
)

#endif

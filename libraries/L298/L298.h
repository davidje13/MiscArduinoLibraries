/*
 * L298 - Dual full-bridge driver communication library.
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

#ifndef L298_H_INCLUDED
#define L298_H_INCLUDED

#include "ext.h"

class L298Channel {
protected:
	L298Channel(void) = default;
	L298Channel(const L298Channel&) = delete;
	L298Channel(L298Channel&&) = default;

	L298Channel &operator=(const L298Channel&) = delete;
	L298Channel &operator=(L298Channel&&) = delete;
};

template <typename FPinT, typename RPinT, typename EnPinT, typename SensePinT>
class L298Channel_impl : public L298Channel {
	ext::Flattener<FPinT,RPinT> fPin;
#define rPin fPin.flattened_value
	ext::Flattener<EnPinT,SensePinT> enPin;
#define sensePin enPin.flattened_value

	void set_dir_fwd(void) {
		rPin.begin_batch();
		fPin.high();
		rPin.low();
		rPin.send_batch();
	}

	void set_dir_rev(void) {
		rPin.begin_batch();
		fPin.low();
		rPin.high();
		rPin.send_batch();
	}

	void set_brake(void) {
		rPin.begin_batch();
		fPin.low();
		rPin.low();
		rPin.send_batch();
	}

	[[gnu::always_inline]]
	inline void set_en_sat(void) {
		enPin.high();
	}

public:
	[[gnu::always_inline]]
	inline void forward(void) {
		set_dir_fwd();
		set_en_sat();
	}

	[[gnu::always_inline]]
	inline void forward(uint8_t pwm) {
		enPin.begin_batch();
		set_dir_fwd();
		enPin.pwm(pwm);
		enPin.send_batch();
	}

	[[gnu::always_inline]]
	inline void reverse(void) {
		set_dir_rev();
		set_en_sat();
	}

	[[gnu::always_inline]]
	inline void reverse(uint8_t pwm) {
		enPin.begin_batch();
		set_dir_rev();
		enPin.pwm(pwm);
		enPin.send_batch();
	}

	[[gnu::always_inline]]
	inline void brake(void) {
		set_brake();
		set_en_sat();
	}

	[[gnu::always_inline]]
	inline void brake(uint8_t pwm) {
		enPin.begin_batch();
		set_brake();
		enPin.pwm(pwm);
	}

	[[gnu::always_inline]]
	inline void freewheel(void) {
		enPin.low();
	}

	void set(int16_t speed) {
		if(speed == 0) {
			freewheel();
			return;
		}

		enPin.begin_batch();

		if(speed > 0) {
			set_dir_fwd();
			enPin.pwm(uint8_t(speed));
		} else {
			set_dir_rev();
			enPin.pwm(uint8_t(-speed));
		}

		enPin.send_batch();
	}

	[[gnu::always_inline]]
	inline uint16_t sensed_voltage(void) const {
		return sensePin.read_analog();
	}

	[[gnu::always_inline]]
	inline float sensed_voltage(float aRef = 5.0f) const {
		return sensed_voltage() / (aRef * 1024.0f);
	}

	[[gnu::always_inline]]
	inline float current(float fixedResistorValue, float aRef = 5.0f) const {
		// Circuit forms a voltage divide
		//  from Vin,
		//  through motor,
		//  sense pin,
		//  fixed external resistor (Rfixed),
		//  then ground
		// Imotor = Vsense / Rfixed
		return sensed_voltage() / (aRef * fixedResistorValue * 1024.0f);
	}

	L298Channel_impl(FPinT fwd, RPinT rev, EnPinT en, SensePinT sense)
		: fPin(fwd, rev)
		, enPin(en, sense)
	{
		fPin.set_output();
		rPin.set_output();
		enPin.set_output();
		sensePin.set_input();
		enPin.low();
	}

	L298Channel_impl(L298Channel_impl&&) = default;

#undef rPin
#undef sensePin
};

template <typename FPinT, typename RPinT, typename EnPinT, typename SensePinT>
[[gnu::always_inline,nodiscard]]
inline L298Channel_impl<FPinT, RPinT, EnPinT, SensePinT> MakeL298Channel(
	FPinT fwd,
	RPinT rev,
	EnPinT en,
	SensePinT sense
) {
	return L298Channel_impl<FPinT, RPinT, EnPinT, SensePinT>(
		fwd,
		rev,
		en,
		sense
	);
}

#define MakeSN754410Channel(fwd, rev, en) \
	MakeL298Channel(fwd, rev, en, VoidPin())

#endif

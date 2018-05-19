/*
 * MY9221 - Sends data to a 12-channel greyscale LED controller.
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

#ifndef MY9221_H_INCLUDED
#define MY9221_H_INCLUDED

#include "ext.h"

#define CHANNELS 12

class MY9221 {
public:
	enum class Frequency : uint8_t {
		F8600000Hz = 0x00, // 8.6MHz
		F4300000Hz = 0x20, // /2
		F2150000Hz = 0x40, // /4
		F1075000Hz = 0x60, // /8
		F537500Hz  = 0x80, // /16
		F134375Hz  = 0xA0, // /64
		F67187Hz   = 0xC0, // /128
		F33594Hz   = 0xE0, // /256

		// Free running mode waits for previous frame to complete before
		// displaying new data (internal clock always enforces free running)
		EXTERNAL_FREE_RUNNING = 0x08,

		// Reset mode shows new frames as soon as they are loaded, even if the
		// current frame has not finished (greyscale appearance may glitch)
		EXTERNAL_RESET        = 0x0A,

		// One shot mode shows exactly one frame after receiving data, then
		// the display will blank until new data is received.
		// If data is received while showing a frame, the frame will be reset
		EXTERNAL_ONE_SHOT     = 0x0B
	};

	enum class Modulation : uint8_t {
		PULSE_WIDTH   = 0x00,
		PULSE_DENSITY = 0x10
	};

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t normalise(uint8_t v) {
		// LED behaviour is closer to linear with this correction
		return uint8_t((uint16_t(v) * v) / 255);
	}

	template <int bits = 16>
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint16_t normalise(uint16_t v) {
		// LED behaviour is closer to linear with this correction
		return uint16_t((uint32_t(v) * v) / ((uint32_t(1) << bits) - 1));
	}

protected:
	enum Grayscale : uint8_t {
		GRAYSCALE_8  = 0x00,
		GRAYSCALE_12 = 0x01,
		GRAYSCALE_14 = 0x02,
		GRAYSCALE_16 = 0x03
	};

	MY9221(void) = default;
	MY9221(const MY9221&) = delete;
	MY9221(MY9221&&) = default;

	MY9221 &operator=(const MY9221&) = delete;
	MY9221 &operator=(MY9221&&) = delete;
};

template <typename ClockPinT, typename DataPinT>
class MY9221_impl : public MY9221 {
	ext::Flattener<ClockPinT,uint8_t> clockPin;
	ext::Flattener<DataPinT,uint8_t> dataPin;
#define cmd0 clockPin.flattened_value
#define cmd1 dataPin.flattened_value

	// Command0 buffer:
	// 0 0 0 0 iout_fast bits0 bits1

	// Command1 buffer:
	// freq0 freq1 freq2 pwm/pdm ext-osc gen reset one-shot

	template <typename CPT, typename DPT>
	[[gnu::always_inline]]
	inline void send_part(uint16_t value, CPT &clock, DPT &data) {
		uint8_t oldSREG = SREG;
		cli();
		for(uint8_t i = 0; i < 8; ++ i) {
			data.set_fast(value >> 15);
			clock.high_fast();
			data.set_fast((value >> 14) & 1);
			clock.low_fast();
			value <<= 2;
		}
		SREG = oldSREG;
	}

	template <typename T>
	void _set(const T *values) {
		auto fastClock = clockPin.fast();
		auto fastData = dataPin.fast();

		send_part((cmd0 << 8) | cmd1, fastClock, fastData);
		for(uint8_t i = 0; i < CHANNELS; ++ i) {
			send_part(values[i], fastClock, fastData);
		}

		// trigger MY9221 internal latch mechanism
		fastData.low(); // hold for >= 220us
		delayMicroseconds(220);
		for(int i = 0; i < 4; ++ i) {
			fastData.high(); // hold for >= 70ns
			fastData.low();  // hold for >= 230ns
		}
	}

	[[gnu::always_inline]]
	inline void set_bits(uint8_t flag) {
		cmd0 = (cmd0 & ~0x03) | flag;
	}

public:
	MY9221_impl(ClockPinT dcki, DataPinT di)
		: clockPin(dcki, 0)
		, dataPin(di, uint8_t(Modulation::PULSE_DENSITY))
	{
		clockPin.set_output();
		dataPin.set_output();
		clockPin.low();
		dataPin.low();
	}

	MY9221_impl(MY9221_impl &&b)
		: clockPin(static_cast<ext::Flattener<ClockPinT,uint8_t>&&>(b.clockPin))
		, dataPin(static_cast<ext::Flattener<DataPinT,uint8_t>&&>(b.dataPin))
	{}

	// Possibly controls speed of switching output data channel when chained
	[[gnu::always_inline]]
	inline void set_iout_fast(bool fast) {
		cmd0 = (cmd0 & ~0x04) | (fast * 0x04);
	}

	// false for LED driver, true for "MY-PWM" driver
	[[gnu::always_inline]]
	inline void set_polarity(bool generator) {
		cmd1 = (cmd1 & ~0x04) | (generator * 0x04);
	}

	[[gnu::always_inline]]
	inline void set_modulation(Modulation mod) {
		cmd1 = (cmd1 & ~0x10) | uint8_t(mod);
	}

	[[gnu::always_inline]]
	inline void set_frequency(Frequency f) {
		cmd1 = (cmd1 & ~0xEB) | uint8_t(f);
	}

	[[gnu::always_inline]]
	inline void set(const uint8_t *values) {
		set_bits(GRAYSCALE_8);
		_set(values);
	}

	[[gnu::always_inline]]
	inline void set_norm(const uint8_t *values) {
		uint8_t normed[CHANNELS];
		for(uint8_t i = 0; i < CHANNELS; ++ i) {
			normed[i] = normalise(values[i]);
		}
		set(normed);
	}

	template <int bits = 16>
	[[gnu::always_inline]]
	inline void set(const uint16_t *values) {
		switch(bits) {
		case 8:
			set_bits(GRAYSCALE_8);
			break;
		case 12:
			set_bits(GRAYSCALE_12);
			break;
		case 14:
			set_bits(GRAYSCALE_14);
			break;
		case 16:
			set_bits(GRAYSCALE_16);
			break;
		}
		_set(values);
	}

	template <int bits = 16>
	[[gnu::always_inline]]
	inline void set_norm(const uint16_t *values) {
		uint16_t normed[CHANNELS];
		for(uint8_t i = 0; i < CHANNELS; ++ i) {
			normed[i] = normalise<bits>(values[i]);
		}
		set<bits>(normed);
	}

#undef cmd0
#undef cmd1
};

template <typename ClockPinT, typename DataPinT>
[[gnu::always_inline,nodiscard]]
inline MY9221_impl<ClockPinT,DataPinT> MakeMY9221(ClockPinT dcki, DataPinT di) {
	return MY9221_impl<ClockPinT, DataPinT>(dcki, di);
}

#undef CHANNELS

#endif

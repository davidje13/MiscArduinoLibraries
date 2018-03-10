/*
 * TM1638 - Shift register controlled 8888 LED display
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

#ifndef TM1638_H_INCLUDED
#define TM1638_H_INCLUDED

#include "ext.h"
#include "74HC595.h"
#include <VoidPin.h>

class TM1638 {
public:
	enum class PulseWidth : uint8_t {
		PULSE_1_16  = 0x00,
		PULSE_2_16  = 0x01,
		PULSE_4_16  = 0x02,
		PULSE_10_16 = 0x03,
		PULSE_11_16 = 0x04,
		PULSE_12_16 = 0x05,
		PULSE_13_16 = 0x06,
		PULSE_14_16 = 0x07,
	};

	enum LED : uint8_t {
		OFF    = 0x00,
		RED    = 0x01,
		GREEN  = 0x02,
		YELLOW = 0x03,
	};

	enum Segment : uint8_t {
		NONE         = 0x00,
		TOP          = 0x01,
		TOP_RIGHT    = 0x02,
		BOTTOM_RIGHT = 0x04,
		BOTTOM       = 0x08,
		BOTTOM_LEFT  = 0x10,
		TOP_LEFT     = 0x20,
		MIDDLE       = 0x40,
		DOT          = 0x80,
	};

	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline constexpr uint8_t build_segment(
		bool t, bool tl, bool tr, bool m, bool bl, bool br, bool b,
		bool dot = false
	) {
		return (
			(t  * Segment::TOP) |
			(tl * Segment::TOP_LEFT) |
			(tr * Segment::TOP_RIGHT) |
			(m  * Segment::MIDDLE) |
			(bl * Segment::BOTTOM_LEFT) |
			(br * Segment::BOTTOM_RIGHT) |
			(b  * Segment::BOTTOM) |
			(dot * Segment::DOT)
		);
	}

	/* Memory arrangement:
	 * 0x00 Digit1
	 * 0x01 LED1
	 * 0x02 Digit2
	 * 0x03 LED2
	 * ...
	 * 0x0E Digit8
	 * 0x0F LED8
	 */

protected:
	enum DataMode : uint8_t {
		// 0 1 0 0 test fixedAddr read 0
		WRITE_AUTOINC_ADDRESS = 0x40,
		WRITE_FIXED_ADDRESS   = 0x44,
		READ                  = 0x42,
		TEST                  = 0x48

		// After setting mode (1 byte), auto mode takes n+1 bytes per n
		// consecutive bytes written (address followed by values), up to a
		// maximum of 14 consecutive bytes

		// Fixed mode takes 2 bytes per byte written (address followed by
		// value). This seems to offer no advantage over auto mode (?!)
	};

	TM1638(void) = default;
	TM1638(const TM1638&) = delete;
	TM1638(TM1638&&) = default;

	TM1638 &operator=(const TM1638&) = delete;
	TM1638 &operator=(TM1638&&) = delete;
};

template <typename LatchPinT, typename ClockPinT, typename DataPinT>
class TM1638_impl : public TM1638 {
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t led_flags(bool red, bool green) {
		return (red * LED::RED) | (green * LED::GREEN);
	}

	ext::Flattener<LatchPinT,uint8_t> latchPin;
	C74HC595_impl<VoidPin,ClockPinT,DataPinT> reg;
#define state latchPin.flattened_value
#define STATE_ON   0x01
#define STATE_READ 0x02

	[[gnu::always_inline]]
	inline void send_command(uint8_t command) {
		latchPin.low();
		reg.send8_lsb(command);
		latchPin.high();
	}

	[[gnu::always_inline]]
	inline void set_write_mode(void) {
		if(state & STATE_READ) {
			send_command(DataMode::WRITE_AUTOINC_ADDRESS);
			state &= ~STATE_READ;
		}
	}

	template <int bytes>
	void set_buffer(uint8_t address, const uint8_t (&data)[bytes]) {
		set_write_mode();
		latchPin.low();
		reg.send8_lsb(0xC0 | (address & 0x0F));
		reg.send_lsb(data);
		latchPin.high();
	}

	void set_buffer(uint8_t address, uint8_t datum) {
		set_write_mode();
		latchPin.low();
		reg.send16_lsb(0xC0 | (address & 0x0F), datum);
		latchPin.high();
	}

public:
	TM1638_impl(LatchPinT stb, ClockPinT clk, DataPinT dio)
		: latchPin(stb, STATE_READ) // Assume the worst
		, reg(Make74HC595(VoidPin(), clk, dio))
	{
		latchPin.set_output();
		latchPin.high();
	}

	TM1638_impl(TM1638_impl &&b)
		: latchPin(static_cast<ext::Flattener<
			LatchPinT,
			uint8_t
		>&&>(b.latchPin))
		, reg(static_cast<C74HC595_impl<VoidPin,ClockPinT,DataPinT>&&>(b.reg))
	{
		// Prevent destructor of 'b' changing the device
		b.state &= ~STATE_ON;
	}

	~TM1638_impl(void) {
		if(state & STATE_ON) {
			send_command(0x80);
		}
	}

	void set_on(bool on, PulseWidth pulseWidth = PulseWidth::PULSE_14_16) {
		if(on) {
			send_command(0x88 | uint8_t(pulseWidth));
			state |= STATE_ON;
		} else if(state & STATE_ON) {
			send_command(0x80);
			state &= ~STATE_ON;
		}
	}

	void clear(void) {
		// Cannot send more than 14 bytes at once, so send as 2 operations
		uint8_t zero[8] = {};
		set_buffer(0x00, zero);
		set_buffer(0x08, zero);
	}

	[[gnu::always_inline]]
	inline void set_digit(uint8_t offset, uint8_t bitmask) {
		set_buffer(offset << 1, bitmask);
	}

	template <uint8_t offset = 0>
	[[gnu::always_inline]]
	inline void set_digits(uint8_t bitmask1, uint8_t bitmask2) {
		set_buffer( offset      << 1, bitmask1);
		set_buffer((offset + 1) << 1, bitmask2);
	}

	template <uint8_t offset = 0>
	[[gnu::always_inline]]
	inline void set_digits(
		uint8_t bitmask1,
		uint8_t bitmask2,
		uint8_t bitmask3
	) {
		set_buffer( offset      << 1, bitmask1);
		set_buffer((offset + 1) << 1, bitmask2);
		set_buffer((offset + 2) << 1, bitmask3);
	}

	template <uint8_t offset = 0>
	[[gnu::always_inline]]
	inline void set_digits(
		uint8_t bitmask1,
		uint8_t bitmask2,
		uint8_t bitmask3,
		uint8_t bitmask4
	) {
		set_buffer( offset      << 1, bitmask1);
		set_buffer((offset + 1) << 1, bitmask2);
		set_buffer((offset + 2) << 1, bitmask3);
		set_buffer((offset + 3) << 1, bitmask4);
	}

	template <uint8_t offset = 0, uint8_t count = 2>
	void set_digits(uint16_t bitmask) {
		for(uint8_t i = offset + count; (i --) > offset;) {
			set_buffer(i << 1, uint8_t(bitmask));
			bitmask >>= 8;
		}
	}

	template <uint8_t offset = 0, uint8_t count = 4>
	void set_digits(uint32_t bitmask) {
		for(uint8_t i = offset + count; (i --) > offset;) {
			set_buffer(i << 1, uint8_t(bitmask));
			bitmask >>= 8;
		}
	}

	template <uint8_t offset = 0, uint8_t count = 8>
	[[gnu::always_inline]]
	inline void set_digits(uint32_t bitmask1, uint32_t bitmask2) {
		set_digits<offset, 4>(bitmask1);
		set_digits<offset + 4, count - 4>(bitmask2);
	}

	template <uint8_t offset = 0, uint8_t count = 8>
	[[gnu::always_inline]]
	inline void set_digits(uint64_t bitmask) {
		set_digits<offset, 4>(uint32_t(bitmask >> 32));
		set_digits<offset + 4, count - 4>(uint32_t(bitmask));
	}

	template <uint8_t offset = 0, uint8_t count>
	[[gnu::always_inline]]
	inline void set_digits(const uint8_t (&bitmask)[count]) {
		for(uint8_t i = offset + count; (i --) > offset;) {
			set_buffer(i << 1, bitmask[i]);
		}
	}

	[[gnu::always_inline]]
	inline void set_led(uint8_t offset, bool red, bool green) {
		set_buffer((offset << 1) | 1, led_flags(red, green));
	}

	template <uint8_t offset = 0, uint8_t count = 4>
	void set_leds(uint8_t ledMask) {
		for(uint8_t i = offset + count; (i --) > offset;) {
			set_buffer((i << 1) | 1, ledMask & 0x03);
			ledMask >>= 2;
		}
	}

	template <uint8_t offset = 0, uint8_t count = 8>
	[[gnu::always_inline]]
	inline void set_leds(uint8_t ledMask1, uint8_t ledMask2) {
		set_leds<offset, 4>(ledMask1);
		set_leds<offset + 4, count - 4>(ledMask2);
	}

	template <uint8_t offset = 0, uint8_t count = 8>
	void set_leds(uint16_t ledMask) {
		for(uint8_t i = offset + count; (i --) > offset;) {
			set_buffer((i << 1) | 1, ledMask & 0x03);
			ledMask >>= 2;
		}
	}

	[[gnu::always_inline]]
	inline void set(
		uint8_t offset,
		uint8_t digitBitmask,
		bool red,
		bool green
	) {
		uint8_t data[] = {
			digitBitmask,
			led_flags(red, green)
		};
		set_buffer(offset << 1, data);
	}

	template <uint8_t offset = 0, uint8_t count = 2>
	void set(uint16_t digitBitmask, uint8_t ledMask) {
		uint8_t data[count*2];
		for(uint8_t i = count; (i --) > 0;) {
			data[ i<<1   ] = uint8_t(digitBitmask);
			data[(i<<1)|1] = ledMask & 0x03;
			digitBitmask >>= 8;
			ledMask >>= 2;
		}
		set_buffer(offset << 1, data);
	}

	template <uint8_t offset = 0, uint8_t count = 4>
	void set(uint32_t digitBitmask, uint8_t ledMask) {
		uint8_t data[count*2];
		for(uint8_t i = count; (i --) > 0;) {
			data[ i<<1   ] = uint8_t(digitBitmask);
			data[(i<<1)|1] = ledMask & 0x03;
			digitBitmask >>= 8;
			ledMask >>= 2;
		}
		set_buffer(offset << 1, data);
	}

	template <uint8_t offset = 0, uint8_t count = 8>
	[[gnu::always_inline]]
	inline void set(
		uint32_t digitBitmask1,
		uint32_t digitBitmask2,
		uint16_t ledMask
	) {
		set<offset, 4>(digitBitmask1, uint8_t(ledMask >> 8));
		set<offset + 4, count - 4>(digitBitmask2, uint8_t(ledMask));
	}

	template <uint8_t offset = 0, uint8_t count = 8>
	[[gnu::always_inline]]
	inline void set(uint64_t digitBitmask, uint16_t ledMask) {
		set<offset, 4>(uint32_t(digitBitmask >> 32), uint8_t(ledMask >> 8));
		set<offset + 4, count - 4>(uint32_t(digitBitmask), uint8_t(ledMask));
	}

	template <uint8_t offset = 0, uint8_t count>
	[[gnu::always_inline]]
	inline void set(const uint8_t (&digitBitmask)[count], uint16_t ledMask) {
		uint8_t data[count*2];
		for(uint8_t i = count; (i --) > 0;) {
			data[ i<<1   ] = digitBitmask[i];
			data[(i<<1)|1] = ledMask & 0x03;
			ledMask >>= 2;
		}
		set_buffer(offset << 1, data);
	}

	[[gnu::always_inline]]
	inline uint32_t read_buttons(void) {
		latchPin.low();
		reg.send8_lsb(DataMode::READ);
		uint32_t result = reg.read32_msb();
		latchPin.high();
		state |= STATE_READ;
		return result;
	}

#undef state
#undef STATE_ON
#undef STATE_READ
};

template <typename LatchPinT, typename ClockPinT, typename DataPinT>
[[gnu::always_inline]]
inline TM1638_impl<
	LatchPinT,
	ClockPinT,
	DataPinT
> MakeTM1638(LatchPinT stb, ClockPinT clk, DataPinT dio) {
	return TM1638_impl<LatchPinT, ClockPinT, DataPinT>(stb, clk, dio);
}

#endif

/*
 * MCP23017 - MCP23017 (IO expander, I2C) communication library.
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

#ifndef MCP23017_H_INCLUDED
#define MCP23017_H_INCLUDED

#include "MCP23017_pin.h"
#include "ext.h"

// TODO: interrupt configuration

class MCP23017 {
public:
	enum class Pin : uint16_t {
		NONE  = 0,
		A_0   = 0x0001,
		A_1   = 0x0002,
		A_2   = 0x0004,
		A_3   = 0x0008,
		A_4   = 0x0010,
		A_5   = 0x0020,
		A_6   = 0x0040,
		A_7   = 0x0080,
		B_0   = 0x0100,
		B_1   = 0x0200,
		B_2   = 0x0400,
		B_3   = 0x0800,
		B_4   = 0x1000,
		B_5   = 0x2000,
		B_6   = 0x4000,
		B_7   = 0x8000,
		ALL_A = 0x00FF,
		ALL_B = 0xFF00,
		ALL   = 0xFFFF
	};

	enum class ConnectionStatus : uint8_t {
		CONNECTED = 0,
		REQUEST_ERR_DATA_TOO_LONG = 1,
		REQUEST_ERR_NACK_ADDR = 2,
		REQUEST_ERR_NACK_DATA = 3,
		REQUEST_ERR_OTHER = 4,
		READ_TIMEOUT
	};

protected:
	enum class Address : uint8_t {
		B0_IODIRA      = 0x00, // input / output mask; 1=input, 0=output
		B0_IODIRB      = 0x01,
		B0_IPOLA       = 0x02, // polarity: 1=inverted reads, 0=normal
		B0_IPOLB       = 0x03,
		B0_INTENA      = 0x04, // interrupt enable: 1=trigger interrupt on change
		B0_INTENB      = 0x05,
		B0_INTDEFA     = 0x06, // default value: if enabled in inten and intcon, triggers an interrupt when value differs
		B0_INTDEFB     = 0x07,
		B0_INTCONA     = 0x08, // interrupt mode: 1=check against default. 0=look for any change
		B0_INTCONB     = 0x09,
		B0_IOCON       = 0x0A, // bank, mirror, seqop, disslw, haen, odr, intpol, -. Also at 0x0B
		// see 0x0A for 0x0B
		B0_GPPUA       = 0x0C, // pullup enabled (inputs only)
		B0_GPPUB       = 0x0D,
		B0_INTFA       = 0x0E, // read only: interrupt flags (which pin caused interrupt)
		B0_INTFB       = 0x0F,
		B0_INTCAPA     = 0x10, // read only: state on interrupt (values of all pins when interrupt triggered). Cleared by read of INTCAP or GPIO
		B0_INTCAPB     = 0x11,
		B0_GPIOA       = 0x12, // current values of ports. writing sets OLAT instead
		B0_GPIOB       = 0x13,
		B0_OLATA       = 0x14, // latches. In output mode, this sets whether a pin is HIGH or LOW
		B0_OLATB       = 0x15,

		B1_IODIRA      = 0x00,
		B1_IPOLA       = 0x01,
		B1_INTENA      = 0x02,
		B1_INTDEFA     = 0x03,
		B1_INTCONA     = 0x04,
		B1_IOCON       = 0x05, // bank, mirror, seqop, disslw, haen, odr, intpol, -. Also at 0x15
		B1_GPPUA       = 0x06,
		B1_INTFA       = 0x07,
		B1_INTCAPA     = 0x08,
		B1_GPIOA       = 0x09,
		B1_OLATA       = 0x0A,
		B1_IODIRB      = 0x10,
		B1_IPOLB       = 0x11,
		B1_INTENB      = 0x12,
		B1_INTDEFB     = 0x13,
		B1_INTCONB     = 0x14,
		// see 0x05 for 0x15
		B1_GPPUB       = 0x16,
		B1_INTFB       = 0x17,
		B1_INTCAPB     = 0x18,
		B1_BPIOB       = 0x19,
		B1_OLATB       = 0x1A
	};

	MCP23017(void) = default;
	MCP23017(const MCP23017&) = delete;
	MCP23017(MCP23017&&) = default;

	MCP23017 &operator=(const MCP23017&) = delete;
	MCP23017 &operator=(MCP23017&&) = delete;
};

#define BATCH_COUNT_MASK 0xF8
#define LATCH_CHANGED 0x01
#define PULLUP_CHANGED 0x02
#define IO_CHANGED 0x04

template <typename TwiT>
class MCP23017_impl : public MCP23017 {
	TwiT twiComm;
	uint16_t state;
	uint16_t dir;
	uint8_t batch;
	uint8_t subAddress;

	typename TwiT::Error send_bank(Address address, uint16_t value) {
		auto t = twiComm.begin_transmission(i2c_addr());
		t.write(uint8_t(address));
		t.write(uint8_t(value));
		t.write(uint8_t(value >> 8));
		return t.stop();
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error send_io(void) {
		return send_bank(Address::B0_IODIRA, dir);
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error send_latch(void) {
		return send_bank(Address::B0_OLATA, state);
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error send_pullup(void) {
		return send_bank(Address::B0_GPPUA, state);
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool is_batching(void) const {
		return batch;
	}

	class FastAccess {
		MCP23017_impl *owner;
		typename TwiT::Transmission tx;

		FastAccess(MCP23017_impl *owner)
			: owner(owner)
			, tx(owner->twiComm.begin_transmission(owner->twiComm.i2c_addr()))
		{
			tx.write(uint8_t(Address::B0_OLATA));
		}

		friend class MCP23017_impl;

	public:
		void set(uint16_t values) {
			if(owner->state == values) {
				return;
			}
			// TODO: is this the correct API?
			owner->state = values;
			tx.write(uint8_t(values));
			tx.write(uint8_t(values >> 8));
		}

		[[gnu::always_inline]]
		inline void set(uint16_t values, uint16_t mask) {
			set((owner->state & ~mask) | (values & mask));
		}

		[[gnu::always_inline]]
		inline void set_a(uint8_t a) {
			set(a, uint16_t(Pin::ALL_A));
		}

		[[gnu::always_inline]]
		inline void set_b(uint8_t b) {
			set(uint16_t(b) << 8, uint16_t(Pin::ALL_B));
		}

		[[gnu::always_inline]]
		inline void set_ab(uint8_t a, uint8_t b) {
			set((uint16_t(b) << 8) | a);
		}

		[[gnu::always_inline]]
		inline void set_port(Pin pin, bool high) {
			uint16_t p = uint16_t(pin);
			set((owner->state & ~p) | (p * high));
		}

		[[gnu::always_inline]]
		inline void set_port_high(Pin pin) {
			uint16_t p = uint16_t(pin);
			set(owner->state | p);
		}

		[[gnu::always_inline]]
		inline void set_port_low(Pin pin) {
			uint16_t p = uint16_t(pin);
			set(owner->state & ~p);
		}

		~FastAccess(void) {
			tx.stop();
		}
	};

public:
	ConnectionStatus connection_status(void) {
		auto err = twiComm.send(i2c_addr(), uint8_t(Address::B0_IOCON));
		if(err != TwiT::Error::SUCCESS) {
			return ConnectionStatus(err);
		}
		uint8_t result;
		if(!twiComm.request_from(i2c_addr(), &result, 1, 10000)) {
			return ConnectionStatus::READ_TIMEOUT;
		}
		return ConnectionStatus::CONNECTED;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	uint8_t i2c_addr(void) const {
		return 0x20 | (subAddress & 0x07);
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr uint32_t i2c_speed(void) {
		return 1700000; // Device supports up to 1.7MHz
	}

	void set(uint16_t values) {
		if(state != values) {
			state = values;
			if(is_batching()) {
				batch |= LATCH_CHANGED;
			} else {
				send_latch();
			}
		}
	}

	[[gnu::always_inline]]
	inline void set(uint16_t values, uint16_t mask) {
		set((state & ~mask) | (values & mask));
	}

	[[gnu::always_inline]]
	inline void set_a(uint8_t a) {
		set(a, uint16_t(Pin::ALL_A));
	}

	[[gnu::always_inline]]
	inline void set_b(uint8_t b) {
		set(uint16_t(b) << 8, uint16_t(Pin::ALL_B));
	}

	[[gnu::always_inline]]
	inline void set_ab(uint8_t a, uint8_t b) {
		set((uint16_t(b) << 8) | a);
	}

	void set_port_high(Pin pin) {
		uint16_t p = uint16_t(pin);
		if((state & p) != p) {
			state |= p;
			if(is_batching()) {
				batch |= LATCH_CHANGED;
			} else {
				send_latch();
			}
		}
	}

	void set_port_low(Pin pin) {
		uint16_t p = uint16_t(pin);
		if((state & p) != 0) {
			state &= ~p;
			if(is_batching()) {
				batch |= LATCH_CHANGED;
			} else {
				send_latch();
			}
		}
	}

	void set_port(Pin pin, bool high) {
		uint16_t p = uint16_t(pin);
		uint16_t target = p * high;
		if((state & p) != target) {
			state = (state & ~p) | target;
			if(is_batching()) {
				batch |= LATCH_CHANGED;
			} else {
				send_latch();
			}
		}
	}

	void set_input(Pin pin, bool pullup = false) {
		uint16_t p = uint16_t(pin);
		uint16_t target = p * pullup;
		if((dir & p) != p) {
			dir |= p;
			state = (state & ~p) | target;
			if(is_batching()) {
				batch |= IO_CHANGED | PULLUP_CHANGED;
			} else {
				send_io();
				send_pullup();
			}
		} else if((state & p) != target) {
			state = (state & ~p) | target;
			if(is_batching()) {
				batch |= PULLUP_CHANGED;
			} else {
				send_pullup();
			}
		}
	}

	void set_output(Pin pin) {
		uint16_t p = uint16_t(pin);
		if((dir & p) != 0) {
			dir &= ~p;
			if(is_batching()) {
				batch |= IO_CHANGED | LATCH_CHANGED;
			} else {
				send_io();
				send_latch();
			}
		}
	}

	[[nodiscard]]
	uint16_t get(uint16_t timeoutMicros = 0xFFFF) {
		uint8_t result[2];
		twiComm.send(i2c_addr(), Address::B0_GPIOA);
		if(!twiComm.request_from(i2c_addr(), &result, 2, timeoutMicros)) {
			return 0;
		}
		return result[0] | (uint16_t(result[1]) << 8);
	}

	[[nodiscard,gnu::always_inline]]
	inline bool get_port(Pin pin, uint16_t timeoutMicros = 0xFFFF) {
		uint16_t p = uint16_t(pin);
		if(!p) {
			return false;
		}

		bool bankA = (p & uint16_t(Pin::ALL_A));

		uint8_t result;
		twiComm.send(i2c_addr(), bankA ? Address::B0_GPIOA : Address::B0_GPIOB);
		if(!twiComm.request_from(i2c_addr(), &result, 1, timeoutMicros)) {
			return false;
		}
		if(bankA) {
			return result & p;
		} else {
			return result & (p >> 8);
		}
	}

	MCP23017_pin<MCP23017_impl> pin(Pin p) {
		return MCP23017_pin<MCP23017_impl>(this, p);
	}

	template <Pin p>
	MCP23017_fixedpin<MCP23017_impl, p> pin(void) {
		return MCP23017_fixedpin<MCP23017_impl, p>(this);
	}

	FastAccess fast_output(void) {
		return FastAccess(this);
	}

	inline void begin_batch(void) {
		batch += 0x08;
	}

	void send_batch(void) {
		batch -= 0x08;
		if((batch & BATCH_COUNT_MASK) == 0) {
			if(batch & IO_CHANGED) {
				send_io();
			}
			if(batch & LATCH_CHANGED) {
				send_latch();
			}
			if(batch & PULLUP_CHANGED) {
				send_pullup();
			}
			batch = 0;
		}
	}

	MCP23017_impl(TwiT twi, uint8_t subAddress)
		: twiComm(twi)
		, state(0x0000)
		, dir(0xFFFF) // all pins default to input
		, batch(0)
		, subAddress(subAddress & 0x07)
	{
		twiComm.set_max_clock(i2c_speed());
	}

	MCP23017_impl(MCP23017_impl &&b)
		: twiComm(static_cast<TwiT&&>(b.twiComm))
		, state(b.state)
		, dir(b.dir)
		, batch(b.batch)
		, subAddress(b.subAddress)
	{}
};

#undef BATCH_COUNT_MASK
#undef LATCH_CHANGED
#undef PULLUP_CHANGED
#undef IO_CHANGED

template <typename TwiT>
[[gnu::always_inline,nodiscard]]
inline MCP23017_impl<TwiT> MakeMCP23017(TwiT twi, uint8_t subAddress = 0) {
	return MCP23017_impl<TwiT>(twi, subAddress);
}

#endif

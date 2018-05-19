/*
 * PCF8574 - IO Expander communication library.
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

#ifndef PCF8574_H_INCLUDED
#define PCF8574_H_INCLUDED

#include "PCF8574_pin.h"
#include "PCF8574_pinbank.h"
#include "ext.h"

class PCF8574 {
protected:
	PCF8574(void) = default;
	PCF8574(const PCF8574&) = delete;
	PCF8574(PCF8574&&) = default;

	PCF8574 &operator=(const PCF8574&) = delete;
	PCF8574 &operator=(PCF8574&&) = delete;
};

template <
	typename TwiT,
	typename IntPinT
>
class PCF8574_impl : public PCF8574 {
	static const constexpr uint8_t I2C_ADDR = 0x20;
	static const constexpr uint8_t I2C_ADDR_A = 0x38;

	ext::Flattener<TwiT,uint8_t> twiComm;
#define addr twiComm.flattened_value
	ext::Flattener<IntPinT,uint8_t> intPin; // TODO: actually use this if available
#define bits intPin.flattened_value

	[[gnu::pure,nodiscard,gnu::always_inline]]
	uint8_t i2c_addr(void) const {
		if(addr & 0x08) {
			return I2C_ADDR_A | (addr & 0x07);
		} else {
			return I2C_ADDR | (addr & 0x07);
		}
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr uint32_t i2c_speed(void) {
		return 100000; // Device supports up to 100kHz
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline bool is_batching(void) const {
		return addr & 0xF0;
	}

	bool read(
		uint8_t *target,
		uint16_t maxMicros
	) {
		return twiComm.request_from(
			i2c_addr(), i2c_speed(),
			target, 1,
			maxMicros
		);
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error send(void) {
		return twiComm.send(i2c_addr(), i2c_speed(), bits);
	}

public:
	bool connected(void) {
		uint8_t state;
		return read(&state, 10000);
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error set(uint8_t value) {
		bits = value;
		return is_batching() ? TwiT::Error::SUCCESS : send();
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error set(uint8_t value, uint8_t mask) {
		bits = (bits & ~mask) | (value & mask);
		return is_batching() ? TwiT::Error::SUCCESS : send();
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error set_port_high(uint8_t port) {
		bits |= (1 << port);
		return is_batching() ? TwiT::Error::SUCCESS : send();
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error set_port_low(uint8_t port) {
		bits &= ~(1 << port);
		return is_batching() ? TwiT::Error::SUCCESS : send();
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error set_port(uint8_t port, bool high) {
		uint8_t mask = (1 << port);
		bits = (bits & ~mask) | (high * mask);
		return is_batching() ? TwiT::Error::SUCCESS : send();
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error set_input(uint8_t port) {
		// when ports are high they are also in an input mode
		return set_port_high(port);
	}

	[[nodiscard]]
	uint8_t get(void) {
		uint8_t result;
		while(true) {
			if(read(&result, 10000)) {
				return result;
			}
		}
	}

	[[nodiscard,gnu::always_inline]]
	inline bool get_port(uint8_t port) {
		return (get() >> port) & 1;
	}

	PCF8574_pin<PCF8574_impl> pin(uint8_t p) {
		return PCF8574_pin<PCF8574_impl>(this, p);
	}

	template <uint8_t p>
	PCF8574_fixedpin<PCF8574_impl, p> pin(void) {
		return PCF8574_fixedpin<PCF8574_impl, p>(this);
	}

	template <uint8_t size>
	PCF8574_pinbank<PCF8574_impl, size> pinBank(uint8_t p) {
		return PCF8574_pinbank<PCF8574_impl, size>(this, p);
	}

	template <uint8_t p, uint8_t size>
	PCF8574_fixedpinbank<PCF8574_impl, p, (p + size)> pinBank(void) {
		return PCF8574_fixedpinbank<PCF8574_impl, p, (p + size)>(this);
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
		addr += 0x10;
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error send_batch(void) {
		addr -= 0x10;
		return is_batching() ? TwiT::Error::SUCCESS : send();
	}

	PCF8574_impl(TwiT twi, IntPinT interrupt, bool isPCF8574A, uint8_t address)
		: twiComm(twi, (address & 0x07) | (isPCF8574A ? 0x08 : 0x00))
		, intPin(interrupt, 0xFF)
	{
		intPin.set_input();
	}

	PCF8574_impl(PCF8574_impl&&) = default;

#undef addr
#undef bits
};

template <
	typename TwiT,
	typename IntPinT
>
[[gnu::always_inline,nodiscard]]
inline PCF8574_impl<TwiT, IntPinT> MakePCF8574(
	TwiT twi,
	IntPinT interrupt, // optional (use VoidPin to omit)
	bool isPCF8574A,
	uint8_t address = 0
) {
	return PCF8574_impl<TwiT, IntPinT>(
		twi,
		interrupt,
		isPCF8574A,
		address
	);
}

#endif

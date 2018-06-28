/*
 * 24LC256 - EEPROM communication library.
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

#ifndef C24LC256_H_INCLUDED
#define C24LC256_H_INCLUDED

#include "ext.h"

class C24LC256 {
protected:
	C24LC256(void) = default;
	C24LC256(const C24LC256&) = delete;
	C24LC256(C24LC256&&) = default;

	C24LC256 &operator=(const C24LC256&) = delete;
	C24LC256 &operator=(C24LC256&&) = delete;
};

// TODO:
// * support writing only parts which have changed (slower but better lifetime)
// * support wear-levelling
//   (e.g. see https://electronics.stackexchange.com/q/60342/38729)

template <typename TwiT>
class C24LC256_impl : public C24LC256 {
	TwiT twiComm;
	uint8_t subAddress;

	typename TwiT::Error _write_bytes_tiny(
		uint16_t address,
		const uint8_t *data,
		uint8_t count
	) {
		// Chip takes up to 5ms to flash memory before
		// it is ready for the next command. During this
		// time it will not acknowledge the address, so
		// we can keep retrying the send to maximise
		// throughput.

		// Wait up to 6ms
		for(uint8_t attempt = 0; attempt < 12; ++ attempt) {
			auto t = twiComm.begin_transmission(i2c_addr());
			t.write(uint8_t(address >> 8));
			t.write(uint8_t(address));
			t.write(data, count);
			auto err = t.stop();
			if(err != TwiT::Error::NACK_ADDR) {
				return err;
			}
			delayMicroseconds(500);
		}
		return TwiT::Error::NACK_ADDR;
	}

	typename TwiT::Error _write_bytes(
		uint16_t address,
		const uint8_t *data,
		uint8_t count
	) {
		const uint8_t batch = TwiT::max_write_bytes() - 2;
		while(count > batch) {
			auto err = _write_bytes_tiny(address, data, batch);
			if(err != TwiT::Error::SUCCESS) {
				return err;
			}
			data += batch;
			address += batch;
			count -= batch;
		}
		return _write_bytes_tiny(address, data, count);
	}

public:
	[[gnu::pure,nodiscard,gnu::always_inline]]
	uint8_t i2c_addr(void) const {
		return 0x50 | (subAddress & 0x07);
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr uint32_t i2c_speed(void) {
		return 400000; // Device supports up to 400kHz
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr uint8_t page_size_bytes(void) {
		return 64;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr uint16_t size_bytes(void) {
		return 32768;
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error write_byte(uint16_t address, uint8_t data) {
		return _write_bytes_tiny(address, &data, 1);
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error write_page(
		uint16_t page,
		const uint8_t data[page_size_bytes()]
	) {
		const uint8_t ps = page_size_bytes();
		return _write_bytes(page * ps, data, ps);
	}

	typename TwiT::Error write_bytes(
		uint16_t address,
		const uint8_t *data,
		uint16_t count
	) {
		const uint8_t ps = page_size_bytes();
		uint8_t leadingBytes = ps - (address % ps);

		if(count <= leadingBytes) {
			return _write_bytes(address, data, count);
		}

		auto err = _write_bytes(address, data, leadingBytes);
		if(err != TwiT::Error::SUCCESS) {
			return err;
		}
		address += leadingBytes;
		data += leadingBytes;
		count -= leadingBytes;

		while(count > ps) {
			err = _write_bytes(address, data, ps);
			if(err != TwiT::Error::SUCCESS) {
				return err;
			}
			address += ps;
			data += ps;
			count -= ps;
		}

		return _write_bytes(address, data, uint8_t(count));
	}

	[[gnu::always_inline]]
	inline typename TwiT::Error write_bytes(
		uint16_t address,
		const void *data,
		uint16_t count
	) {
		return write_bytes(address, static_cast<const uint8_t*>(data), count);
	}

	bool read_bytes(
		uint16_t address,
		uint8_t *result,
		uint16_t count,
		uint16_t timeoutMicros = 0xFFFF
	) {
		if(_write_bytes_tiny(address, nullptr, 0) != TwiT::Error::SUCCESS) {
			return false;
		}
		const uint8_t batch = TwiT::max_read_bytes();
		while(count > batch) {
			if(!twiComm.request_from(i2c_addr(), result, batch, timeoutMicros)) {
				return false;
			}
			result += batch;
			count -= batch;
		}
		return twiComm.request_from(i2c_addr(), result, count, timeoutMicros);
	}

	[[gnu::always_inline]]
	inline bool read_bytes(
		uint16_t address,
		void *result,
		uint16_t count,
		uint16_t timeoutMicros = 0xFFFF
	) {
		return read_bytes(
			address,
			static_cast<uint8_t*>(result),
			count,
			timeoutMicros
		);
	}

	[[gnu::always_inline]]
	inline bool read_page(
		uint16_t page,
		uint8_t result[page_size_bytes()]
	) {
		const uint8_t ps = page_size_bytes();
		return read_bytes(page * ps, result, ps);
	}

	C24LC256_impl(TwiT twi, uint8_t subAddress)
		: twiComm(twi)
		, subAddress(subAddress & 0x07)
	{
		twiComm.set_max_clock(i2c_speed());
	}

	C24LC256_impl(C24LC256_impl &&b)
		: twiComm(static_cast<TwiT&&>(b.twiComm))
		, subAddress(b.subAddress)
	{}
};

template <typename TwiT>
[[gnu::always_inline,nodiscard]]
inline C24LC256_impl<TwiT> Make24LC256(TwiT twi, uint8_t subAddress = 0) {
	return C24LC256_impl<TwiT>(twi, subAddress);
}

#endif

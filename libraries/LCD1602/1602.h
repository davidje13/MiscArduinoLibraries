/*
 * 1602 - LCD communication library
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

#ifndef LCD1602_H_INCLUDED
#define LCD1602_H_INCLUDED

#include "ext.h"
#include <bootup.h>

class LCD1602 {
public:
	enum class EntryMode : uint8_t {
		CURSOR_LEFT  = 0x00,
		SHIFT_RIGHT  = 0x01,
		CURSOR_RIGHT = 0x02,
		SHIFT_LEFT   = 0x03
	};

	enum class Movement : uint8_t {
		CURSOR_LEFT  = 0x00,
		CURSOR_RIGHT = 0x04,
		SHIFT_LEFT   = 0x08,
		SHIFT_RIGHT  = 0x0C
	};

	enum class CursorDisplay : uint8_t {
		OFF   = 0x00,
		ON    = 0x02,
		BLINK = 0x03
	};

	enum class Font : uint8_t {
		REGULAR = 0x00,
		LARGE   = 0x04
	};

protected:
	enum class DataState : uint8_t {
		CMD_WRITE  = 0x00,
		BUSY_READ  = 0x01,
		DATA_WRITE = 0x02,
		DATA_READ  = 0x03
	};

	LCD1602(void) = default;
	LCD1602(const LCD1602&) = delete;
	LCD1602(LCD1602&&) = default;

	LCD1602 &operator=(const LCD1602&) = delete;
	LCD1602 &operator=(LCD1602&&) = delete;
};

#define DATA_STATE 0x03
#define CURSOR_MODE 0x0C
#define POWER 0x10
#define FONT 0x20

template <
	uint8_t columns,
	uint8_t lines,
	typename BankT,
	typename RSPinT,
	typename RWPinT,
	typename ENPinT
>
class LCD1602_impl : public LCD1602 {
	ext::Flattener<BankT,uint8_t> bank;
#define state bank.flattened_value
	RSPinT rs;
	RWPinT rw;
	ENPinT en;

	void set_mode(DataState ds) {
		if((state & DATA_STATE) == uint8_t(ds)) {
			return;
		}
		uint8_t read = uint8_t(ds) & 1;
		rs.begin_batch();
		rs.set(uint8_t(ds) >> 1);
		if(read != (state & 1)) {
			rw.set(read);
			if(read) {
				bank.set_input();
			} else {
				bank.set_output();
			}
		}
		rs.send_batch();
		state = (state & ~DATA_STATE) | uint8_t(ds);
		// mode must be stable for 30ns before en goes high
//		delayMicroseconds(1);
	}

	void send_part(uint8_t data) {
		bank.begin_batch(); // data and en-high can be sent simultaneously
		en.high();
		bank.set(data);
		bank.send_batch();
		delayMicroseconds(1); // hold for >=1.2us
		en.low();
	}

	uint8_t read_part(void) {
		en.high();
		delayMicroseconds(1); // wait 140ns for data to appear
		uint8_t value = bank.get();
		en.low();
		return value;
	}

	[[gnu::always_inline]]
	inline void send(uint8_t data) {
		if(bank.size() == 8) {
			send_part(data);
		} else {
			send_part(data >> 4);
			send_part(data & 0x0F);
		}
		delayMicroseconds(37); // wait for command to execute
	}

	[[gnu::always_inline]]
	inline uint8_t perform_read(void) {
		uint8_t value = read_part();
		if(bank.size() == 8) {
			return value;
		}
		delayMicroseconds(1);
		return (value << 4) | read_part();
	}

	void send_display(void) {
		set_mode(DataState::CMD_WRITE);
		send(0x08 | ((state & (POWER | CURSOR_MODE)) >> 2));
	}

	void send_config(void) {
		set_mode(DataState::CMD_WRITE);
		send(
			0x20 |
			((bank.size() == 8) ? 0x10 : 0x00) |
			((lines > 1) ? 0x08 : 0x00) |
			((state & FONT) >> 3)
		);
	}

	bool is_busy(void) {
		set_mode(DataState::BUSY_READ);
		return perform_read() & 0x80;
	}

	void wait_busy(uint16_t maxMicros) {
		if(rw.exists()) {
			uint16_t tm0 = micros();
			while(is_busy()) {
				if(uint16_t(micros() - tm0) > maxMicros) {
					break;
				}
			}
		} else {
			delayMicroseconds(maxMicros);
		}
	}

public:
	LCD1602_impl(BankT bank, RSPinT rs, RWPinT rw, ENPinT en)
		: bank(bank, uint8_t(DataState::CMD_WRITE))
		, rs(rs)
		, rw(rw)
		, en(en)
	{
		bank.begin_batch();
		bank.set_output();
		rs.set_output();
		rw.set_output();
		en.set_output();

		bank.set(0);
		rs.low();
		rw.low();
		en.low();
		bank.send_batch();

		awaitBootMillis(40); // Wait 40ms for power on
		handshake();
	}

	LCD1602_impl(LCD1602_impl &&b)
		: bank(static_cast<ext::Flattener<BankT,uint8_t>&&>(b.bank))
		, rs(static_cast<RSPinT&&>(b.rs))
		, rw(static_cast<RWPinT&&>(b.rw))
		, en(static_cast<ENPinT&&>(b.en))
	{}

	void handshake(void) {
		// We can reliably set 8-bit mode whether we have 4- or 8- bits
		// connected, regardless of the old device mode. However, we cannot
		// reliably set 4-bit mode from all starting states. Therefore, we
		// begin by setting 8-bit mode, then move back to 4-bit mode if
		// required:

		// We send the instruction 3 times, because we might have been
		// in 4-bit mode before, or worse: we might have been in 4-bit
		// mode and had a spurious half-instruction. The first send will
		// clear the half-instruction, the second will fill the first
		// half of the new instruction, and the third will send the
		// instruction. Once we are in 8-bit mode, subsequent sends will
		// be no-ops.

		set_mode(DataState::CMD_WRITE);
		send_part(0x33);

		// We might have just completed a slow instruction, so wait for it
		// (we cannot poll the busy flag here, just have to wait)
		delayMicroseconds(1500);

		// Send instruction 2nd and 3rd times
		for(int i = 0; i < 2; ++ i) {
			en.high();
			delayMicroseconds(1); // hold for >=1.2us
			en.low();
			delayMicroseconds(37); // wait for command to execute
		}

		// Now we know we are in 8-bit mode

		if(bank.size() != 8) {
			// set 4-bit mode
			send_part(0x2);
			delayMicroseconds(37);
		}

		// Finally we can send the actual configuration
		send_config();

		// Ensure state is consistent
		send_display();
		set_entry_mode(EntryMode::CURSOR_RIGHT);
		clear();
	}

	void clear(void) {
		set_mode(DataState::CMD_WRITE);
		send(0x01);
		wait_busy(1500); // 1.52ms processing time
	}

	void home(void) {
		set_mode(DataState::CMD_WRITE);
		send(0x02);
		wait_busy(1500); // 1.52ms processing time
	}

	void set_entry_mode(EntryMode mode) {
		set_mode(DataState::CMD_WRITE);
		send(0x04 | uint8_t(mode));
	}

	void set_font(Font font) {
		if((state & FONT) == uint8_t(font) << 3) {
			return;
		}
		state = (state & ~FONT) | (uint8_t(font) << 3);
		send_config();
	}

	void set_cursor_display(CursorDisplay mode) {
		if((state & CURSOR_MODE) == uint8_t(mode) << 2) {
			return;
		}
		state = (state & ~CURSOR_MODE) | (uint8_t(mode) << 2);
		send_display();
	}

	void move(Movement movement) {
		set_mode(DataState::CMD_WRITE);
		send(0x10 | uint8_t(movement));
	}

	void set_cursor(uint8_t column, uint8_t line) {
		set_mode(DataState::CMD_WRITE);
		if(line > 2) {
			send(0x80 | (((line - 2) << 6) | (column + columns)));
		} else {
			send(0x80 | ((line << 6) | column));
		}
	}

	void print(uint8_t value) {
		set_mode(DataState::DATA_WRITE);
		send(value);
	}

	template <typename T> // T = ProgMem<uint8_t> / const uint8_t*
	void print(T message) {
		set_mode(DataState::DATA_WRITE);
		for(T v = message; ; v += 1) {
			uint8_t c = v[0];
			if(!c) {
				break;
			}
			send(c);
		}
	}

	void print(uint16_t v) {
		char s[6];
		uint8_t p = sizeof(s) - 1;
		s[p] = 0;

		do {
			-- p;
			s[p] = (v % 10) + '0';
			v /= 10;
		} while(v);

		print(&s[p]);
	}

	uint8_t set_char_raw(uint8_t index, const uint8_t data[8]) {
		set_mode(DataState::CMD_WRITE);
		uint8_t ident = (index & 7);
		send(0x40 | (ident << 3));
		set_mode(DataState::DATA_WRITE);
		for(int i = 0; i < 8; ++ i) {
			send(data[i]);
		}
		set_mode(DataState::CMD_WRITE);
		send(0x80); // Return to text memory (0,0)
		return ident;
	}

	uint8_t set_char(uint8_t index, const uint8_t colData[5]) {
		set_mode(DataState::CMD_WRITE);
		uint8_t ident = (index & 7);
		send(0x40 | (ident << 3));
		uint8_t mut[5];
		set_mode(DataState::DATA_WRITE);
		for(int i = 0; i < 5; ++ i) {
			mut[i] = colData[i];
		}
		for(int i = 0; i < 8; ++ i) {
			uint8_t v = 0;
			for(int x = 0; x < 5; ++ x) {
				v = (v << 1) | (mut[x] & 1);
				mut[x] >>= 1;
			}
			send(v);
		}
		set_mode(DataState::CMD_WRITE);
		send(0x80); // Return to text memory (0,0)
		return ident;
	}

	[[gnu::always_inline]]
	inline uint8_t set_char(
		uint8_t index,
		uint8_t col1,
		uint8_t col2,
		uint8_t col3,
		uint8_t col4,
		uint8_t col5
	) {
		const uint8_t d[] = {col1, col2, col3, col4, col5};
		return set_char(index, d);
	}

	[[gnu::always_inline]]
	inline uint8_t set_char(
		uint8_t index,
		uint8_t r1,
		uint8_t r2,
		uint8_t r3,
		uint8_t r4,
		uint8_t r5,
		uint8_t r6,
		uint8_t r7,
		uint8_t r8
	) {
		const uint8_t d[] = {r1, r2, r3, r4, r5, r6, r7, r8};
		return set_char_raw(index, d);
	}

	uint8_t read(void) {
		if(!rw.exists()) {
			return 0;
		}

		set_mode(DataState::DATA_READ);
		return perform_read();
	}

	void on(void) {
		if(state & POWER) {
			return;
		}
		state |= POWER;
		send_display();
	}

	void off(void) {
		if(!(state & POWER)) {
			return;
		}
		state &= ~POWER;
		send_display();
	}

#undef state
};

#undef DATA_STATE
#undef CURSOR_MODE
#undef POWER
#undef BIG_FONT

template <
	uint8_t columns,
	uint8_t lines,
	typename BankT,
	typename RSPinT,
	typename RWPinT,
	typename ENPinT
>
[[gnu::always_inline,nodiscard]]
inline LCD1602_impl<columns,lines,BankT,RSPinT,RWPinT,ENPinT> Make1602(
	BankT bank,
	RSPinT rs,
	RWPinT rw,
	ENPinT en
) {
	return LCD1602_impl<columns,lines,BankT,RSPinT,RWPinT,ENPinT>(
		bank,
		rs,
		rw,
		en
	);
}

#endif

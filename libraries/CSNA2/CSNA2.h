/*
 * CSNA2 - Thermal printer communication library.
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

#ifndef CSNA2_H_INCLUDED
#define CSNA2_H_INCLUDED

#include "ext.h"
#include <ProgMem.h>

class CSNA2 {
public:
	enum Status : uint8_t {
		ONLINE       = 0x01,
		PAPER_OUT    = 0x04,
		OVER_VOLTAGE = 0x08,
		OVER_HEAT    = 0x40
	};

	enum class Configuration : uint32_t {
		FACTORY   = 0x075002, // 64 dots, 800us heating,  20us interval  5ms/ln
		PRECISION = 0x076414, // 64 dots, 1000us heating,  200us interval 7ms/ln
		ECO       = 0x016404, // 16 dots, 1000us heating, 40us interval  25ms/ln
		FAST      = 0x0B4604  // 96 dots, 700us heating,  40us interval  3ms/ln
	};

	enum class Justification : uint8_t {
		LEFT   = 0,
		CENTRE = 1,
		RIGHT  = 2
	};

	enum class Rotation : uint8_t {
		NORMAL   = 0,
		CW_90    = 1,
		REVERSED = 2,
		CCW_90   = 3
	};

	enum class Underline : uint8_t {
		NONE  = 0,
		THIN  = 1,
		THICK = 2
	};

	enum class CutMode : uint8_t {
		FULL,
		PARTIAL_1,
		PARTIAL_2
	};

	enum class Pin : uint8_t {
		DRAW_KICKOUT_2 = 0,
		DRAW_KICKOUT_5 = 1
	};

	static constexpr inline uint8_t dots_per_mm(void) {
		return 8;
	}

protected:
	CSNA2(void) = default;
	CSNA2(const CSNA2&) = delete;
	CSNA2(CSNA2&&) = default;

	CSNA2 &operator=(const CSNA2&) = delete;
	CSNA2 &operator=(CSNA2&&) = delete;
};

#define ESC uint8_t(0x1B)
#define FS uint8_t(0x1C)
#define GS uint8_t(0x1D)
#define DC2 uint8_t(0x12)
#define AWAKE uint8_t(0xFF)

#define SMALLEST_FONT_W 12
#define MAX_TABSTOPS 32

template <typename SerialT, uint16_t widthDots>
class CSNA2_impl : public CSNA2 {
	SerialT serial;

	[[gnu::always_inline]]
	inline void write_16le(uint16_t v) {
		serial.write(uint8_t(v & 0xFF));
		serial.write(uint8_t(v >> 8));
	}

	void set_sleep(uint16_t seconds) {
		serial.write(ESC);
		serial.write('8');
		write_16le(seconds);
	}

	void set_device(uint8_t device) {
		serial.write(ESC);
		serial.write('=');
		serial.write(device);
	}

	bool read(uint8_t *target, uint16_t timeoutMillis) {
		uint16_t tm0 = millis();
		while(!serial.available()) {
			if(uint16_t(millis() - tm0) > timeoutMillis) {
				return false;
			}
		}

		// Ignore all except latest status
		do {
			*target = serial.read();
		} while(serial.available());

		return true;
	}

	bool read_status(uint8_t *target, uint16_t timeoutMillis) {
		if(!serial.can_listen()) {
			return false;
		}
		serial.listen();
		serial.write(ESC);
		serial.write('v');
		serial.write(uint8_t(0));
		return read(target, timeoutMillis);
	}

public:
	static constexpr inline uint16_t width_dots(void) {
		return widthDots;
	}

	static constexpr inline uint16_t width_mm(void) {
		return width_dots() / dots_per_mm();
	}

	[[nodiscard]]
	bool connected(void) {
		if(!serial.can_listen()) {
			// Can't tell, so assume connected
			return true;
		}
		awake();
		uint8_t status;
		return read_status(&status, 100);
	}

	void reset(void) {
		serial.write(ESC);
		serial.write('@');
	}

	[[gnu::always_inline]]
	inline void off(void) {
		set_device(0);
	}

	[[gnu::always_inline]]
	inline void on(void) {
		set_device(1);
	}

	inline void configure(
		uint16_t heatingDots,
		uint16_t heatingTimeMicros,
		uint16_t heatingIntervalMicros
	) {
		serial.write(ESC);
		serial.write('7');
		serial.write(uint8_t((heatingDots > 3) - 1));      // default 64 dots
		serial.write(uint8_t(heatingTimeMicros / 10));     // default 800us
		serial.write(uint8_t(heatingIntervalMicros / 10)); // default 20us
	}

	inline void configure(Configuration mode) {
		uint32_t bits = uint32_t(mode);
		serial.write(ESC);
		serial.write('7');
		serial.write(uint8_t(bits >> 16));
		serial.write(uint8_t((bits >> 8) & 0xFF));
		serial.write(uint8_t(bits));
	}

	void set_density(uint8_t density, uint8_t delayQuarterMillis) {
		serial.write(DC2);
		serial.write('#');
		serial.write(
			min((max(density, 50) - 50) / 5, 0x1F) |
			(min(delayQuarterMillis, 7) << 5)
		);
	}

	inline void set_buttons_enabled(bool on) {
		serial.write(ESC);
		serial.write('c');
		serial.write('5');
		serial.write(uint8_t(on ? 1 : 0));
	}

	[[nodiscard]]
	inline Status get_status(void) {
		uint8_t status;
		if(!read_status(&status, 200)) {
			return 0;
		}
		return status;
	}

	[[nodiscard]]
	bool is_drawer_open(void) {
		if(!serial.can_listen()) {
			return false; // assume closed
		}
		serial.listen();
		serial.write(ESC);
		serial.write('u');
		serial.write(uint8_t(0));
		uint8_t status;
		if(!read(&status, 200)) {
			return false;
		}
		return status & 0x01;
	}

	[[nodiscard]]
	bool is_near_paper_end(void) {
		if(!serial.can_listen()) {
			return false; // assume not near end
		}
		serial.listen();
		serial.write(GS);
		serial.write('r');
		serial.write(uint8_t(1));
		uint8_t status;
		if(!read(&status, 200)) {
			return false; // assume not near end
		}
		return (status & 0x0C) == 0x0C;
	}

	void reset_linespacing(void) {
		// same as set_linespacing(30)
		serial.write(ESC);
		serial.write('2');
	}

	void set_linespacing(uint8_t dots) {
		serial.write(ESC);
		serial.write('3');
		serial.write(dots);
	}

	void clear_command_timeout(void) {
		serial.write(FS);
		serial.write('t');
		serial.write(uint8_t(0));
	}

	void set_command_timeout(uint16_t millisPerByte) {
		serial.write(FS);
		serial.write('t');
		serial.write(uint8_t(max(millisPerByte / 10, 1)));
	}

	void set_sleep_delay(uint16_t seconds) {
		set_sleep((seconds == 0) ? 1 : seconds);
	}

	[[gnu::always_inline]]
	inline void disable_sleep(void) {
		set_sleep(0);
	}

	void awake(void) {
		serial.write(AWAKE);
		// Data sheet promises 50ms is enough, but that seems to clip the first
		// few bytes of the following message, so go for a safer 100ms instead
		delay(100);
	}

	inline void print(char c) {
		if(c == '\r') {
			// CR is not supported over serial interface, and pointless too
			return;
		}
		serial.write(uint8_t(c));
	}

	template <typename T> // T = ProgMem<uint8_t> / const uint8*
	void print(T message) {
		for(T v = message; ; v += 1) {
			char c = v[0];
			if(!c) {
				break;
			}
			print(c);
		}
	}

	[[gnu::always_inline]]
	inline void linefeed(void) {
		print('\n');
	}

	inline void skip_dots(uint8_t dots) {
		// Print current line then skip a number of dots vertically
		serial.write(ESC);
		serial.write('J');
		serial.write(dots);
	}

	inline void skip_lines(uint8_t lines) {
		// Print current line then skip a number of lines vertically
		serial.write(ESC);
		serial.write('d');
		serial.write(lines);
	}

	inline void set_justification(Justification justification) {
		serial.write(ESC);
		serial.write('a');
		serial.write(uint8_t(justification));
	}

	inline void set_margin_left(uint16_t dots) {
		// Sets a global left margin for all following lines
		// (must be specified at the start of a line)
		serial.write(GS);
		serial.write('L');
		write_16le(dots);
	}

	inline void set_x(uint16_t dots) {
		// Move the cursor to a specific position
		// (e.g. can be used to draw tabulated data
		// without setting tab positions)
		serial.write(ESC);
		serial.write('$');
		write_16le(dots);
	}

	inline void set_blank_char_numbers(uint8_t chars) {
		// TODO: what does this do?
		// "Set left blank char numbers"
		// 0 <= chars <= 47
		serial.write(ESC);
		serial.write('B');
		serial.write(chars);
	}

	inline void set_character_size(uint8_t width, uint8_t height) {
		// width & height can be 1, 2, 3, 4, 5, 6, 7, 8
		serial.write(GS);
		serial.write('!');
		serial.write(((width - 1) << 4) | (height - 1));
	}

	inline void set_inverted(bool on) {
		serial.write(GS);
		serial.write('B');
		serial.write(uint8_t(on ? 1 : 0));
	}

	inline void set_emphasised(bool on) {
		serial.write(ESC);
		serial.write('E');
		serial.write(uint8_t(on ? 1 : 0));
	}

	inline void set_doublestrike(bool on) {
		// Same as emphasised mode
		// TODO: can have emphasised and doublestrike for even bolder?
		serial.write(ESC);
		serial.write('G');
		serial.write(uint8_t(on ? 1 : 0));
	}

	inline void set_underline(Underline thickness) {
		// No effect on rotated or inverted text, or tab spacing
		serial.write(ESC);
		serial.write('-');
		serial.write(uint8_t(thickness));
	}

	inline void set_underline(bool on) {
		set_underline(on ? Underline::THIN : Underline::NONE);
	}

	inline void set_width_1(void) {
		// TODO: same as set_character_size?
		serial.write(ESC);
		serial.write(uint8_t(0x14));
		serial.write(uint8_t(1));
	}

	inline void set_width_2(void) {
		// TODO: same as set_character_size?
		serial.write(ESC);
		serial.write(uint8_t(0x0E));
		serial.write(uint8_t(1));
	}

	inline void set_character_spacing(uint8_t dots) {
		// Changes spacing to right of characters
		// Spacing = dots * character width set by set_character_size
		// Default = 0
		serial.write(ESC);
		serial.write(' ');
		serial.write(dots);
	}

	inline void set_rotation(Rotation rotation) {
		serial.write(ESC);
		serial.write('V');
		serial.write(uint8_t(rotation) & 1);
		serial.write(ESC);
		serial.write('{');
		serial.write(uint8_t(rotation) >> 1);
	}

	[[gnu::always_inline]]
	inline void tab(void) {
		print('\t');
	}

	inline void print_buffer_and_advance(void) {
		// TODO: what does this do?
		// "Print the data in buffer and locate to the next black mark"
		// "Only board with black mark function support this command"
		print('\f');
	}

	inline void cancel_tab_positions(void) {
		serial.write(ESC);
		serial.write('D');
		serial.write(uint8_t(0));
	}

	void set_tab_size(uint8_t size) {
		if(size == 0) {
			cancel_tab_positions();
			return;
		}
		serial.write(ESC);
		serial.write('D');
		for(uint8_t i = 1; i < MAX_TABSTOPS; ++ i) {
			const uint16_t pos = uint16_t(i) * size;
			if(pos > 0xFF || pos > widthDots / SMALLEST_FONT_W) {
				serial.write(uint8_t(0));
				return;
			}
			serial.write(uint8_t(pos));
		}
	}

	void set_tab_positions(const uint8_t *positions) {
		uint8_t last = 0;
		serial.write(ESC);
		serial.write('D');
		for(uint8_t i = 0; i < MAX_TABSTOPS; ++ i) {
			const uint8_t pos = positions[i];
			if(pos <= last || pos > widthDots / SMALLEST_FONT_W) {
				serial.write(uint8_t(0));
				return;
			}
			serial.write(pos);
			last = pos;
		}
	}

	void set_vertical_markers(uint16_t dots, bool plus3 = false) {
		serial.write(DC2);
		serial.write('m');
		serial.write(uint8_t(plus3 ? 1 : 0));
		write_16le(dots);
	}

	void skip_to_vertical_marker(void) {
		serial.write(DC2);
		serial.write('E');
	}

	void cut(CutMode cut = CutMode::FULL) {
		switch(cut) {
		case CutMode::FULL:
			serial.write(ESC);
			serial.write('i');
			break;
		case CutMode::PARTIAL_1:
			serial.write(ESC);
			serial.write('m');
			break;
		case CutMode::PARTIAL_2:
			serial.write(GS);
			serial.write('V');
			serial.write(uint8_t(1));
			break;
		}
	}

	void cut_partial(uint8_t feedDots) {
		serial.write(GS);
		serial.write('V');
		serial.write(uint8_t(66));
		serial.write(feedDots);
	}

	void pulse_pin(Pin pin, uint16_t onTimeMillis, uint16_t offTimeMillis) {
		serial.write(ESC);
		serial.write('p');
		serial.write(uint8_t(pin));
		serial.write(uint8_t(onTimeMillis / 2));
		serial.write(uint8_t(offTimeMillis / 2));
	}

	void print_test_page(void) {
		serial.write(DC2);
		serial.write('T');
	}

	CSNA2_impl(SerialT &&serial, uint32_t baud)
		: serial(static_cast<SerialT&&>(serial))
	{
		serial.begin(baud);
		ext::awaitBootMillis(500); // Wait 0.5s to power on
		awake();
		reset();
		set_sleep(0);
	}

	CSNA2_impl(CSNA2_impl&&) = default;

#undef addr
#undef bits
};

#undef ESC
#undef FS
#undef GS
#undef DC2
#undef AWAKE
#undef SMALLEST_FONT_W
#undef MAX_TABSTOPS

template <typename SerialT, uint16_t widthDots = 384>
[[gnu::always_inline,nodiscard]]
inline CSNA2_impl<SerialT, widthDots> MakeCSNA2(
	SerialT &&serial,
	uint32_t baud
) {
	return CSNA2_impl<SerialT, widthDots>(static_cast<SerialT&&>(serial), baud);
}

#endif

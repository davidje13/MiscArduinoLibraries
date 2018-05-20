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
		FACTORY   = 0x075002, // 64 dots, 800us heating, 20us interval    5ms/ln
		PRECISION = 0x016EC8, // 16 dots, 1.1ms heating, 2.00ms interval 74ms/ln
		ECO       = 0x0164FF, // 16 dots, 1.0ms heating, 2.56ms interval 85ms/ln
		FAST      = 0x0B4604  // 96 dots, 700us heating, 40us interval    3ms/ln
	};

	enum class Font : uint8_t {
		A_12_24,
		B_9_17
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

#define SMALLEST_FONT_W 9
#define MAX_TABSTOPS 32

#define MODE_FONTB      0x01
#define MODE_INVERTED   0x02
#define MODE_REVERSED   0x04
#define MODE_EMPHASISED 0x08
#define MODE_DBLHEIGHT  0x10
#define MODE_DBLWIDTH   0x20
#define MODE_DELETELINE 0x40
#define MODE_MASK (0x7F & ~(MODE_DBLHEIGHT | MODE_DBLWIDTH))

template <typename SerialT, uint16_t widthDots>
class CSNA2_impl : public CSNA2 {
	SerialT serial;
	uint8_t mode;
	uint8_t size;

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

	void set_rot90(bool on) {
		serial.write(ESC);
		serial.write('V');
		serial.write(on ? 1 : 0);
	}

	void set_rot180(bool on) {
		if(on) {
			mode |= MODE_REVERSED;
		} else {
			mode &= ~MODE_REVERSED;
		}
		serial.write(ESC);
		serial.write('{');
		serial.write(on ? 1 : 0);
	}

	void send_size(void) {
		serial.write(GS);
		serial.write('!');
		serial.write(size);
	}

	void send_mode(void) {
		serial.write(ESC);
		serial.write('!');
		uint8_t m = mode & MODE_MASK;
		uint8_t w = size >> 4;
		uint8_t h = size & 0x0F;
		if(w == 1) {
			m |= MODE_DBLWIDTH;
		}
		if(h == 1) {
			m |= MODE_DBLHEIGHT;
		}
		serial.write(m);

		// Ensure size is not clobbered
		if(w > 1 || h > 1) {
			send_size();
		}
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
		awake();
		serial.write(ESC);
		serial.write('@');
		mode = 0x00;
		size = 0x00;
	}

	inline void soft_reset(void) {
		mode = 0x00;
		size = 0x00;

		awake();
		send_mode();
		reset_linespacing();
		set_character_spacing(0);
		set_rot90(false);
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

	template <typename T> // T = ProgMem<uint8_t> / const uint8_t*
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
		// Note that width takes effect per letter, but height is per line
		width = max(1, min(8, width));
		height = max(1, min(8, height));
		size = ((width - 1) << 4) | (height - 1);
		send_size();
	}

	inline void set_character_width(uint8_t width) {
		width = max(1, min(8, width));
		size = (size & 0x0F) | ((width - 1) << 4);
		if(width == 1) {
			serial.write(ESC);
			serial.write(uint8_t(0x14));
			serial.write(uint8_t(1));
		} else if(width == 2) {
			serial.write(ESC);
			serial.write(uint8_t(0x0E));
			serial.write(uint8_t(1));
		} else {
			send_size();
		}
	}

	inline void set_character_height(uint8_t height) {
		height = max(1, min(8, height));
		size = (size & 0xF0) | (height - 1);
		send_size();
	}

	inline void set_font(Font font) {
		switch(font) {
		case Font::A_12_24:
			mode &= ~MODE_FONTB;
			break;
		case Font::B_9_17:
			mode |= MODE_FONTB;
			break;
		}
		send_mode();
	}

	inline void set_inverted(bool on) {
		if(on) {
			mode |= MODE_INVERTED;
		} else {
			mode &= ~MODE_INVERTED;
		}
		serial.write(GS);
		serial.write('B');
		serial.write(uint8_t(on ? 1 : 0));
	}

	inline void set_strikeout(bool on) {
		if(on) {
			mode |= MODE_DELETELINE;
		} else {
			mode &= MODE_DELETELINE;
		}
		send_mode();
	}

	inline void set_bold(bool on) {
		if(on) {
			mode |= MODE_EMPHASISED;
		} else {
			mode &= ~MODE_EMPHASISED;
		}
		serial.write(ESC);
		serial.write('E');
		serial.write(uint8_t(on ? 1 : 0));
	}

	inline void set_doublestrike(bool on) {
		// Same as bold
		// TODO: can have bold and doublestrike for even bolder?
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

	inline void set_character_spacing(uint8_t dots) {
		// Changes spacing to right of characters
		// Spacing = dots * character width set by set_character_size
		// Default = 0
		serial.write(ESC);
		serial.write(' ');
		serial.write(dots);
	}

	[[gnu::always_inline]]
	inline void set_rotation(Rotation rotation) {
		set_rot90(uint8_t(rotation) & 1);
		set_rot180(uint8_t(rotation) >> 1);
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

	template <typename Bitmask>
	void print_bitmask81msb(
		const Bitmask &bitmask,
		bool dblWidth = false,
		bool dblHeight = false
	) {
		uint16_t wb = (min(bitmask.width(), widthDots) + 7) / 8;
		uint16_t hb = min(bitmask.height(), 4095);

		set_command_timeout(10);
		serial.write(GS);
		serial.write('v');
		serial.write('0');
		serial.write((dblWidth ? 1 : 0) | (dblHeight ? 2 : 0));
		write_16le(wb);
		write_16le(hb);
		for(uint16_t y = 0; y < hb; ++ y) {
			for(uint16_t x = 0; x < wb; ++ x) {
				uint8_t byte = 0;
				for(uint8_t xx = 0; xx < 8; ++ xx) {
					byte = (byte << 1) | bitmask.get_pixel((x << 3) | xx, y);
				}
				serial.write(byte);
			}
			delayMicroseconds(wb * 200); // avoid overrunning buffer
		}
	}

	template <typename Bitmask>
	void print_bitmask81msb_small(const Bitmask &bitmask) {
		uint8_t wb = uint8_t((min(bitmask.width(), widthDots) + 7) / 8);
		uint8_t hb = uint8_t(min(bitmask.height(), 255));

		set_command_timeout(10);
		serial.write(DC2);
		serial.write('*');
		serial.write(hb);
		serial.write(wb);
		for(uint16_t y = 0; y < hb; ++ y) {
			for(uint8_t x = 0; x < wb; ++ x) {
				uint8_t byte = 0;
				for(uint8_t xx = 0; xx < 8; ++ xx) {
					byte = (byte << 1) | bitmask.get_pixel((x << 3) | xx, y);
				}
				serial.write(byte);
			}
			delay(1); // TODO: needed?
		}
	}

	template <typename Bitmask>
	void print_bitmask81msb_fullwidth(const Bitmask &bitmask) {
		uint16_t hb = bitmask.height();

		set_command_timeout(10);
		serial.write(DC2);
		serial.write('V');
		write_16le(hb);
		for(uint16_t y = 0; y < hb; ++ y) {
			for(uint16_t x = 0; x < widthDots / 8; ++ x) {
				uint8_t byte = 0;
				for(uint8_t xx = 0; xx < 8; ++ xx) {
					byte = (byte << 1) | bitmask.get_pixel((x << 3) | xx, y);
				}
				serial.write(byte);
			}
			delay(1); // TODO: needed?
		}
	}

	template <typename Bitmask>
	void print_bitmask81lsb_fullwidth(const Bitmask &bitmask) {
		uint16_t hb = bitmask.height();

		set_command_timeout(10);
		serial.write(DC2);
		serial.write('v');
		write_16le(hb);
		for(uint16_t y = 0; y < hb; ++ y) {
			for(uint16_t x = 0; x < widthDots / 8; ++ x) {
				uint8_t byte = 0;
				for(uint8_t xx = 8; (xx --) > 0;) {
					byte = (byte << 1) | bitmask.get_pixel((x << 3) | xx, y);
				}
				serial.write(byte);
			}
			delay(1); // TODO: needed?
		}
	}

	template <typename Bitmask>
	void print_bitmask18(
		const Bitmask &bitmask,
		bool dblWidth = false,
		bool dblHeight = false
	) {
		uint8_t wb = uint8_t((min(bitmask.width(), widthDots) + 7) / 8);
		uint16_t hb = (bitmask.height() + 7) / 8;
		uint8_t hp = uint8_t(min(hb, 255));

		// data sheet claims the following:
//		if(hp > 48) {
//			hp = 48;
//		}
//		if(wb * hp > 1536) {
//			hp = 1536 / wb;
//		}
		// but in reality, seems this is the behaviour:
		if(wb * hp > 48) {
			hp = 48 / wb;
		}

		uint16_t w = uint16_t(wb) * 8;

		set_command_timeout(10);

		for(uint16_t y0 = 0; y0 < hb; y0 += hp) {
			if(y0 + hp > hb) {
				hp = uint8_t(hb - y0);
			}

			// Upload image
			serial.write(GS);
			serial.write('*');
			serial.write(wb);
			serial.write(hp);
			for(uint16_t x = 0; x < w; ++ x) {
				for(uint16_t y = 0; y < hp; ++ y) {
					uint8_t byte = 0;
					for(uint8_t yy = 0; yy < 8; ++ yy) {
						byte = (byte << 1) | bitmask.get_pixel(
							x,
							((y + y0) << 3) | yy
						);
					}
					serial.write(byte);
				}
			}

			// Print image
			reprint_bitmask18(dblWidth, dblHeight);
			delay(10); // TODO: needed?
		}
	}

	inline void reprint_bitmask18(
		bool dblWidth = false,
		bool dblHeight = false
	) {
		serial.write(GS);
		serial.write('/');
		serial.write((dblWidth ? 1 : 0) | (dblHeight ? 2 : 0));
	}

	// For compatibility with renderScene (3D renderer)
	template <typename Bitmask>
	void send(const Bitmask &bitmask, uint8_t, uint8_t) {
		print_bitmask81msb(
			bitmask,
			(size & 0xF0) != 0,
			(size & 0x0F) != 0
		);
	}

	void print_test_page(void) {
		serial.write(DC2);
		serial.write('T');
	}

	CSNA2_impl(SerialT &&serial, uint32_t baud)
		: serial(static_cast<SerialT&&>(serial))
		, mode(0x00)
		, size(0x00)
	{
		serial.begin(baud);
		ext::awaitBootMillis(500); // Wait 0.5s to power on
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

#undef MODE_FONTB
#undef MODE_INVERTED
#undef MODE_REVERSED
#undef MODE_EMPHASISED
#undef MODE_DBLHEIGHT
#undef MODE_DBLWIDTH
#undef MODE_DELETELINE
#undef MODE_MASK

template <typename SerialT, uint16_t widthDots = 384>
[[gnu::always_inline,nodiscard]]
inline CSNA2_impl<SerialT, widthDots> MakeCSNA2(
	SerialT &&serial,
	uint32_t baud
) {
	return CSNA2_impl<SerialT, widthDots>(static_cast<SerialT&&>(serial), baud);
}

#endif

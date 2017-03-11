/*
 * SSD1306 - SSD1306 (Display) communication library.
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

#ifndef SSD1306_H_INCLUDED
#define SSD1306_H_INCLUDED

#include "SolidFill.h"

#include "ext.h"

class SSD1306 {
protected:
	SSD1306(void) = default;
	SSD1306(const SSD1306&) = delete;
	SSD1306(SSD1306&&) = default;

	enum Command : uint8_t {
		LOWER_COLUMN =             0x00, // Memory mode: page. values [00-0F]
		HIGHER_COLUMN =            0x10, // Memory mode: page. values [10-1F]
		SET_MEMORY_ADDR_MODE =     0x20, // Horiz/Vert/Page
		SET_COLUMN_ADDR =          0x21, // Memory mode: horiz/vert
		SET_PAGE_ADDR =            0x22, // Memory mode: horiz/vert
		// ...
		CONFIG_SCROLL_RIGHT =      0x26,
		CONFIG_SCROLL_LEFT =       0x27,
		// ...
		CONFIG_SCROLL_VRIGHT =     0x29,
		CONFIG_SCROLL_VLEFT =      0x2A,
		// ...
		SCROLL_OFF =               0x2E,
		SCROLL_ON =                0x2F,
		// ...
		DISPLAY_START_LINE =       0x40, // values [40-7F]
		// ...
		SET_CONTRAST_CONTROL =     0x81,
		// ...
		X_CHARGE_PUMP_SETTING =    0x8D,
		// ...
		SEGMENT_REMAP_LTR =        0xA0,
		SEGMENT_REMAP_RTL =        0xA1,
		// ...
		CONFIG_SCROLL_VERT_AREA =  0xA3,
		DISPLAY_SHOWS_RAM =        0xA4,
		DISPLAY_SHOWS_ON =         0xA5,
		NORMAL_DISPLAY =           0xA6, // 0 = off, 1 = on
		INVERT_DISPLAY =           0xA7, // 0 = on, 1 = off
		SET_MULTIPLEX_RATIO =      0xA8,
		// ...
		DISPLAY_OFF =              0xAE,
		DISPLAY_ON =               0xAF,
		PAGE_START =               0xB0, // Memory mode: page. values [B0-B7]
		// ...
		COM_SCAN_DOWN =            0xC0,
		// ...
		COM_SCAN_UP =              0xC8,
		// ...
		SET_DISPLAY_OFFSET =       0xD3,
		// ...
		SET_DISP_CLOCK_DIVIDE =    0xD5,
		// ...
		SET_PRECHARGE_PERIOD =     0xD9,
		SET_COM_PINS_CONFIG =      0xDA,
		SET_VCOMH_DESELECT_LEVEL = 0xDB,
		// ...
		NOP =                      0xE3
	};

	enum DisplayMode : uint8_t {
		SEQUENTIAL           = 0x0,
		ALTERNATING          = 0x1, // Default
		SEQUENTIAL_LR_REMAP  = 0x2,
		ALTERNATING_LR_REMAP = 0x3
	};

	enum Voltage : uint8_t {
		V065VCC = 0x0,
		V077VCC = 0x2, // Default
		V083VCC = 0x3
		// several reference libraries use 4, but it is not in the data sheet
		// (although the sheet does allow 3 bit values, so maybe this is a
		// non-public value?)
//		V_UNKNOWN = 0x4
	};

	template <typename T>
	[[gnu::pure,gnu::always_inline,gnu::nonnull]]
	static inline uint8_t read_shifted(T r0, T r1, uint8_t x, uint8_t shift) {
		return ((r0[x] >> (8 - shift)) | (r1[x] << shift));
	}
};

template <
	uint8_t DISP_WIDTH,
	uint8_t DISP_HEIGHT,
	typename SpiT,
	typename CSPinT,
	typename RSTPinT,
	typename DCPinT,
	uint8_t DISP_HEIGHT_BYTES = (DISP_HEIGHT + 7) / 8
>
class SSD1306_impl : public SSD1306 {
	ext::Flattener<SpiT,uint8_t> spiComm;
#define spiNesting spiComm.flattened_value
	ext::Flattener<CSPinT,uint8_t> csPin;
#define curColStart csPin.flattened_value
	ext::Flattener<RSTPinT,uint8_t> rstPin;
#define curColEnd rstPin.flattened_value
	ext::Flattener<DCPinT,uint8_t> dcPin;
#define framerate dcPin.flattened_value
	uint8_t curPageStart : 4;
	uint8_t curPageEnd   : 4;
	bool display_on : 4;
	bool scrolling  : 4;

public:
	void begin_communication(void) {
		if(spiNesting == 0) {
			// Data to device on RISING edge => mode 0 or 3
			// Doesn't appear to matter whether mode 0 or 3 is used
			// (even mode 2 seems to work)
			spiComm.begin_transaction(
				10000000, // SSD1306 requires tCycle >= 100ns (<= 10MHz)
				SpiT::ByteOrder::MSB_FIRST,
				SpiT::DataMode::MODE0
			);
			csPin.low();
		}
		++ spiNesting;
	}

	[[gnu::always_inline]]
	inline void end_communication(void) {
		if((-- spiNesting) == 0) {
			csPin.high();
			spiComm.end_transaction();
		}
	}

private:
	[[gnu::always_inline]]
	inline void transfer_wrapped(uint8_t data) {
		begin_communication();
		spiComm.transfer(data);
		end_communication();
	}

	void set_voltage(Voltage v) {
		begin_communication();
		spiComm.transfer(SET_VCOMH_DESELECT_LEVEL);
		spiComm.transfer(v << 4);
		end_communication();
	}

	void set_precharge_periods(uint8_t phase1, uint8_t phase2) {
		// 1 <= phase1 <= 15 (default = 2)
		// 1 <= phase2 <= 15 (default = 2)
		begin_communication();
		spiComm.transfer(SET_PRECHARGE_PERIOD);
		spiComm.transfer((phase2 << 4) | phase1);
		end_communication();
	}

	void set_charge_pump(bool enabled) {
		begin_communication();
		if(display_on) {
			spiComm.transfer(DISPLAY_OFF);
		}

		spiComm.transfer(X_CHARGE_PUMP_SETTING);
		spiComm.transfer(enabled ? 0x14 : 0x10);

		// Must switch display on to take effect
		if(display_on) {
			spiComm.transfer(DISPLAY_ON);
		}
		end_communication();
	}

	void set_com_pins_config(DisplayMode d) {
		begin_communication();
		spiComm.transfer(SET_COM_PINS_CONFIG);
		spiComm.transfer((d << 4) | 0x02);
		end_communication();
	}

	void set_display_frequency(uint8_t freq, uint8_t divide) {
		// 0 <= freq <= 15 (higher = higher frequency)
		// 0 => ~250-300kHz, 15 => ~500-600kHz, non-linear interpolation
		// 1 <= divide <= 16

		// f_display = f_oscillator / (D * K * MULTIPLEX_RATIO)
		// D = configurable divide ratio
		// K = period_phase1 + period_phase2 + pulsewidth_bank0
		//   = 54 when device starts
		// MULTIPLEX_RATIO controlled by set_displayed_height

		begin_communication();
		spiComm.transfer(SET_DISP_CLOCK_DIVIDE);
		spiComm.transfer((freq << 4) | (divide - 1));
		end_communication();
	}

	void reset_cursor_cache(void) {
		curPageStart = 0xF;
		curPageEnd = 0xF;
		curColStart = 0xFF;
		curColEnd = 0xFF;
	}

	void set_memory_mode_horizontal(void) {
		// Use set_region to move
		// Writing advances column, then row, then reset to 0, 0
		begin_communication();
		spiComm.transfer(SET_MEMORY_ADDR_MODE);
		spiComm.transfer(0x00);
		end_communication();
		reset_cursor_cache();
	}

	void set_memory_mode_vertical(void) {
		// Use set_region to move
		// Writing advances row, then column, then reset to 0, 0
		begin_communication();
		spiComm.transfer(SET_MEMORY_ADDR_MODE);
		spiComm.transfer(0x01);
		end_communication();
		reset_cursor_cache();
	}

	void set_memory_mode_page(void) {
		// Use set_page_start to move
		// Writing advances column, then reset to column 0 (row will not change)
		begin_communication();
		spiComm.transfer(SET_MEMORY_ADDR_MODE);
		spiComm.transfer(0x10); // Default
		end_communication();
		reset_cursor_cache();
	}

	// MUST ALREADY BE INSIDE begin_communication/end_communication block!
	void set_region(
		uint8_t colStart,
		uint8_t colEnd,
		uint8_t pageStart,
		uint8_t pageEnd
	) {
		if(pageStart != curPageStart || pageEnd != curPageEnd) {
			spiComm.transfer(SET_PAGE_ADDR);
			spiComm.transfer(pageStart);
			spiComm.transfer(pageEnd);
			curPageStart = pageStart;
			curPageEnd = pageEnd;
		}

		if(colStart != curColStart || colEnd != curColEnd) {
			spiComm.transfer(SET_COLUMN_ADDR);
			spiComm.transfer(colStart);
			spiComm.transfer(colEnd);
			curColStart = colStart;
			curColEnd = colEnd;
		}
	}

	// MUST ALREADY BE INSIDE begin_communication/end_communication block!
	void set_page_start(uint8_t page, uint8_t column) {
		if(page != curPageStart) {
			spiComm.transfer(PAGE_START | page);
			curPageStart = page;
		}
		if((column & 0x0F) != (curColStart & 0x0F)) {
			spiComm.transfer(LOWER_COLUMN | (column & 0x0F));
		}
		if((column >> 4) != (curColStart >> 4)) {
			spiComm.transfer(HIGHER_COLUMN | (column >> 4));
		}
		curColStart = column;
	}

	template <typename T> // T = ProgMem<uint8_t> / const uint8*
	[[gnu::noinline]] // Inlining slows down the common full-screen case
	void send_raw_b(
		T data, // nullable
		int16_t step,
		uint8_t w,
		uint8_t h,
		uint8_t hPage,
		int8_t yShift
	) {
		uint8_t mask = 0xFF << yShift;
		uint8_t baseMask = ~(0xFE << ((h + yShift + 7) & 7));
		if(hPage == 1) {
			mask &= baseMask;
		}
		for(uint8_t X = 0; X != w; ++ X) {
			spiComm.transfer(read_shifted(data, data, X, yShift) & mask);
		}
		for(uint8_t Y = 1; Y < hPage; ++ Y) {
			const auto Y0 = data + (Y * step - step);
			const auto Y1 = Y0 + step;
			mask = (Y == hPage - 1) ? baseMask : 0xFF;
			for(uint8_t X = 0; X < w; ++ X) {
				spiComm.transfer(read_shifted(Y0, Y1, X, yShift) & mask);
			}
		}
	}

	template <typename T> // T = ProgMem<uint8_t> / const uint8*
	[[gnu::always_inline]]
	inline void send_raw(
		T data, // nullable
		int16_t step,
		uint8_t x = 0,
		uint8_t yPage = 0,
		uint8_t w = DISP_WIDTH,
		uint8_t h = DISP_HEIGHT,
		uint8_t hPage = DISP_HEIGHT_BYTES,
		int8_t yShift = 0
	) {
		if(!data) {
			return;
		}

		begin_communication();
		if(scrolling) {
			// Cannot send data while scrolling
			stop_marquee();
		}

		set_region(x, x + w - 1, yPage, yPage + hPage - 1);

		dcPin.high();
		if(yShift == 0 && (h & 7) == 0) {
			// Optimised version for common case
			for(uint8_t Y = 0; Y < hPage; ++ Y) {
				for(uint8_t X = 0; X < w; ++ X) {
					spiComm.transfer(data[Y * step + X]);
				}
			}
		} else {
			send_raw_b(data, step, w, h, hPage, yShift);
		}
		dcPin.low();
		end_communication();
	}

public:
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t width(void) {
		return DISP_WIDTH;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t height(void) {
		return DISP_HEIGHT;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t height_bytes(void) {
		return DISP_HEIGHT_BYTES;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t fps(void) const {
		// With defaults, framerate is ~90 (11.4ms per frame)
		return framerate;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t fps(
		uint32_t Fosc,      // clocks / sec
		uint8_t divide,     // unit
		uint8_t precharge0, // clocks
		uint8_t precharge1, // clocks
		uint8_t mux         // unit
	) const {
		return Fosc / (divide * (precharge0 + precharge1 + 50) * mux);
	}

	void reset(void) {
		// Set RES# LOW for at least 3 microseconds
		rstPin.low();
		delayMicroseconds(5);
		rstPin.high();

		display_on = false; // Reset process leaves display OFF
		scrolling = false;
		reset_cursor_cache();

		// Apply tweaks to standard reset values:

		begin_communication();
		set_memory_mode_horizontal(); // Easier to send full screens of data
		set_display_frequency(8, 1);  // controls Fosc, divide
		set_precharge_periods(1, 15); // controls precharge0, 1
		set_charge_pump(true);
		framerate = fps(370000, 1, 1, 15, 64);

		// Switch on display (removed; allow library user to perform their own
		// configuration before switching on explicitly)
//		set_on(true);
		end_communication();
	}

	void set_on(bool active) {
		if(display_on != active) {
			transfer_wrapped(active ? DISPLAY_ON : DISPLAY_OFF);
			display_on = active;
		}
	}

	void set_invert(bool active) {
		transfer_wrapped(active ? INVERT_DISPLAY : NORMAL_DISPLAY);
	}

	void set_fs_white(bool active) {
		// When active, ignores current image and shows pure white
		// (returns to image when off)
		transfer_wrapped(active ? DISPLAY_SHOWS_ON : DISPLAY_SHOWS_RAM);
	}

	void set_contrast(uint8_t value) {
		// 0 <= value <= 255 (default = 0x7F) higher value = brighter
		begin_communication();
		spiComm.transfer(SET_CONTRAST_CONTROL);
		spiComm.transfer(value);
		end_communication();
	}

	void set_x_flip(bool active) {
		// Applies to data INPUT (will not change existing data on the chip)
		// Call refresh_all after changing to actually make a difference.
		transfer_wrapped(active ? SEGMENT_REMAP_RTL : SEGMENT_REMAP_LTR);
	}

	void set_y_flip(bool active) {
		// Applies to data OUTPUT (will affect display immediately)
		transfer_wrapped(active ? COM_SCAN_UP : COM_SCAN_DOWN);
	}

	void set_displayed_height(uint8_t h) {
		// 16 <= h < height()
		// Crops from the bottom of the display. This affects the fps of the
		// display, which in turn affects the brightness. Applies even if
		// display is set to set_fs_white = true
		begin_communication();
		spiComm.transfer(SET_MULTIPLEX_RATIO);
		spiComm.transfer(h - 1);
		end_communication();
	}

	void set_vertical_offset(int16_t offset) {
		// Pans (with modulo) the rendered image
		uint8_t shift = ext::posmod(offset, int16_t(height()));
		transfer_wrapped(DISPLAY_START_LINE | shift);
	}

	void set_vertical_offset_b(int16_t offset) {
		// Pans (with modulo) the source image from RAM
		// This is the same as set_vertical_offset UNLESS the MUX ratio has
		// been reduced, in which case this will maintain the "off" bar at the
		// bottom of the screen, whereas set_vertical_offset will pan the "off"
		// bar with the image.

		// This one applies even if the display is set to set_fs_white = true
		uint8_t shift = ext::posmod(offset, int16_t(height()));
		begin_communication();
		spiComm.transfer(SET_DISPLAY_OFFSET);
		spiComm.transfer(shift);
		end_communication();
	}

	void send(
		SolidFill s,
		uint8_t x = 0,
		uint8_t y = 0,
		uint8_t w = DISP_WIDTH,
		uint8_t h = DISP_HEIGHT
	) {
		send_raw(s, 0, x, (y >> 3), w, h, ((y + h + 7) >> 3) - (y >> 3), 0);
	}

	[[gnu::always_inline]]
	inline void send(
		bool s,
		uint8_t x = 0,
		uint8_t y = 0,
		uint8_t w = DISP_WIDTH,
		uint8_t h = DISP_HEIGHT
	) {
		send(SolidFill(s), x, y, w, h);
	}

	template <typename Bitmask>
	void send_part(
		const Bitmask &bitmask,
		uint8_t x, uint8_t y, // clipping region for bitmask
		uint8_t w, uint8_t h,
		int8_t xPos = 0, // point where 0,0 of the UNCLIPPED
		int8_t yPos = 0  // bitmask would appear
	) {
		int16_t step = bitmask.raw_step();
		uint8_t y0 = yPos + y;
		uint8_t hPage = ((y0 + h + 7) >> 3) - (y0 >> 3);
		uint8_t yShift = yPos & 7;
		uint8_t hh = (hPage << 3) - yShift;
		send_raw(
			bitmask.raw() + (x + (y >> 3) * step),
			step,
			xPos + x,
			y0 >> 3,
			w, ext::min2(bitmask.height(), hh),
			hPage,
			yShift
		);
	}

	template <typename Bitmask>
	[[gnu::always_inline]]
	inline void send(
		const Bitmask &bitmask,
		uint8_t xPos = 0,
		uint8_t yPos = 0
	) {
		send_part(bitmask, 0, 0, bitmask.width(), bitmask.height(), xPos, yPos);
	}

	void marquee(
		uint16_t frame_freq,
		int8_t x_speed,
		int16_t y_speed = 0,
		uint8_t horizontal_top_page = 0,
		uint8_t horizontal_base_page = DISP_HEIGHT_BYTES - 1,
		uint8_t vertical_top = 0,
		uint8_t vertical_base = DISP_HEIGHT
	) {
		// frame_freq = {2, 3, 4, 5, 25, 64, 128, 256}
		// x_speed = {-1, 1}
		// 0 <= horizontal_top_page <= horizontal_base_page < height()
		// 0 <= vertical_top < vertical_base <= height()

		// vertical_top & vertical_base control area which scrolls vertically.
		// horizontal_top_page & horizontal_base_page control area which scrolls
		// horizontally. This area is then moved according to the vertical
		// scroll and wraps around (i.e. the horizontal appears to happen first
		// within its horizontal_*_page, then the vertical within vertical_*)

		uint8_t v_height = vertical_base - vertical_top;
		uint8_t v_wrap = ((v_height <= 0)
			? 0
			: ext::posmod(y_speed, int16_t(v_height))
		);

		uint8_t frame_freq_value;
		switch(frame_freq) {
		case 2:   frame_freq_value = 0x07; break;
		case 3:   frame_freq_value = 0x04; break;
		case 4:   frame_freq_value = 0x05; break;
		case 5:   frame_freq_value = 0x00; break;
		case 25:  frame_freq_value = 0x06; break;
		case 64:  frame_freq_value = 0x01; break;
		case 128: frame_freq_value = 0x02; break;
		case 256: frame_freq_value = 0x03; break;
		default:  return; // unsupported value
		}

		begin_communication();
		
		if(scrolling) {
			// Cannot scroll while already scrolling; stop the previous one
			stop_marquee();
		}

		if(v_wrap != 0) {
			spiComm.transfer(CONFIG_SCROLL_VERT_AREA);
			spiComm.transfer(vertical_top);
			spiComm.transfer(v_height);
		}

		spiComm.transfer((x_speed > 0)
			? CONFIG_SCROLL_VRIGHT
			: CONFIG_SCROLL_VLEFT
		);
		spiComm.transfer(0x00); // Dummy byte
		spiComm.transfer(horizontal_top_page);
		spiComm.transfer(frame_freq_value);
		spiComm.transfer(horizontal_base_page);
		spiComm.transfer(v_wrap);

		spiComm.transfer(SCROLL_ON);
		end_communication();

		scrolling = true;
	}

	void stop_marquee(void) {
		if(scrolling) {
			transfer_wrapped(SCROLL_OFF);
			scrolling = false;
			// The spec sheet says the RAM is corrupt after scrolling, but it
			// appears to actually hold a valid image (the final frame of the
			// scroll), so we just stop the scroll and leave it where it is.
			// (The reference to corrupt RAM probably just means that it won't
			// be the same as the last written values, which is pretty obvious
			// anyway).
			// If this ever becomes an issue, include a forced refresh here.
		}
	}

	SSD1306_impl(SpiT spi, CSPinT cs, RSTPinT rst, DCPinT dc)
		: spiComm(spi, 0) // spiNesting
		, csPin(cs, 0) // curColStart
		, rstPin(rst, 0) // curColEnd
		, dcPin(dc, 0) // framerate
		, curPageStart(0)
		, curPageEnd(0)
		, display_on(false)
		, scrolling(false)
	{
		csPin.set_output();
		rstPin.set_output();
		dcPin.set_output();
		csPin.high();
		rstPin.high();
		dcPin.low();

		// Reset pin should stay HIGH until V_DD is stable, then run the reset
		// process to initialise
		delay(1); // Wait for V_DD to become stable

		reset();
	}

	SSD1306_impl(SSD1306_impl &&b)
		: spiComm(static_cast<ext::Flattener<SpiT,uint8_t>&&>(b.spiComm))
		, csPin(static_cast<ext::Flattener<CSPinT,uint8_t>&&>(b.csPin))
		, rstPin(static_cast<ext::Flattener<RSTPinT,uint8_t>&&>(b.rstPin))
		, dcPin(static_cast<ext::Flattener<DCPinT,uint8_t>&&>(b.dcPin))
		, curPageStart(b.curPageStart)
		, curPageEnd(b.curPageEnd)
		, display_on(b.display_on)
		, scrolling(b.scrolling)
	{
		// Prevent destructor of 'b' changing the device
		b.display_on = false;
		b.spiNesting = 0;
	}

	~SSD1306_impl(void) {
		// Shutdown process should be:
		// * DISPLAY_OFF
		// * V_CC OFF
		// * Delay 100ms
		// * V_DD OFF
		// But we don't have access to V_CC/V_DD, so must assume that the
		// breakout board does the right thing there. All we can do is send the
		// DISPLAY_OFF request, and shut down the SPI interface.
		set_on(false);

		if(spiNesting > 0) {
			spiNesting = 1;
			end_communication();
		}
	}
#undef spiNesting
#undef curColStart
#undef curColEnd
#undef framerate
};

template <
	uint8_t DISP_WIDTH = 128,
	uint8_t DISP_HEIGHT = 64,
	typename SpiT,
	typename CSPinT,
	typename RSTPinT,
	typename DCPinT
>
[[gnu::always_inline]]
inline SSD1306_impl<
	DISP_WIDTH,
	DISP_HEIGHT,
	SpiT,
	CSPinT,
	RSTPinT,
	DCPinT
> MakeSSD1306(
	SpiT spi,
	CSPinT cs,
	RSTPinT rst,
	DCPinT dc
) {
	return SSD1306_impl<
		DISP_WIDTH,
		DISP_HEIGHT,
		SpiT,
		CSPinT,
		RSTPinT,
		DCPinT
	>(
		spi, cs, rst, dc
	);
}

#endif

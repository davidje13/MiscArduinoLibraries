/*
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

/*
 * Hardware connections:
 *
 * GND -- GND
 *  +V -- 5V
 * DAT -- D* (any available pin; set below)
 */

#include <IRReceiver.h>
#include <SSD1306.h>
#include <Bitmask18.h>
#include <Font.h>
#include <FontFixed.h>
#include <FontRenderer.h>
#include <ArduinoPin.h>
#include <RawArduinoPin.h>
#include <ArduinoSPI.h>

// This can be set to any available pin, but it is recommended to use a pin
// which can have interrupts set
#define IR_PIN FixedArduinoPin<2>()

#define OLED_PIN_CS  FixedArduinoPin<10>()
#define OLED_PIN_RST FixedArduinoPin<7>()
#define OLED_PIN_DC  FixedArduinoPin<9>()

#define DISPLAY_RESOLUTION 200

void setup(void) {
	auto ir = MakeAsynchronousIRReceiver(
		IR_PIN,
		IRReceiver::Activation::ACTIVE_LOW
	);
	ir.start();

	auto oled = MakeSSD1306<128,64>(
		ArduinoSPI(),
		OLED_PIN_CS,
		OLED_PIN_RST,
		OLED_PIN_DC
	);
	oled.set_on(true);

	Bitmask18<oled.width(),oled.height()> bitmask;

//	auto pin = IR_PIN.fast();
//	uint8_t y = 0;
//	while(true) {
//		for(uint8_t x = 0; x < 128; ++ x) {
//			bitmask.set_pixel(
//				x,
//				y,
//				pin.read_digital() ? BlendMode::ON : BlendMode::OFF
//			);
//		}
//		if((++ y) >= 64) {
//			y = 0;
//		}
//		oled.send(bitmask);
//	}

	Font f(FONT_FIXED_DATA, FONT_FIXED_IMG, FONT_FIXED_MASK);
	auto r = MakeFontRenderer(&bitmask, 0, 0);

	while(true) {
		bitmask.clear();
		r.move(0, 0);
		r.print(f, ProgMemString("SCANNING..."));
		oled.send(bitmask);

		const IRCommand *recorded = ir.await_command();
		if(recorded) {
			uint8_t length = recorded->length();
			uint16_t headerDuration = recorded->header_duration();
			uint64_t data = recorded->decode_data();

			bitmask.clear();
			r.move(0, 0);
			r.print(f, ProgMemString("Header: "));
			r.print_number(f, headerDuration);
			r.print(f, ProgMemString("us\n\n"));
			r.print_number(f, length);
			r.print(f, ProgMemString("-bit data:\n"));
			r.print_number_base(f, data, 16, 16);
			r.print(f, '\n');

			uint8_t y = r.cursor_y() + 4;
			uint8_t x = 0;
			for(uint8_t n = 0; n < length; ++ n) {
				uint8_t w = (
					(recorded->duration(n) + DISPLAY_RESOLUTION / 2) /
					DISPLAY_RESOLUTION
				);
				if(w < 1) {
					w = 1;
				}
				if(!(n & 1)) {
					bitmask.fill_rect(x, y, w, 6);
					if(x + w > bitmask.width()) {
						bitmask.fill_rect(0, y + 8, x + w - bitmask.width(), 6);
					}
				}
				x += w;
				if(x > bitmask.width()) {
					x -= bitmask.width();
					y += 8;
				}
			}

			ir.invalidate(recorded);

			oled.send(bitmask);
			delay(2000);
		}
	}
}

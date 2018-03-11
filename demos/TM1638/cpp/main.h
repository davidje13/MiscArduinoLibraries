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
 *  GND -- GND
 *   +V -- 5V
 *  DIO -- D* (any available pin; set below)
 *  CLK -- D* (any available pin; set below)
 *  STB -- D* (any available pin; set below)
 */

#include <TM1638.h>
#include <SegmentDigits.h>
#include <ArduinoPin.h>
#include <ProgMem.h>

// These can be set to any available pin
#define DIO_PIN FixedArduinoPin<10>()
#define CLK_PIN FixedArduinoPin<11>()
#define STB_PIN FixedArduinoPin<12>()

static PROGMEM const char MESSAGE[] =
	"    this TM1638-controlled LED display is easy!";

void setup(void) {
	auto display = MakeTM1638(STB_PIN, CLK_PIN, DIO_PIN);

	uint8_t disp[4];
	auto digits = MakeSegmentDigits(display);

	while(true) {
		display.set_on(true);
		display.set_digits(0xFFFFFFFFu);
		delay(500);

		for(int i = 0; i < 10000; ++ i) {
			digits.print_int(i, disp);
			display.set_digits(disp);
		}

		digits.print("12.34", disp);
		display.set_digits(disp);
		delay(1000);

		display.clear();
		delay(300);

		auto messagePos = MakeProgMem(MESSAGE);

		do {
			digits.print(messagePos, disp);
			display.set_digits(disp);
			delay(150);
		} while(digits.advance(messagePos));
		delay(300);
	}
}

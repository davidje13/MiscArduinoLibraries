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
 *  VCC -- 5V
 * TRIG -- D12 (any available pin; set below)
 * ECHO -- D2  (any interrupt-enabled pin; set below)
 *  GND -- GND
 */

#include <HCSR04.h>
#include <ArduinoPin.h>
#include <74HC595.h>
#include <LCDMatrix88.h>
#include <Bitmask18.h>

#define TRIG_PIN FixedArduinoPin<12>()
#define ECHO_PIN FixedArduinoPin<2>()

#define LCD_PIN_ST_CP FixedArduinoPin<7>()
#define LCD_PIN_SH_CP FixedArduinoPin<8>()
#define LCD_PIN_DS    FixedArduinoPin<9>()

// Show full circle at 50cm, shrink as object approaches
#define DISP_DIST 500
#define MAX_DIST 600

void setup(void) {
	auto lcd = MakeLCDMatrix88(Make74HC595(
		LCD_PIN_ST_CP,
		LCD_PIN_SH_CP,
		LCD_PIN_DS
	));
	constexpr uint8_t dim = lcd.width() * 4;
	Bitmask18<dim,dim> bitmask;

	auto echo = MakeAsynchronousHCSR04(TRIG_PIN, ECHO_PIN);
	uint16_t averaged = 0;

	while(true) {
		uint16_t tm0 = millis();
		uint16_t lastDistance = echo.measure_millimetres();

		if(lastDistance > MAX_DIST || lastDistance == HCSR04::no_reflection()) {
			lastDistance = MAX_DIST;
		}

		averaged = (averaged * 3 + lastDistance) / 4;

		bitmask.clear();
		uint8_t rad = averaged * (dim / 2) / DISP_DIST;
		bitmask.fill_ellipse(
			dim / 2 - rad,
			dim / 2 - rad,
			rad * 2,
			rad * 2
		);
		do {
			lcd.send(bitmask);
		} while(uint16_t(millis()) - tm0 < 50);
	}
}

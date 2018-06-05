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
 *   GND -- GND
 *    +V -- 5V
 * ST_CP -- D* (any available pin; set below)
 * SH_CP -- D* (any available pin; set below)
 *    DS -- D* (any available pin; set below)
 */

#include <74HC595.h>
#include <LEDMatrix88.h>
#include <ArduinoPin.h>
#include <DemoText.h>
#include <DemoPatterns.h>
#include <Demo3D.h>
#include <DemoShapes.h>

// These can be set to any available pin
#define LCD_PIN_ST_CP FixedArduinoPin<7>()
#define LCD_PIN_SH_CP FixedArduinoPin<8>()
#define LCD_PIN_DS    FixedArduinoPin<9>()

void setup(void) {
	auto lcd = MakeLEDMatrix88(Make74HC595(
		LCD_PIN_ST_CP,
		LCD_PIN_SH_CP,
		LCD_PIN_DS
	));

	while(true) {
		demoText(lcd);
		demoPatterns(lcd);
		demo3D(lcd);
		demoShapes(lcd);
	}
}

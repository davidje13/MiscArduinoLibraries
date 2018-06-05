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
 * A -- D* (any available pin; set below)
 * B -- D* (any available pin; set below)
 * ? -- D* (attach a button to any available pin; set below)
 */

#include <RotaryEncoder.h>
#include <ArduinoPin.h>
#include <74HC595.h>
#include <LEDMatrix88.h>
#include <Bitmask18.h>

// These can be set to any available pin, but it is recommended to use pins
// which can have interrupts set
#define ROT_PIN_A FixedArduinoPin<2>()
#define ROT_PIN_B FixedArduinoPin<3>()

// This can be any available pin
#define TOGGLE_PIN FixedArduinoPin<4>()

#define LCD_PIN_ST_CP FixedArduinoPin<7>()
#define LCD_PIN_SH_CP FixedArduinoPin<8>()
#define LCD_PIN_DS    FixedArduinoPin<9>()

#define SCALE 4

void setup(void) {
	auto lcd = MakeLEDMatrix88(Make74HC595(
		LCD_PIN_ST_CP,
		LCD_PIN_SH_CP,
		LCD_PIN_DS
	));
	Bitmask18<lcd.width(),lcd.height()> bitmask;

	bool toggleState = false;
	TOGGLE_PIN.set_input();

	auto encoder = MakeInterruptRotaryEncoder(ROT_PIN_A, ROT_PIN_B);

	int raw = (encoder.fraction() + 2) % SCALE;

	while(true) {
		raw += encoder.delta();
		raw = (raw + (65 * SCALE)) % (65 * SCALE);
		uint8_t pos = raw / SCALE - 1;
		if(pos == 0xFF) {
			lcd.send(bitmask);
			continue;
		}

		uint8_t x = pos & 7;
		uint8_t y = pos / 8;

		bool toggle = TOGGLE_PIN.read_digital();
		if(toggle != toggleState) {
			toggleState = toggle;
			if(!toggle) {
				bitmask.pixel_toggle(x, y);
			}
		}
		bitmask.outline_rect(x - 1, y - 1, 3, 3, 1, BlendMode::XOR);
		lcd.send(bitmask);
		bitmask.outline_rect(x - 1, y - 1, 3, 3, 1, BlendMode::XOR);
	}
}

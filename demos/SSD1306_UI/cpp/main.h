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
 *        GND -- GND
 *         +V -- 3.3V
 *   DAT (D1) -- D11 (MOSI)
 *   CLK (D0) -- D13 (SCK)
 *         DC -- D* (any available pin; set below)
 *        RST -- D* (any available pin; set below)
 *         CS -- D* (any available pin; set below)
 */

#include "Menu.h"
#include "TextField.h"

#include <ArduinoPin.h>
#include <ArduinoAnalogPin.h>
#include <ArduinoSPI.h>
#include <ProgMem.h>

#include <IRReceiver.h>

#include <RotaryEncoder.h>
#include <NotchedEncoder.h>
#include <DebouncedButton.h>

#include <ArduinoTWIMaster.h>
#include <24LC256.h>

#include <SSD1306.h>
#include <Bitmask18.h>
#include <Font.h>
#include <FontVariable.h>
#include <FontFixed.h>

FixedArduinoPin<10> oledCsPin;
FixedArduinoPin<7> oledRstPin;
FixedArduinoPin<9> oledDcPin;

//FixedArduinoPin<3> irPin;

FixedArduinoPin<2> rotAPin;
FixedArduinoPin<8> rotBPin;
FixedArduinoPin<12> btnPin;

//FixedArduinoAnalogPin<0> potL;
//FixedArduinoAnalogPin<1> potR;

//auto ir = MakeAsynchronousIRReceiver(irPin);
auto encoder = MakeInterruptRotaryEncoder(rotAPin, rotBPin, true);
auto notchedEncoder = MakeNotchedEncoder(&encoder, 2);
auto btn = MakeDebouncedButton(btnPin, false);

//ArduinoTWIMaster twi;
//auto eeprom = Make24LC256(twi, 0x0);

template <typename Display>
int showMainMenu(Display &display) {
	uint8_t itemCount = 0;

	ProgMem<char> items[16];
	items[itemCount++] = ProgMemString("Show Demo");
	items[itemCount++] = ProgMemString("Options");
	items[itemCount++] = ProgMemString("Enter Name");
	items[itemCount++] = ProgMemString("Configure Remote");

	auto menu = MakeMenu([&items, itemCount] (int16_t index) {
		if(index < 0 || index >= itemCount) {
			return ProgMem<char>(nullptr);
		} else {
			return items[index];
		}
	});

	notchedEncoder.reset();

	Bitmask18<display.width(),display.height()> bitmask;
	auto font = MakeVariableFont();
	while(true) {
		if(btn.released()) {
			return menu.index();
		}
		menu.scroll(notchedEncoder, true);

		bitmask.clear(false);
		menu.render(
			bitmask, font,
			24,
			(display.height() - 8) / 2,
			display.width() - 24 * 2,
			8
		);
		display.send(bitmask);
		delay(10);
	}
}

template <typename Display>
void showNameSelect(Display &display) {
	constexpr uint8_t maxLen = 8;

	auto chars = ProgMemString(
		" "
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"_.,:;-+="
		"0123456789"
		"!@#$%^&*"
		"(){}[]<>"
		"\\/|~'\""
	);

	char input[maxLen] = {};

	auto field = MakeTextField(input, maxLen, chars);

	notchedEncoder.reset();
	bool navigating = false;

	Bitmask18<display.width(),display.height()> bitmask;
	auto font = MakeFixedFont();
	while(true) {
		if(btn.released()) {
			navigating = !navigating;
			field.reset_fractions();
		}

		if(navigating) {
			field.navigate(notchedEncoder, true);
		} else {
			field.spin(notchedEncoder, true);
		}

		bitmask.clear(false);
		field.render(
			bitmask, font,
			!navigating,
			(display.width() - 9 * maxLen) / 2,
			(display.height() - 8) / 2,
			9,
			8
		);
		display.send(bitmask);
		delay(10);
	}
}

void setup(void) {
	// To allow easy attachment of an IR receiver,
	// set pin 4 to GND (allows direct attachment on 3, 4 and VCC)
	FixedArduinoPin<4>().set_output();
	btnPin.set_input(true);

	auto oled = MakeSSD1306<128,64>(
		ArduinoSPI(),
		oledCsPin,
		oledRstPin,
		oledDcPin
	);

	oled.set_on(true);
	oled.set_x_flip(true);
	oled.set_y_flip(true);

	// TODO: use a state machine to track screens (also use transitions)
	while(true) {
		switch(showMainMenu(oled)) {
		case 0: // Show Demo
			break;
		case 1: // Options
			break;
		case 2: // Enter Name
			showNameSelect(oled);
			break;
		case 3: // Configure Remote
			break;
		}
	}
}

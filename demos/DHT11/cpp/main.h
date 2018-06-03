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
 * DATA -- D* (any available pin; set below)
 *  GND -- GND
 */

#include <DHT11.h>
#include <ArduinoPin.h>
#include <VoidPin.h>
#include <PinBank.h>
#include <ProgMem.h>
#include <1602.h>

FixedArduinoPin<10> DATA_PIN;

FixedArduinoPin<12> LCD_RS_PIN;
VoidPin LCD_RW_PIN;
FixedArduinoPin<11> LCD_EN_PIN;
FixedArduinoPin<5> LCD_DAT7;
FixedArduinoPin<6> LCD_DAT6;
FixedArduinoPin<7> LCD_DAT5;
FixedArduinoPin<8> LCD_DAT4;

FixedArduinoPin<13> STATUS;

void setup(void) {
	STATUS.set_output();
	STATUS.low();

	auto lcd = Make1602<16, 2>(
		MakePinBank(LCD_DAT7, LCD_DAT6, LCD_DAT5, LCD_DAT4),
		LCD_RS_PIN,
		LCD_RW_PIN,
		LCD_EN_PIN
	);

	// Change to MakeDHT11 if you have no external pullup resistor
	auto sensor = MakeDHT11ExternalPullup(DATA_PIN);

	lcd.on();

	while(true) {
		STATUS.high();

		auto measurement = sensor.read();

		STATUS.low();

		switch(measurement.state()) {
		case DHT11::State::OK:
			lcd.set_cursor(0, 0);
			lcd.print(ProgMemString("Temp.:  "));
			lcd.print(uint16_t(measurement.temperature()));
			lcd.print('.');
			lcd.print(uint16_t(measurement.temperature() * 10 + 0.5) % 10);
			lcd.print(ProgMemString("C       "));

			lcd.set_cursor(0, 1);
			lcd.print(ProgMemString("Humid.: "));
			lcd.print(uint16_t(measurement.humidity()));
			lcd.print('.');
			lcd.print(uint16_t(measurement.humidity() * 10 + 0.5) % 10);
			lcd.print(ProgMemString("%       "));
			break;

		case DHT11::State::TIMEOUT:
			lcd.set_cursor(0, 0);
			lcd.print(ProgMemString("Invalid Reading "));
			lcd.set_cursor(0, 1);
			lcd.print(ProgMemString("Timeout         "));
			break;

		case DHT11::State::CHECKSUM_MISMATCH:
			lcd.set_cursor(0, 0);
			lcd.print(ProgMemString("Invalid Reading "));
			lcd.set_cursor(0, 1);
			lcd.print(ProgMemString("Checksum Error  "));
			break;
		}
	}
}

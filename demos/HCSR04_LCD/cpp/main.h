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
 * TRIG -- D* (any available pin; set below)
 * ECHO -- D* (any available pin; set below)
 *  GND -- GND
 */

#include <HCSR04.h>
#include <ArduinoPin.h>
#include <VoidPin.h>
#include <PinBank.h>
#include <ProgMem.h>
#include <1602.h>

// To use the TWI adapter, uncomment these two lines:
//#include <ArduinoTWIMaster.h>
//#include <PCF8574_1602.h>

#define TRIG_PIN FixedArduinoPin<2>()
#define ECHO_PIN FixedArduinoPin<3>()

#define LCD_RS_PIN FixedArduinoPin<12>()
#define LCD_RW_PIN VoidPin()
#define LCD_EN_PIN FixedArduinoPin<11>()
#define LCD_DAT7 FixedArduinoPin<5>()
#define LCD_DAT6 FixedArduinoPin<6>()
#define LCD_DAT5 FixedArduinoPin<7>()
#define LCD_DAT4 FixedArduinoPin<8>()

#define STATUS FixedArduinoPin<13>()

void setup(void) {
	STATUS.set_output();
	STATUS.low();

#ifdef PCF8574_1602_H_INCLUDED
	bool isPCF8574A = true;
	uint8_t address = 0x7;
	auto pcf8504 = MakePCF8574(
		ArduinoTWIMaster(),
		VoidPin(),
		isPCF8574A,
		address
	);
	if(!pcf8504.connected()) {
		// Failed to find device; flash the status LED to show the error
		while(true) {
			STATUS.high();
			delay(500);
			STATUS.low();
			delay(500);
		}
	}

	auto lcd = Make1602<16, 2>(pcf8504);
#else
	auto lcd = Make1602<16, 2>(
		MakePinBank(LCD_DAT7, LCD_DAT6, LCD_DAT5, LCD_DAT4),
		LCD_RS_PIN,
		LCD_RW_PIN,
		LCD_EN_PIN
	);
#endif

	auto echo = MakeSynchronousHCSR04(TRIG_PIN, ECHO_PIN);
	uint16_t averaged = 0;

	lcd.on();
	lcd.print(ProgMemString("Distance:"));

	while(true) {
		STATUS.high();

		uint16_t tm0 = millis();
		uint16_t distance = echo.measure_millimetres();

		STATUS.low();

		if(distance == HCSR04::no_reflection()) {
			distance = 4000;
		}

		averaged = (averaged * 3 + distance) / 4;

		lcd.set_cursor(0, 1);
		lcd.print(averaged);
		lcd.print(ProgMemString("mm        "));

		while(uint16_t(millis()) - tm0 < 50);
	}
}

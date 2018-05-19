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
 *  TX  -- D5 (any serial receive pin)
 *  RX  -- D6 (any serial transmit pin)
 */

#include <CSNA2.h>
#include <ProgMem.h>
#include <ArduinoSoftwareSerial.h>
#include <ArduinoPin.h>

// Note that TX/RX pins are reversed compared to printer connections
FixedArduinoPin<6> TX;
FixedArduinoPin<5> RX;

FixedArduinoPin<13> STATUS;

void setup(void) {
	STATUS.set_output();

	STATUS.high();
	auto printer = MakeCSNA2(MakeArduinoSoftwareSerial(TX, RX), 19200);
	STATUS.low();

	if(!printer.connected()) {
		while(true) {
			STATUS.high();
			delay(100);
			STATUS.low();
			delay(100);
		}
	}

	printer.configure(CSNA2::Configuration::FACTORY);
	printer.set_sleep_delay(1);

	printer.print(ProgMemString("normal "));
	printer.set_emphasised(true);
	printer.print(ProgMemString("emphasis "));
	printer.set_emphasised(false);
	printer.set_doublestrike(true);
	printer.print(ProgMemString("dblstrike "));
	printer.set_emphasised(true);
	printer.print(ProgMemString("both "));
	printer.set_doublestrike(false);
	printer.set_emphasised(false);
	printer.linefeed();

	printer.print(ProgMemString("normal "));
	printer.set_underline(true);
	printer.print(ProgMemString("underline "));
	printer.set_underline(CSNA2::Underline::THICK);
	printer.print(ProgMemString("dblunderline "));
	printer.set_underline(false);
	printer.linefeed();

	printer.print(ProgMemString("normal "));
	printer.set_inverted(true);
	printer.print(ProgMemString("inverted "));
	printer.set_inverted(false);
	printer.linefeed();

	printer.set_rotation(CSNA2::Rotation::CW_90);
	printer.print(ProgMemString("CW_90"));
	printer.linefeed();
	printer.set_rotation(CSNA2::Rotation::CCW_90);
	printer.print(ProgMemString("CCW_90"));
	printer.linefeed();
	printer.set_rotation(CSNA2::Rotation::REVERSED);
	printer.print(ProgMemString("REVERSED"));
	printer.linefeed();
	printer.set_rotation(CSNA2::Rotation::NORMAL);

	printer.print(ProgMemString("x0"));
	printer.set_x(100);
	printer.print(ProgMemString("x100"));
	printer.set_x(200);
	printer.print(ProgMemString("x200"));
	printer.linefeed();

	printer.print(ProgMemString("x0"));
	printer.set_x(100);
	printer.print(ProgMemString("x100"));
	printer.set_x(85);
	printer.print(ProgMemString("x85"));
	printer.linefeed();

	printer.awake();
	printer.print(ProgMemString("\n\n")); // Make text visible

	while(true) {
		STATUS.high();
		delay(500);
		STATUS.low();
		delay(500);
	}
}

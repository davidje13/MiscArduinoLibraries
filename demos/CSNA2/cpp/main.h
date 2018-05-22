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

#include "DemoText.h"
#include "DemoBarcodes.h"
#include "DemoCharset.h"
#include "DemoBitmaps.h"
#include "Demo3D.h"
#include "DemoConfig.h"

#include <CSNA2.h>
#include <ProgMem.h>
#include <ArduinoSoftwareSerial.h>
#include <ArduinoPin.h>

// Note that TX/RX pins are reversed compared to printer connections
FixedArduinoPin<6> TX;
FixedArduinoPin<5> RX;

FixedArduinoPin<7> fakeGround;
FixedArduinoPin<13> status;

void setup(void) {
	// Set pin 7 to a fake ground to allow easy wiring on an Arduino Uno
	fakeGround.set_output();
	fakeGround.low();

	status.set_output();

	status.high();
	auto printer = MakeCSNA2(MakeArduinoSoftwareSerial(TX, RX), 19200);
	status.low();

	if(!printer.connected()) {
		// Keep trying to reset printer and reconnect
		// (might be waiting to fill a buffer from a previous run)
		do {
			status.high();
			delay(100);
			status.low();

			printer.awake();
		} while(!printer.connected());

		printer.reset();
	}

	printer.configure(CSNA2::Configuration::FACTORY);
	printer.set_sleep_delay(1);

	demoText(printer);
	demoBarcodes(printer);
	demoCharset(printer);
	demoBitmaps(printer);
	demo3D(printer);
	demoConfig(printer);

	// Make last output visible
	delay(1000);
	printer.awake();
	printer.print(ProgMemString("\n\n\n"));

	while(true) {
		status.high();
		delay(500);
		status.low();
		delay(500);
	}
}

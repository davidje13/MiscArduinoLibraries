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
 * DCKI -- D* (any available pin; set below)
 *   DI -- D* (any available pin; set below)
 */

#include <MY9221.h>
#include <ArduinoPin.h>
#include <FastMath.h>

// These can be set to any available pin
#define DCKI_PIN FixedArduinoPin<0>()
#define DI_PIN   FixedArduinoPin<1>()

void setup(void) {
	auto bar = MakeMY9221(DCKI_PIN, DI_PIN);
	bar.set_frequency(MY9221::Frequency::F8600000Hz);
	bar.set_modulation(MY9221::Modulation::PULSE_DENSITY);

	uint8_t values[12];
	uint16_t fineValues[12];

	while(true) {
		for(uint8_t i = 0; i < 12; ++ i) {
			values[i] = 0xFF;
		}
		bar.set_norm(values);
		delay(1000);

		for(uint8_t i = 0; i < 12; ++ i) {
			values[i] = uint8_t((uint16_t(i) * 256) / 12);
			fineValues[i] = uint16_t((uint32_t(i) * 65536) / 12);
		}
		bar.set_norm(fineValues);
		delay(1000);
		bar.set_norm(values);
		delay(1000);

		for(uint16_t p = 0; p < 1024; p += 8) {
			for(uint8_t i = 0; i < 12; ++ i) {
				values[i] = sin8(p + i * 64) + 128;
			}
			p += 1;
			bar.set_norm(values);
			delay(20);
		}
	}
}

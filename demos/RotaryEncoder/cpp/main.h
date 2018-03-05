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
 * D13 is used to output status (via built-in LED)
 */

#include <RotaryEncoder.h>
#include <ArduinoPin.h>

// These can be set to any available pin, but it is recommended to use pins
// which can have interrupts set
#define ROT_PIN_A FixedArduinoPin<2>()
#define ROT_PIN_B FixedArduinoPin<3>()
#define OUT_PIN   FixedArduinoPin<13>()

#define SCALE 4
#define INITIAL_SPEED 5
#define MIN_SPEED 0
#define MAX_SPEED 20
#define TARGET 1000

void setup(void) {
	OUT_PIN.set_output();
	OUT_PIN.low();

	auto encoder = MakeInterruptRotaryEncoder(ROT_PIN_A, ROT_PIN_B);

	int raw = INITIAL_SPEED * SCALE + ((encoder.fraction() + 2) % SCALE);
	int lastTm = millis();
	int progress = 0;
	int state = 0;

	while(true) {
		raw += encoder.delta();
		if(raw < MIN_SPEED * SCALE) {
			raw = MIN_SPEED * SCALE;
		} else if(raw > MAX_SPEED * SCALE) {
			raw = MAX_SPEED * SCALE;
		}
		int speed = raw / SCALE;

		int tm = millis();
		progress += (tm - lastTm) * speed;
		lastTm = tm;
		if(progress >= TARGET) {
			progress -= TARGET;
			state = !state;
			if(state) {
				OUT_PIN.high();
			} else {
				OUT_PIN.low();
			}
		}
	}
}

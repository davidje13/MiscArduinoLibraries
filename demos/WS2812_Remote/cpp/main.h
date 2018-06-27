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
 *  In -- D6 (any available pin)
 *  IR -- D3 (any interrupt-capable pin)
 */

#include <ArduinoPin.h>

#include <WS2812.h>
#include <HumanLuminosity.h>

#include <IRReceiver.h>

#include <RotaryEncoder.h>

FixedArduinoPin<6> stripPin;
FixedArduinoPin<3> irPin;
FixedArduinoPin<2> rotAPin;
FixedArduinoPin<7> rotBPin;
FixedArduinoPin<8> btnPin;

const uint8_t pixelCount = 12;

#define SCALE 4

void setup(void) {
	// To allow easy attachment of an IR receiver,
	// set pins 4 and 5 to GND and V+ (allows direct attachment on 3, 4 and 5)
	FixedArduinoPin<4>().set_output();
	FixedArduinoPin<5>().set_output();
	FixedArduinoPin<5>().high();

	btnPin.set_input(true);
	auto encoder = MakeInterruptRotaryEncoder(rotAPin, rotBPin, true);

	auto strip = MakeWS2812(stripPin);
	auto ir = MakeAsynchronousIRReceiver(irPin);

	uint16_t countdown = 0;
	const uint16_t frameDelay = 50;
	const uint16_t displayMaxDelay = 2000;
	int raw = encoder.fraction() % SCALE;

	while(true) {
		const IRCommand *recorded = ir.await_command(frameDelay);
		if(recorded) {
			const auto data = recorded->compress();
			ir.invalidate(recorded);

			strip.send_rgb_fn(pixelCount, [&data] (
				uint8_t i,
				uint8_t *r,
				uint8_t *g,
				uint8_t *b
			) {
				*r = HUMAN_LUMINOSITY_R[data.read(i * 3) ? 128 : 0];
				*g = HUMAN_LUMINOSITY_G[data.read(i * 3 + 1) ? 128 : 0];
				*b = HUMAN_LUMINOSITY_B[data.read(i * 3 + 2) ? 128 : 0];
			});
			countdown = displayMaxDelay / frameDelay;
		}

		if(countdown) {
			-- countdown;
			continue;
		}

		const int scaledPixelCount = pixelCount * SCALE;
		raw = (raw - encoder.delta() + scaledPixelCount * 2) % scaledPixelCount;
		const uint8_t spin = raw / SCALE;
		const uint8_t frac = raw % SCALE;

		strip.send_rgb_fn(pixelCount, [spin, frac] (
			uint8_t i,
			uint8_t *r,
			uint8_t *g,
			uint8_t *b
		) {
			bool m1 = (i == spin);
			bool m2 = (i == frac * 3);
			*r = HUMAN_LUMINOSITY_R[m2 ? 64 : 0];
			*g = HUMAN_LUMINOSITY_G[m1 ? 64 : 0];
			*b = HUMAN_LUMINOSITY_B[m1 ? 64 : 0];
		});
	}
}

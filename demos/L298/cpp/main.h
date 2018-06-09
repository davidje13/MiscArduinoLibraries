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
 * Motor shield attached to Arduino Uno;
 * Channel A:
 *  Direction -- D12
 *  EN / PWM  -- D3
 *  Brake     -- D9
 *  Current   -- A0
 *
 * Channel B:
 *  Direction -- D13
 *  EN / PWM  -- D11
 *  Brake     -- D8
 *  Current   -- A1
 */

#include <RotaryEncoder.h>
#include <ArduinoPin.h>
#include <ArduinoMotorShield.h>

FixedArduinoPin<7> rotPinA;
FixedArduinoPin<6> rotPinB;
FixedArduinoPin<5> rotPinPress;

#define SCALE 4

void setup(void) {
	auto channelA = MakeArduinoMotorShieldChannelA();
	auto channelB = MakeArduinoMotorShieldChannelB();

	channelA.forward();
	channelB.reverse();
	delay(2000);

	channelA.brake();
	channelB.brake();
	delay(1000);

	channelA.reverse();
	channelB.forward();
	delay(2000);

	channelA.freewheel();
	channelB.freewheel();
	delay(1000);

	for(int speed = 0; speed < 255; speed += 5) {
		channelA.set(speed);
		channelB.set(-speed);
		delay(20);
	}

	for(int speed = 255; speed >= 0; speed -= 5) {
		channelA.set(speed);
		channelB.set(-speed);
		delay(20);
	}

	auto encoder = MakeRotaryEncoder(rotPinA, rotPinB);
	bool pressState = false;
	rotPinPress.set_input();

	int raw = (encoder.fraction() + 2) % SCALE;

	int16_t speeds[] = {0, 0};
	uint8_t channel = 0;

	while(true) {
		raw += encoder.delta();

		if(raw >= SCALE) {
			speeds[channel] = min(255, speeds[channel] + 15);
			raw -= SCALE;
		} else if(raw < 0) {
			speeds[channel] = max(-255, speeds[channel] - 15);
			raw += SCALE;
		}

		bool press = rotPinPress.read_digital();
		if(press != pressState) {
			pressState = press;
			if(!press) {
				channel = !channel;
			}
		}

		channelA.set(speeds[0]);
		channelB.set(speeds[1]);
	}
}

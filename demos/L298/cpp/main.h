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

#include <L298.h>
#include <ArduinoTWIMaster.h>
#include <ArduinoPin.h>
#include <VoidPin.h>
#include <MCP23017.h>

void err(int n) {
	FixedArduinoPin<2> ledErr;
	ledErr.set_output();

	if(n == 0) {
		ledErr.high();
		return;
	}
	while(true) {
		for(int i = 0; i < n; ++ i) {
			ledErr.high();
			delay(150);
			ledErr.low();
			delay(150);
		}
		delay(500);
	}
}

void setup(void) {
	auto twi = ArduinoTWIMaster();
	auto io = MakeMCP23017(twi, 0x7);

	switch(io.connection_status()) {
	case MCP23017::ConnectionStatus::CONNECTED:
		err(0);
		break;
	case MCP23017::ConnectionStatus::REQUEST_ERR_DATA_TOO_LONG:
		err(1);
		break;
	case MCP23017::ConnectionStatus::REQUEST_ERR_NACK_ADDR:
		err(2);
		break;
	case MCP23017::ConnectionStatus::REQUEST_ERR_NACK_DATA:
		err(3);
		break;
	case MCP23017::ConnectionStatus::REQUEST_ERR_OTHER:
		err(4);
		break;
	case MCP23017::ConnectionStatus::READ_TIMEOUT:
		err(5);
		break;
	}

	auto led = io.pin<MCP23017::Pin::A_0>();
	led.set_output();

	auto channelA = MakeSN754410Channel(
		io.pin<MCP23017::Pin::B_0>(),
		io.pin<MCP23017::Pin::B_2>(),
		FixedArduinoPin<5>()
	);
	auto channelB = MakeSN754410Channel(
		io.pin<MCP23017::Pin::B_1>(),
		io.pin<MCP23017::Pin::B_3>(),
		FixedArduinoPin<13>()
	);

	led.high();
	delay(200);
	led.low();

	channelA.forward();
	channelB.reverse();
	delay(2000);

	led.high();
	delay(200);
	led.low();

	channelA.brake();
	channelB.brake();
	delay(1000);

	led.high();
	delay(200);
	led.low();

	channelA.reverse();
	channelB.forward();
	delay(2000);

	led.high();
	delay(200);
	led.low();

	channelA.freewheel();
	channelB.freewheel();
	delay(1000);

	led.high();
	delay(200);
	led.low();

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

	while(true) {
		led.high();
		delay(200);
		led.low();
		delay(200);
	}
}

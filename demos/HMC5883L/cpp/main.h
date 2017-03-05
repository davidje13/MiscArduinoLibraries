/*
 * Written in 2017 by David Evans
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
 * VCC_IN -- 3.3V or 5V (VCC of Arduino)
 *   3.3V -- 3.3V
 *    GND -- GND
 *    SCL -- A5 (SCL), with pull-up resistor (e.g. 10k to 3.3V)
 *    SDA -- A4 (SDA), with pull-up resistor (e.g. 10k to 3.3V)
 * M_DRDY -- D* (optional, any pin, ideally interrupt-capable; set below)
 * A_INT1 -- none (not used by compass)
 *  G_INT -- none (not used by compass)
 */

#include <HMC5883L.h>
#include <SSD1306.h>
#include <Bitmask18.h>
#include <Font.h>
#include <FontVariable.h>
#include <FontRenderer.h>
#include <ArduinoPin.h>
#include <VoidPin.h>
#include <ArduinoSPI.h>
#include <Wire.h>

#define MAGNETOMETER_DRDY VoidPin()

#define OLED_PIN_CS  FixedArduinoPin<10>()
#define OLED_PIN_RST FixedArduinoPin<7>()
#define OLED_PIN_DC  FixedArduinoPin<9>()

template <typename Display, typename Bitmask, typename Message>
void message(
	Display &display,
	Bitmask &bitmask,
	Message msg1,
	Message msg2
) {
	Font f(FONTVARIABLE_DATA, FONTVARIABLE_IMG, FONTVARIABLE_MASK);

	bitmask.clear();
	auto r = MakeFontRenderer(&bitmask, 0, 0, bitmask.width(), 0);
	r.print(f, msg1);
	r.print(f, '\n');
	r.print(f, msg2);
	display.send(bitmask);
}

template <typename Display, typename Bitmask, typename Message>
void message(
	Display &display,
	Bitmask &bitmask,
	Message msg
) {
	message(display, bitmask, msg, Message(nullptr));
}

template <typename Bitmask>
void show_reading(
	Bitmask &bitmask,
	int x, int y,
	int w, int h,
	int16_t vx, int16_t vy,
	int16_t scale
) {
	uint8_t shift = 4;
	bitmask.fill_ellipse(x, y, w, h, BlendMode::ON);
	int cx = (x * 2 + w) << (shift - 1);
	int cy = (y * 2 + h) << (shift - 1);
	bitmask.line(
		cx,
		cy,
		cx + (int32_t(vx * w) << (shift - 1)) / scale,
		cy + (int32_t(vy * h) << (shift - 1)) / scale,
		BlendMode::OFF,
		PATTERN_ON,
		shift
	);
}

template <typename Compass, typename Display>
void demoCompass(
	Compass &compass,
	Display &display
) {
	Bitmask18<display.width(),display.height()> bitmask;

	auto status = compass.connection_status();
	if(status != Compass::ConnectionStatus::CONNECTED) {
		ProgMem<char> detail;
		switch(status) {
		case Compass::ConnectionStatus::CONNECTED:
			break;
		case Compass::ConnectionStatus::REQUEST_ERR_DATA_TOO_LONG:
			detail = ProgMemString("REQUEST_ERR_DATA_TOO_LONG");
			break;
		case Compass::ConnectionStatus::REQUEST_ERR_NACK_ADDR:
			detail = ProgMemString("REQUEST_ERR_NACK_ADDR");
			break;
		case Compass::ConnectionStatus::REQUEST_ERR_NACK_DATA:
			detail = ProgMemString("REQUEST_ERR_NACK_DATA");
			break;
		case Compass::ConnectionStatus::REQUEST_ERR_OTHER:
			detail = ProgMemString("REQUEST_ERR_OTHER");
			break;
		case Compass::ConnectionStatus::READ_TIMEOUT:
			detail = ProgMemString("READ_TIMEOUT");
			break;
		case Compass::ConnectionStatus::ID_MISMATCH:
			detail = ProgMemString("ID_MISMATCH");
			break;
		}
		message(display, bitmask, ProgMemString(
			"No compass (HMC5883L) found on I2C!"
		), detail);
		delay(3000);
		return;
	}

	message(display, bitmask, ProgMemString("Running compass self-test..."));

	if(compass.test()) {
		message(display, bitmask, ProgMemString("Self-test passed :D"));
	} else {
		message(display, bitmask, ProgMemString("Self-test failed :("));
	}

	delay(2000);

	compass.set_averaging(Compass::Averaging::A4);
	compass.set_gain(Compass::Gain::G1370);
	compass.read_continuous_async(Compass::Rate::R75_HZ);

	uint16_t t0 = millis();
	while(uint16_t(millis() - t0) < 20000) {
		auto reading = compass.next_reading();
		if(reading.is_saturated()) {
			message(display, bitmask, ProgMemString("SATURATED!"));
		} else if(!reading.is_valid()) {
			message(display, bitmask, ProgMemString("NO DATA!"));
		} else {
			// Rotate according to angle from chip to display
			// (this should be changed to match the circuit. For non-trivial
			// rotations, consider using quaternions from the Geometry lib)
			int16_t x = -reading.y();
			int16_t y = reading.z();
//			int16_t z = reading.x();
			int16_t d = sqrt(x * int32_t(x) + y * int32_t(y));

			bitmask.clear();
			show_reading(bitmask, 33, 1, 61, 61, x, y, d);
			display.send(bitmask);
		}
	}

	compass.stop();
}

void setup(void) {
	Wire.begin();

	auto compass = MakeHMC5883L(MAGNETOMETER_DRDY);

	auto oled = MakeSSD1306<128,64>(
		ArduinoSPI(),
		OLED_PIN_CS,
		OLED_PIN_RST,
		OLED_PIN_DC
	);
	oled.set_on(true);

	while(true) {
		demoCompass(compass, oled);
	}
}

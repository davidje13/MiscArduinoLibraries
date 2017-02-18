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
 * M_DRDY -- none (not used by accelerometer)
 * A_INT1 -- D* (optional, any pin, ideally interrupt-capable; set below)
 *  G_INT -- none (not used by accelerometer)
 */

#define ACCELEROMETER_INT1 ADXL345::NO_PIN
#define ACCELEROMETER_INT2 ADXL345::NO_PIN
#define ACCELEROMETER_VOLTAGE ADXL345::Voltage::V3_3

#define OLED_PIN_CS  10
#define OLED_PIN_RST 7
#define OLED_PIN_DC  9

#include <ADXL345.h>
#include <SSD1306.h>
#include <Bitmask18.h>
#include <Font.h>
#include <FontFixed.h>
#include <FontRenderer.h>
#include <Wire.h>

template <typename Display, typename Bitmask, typename Message>
void message(
	Display &display,
	Bitmask &bitmask,
	Message msg1,
	Message msg2
) {
	Font f(FONTFIXED_DATA, FONTFIXED_IMG, FONTFIXED_MASK);

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

template <typename Display>
void demoAccelerometer(
	ADXL345 &accelerometer,
	Display &display
) {
	Bitmask18<display.width(),display.height()> bitmask;

	Font f(FONTFIXED_DATA, FONTFIXED_IMG, FONTFIXED_MASK);

	ADXL345::ConnectionStatus status = accelerometer.connection_status();
	if(status != ADXL345::ConnectionStatus::CONNECTED) {
		ProgMem<char> detail;
		switch(status) {
		case ADXL345::ConnectionStatus::CONNECTED:
			break;
		case ADXL345::ConnectionStatus::REQUEST_ERR_DATA_TOO_LONG:
			detail = ProgMemString("REQUEST_ERR_DATA_TOO_LONG");
			break;
		case ADXL345::ConnectionStatus::REQUEST_ERR_NACK_ADDR:
			detail = ProgMemString("REQUEST_ERR_NACK_ADDR");
			break;
		case ADXL345::ConnectionStatus::REQUEST_ERR_NACK_DATA:
			detail = ProgMemString("REQUEST_ERR_NACK_DATA");
			break;
		case ADXL345::ConnectionStatus::REQUEST_ERR_OTHER:
			detail = ProgMemString("REQUEST_ERR_OTHER");
			break;
		case ADXL345::ConnectionStatus::READ_TIMEOUT:
			detail = ProgMemString("READ_TIMEOUT");
			break;
		case ADXL345::ConnectionStatus::ID_MISMATCH:
			detail = ProgMemString("ID_MISMATCH");
			break;
		}
		message(display, bitmask, ProgMemString(
			"No accelerometer (ADXL345) found on I2C!"
		), detail);
		delay(3000);
		return;
	}

	accelerometer.configure_fifo_bypass();
	accelerometer.configure_double_tap_detection(
		3000,   // 3g    (tap threshold)
		50000,  // 50ms  (max tap duration)
		100000, // 100ms (min gap between taps in double-tap)
		500000  // 500ms (max gap between taps in double-tap)
	);
	accelerometer.set_on(true);

	message(display, bitmask, ProgMemString(
		"Running accelerometer self-test..."
	));

	if(accelerometer.test(ACCELEROMETER_VOLTAGE)) {
		message(display, bitmask, ProgMemString("Self-test passed :D"));
	} else {
		message(display, bitmask, ProgMemString("Self-test failed :("));
	}

	delay(2000);

	uint16_t t0 = millis();
	while(uint16_t(millis() - t0) < 20000) {
		auto reading = accelerometer.next_reading();
		if(!reading.is_valid()) {
			message(display, bitmask, ProgMemString("NO DATA!"));
		} else {
			// Rotate according to angle from chip to display
			// (this should be changed to match the circuit. For non-trivial
			// rotations, consider using quaternions from the Geometry lib)
			int16_t x = -reading.y_millig();
			int16_t y = reading.z_millig();
//			int16_t z = reading.x_millig();

			bitmask.clear();
			show_reading(bitmask, 33, 1, 61, 61, x / 10, y / 10, 100);
			if(accelerometer.has_detected_tap()) {
				bitmask.fill_rect(120, 0, 8, 8);
			}
			if(accelerometer.has_detected_double_tap()) {
				bitmask.fill_rect(120, 8, 8, 8);
			}
			display.send(bitmask);
		}
	}
}

void setup(void) {
	Wire.begin();

	ADXL345 accelerometer(true, ACCELEROMETER_INT1, ACCELEROMETER_INT2);

	SSD1306<> oled(OLED_PIN_CS, OLED_PIN_RST, OLED_PIN_DC);
	oled.set_on(true);

	while(true) {
		demoAccelerometer(accelerometer, oled);
	}
}

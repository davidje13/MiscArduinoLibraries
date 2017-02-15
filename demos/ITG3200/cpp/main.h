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
 * M_DRDY -- none (not used by gyroscope)
 * A_INT1 -- none (not used by gyroscope)
 *  G_INT -- D* (optional, any pin, ideally interrupt-capable; set below)
 */

#define GYROSCOPE_INT ITG3200::NO_PIN

#define OLED_PIN_CS  10
#define OLED_PIN_RST 7
#define OLED_PIN_DC  9

#include <ITG3200.h>
#include <GyroAccumulator.h>
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
void demoGyroscope(
	ITG3200 &gyroscope,
	Display &display
) {
	Bitmask18<display.width(),display.height()> bitmask;

	Font f(FONTFIXED_DATA, FONTFIXED_IMG, FONTFIXED_MASK);

	gyroscope.awake();

	ITG3200::ConnectionStatus status = gyroscope.connection_status();
	if(status != ITG3200::ConnectionStatus::CONNECTED) {
		ProgMem<char> detail;
		switch(status) {
		case ITG3200::ConnectionStatus::CONNECTED:
			break;
		case ITG3200::ConnectionStatus::REQUEST_ERR_DATA_TOO_LONG:
			detail = ProgMemString("REQUEST_ERR_DATA_TOO_LONG");
			break;
		case ITG3200::ConnectionStatus::REQUEST_ERR_NACK_ADDR:
			detail = ProgMemString("REQUEST_ERR_NACK_ADDR");
			break;
		case ITG3200::ConnectionStatus::REQUEST_ERR_NACK_DATA:
			detail = ProgMemString("REQUEST_ERR_NACK_DATA");
			break;
		case ITG3200::ConnectionStatus::REQUEST_ERR_OTHER:
			detail = ProgMemString("REQUEST_ERR_OTHER");
			break;
		case ITG3200::ConnectionStatus::READ_TIMEOUT:
			detail = ProgMemString("READ_TIMEOUT");
			break;
		case ITG3200::ConnectionStatus::ID_MISMATCH:
			detail = ProgMemString("ID_MISMATCH");
			break;
		}
		message(display, bitmask, ProgMemString(
			"No gyroscope (ITG3200) found on I2C!"
		), detail);
		delay(3000);
		return;
	}

	// Filter out low frequency noise. Also sets base frequency to 1kHz
	gyroscope.set_filter_bandwidth(ITG3200::LowPassBandwidth::L188_HZ);

	// No point sampling too fast; takes 10ms to update screen; divide the
	// 1kHz above by 16 to sample ~every 60ms
	gyroscope.set_sample_rate_divider(16);

	// More reliable clock source according to data sheet
	gyroscope.set_clock_source_sync(ITG3200::ClockSource::X_GYRO);

	GyroAccumulator<ITG3200::reading> accumulator;

	message(display, bitmask, ProgMemString("Finding zero point..."));
	accumulator.begin_calibration();
	for(int n = 0; n < 128; ++ n) {
		auto reading = gyroscope.next_reading();
		if(reading.is_valid()) {
			accumulator.accumulate(reading);
		}
	}
	accumulator.complete_calibration();

	uint16_t t0 = millis();
	bool frameDot = false;
	while(uint16_t(millis() - t0) < 20000) {
		auto reading = gyroscope.next_reading();
		if(!reading.is_valid()) {
			message(display, bitmask, ProgMemString("NO DATA!"));
		} else {
			accumulator.accumulate(reading);

			bitmask.clear();
			frameDot = !frameDot;
			if(frameDot) {
				bitmask.fill_rect(120, 0, 8, 8);
			}

			show_reading(bitmask,  1, 17, 31, 31,
				sin(accumulator.x_millideg() * PI * 2.0 / 360000.0) * 1000,
				cos(accumulator.x_millideg() * PI * 2.0 / 360000.0) * 1000,
				1000
			);
			show_reading(bitmask, 33, 17, 31, 31,
				sin(accumulator.y_millideg() * PI * 2.0 / 360000.0) * 1000,
				cos(accumulator.y_millideg() * PI * 2.0 / 360000.0) * 1000,
				1000
			);
			show_reading(bitmask, 65, 17, 31, 31,
				sin(accumulator.z_millideg() * PI * 2.0 / 360000.0) * 1000,
				cos(accumulator.z_millideg() * PI * 2.0 / 360000.0) * 1000,
				1000
			);
			auto r = MakeFontRenderer(&bitmask, 97, 54, 31, 0);
			r.print_number(f, reading.temperature_millicelsius() * 0.001, 1, 2);
			r.print(f, 'C');
			display.send(bitmask);
		}
	}

	gyroscope.sleep();
}

void setup(void) {
	Wire.begin();

	ITG3200 gyroscope(false, GYROSCOPE_INT);

	SSD1306<> oled(OLED_PIN_CS, OLED_PIN_RST, OLED_PIN_DC);
	oled.set_on(true);

	while(true) {
		demoGyroscope(gyroscope, oled);
	}
}

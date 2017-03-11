/*
 * GyroAccumulator - Accumulator for readings from a gyroscope
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

#ifndef GYROACCUMULATOR_H_INCLUDED
#define GYROACCUMULATOR_H_INCLUDED

#include "ext.h"

template <typename Reading>
class GyroAccumulator {
	int32_t zx;
	int32_t zy;
	int32_t zz;
	int32_t ex;
	int32_t ey;
	int32_t ez;
	int32_t x; // TODO: use Quaternion to store current angle to allow proper relative angles
	int32_t y;
	int32_t z;
	uint16_t counter;
	bool calibrating;

public:
	GyroAccumulator(void)
		: zx(0)
		, zy(0)
		, zz(0)
		, ex(0)
		, ey(0)
		, ez(0)
		, x(0)
		, y(0)
		, z(0)
		, counter(0)
		, calibrating(false)
	{}

	void reset_timer(void) {
		if(!calibrating) {
			counter = millis();
		}
	}

	void reset_rotation(void) {
		x = 0;
		y = 0;
		z = 0;
	}

	void begin_calibration(void) {
		zx = 0;
		zy = 0;
		zz = 0;
		counter = 0;
		calibrating = true;
	}

	void complete_calibration(void) {
		if(calibrating && counter > 0) {
			zx = (zx * Reading::RAW_DIVISOR) / counter;
			zy = (zy * Reading::RAW_DIVISOR) / counter;
			zz = (zz * Reading::RAW_DIVISOR) / counter;
		}
		counter = millis();
		calibrating = false;
	}

	void accumulate(const Reading &reading) {
		if(calibrating) {
			zx += reading.raw_x();
			zy += reading.raw_y();
			zz += reading.raw_z();
			++ counter;
		} else {
			uint16_t now = millis();
			uint16_t elapsed = now - counter;
			counter = now;
			ex += (int32_t(reading.raw_x()) * Reading::RAW_DIVISOR - zx) * elapsed;
			ey += (int32_t(reading.raw_y()) * Reading::RAW_DIVISOR - zy) * elapsed;
			ez += (int32_t(reading.raw_z()) * Reading::RAW_DIVISOR - zz) * elapsed;
			x += ex / Reading::RAW_MULTIPLIER;
			ex %= Reading::RAW_MULTIPLIER;
			y += ey / Reading::RAW_MULTIPLIER;
			ey %= Reading::RAW_MULTIPLIER;
			z += ez / Reading::RAW_MULTIPLIER;
			ez %= Reading::RAW_MULTIPLIER;
			x = ext::posmod(x, 360000);
			y = ext::posmod(y, 360000);
			z = ext::posmod(z, 360000);
		}
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	int32_t x_millideg(void) const {
		return x;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	int32_t y_millideg(void) const {
		return y;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	int32_t z_millideg(void) const {
		return z;
	}
};

#endif

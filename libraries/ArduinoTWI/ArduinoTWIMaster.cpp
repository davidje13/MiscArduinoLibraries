/*
 * ArduinoTWI - Wrapper of standard Arduino TWI / I2C access.
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

#include <Arduino.h>
#include "ArduinoTWIMaster.h"

uint8_t ArduinoTWIMaster::refCount = 0;
uint32_t ArduinoTWIMaster::lastClockHz = 0;

void ArduinoTWIMaster::inc(void) {
	if(refCount == 0) {
		Wire.begin();
	}
	++ refCount;
}

void ArduinoTWIMaster::dec(void) {
	if((-- refCount) == 0) {
		Wire.end();
	}
}

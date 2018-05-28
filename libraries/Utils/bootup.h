/*
 * bootup - Simple helper to wait for devices to start
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

#ifndef BOOTUP_H_INCLUDED
#define BOOTUP_H_INCLUDED

static inline void awaitBootMillis(uint16_t ms) {
	// We ignore rollover here, since this is intended to be used in
	// setup-type code, so it's unlikely we've already been running
	// for 50 days.
	uint32_t tm0 = millis();
	if(tm0 < ms) {
		// Has not been running long enough yet
		delay(ms - tm0);
	}
}

#endif

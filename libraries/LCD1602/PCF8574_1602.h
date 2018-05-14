/*
 * PCF8574 wrapper to 1602
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

#ifndef PCF8574_1602_H_INCLUDED
#define PCF8574_1602_H_INCLUDED

#include "1602.h"
#include <PCF8574.h>

template <
	uint8_t columns,
	uint8_t lines,
	typename TwiT,
	typename IntPinT
>
[[gnu::always_inline]]
inline auto Make1602(
	PCF8574_impl<TwiT, IntPinT> &wrapper
) -> decltype(Make1602<columns, lines>(
	wrapper.template pinBank<4, 4>(),
	wrapper.template pin<0>(),
	wrapper.template pin<1>(),
	wrapper.template pin<2>()
)) {
	auto backlight = wrapper.template pin<3>();
	backlight.set_output();
	backlight.high();

	return Make1602<columns, lines>(
		wrapper.template pinBank<4, 4>(),
		wrapper.template pin<0>(), // rs
		wrapper.template pin<1>(), // rw
		wrapper.template pin<2>()  // en
	);
}

#endif

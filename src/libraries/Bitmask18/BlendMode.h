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

#ifndef BLENDMODE_H_INCLUDED
#define BLENDMODE_H_INCLUDED

enum class BlendMode : uint8_t {
	XOR = 0x0,
	NOP = 0x1,
	ON  = 0x2,
	OFF = 0x3
};

#endif

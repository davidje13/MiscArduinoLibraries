/*
 * Fast integer arithmetic library
 *
 * @license MIT
 */

/*
 * The sin8/cos8/sin16/cos16 functions in this file are taken from
 * https://github.com/FastLED/FastLED by Daniel Garcia / Mark Kriegsman
 * (with minor modifications)
 * See the license file for full details.
 */

#include "FastMath.h"

#if defined(__AVR__)
#  include <avr/pgmspace.h>
#else
#  define PROGMEM
#  define pgm_read_byte_near(x) (*static_cast<const uint8_t*>(x))
#  define pgm_read_word_near(x) (*static_cast<const uint16_t*>(x))
#endif

[[gnu::const,nodiscard]]
int16_t sin16(uint16_t theta) {
	static PROGMEM const uint16_t base[] = {
		0, 6393, 12539, 18204, 23170, 27245, 30273, 32137
	};
	static PROGMEM const uint8_t slope[] = {
		49, 48, 44, 38, 31, 23, 14, 4
	};

	uint16_t offset = (theta & 0x3FFF);

#if defined(__AVR__)
	// AVR doesn't have a multi-bit shift instruction,
	// so if we say "offset >>= 3", gcc makes a tiny loop.
	// Inserting empty volatile statements between each
	// bit shift forces gcc to unroll the loop.
	offset >>= 1; // 0..8191
	asm volatile("");
	offset >>= 1; // 0..4095
	asm volatile("");
	offset >>= 1; // 0..2047
#else
	offset >>= 3;
#endif

	if(theta & 0x4000) {
		offset = 2047 - offset;
	}

	uint8_t section = offset / 256; // 0..7
	uint16_t b = pgm_read_word_near(base + section);
	uint8_t m = pgm_read_byte_near(slope + section);

	uint16_t mx = m * (uint8_t(offset) >> 1);

	mx += b;
	if(theta & 0x8000) {
		return -mx;
	} else {
		return mx;
	}
}

[[gnu::const,nodiscard]]
int8_t sin8(uint8_t theta) {
	static PROGMEM const uint8_t data[] = {0, 49, 49, 41, 90, 27, 117, 10};

	uint8_t offset = theta;

#if defined(__AVR__) && !defined(LIB8_ATTINY)
	asm volatile(
		"sbrc %[theta],6 \n\t"
		"com  %[offset]  \n\t"
		: [theta] "+r" (theta)
		, [offset] "+r" (offset)
	);
#else
	if(theta & 0x40) {
		offset = uint8_t(255) - offset;
	}
#endif

	offset &= 0x3F; // 0..63

	uint8_t secoffset = offset & 0x0F; // 0..15
	if(theta & 0x40) {
		++ secoffset;
	}

	uint8_t section = offset >> 4; // 0..3
	const uint8_t *p = data + (section << 1);
	uint8_t b = pgm_read_byte_near(p);
	uint8_t m16 = pgm_read_byte_near(p + 1);

#if defined(__AVR__) && !defined(LIB8_ATTINY)
	uint8_t mx;
	uint8_t xr1;
	asm volatile(
		"mul %[m16],%[secoffset]   \n\t"
		"mov %[mx],r0              \n\t"
		"mov %[xr1],r1             \n\t"
		"eor  r1, r1               \n\t"
		"swap %[mx]                \n\t"
		"andi %[mx],0x0F           \n\t"
		"swap %[xr1]               \n\t"
		"andi %[xr1], 0xF0         \n\t"
		"or   %[mx], %[xr1]        \n\t"
		: [mx] "=d" (mx)
		, [xr1] "=d" (xr1)
		: [m16] "d" (m16)
		, [secoffset] "d" (secoffset)
	);
#else
	uint8_t mx = (m16 * secoffset) >> 4;
#endif

	mx += b;
	if(theta & 0x80) {
		return -mx;
	} else {
		return mx;
	}
}

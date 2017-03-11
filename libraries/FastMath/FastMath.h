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

#ifndef FASTMATH_H_INCLUDED
#define FASTMATH_H_INCLUDED

#ifdef ARDUINO
#  include <arduino.h>
#else
#  include <cstdint>
#endif

#include "ext.h"

/**
 * Fast 16-bit approximation of sin(x). This approximation never varies more
 * than 0.69% from the correct value
 *
 *     sin16(angle) ~= sin(angle * 2 * PI / 65536) * 32767
 *
 * @param theta input angle from 0-65535
 * @returns sin of theta, value between -32767 to 32767.
 */
[[gnu::const,nodiscard]]
int16_t sin16(uint16_t theta);

/**
 * Fast 8-bit approximation of sin(x). This approximation never varies more
 * than 2% from the correct value
 *
 *     sin8(angle) ~= sin(angle * 2 * PI / 256) * 128
 *
 * @param theta input angle from 0-255
 * @returns sin of theta, value between 0 to 255.
 */
[[gnu::const,nodiscard]]
int8_t sin8(uint8_t theta);

[[gnu::const,nodiscard,gnu::always_inline]]
static inline int16_t cos16(uint16_t theta) {
	return sin16(theta + 16384);
}

[[gnu::const,nodiscard,gnu::always_inline]]
static inline int8_t cos8(uint8_t theta) {
	return sin8(theta + 64);
}

[[gnu::const,nodiscard,gnu::always_inline]]
static inline int16_t sin8_fine(uint8_t theta) {
	return sin16(theta << 8) / 256;
}

[[gnu::const,nodiscard,gnu::always_inline]]
static inline int16_t cos8_fine(uint8_t theta) {
	return cos16(theta << 8) / 256;
}

[[gnu::const,nodiscard,gnu::always_inline]]
static inline int16_t sin8_mult_shift(uint8_t theta, uint8_t radius, uint8_t shift) {
	return (radius * sin8_fine(theta)) / (128 >> shift);
}

[[gnu::const,nodiscard,gnu::always_inline]]
static inline int16_t cos8_mult_shift(uint8_t theta, uint8_t radius, uint8_t shift) {
	return (radius * cos8_fine(theta)) / (128 >> shift);
}

[[gnu::const,nodiscard,gnu::always_inline]]
static inline int16_t sin16_mult_shift(uint16_t theta, uint16_t radius, uint8_t shift) {
	return (radius * uint32_t(sin16(theta))) / (32768 >> shift);
}

[[gnu::const,nodiscard,gnu::always_inline]]
static inline int16_t cos16_mult_shift(uint16_t theta, uint16_t radius, uint8_t shift) {
	return (radius * uint32_t(cos16(theta))) / (32768 >> shift);
}

#endif

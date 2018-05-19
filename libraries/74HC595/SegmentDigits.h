/*
 * SegmentDigits - Helper class for working with digits in a segment display.
 * Written in 2018 by David Evans
 *
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef SEGMENT_DIGITS_H_INCLUDED
#define SEGMENT_DIGITS_H_INCLUDED

#ifdef ARDUINO
#  include <ProgMem.h>
#else
#  include <ProgMem/ProgMem.h>
#endif

#include "ext.h"

template <typename Target>
class SegmentDigits {
public:
	static const uint8_t CHARS[0x60];

private:
	[[gnu::const,nodiscard,gnu::always_inline]]
	constexpr inline uint8_t blank(void) const {
		return CHARS[0];
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	constexpr inline uint8_t _chr(char c) const {
		return CHARS[c - 0x20];
	}

public:
	[[gnu::const,nodiscard,gnu::always_inline]]
	constexpr inline uint8_t chr(char c) const {
		return (c < 0x20 || c > 0x7F) ? blank() : _chr(c);
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	constexpr inline uint8_t digit(uint8_t v) const {
		return (
			(v < 10) ? _chr(v + '0') :
			(v < 36) ? _chr((v - 10) + 'a') :
			(v < 62) ? _chr((v - 36) + 'A') :
			blank()
		);
	}

private:
	template <typename String>
	uint8_t _print(String message, uint8_t *target, uint8_t cap) const {
		uint8_t pos = 0;
		uint8_t dot = _chr('.');
		for(uint8_t i = 0; i < cap; ++ i) {
			char c = message[pos];
			if(c == '\0') {
				target[i] = blank();
				continue;
			}
			uint8_t m = chr(c);
			++ pos;
			// collapse following . into previous character if possible
			if((m & dot) == 0 && message[pos] == '.') {
				m |= dot;
				++ pos;
			}
			target[i] = m;
		}
		return pos;
	}

public:
	[[gnu::always_inline]]
	inline uint8_t print(
		const char *message,
		uint8_t *target,
		uint8_t cap
	) const {
		return _print(message, target, cap);
	}

	[[gnu::always_inline]]
	inline uint8_t print(
		ProgMem<char> message,
		uint8_t *target,
		uint8_t cap
	) const {
		return _print(message, target, cap);
	}

	template <int cap>
	[[gnu::always_inline]]
	inline uint8_t print(const char *message, uint8_t (&target)[cap]) const {
		return _print(message, target, cap);
	}

	template <int cap>
	[[gnu::always_inline]]
	inline uint8_t print(ProgMem<char> message, uint8_t (&target)[cap]) const {
		return _print(message, target, cap);
	}

	[[nodiscard]]
	uint8_t advance(const char *(&message)) const {
		uint8_t result;
		uint8_t skip = _print(message, &result, 1);
		message += skip;
		return skip;
	}

	[[nodiscard]]
	uint8_t advance(ProgMem<char> &message) const {
		uint8_t result;
		uint8_t skip = _print(message, &result, 1);
		message += skip;
		return skip;
	}

	[[gnu::pure,nodiscard]]
	uint8_t chr(const char *message) const {
		uint8_t result;
		_print(message, &result, 1);
		return result;
	}

	[[gnu::pure,nodiscard]]
	uint8_t chr(ProgMem<char> message) const {
		uint8_t result;
		_print(message, &result, 1);
		return result;
	}

	template <int base = 10, typename T>
	void print_int(T value, uint8_t *target, uint8_t cap) const {
		bool neg = value < 0;
		if(neg) {
			value *= -1;
		}
		uint8_t i = cap;
		while((i --) > 0) {
			target[i] = digit(uint8_t(value % base));
			value /= base;
			if(value == 0) {
				break;
			}
		}
		if(neg) {
			if(i > 0) {
				-- i;
			}
			target[i] = _chr('-');
		}
		while((i --) > 0) {
			target[i] = blank();
		}
	}

	template <int base = 10, typename T, int cap>
	[[gnu::always_inline]]
	inline void print_int(T value, uint8_t (&target)[cap]) const {
		print_int<base, T>(value, target, cap);
	}
};

template <typename Target>
const uint8_t SegmentDigits<Target>::CHARS[] = {
	Target::build_segment(0, 0, 0, 0, 0, 0, 0), // ' '
	Target::build_segment(0, 0, 1, 0, 0, 0, 0, 1), // !
	Target::build_segment(0, 1, 1, 0, 0, 0, 0), // "
	Target::build_segment(0, 1, 1, 1, 1, 1, 1), // #
	Target::build_segment(1, 1, 0, 1, 1, 0, 1), // $ (pound / euro) - same as E
	Target::build_segment(1, 1, 0, 0, 0, 1, 1), // %
	Target::build_segment(0, 0, 0, 0, 0, 0, 0), // & - not supported
	Target::build_segment(0, 1, 0, 0, 0, 0, 0), // '
	Target::build_segment(1, 1, 0, 0, 1, 0, 1), // ( - same as [ / C
	Target::build_segment(1, 0, 1, 0, 0, 1, 1), // ) - same as ]
	Target::build_segment(1, 1, 1, 1, 0, 0, 0), // *
	Target::build_segment(0, 0, 0, 0, 0, 0, 0), // + - not supported
	Target::build_segment(0, 0, 0, 0, 0, 0, 0, 1), // , - same as .
	Target::build_segment(0, 0, 0, 1, 0, 0, 0), // -
	Target::build_segment(0, 0, 0, 0, 0, 0, 0, 1), // .
	Target::build_segment(1, 1, 0, 0, 1, 0, 0), // / (forward slash)

	// 0-9
	Target::build_segment(1, 1, 1, 0, 1, 1, 1), // 0
	Target::build_segment(0, 0, 1, 0, 0, 1, 0), // 1
	Target::build_segment(1, 0, 1, 1, 1, 0, 1), // 2
	Target::build_segment(1, 0, 1, 1, 0, 1, 1), // 3
	Target::build_segment(0, 1, 1, 1, 0, 1, 0), // 4
	Target::build_segment(1, 1, 0, 1, 0, 1, 1), // 5
	Target::build_segment(1, 1, 0, 1, 1, 1, 1), // 6
	Target::build_segment(1, 0, 1, 0, 0, 1, 0), // 7
	Target::build_segment(1, 1, 1, 1, 1, 1, 1), // 8
	Target::build_segment(1, 1, 1, 1, 0, 1, 1), // 9

	Target::build_segment(0, 0, 0, 0, 0, 0, 0), // : - not supported
	Target::build_segment(0, 0, 0, 0, 0, 0, 0), // ; - not supported
	Target::build_segment(0, 0, 0, 1, 1, 0, 1), // < - same as c
	Target::build_segment(0, 0, 0, 1, 0, 0, 1), // =
	Target::build_segment(0, 0, 0, 1, 0, 1, 1), // >
	Target::build_segment(1, 0, 1, 1, 1, 0, 1), // ?
	Target::build_segment(1, 1, 1, 0, 1, 0, 1), // @

	// A-Z
	Target::build_segment(1, 1, 1, 1, 1, 1, 0), // A
	Target::build_segment(0, 1, 0, 1, 1, 1, 1), // B (b)
	Target::build_segment(1, 1, 0, 0, 1, 0, 1), // C
	Target::build_segment(0, 0, 1, 1, 1, 1, 1), // D (d)
	Target::build_segment(1, 1, 0, 1, 1, 0, 1), // E
	Target::build_segment(1, 1, 0, 1, 1, 0, 0), // F
	Target::build_segment(1, 1, 0, 0, 1, 1, 1), // G
	Target::build_segment(0, 1, 1, 1, 1, 1, 0), // H
	Target::build_segment(0, 0, 0, 0, 0, 1, 0), // I (i)
	Target::build_segment(0, 0, 1, 0, 0, 1, 1), // J
	Target::build_segment(0, 1, 1, 1, 1, 1, 0), // K - same as H
	Target::build_segment(0, 1, 0, 0, 1, 0, 1), // L
	Target::build_segment(0, 0, 0, 1, 1, 1, 0), // M - same as n
	Target::build_segment(0, 0, 0, 1, 1, 1, 0), // N (n)
	Target::build_segment(1, 1, 1, 0, 1, 1, 1), // O - same as 0
	Target::build_segment(1, 1, 1, 1, 1, 0, 0), // P
	Target::build_segment(1, 1, 1, 1, 0, 1, 0), // Q (q)
	Target::build_segment(0, 0, 0, 1, 1, 0, 0), // R (r)
	Target::build_segment(1, 1, 0, 1, 0, 1, 1), // S - same as 5
	Target::build_segment(0, 1, 0, 1, 1, 0, 1), // T (t)
	Target::build_segment(0, 1, 1, 0, 1, 1, 1), // U
	Target::build_segment(0, 1, 1, 0, 1, 1, 1), // V - same as U
	Target::build_segment(0, 1, 1, 0, 1, 1, 1), // W - same as U
	Target::build_segment(0, 1, 1, 1, 1, 1, 0), // X - same as H
	Target::build_segment(0, 1, 1, 1, 0, 1, 1), // Y (y)
	Target::build_segment(1, 0, 1, 1, 1, 0, 1), // Z - same as 2

	Target::build_segment(1, 1, 0, 0, 1, 0, 1), // [ - same as C
	Target::build_segment(1, 0, 1, 0, 0, 1, 0), // \ (backslash)
	Target::build_segment(1, 0, 1, 0, 0, 1, 1), // ]
	Target::build_segment(1, 1, 1, 0, 0, 0, 0), // ^
	Target::build_segment(0, 0, 0, 0, 0, 0, 1), // _
	Target::build_segment(0, 0, 1, 0, 0, 0, 0), // `

	// a-z
	Target::build_segment(1, 1, 1, 1, 1, 1, 0), // a (A)
	Target::build_segment(0, 1, 0, 1, 1, 1, 1), // b
	Target::build_segment(0, 0, 0, 1, 1, 0, 1), // c
	Target::build_segment(0, 0, 1, 1, 1, 1, 1), // d
	Target::build_segment(1, 1, 0, 1, 1, 0, 1), // e (E)
	Target::build_segment(1, 1, 0, 1, 1, 0, 0), // f (F)
	Target::build_segment(1, 1, 1, 1, 0, 1, 1), // g
	Target::build_segment(0, 1, 0, 1, 1, 1, 0), // h
	Target::build_segment(0, 0, 0, 0, 0, 1, 0), // i
	Target::build_segment(0, 0, 1, 0, 0, 1, 1), // j (J)
	Target::build_segment(0, 1, 1, 1, 1, 1, 0), // k (K) - same as H
	Target::build_segment(0, 1, 0, 0, 1, 0, 1), // l (L)
	Target::build_segment(0, 0, 0, 1, 1, 1, 0), // m (M) - same as n
	Target::build_segment(0, 0, 0, 1, 1, 1, 0), // n
	Target::build_segment(0, 0, 0, 1, 1, 1, 1), // o
	Target::build_segment(1, 1, 1, 1, 1, 0, 0), // p (P)
	Target::build_segment(1, 1, 1, 1, 0, 1, 0), // q
	Target::build_segment(0, 0, 0, 1, 1, 0, 0), // r
	Target::build_segment(1, 1, 0, 1, 0, 1, 1), // s (S) - same as 5
	Target::build_segment(0, 1, 0, 1, 1, 0, 1), // t
	Target::build_segment(0, 0, 0, 0, 1, 1, 1), // u
	Target::build_segment(0, 0, 0, 0, 1, 1, 1), // v - same as u
	Target::build_segment(0, 0, 0, 0, 1, 1, 1), // w - same as u
	Target::build_segment(0, 1, 1, 1, 1, 1, 0), // x (X) - same as H
	Target::build_segment(0, 1, 1, 1, 0, 1, 1), // y (Y)
	Target::build_segment(1, 0, 1, 1, 1, 0, 1), // z (Z) - same as 2

	Target::build_segment(1, 1, 0, 0, 1, 0, 1), // { - same as [ / C
	Target::build_segment(0, 1, 0, 0, 1, 0, 0), // |
	Target::build_segment(1, 0, 1, 0, 0, 1, 1), // } - same as ]
	Target::build_segment(0, 0, 0, 1, 0, 0, 0), // ~ - same as -

	Target::build_segment(1, 1, 1, 1, 1, 1, 1, 1), // DEL
};

template <typename Target>
[[gnu::always_inline]]
inline SegmentDigits<Target> MakeSegmentDigits(const Target&) {
	return SegmentDigits<Target>();
}

#endif

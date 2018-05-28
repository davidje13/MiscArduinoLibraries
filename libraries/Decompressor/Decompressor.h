/*
 * Decompressor - Minimal RAM implementation of an INFLATE-like algorithm.
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

#ifndef DECOMPRESSOR_H_INCLUDED
#define DECOMPRESSOR_H_INCLUDED

#include "ext.h"

typedef uint8_t windowsz_t;

template <typename BaseT, windowsz_t windowSize>
class Decompressor {
	enum Mode : uint8_t {
		LEN_DIST,
		DIST_LEN,
		SPLIT
	};

	typedef uint16_t bits_t;
	static constexpr uint8_t maxBits = 15;
	static constexpr windowsz_t zeroDist = 1;
	static constexpr uint16_t zeroLen = 2;

	BaseT baseData;
	uint16_t currentDataPos;
	uint16_t currentStreamBit;
	uint16_t blockLen;
	windowsz_t blockDist;
	windowsz_t currentWindowPos;
	bits_t beginnings[maxBits - 1];
	uint8_t window[windowSize];

	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline uint8_t read_bit(uint16_t bit) const {
		return (baseData[bit >> 3] >> (7 - (bit & 7))) & 1;
	}

	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline uint8_t read_bits_8(uint16_t bit, uint8_t length) const {
		uint16_t p = bit >> 3;
		uint8_t b = bit & 7;
		uint8_t r;
		if(b + length > 8) {
			r = (
				uint8_t(baseData[p] << (b + length - 8)) |
				uint8_t(baseData[p + 1] >> (16 - b - length))
			);
		} else {
			r = uint8_t(baseData[p] >> (8 - b - length));
		}
		return r & ((uint8_t(1) << length) - 1);
	}

	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline uint16_t read_bits_16(uint16_t bit, uint8_t length) const {
		uint16_t p = bit >> 3;
		uint8_t b = bit & 7;
		uint8_t r;
		if(b + length > 16) {
			r = (
				uint8_t(baseData[p] << (b + length - 8)) |
				uint8_t(baseData[p + 1] << (b + length - 16)) |
				uint8_t(baseData[p + 2] >> (24 - b - length))
			);
		} else {
			r = (
				uint8_t(baseData[p] << (b + length - 8)) |
				uint8_t(baseData[p + 1] >> (16 - b - length))
			);
		}
		return r & ((uint16_t(1) << length) - 1);
	}

	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline uint8_t symbol_size(uint16_t v) const {
//		return read_bits_8(12 + v * 4, 4);

		// inlined version for speed
		uint16_t p = ((v + 3) >> 1);
		if(v & 1) {
			return baseData[p] >> 4;
		} else {
			return baseData[p] & 0xF;
		}
	}

	[[gnu::pure,nodiscard]]
	bool is_known(bits_t value, uint8_t len) const {
		if(len == maxBits) {
			// We assume input data is non-malicious, so if we get this
			// far it must be OK (saves us storing 2 bytes)
			return true;
		}
		return (
			(len == 1 || value >= beginnings[len - 2]) &&
			value < (beginnings[len - 1] >> 1)
		);
	}

	[[gnu::pure,nodiscard]]
	uint16_t identify(bits_t value, uint8_t len) const {
		bits_t v = (len > 1 ? beginnings[len - 2] : 0);
		for(uint16_t i = 0; ; ++ i) {
			if(symbol_size(i) == len) {
				if(value == v) {
					return i;
				}
				++ v;
			}
		}
		// no need for end condition on loop, as this is only called if
		// is_known returned true, so we are guaranteed to find the token
	}

	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline Mode mode(void) const {
		return Mode(read_bits_8(0, 2));
	}

	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline uint16_t symbol_count(void) const {
		return read_bits_16(2, 10);
	}

	void reset(void) {
		currentDataPos = 0;
		currentStreamBit = 12 + symbol_count() * 4;
		blockLen = 0;
		currentWindowPos = 0;
		for(uint8_t i = 0; i < windowSize; ++ i) {
			window[i] = 0;
		}
	}

	void prepare(void) {
		// bit 0: using split marker?
		// bit 1: using dist-len?
		// bits 2-11 (10-bit): number of symbols
		// bits 12+: 4-bit symbol length (one per symbol)
		uint16_t symbolCount = symbol_count();
		bits_t v = 0;
		for(uint8_t len = 1; len < maxBits; ++ len) {
			for(uint16_t i = 0; i < symbolCount; ++ i) {
				if(symbol_size(i) == len) {
					++ v;
				}
			}
			v <<= 1;
			beginnings[len - 1] = v;
		}
	}

	[[gnu::pure,nodiscard]]
	uint8_t read_window(windowsz_t dist) const {
		return window[(currentWindowPos + windowSize - dist) % windowSize];
	}

	void advance_byte(uint8_t value) {
		window[currentWindowPos] = value;
		currentWindowPos = (currentWindowPos + 1) % windowSize;
		++ currentDataPos;
	}

	uint16_t read_next_symbol(void) {
		bits_t v = 0;
		uint8_t len = 0;
		do {
			v = bits_t(v << 1) | read_bit(currentStreamBit);
			++ len;
			++ currentStreamBit;
		} while(!is_known(v, len));
		return identify(v, len);
	}

	void advance_block(void) {
		-- blockLen;
		advance_byte(read_window(blockDist));
	}

	void advance(void) {
		if(blockLen) {
			advance_block();
			return;
		}
		uint16_t decoded = read_next_symbol();
		switch(mode()) {
		case Mode::LEN_DIST:
			if(decoded >= 256) {
				blockDist = windowsz_t(decoded - 256 + zeroDist);
				blockLen = uint16_t(read_next_symbol() + zeroLen);
				advance_block();
			} else {
				advance_byte(uint8_t(decoded));
			}
			break;
		case Mode::DIST_LEN:
			if(decoded >= 256) {
				blockLen = uint16_t(decoded - 256 + zeroLen);
				blockDist = windowsz_t(read_next_symbol() + zeroDist);
				advance_block();
			} else {
				advance_byte(uint8_t(decoded));
			}
			break;
		case Mode::SPLIT:
			if(decoded == 0) {
				blockDist = windowsz_t(read_next_symbol() + zeroDist);
				blockLen = uint16_t(read_next_symbol() + zeroLen);
				advance_block();
			} else {
				advance_byte(uint8_t(decoded - 1));
			}
			break;
		}
	}

public:
	Decompressor(const BaseT &baseData)
		: baseData(baseData)
	{
		prepare();
		reset();
	}

	Decompressor(BaseT &&baseData)
		: baseData(static_cast<BaseT&&>(baseData))
	{
		prepare();
		reset();
	}

	uint8_t get(int index) {
		if(index < currentDataPos - windowSize) {
			// cannot decode backwards, so must rewind stream
			reset();
		}
		while(index >= currentDataPos) {
			advance();
		}
		return read_window(windowsz_t(currentDataPos - index));
	}
};

template <windowsz_t windowSize, typename BaseT>
[[gnu::always_inline,nodiscard]]
inline Decompressor<BaseT, windowSize> MakeDecompressor(const BaseT &baseData) {
	return Decompressor<BaseT, windowSize>(baseData);
}

template <windowsz_t windowSize, typename BaseT>
[[gnu::always_inline,nodiscard]]
inline Decompressor<BaseT, windowSize> MakeDecompressor(BaseT &&baseData) {
	return Decompressor<BaseT, windowSize>(static_cast<BaseT&&>(baseData));
}

template <windowsz_t windowSize, typename T>
[[gnu::always_inline,nodiscard]]
inline Decompressor<const T*, windowSize> MakeDecompressor(const T *baseData) {
	return Decompressor<const T*, windowSize>(baseData);
}

#endif

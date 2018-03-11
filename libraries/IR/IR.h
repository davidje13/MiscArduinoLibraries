/*
 * IR - Storage and interpretation of IR signals.
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

#ifndef IR_H_INCLUDED
#define IR_H_INCLUDED

#include "ext.h"

#define MAX_LENGTH 79

class IRCommand {
	uint8_t n;
	uint8_t values[MAX_LENGTH];

public:
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint16_t min_header_thresh(void) {
		// Header activations are >2000us, data is usually <800us
		return 1000;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint16_t max_header_thresh(void) {
		// Header activations are < 20000us
		return 20000;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint16_t max_data_thresh(void) {
		// Data can sometimes be up to ~3000us
		return 3500;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint16_t max_silence_thresh(void) {
		// Gaps within command are usually <2000us,
		// but for some devices can be longer
		return 10000;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint16_t bit_thresh(void) {
		// Data is usually encoded as ~600ms vs ~1200ms bands
		// (on either high or low signal)
		return 900;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint16_t pulse_resolution(void) {
		// Pulse lengths are stored in 8-bit integers, up to ~10000us
		return 40;
	}

private:
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t cap(uint16_t v) {
		return (v > 255) ? 255 : uint8_t(v);
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint8_t compress_pulse(uint16_t width) {
		return cap((width + pulse_resolution() / 2) / pulse_resolution());
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline uint16_t decompress_pulse(uint8_t stored) {
		return uint16_t(stored) * pulse_resolution();
	}

public:
	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint16_t duration(uint8_t pos) const {
		if(pos >= n) {
			return 0;
		}
		return decompress_pulse(values[pos]);
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint16_t header_duration(void) const {
		return duration(0);
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline uint8_t length(void) const {
		return n;
	}

	template <typename T = uint64_t>
	[[gnu::pure,nodiscard]]
	T decode_data(void) const {
		T result = 1;
		const uint8_t thresh = bit_thresh() / pulse_resolution();
		for(uint8_t i = 1; i < n; i += 2) {
			bool low1  = values[i  ] > thresh;
			bool high1 = values[i+1] > thresh;
			result = (result << 1) | (low1 ^ high1);
		}
		return result;
	}

	[[gnu::always_inline]]
	inline void reset(uint16_t headerLength) {
		n = 1;
		values[0] = compress_pulse(headerLength);
	}

	void add(uint16_t duration) {
		if(n < MAX_LENGTH) {
			values[n] = compress_pulse(duration);
			++ n;
		}
	}

	[[gnu::always_inline]]
	inline void pop(void) {
		if(n > 1) {
			-- n;
		}
	}

	IRCommand(void) = default;

	IRCommand(const IRCommand&) = delete;
	IRCommand(IRCommand&&) = default;

	IRCommand &operator=(const IRCommand&) = delete;
	IRCommand &operator=(IRCommand&&) = delete;
};

#undef MAX_LENGTH

#endif

/*
 * DecompressorPointer - Simple pointer-like wrapper around a decompressor
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

#ifndef DECOMPRESSOR_POINTER_H_INCLUDED
#define DECOMPRESSOR_POINTER_H_INCLUDED

#include "ext.h"

template <typename DecompressorT>
class DecompressorPointer {
	typedef DecompressorPointer<DecompressorT> Me;

	DecompressorT *decompressor;
	int p;

public:
	[[gnu::always_inline]]
	constexpr inline DecompressorPointer(DecompressorT *decompressor, int p = 0)
		: decompressor(decompressor)
		, p(p)
	{}

	[[gnu::always_inline]]
	inline DecompressorPointer(const Me&) = default;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline operator bool(void) const {
		return decompressor != nullptr;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline Me operator+(int n) const {
		return Me(decompressor, p + n);
	}

	[[gnu::always_inline]]
	inline Me &operator+=(int n) {
		p += n;
		return *(static_cast<Me*>(this));
	}

	// Reading values can have side-effects (advancing the window), so this is
	// not a pure function
	[[gnu::always_inline,nodiscard]]
	inline uint8_t operator[](int index) const {
		return decompressor->get(p + index);
	}
};

template <typename DecompressorT>
[[gnu::always_inline,nodiscard]]
inline DecompressorPointer<DecompressorT> MakeDecompressorPointer(
	DecompressorT *decompressor
) {
	return DecompressorPointer<DecompressorT>(decompressor);
}

#endif

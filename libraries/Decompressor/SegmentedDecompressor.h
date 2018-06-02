/*
 * SegmentedDecompressor - Decompressor which can read many small streams
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

#ifndef SEGMENTED_DECOMPRESSOR_H_INCLUDED
#define SEGMENTED_DECOMPRESSOR_H_INCLUDED

#include "Decompressor.h"

template <typename BaseT, windowsz_t windowSize>
class SegmentedDecompressor : private Decompressor<BaseT, windowSize> {
	uint16_t zeroStreamBit;

public:
	inline SegmentedDecompressor(const BaseT &baseData)
		: Decompressor<BaseT, windowSize>(baseData)
		, zeroStreamBit(this->currentStreamBit)
	{}

	inline SegmentedDecompressor(BaseT &&baseData)
		: Decompressor<BaseT, windowSize>(static_cast<BaseT&&>(baseData))
		, zeroStreamBit(this->currentStreamBit)
	{}

	uint8_t get(uint16_t zeroStreamBit, uint16_t index) {
		if(zeroStreamBit != this->zeroStreamBit || this->beyond_index(index)) {
			this->zeroStreamBit = zeroStreamBit;
			this->reset(this->zeroStreamBit);
		}
		this->advance_to_index(index);
		return this->get_index(index);
	}
};

template <typename SegDecompressorT, SegDecompressorT *decompressor, typename T>
class SegmentedDecompressorPointer {
	typedef SegmentedDecompressorPointer<SegDecompressorT, decompressor, T> Me;

	const uint16_t zeroStreamBit;
	int p;

public:
	constexpr inline SegmentedDecompressorPointer(uint16_t zeroStreamBit)
		: zeroStreamBit(zeroStreamBit)
		, p(0)
	{}

	[[gnu::always_inline]]
	inline SegmentedDecompressorPointer(const Me&) = default;

	[[gnu::const,nodiscard,gnu::always_inline]]
	inline operator bool(void) const {
		return true;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline Me operator+(int n) const {
		return Me(zeroStreamBit, p + n);
	}

	[[gnu::always_inline]]
	inline Me &operator+=(int n) {
		p += n;
		return *(static_cast<Me*>(this));
	}

	// Reading values can have side-effects (advancing the window), so this is
	// not a pure function
	[[gnu::always_inline,nodiscard]]
	inline T operator[](int index) const {
		return T(decompressor->get(zeroStreamBit, uint16_t(p + index)));
	}
};

template <windowsz_t windowSize, typename BaseT>
[[gnu::always_inline,nodiscard]]
inline SegmentedDecompressor<BaseT, windowSize> MakeSegmentedDecompressor(
	const BaseT &baseData
) {
	return SegmentedDecompressor<BaseT, windowSize>(baseData);
}

template <windowsz_t windowSize, typename BaseT>
[[gnu::always_inline,nodiscard]]
inline SegmentedDecompressor<BaseT, windowSize> MakeSegmentedDecompressor(
	BaseT &&baseData
) {
	return SegmentedDecompressor<BaseT, windowSize>(
		static_cast<BaseT&&>(baseData)
	);
}

template <windowsz_t windowSize, typename T>
[[gnu::always_inline,nodiscard]]
inline SegmentedDecompressor<const T*, windowSize> MakeSegmentedDecompressor(
	const T *baseData
) {
	return SegmentedDecompressor<const T*, windowSize>(baseData);
}

template <typename T, typename SegDecompressorT, SegDecompressorT *decompressor>
[[gnu::always_inline,nodiscard]]
inline SegmentedDecompressorPointer<SegDecompressorT, decompressor, T>
MakeSegPointerImpl(uint16_t zeroStreamBit) {
	return SegmentedDecompressorPointer<SegDecompressorT, decompressor, T>(
		zeroStreamBit
	);
}

#define MakeSegmentedDecompressorPointer(decompressor, zeroStreamBit, type) \
	MakeSegPointerImpl<type, decltype(decompressor), &decompressor>(zeroStreamBit)

#endif

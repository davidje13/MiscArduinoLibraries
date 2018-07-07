/*
 * NotchedEncoder - Helper class for reading notched encoder values.
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

#ifndef NOTCHEDENCODER_H_INCLUDED
#define NOTCHEDENCODER_H_INCLUDED

#include "ext.h"

class NotchedEncoder {
};

#define STATE_LEAN 0x80

template <typename EncoderT, uint8_t notchPos>
class NotchedEncoder_impl : public NotchedEncoder {
	EncoderT *enc;
	uint8_t state;

public:
	[[nodiscard]]
	int16_t delta(void) {
		const int16_t d = enc->delta();
		if(d == 0) {
			return 0; // avoid updating lean if nothing has changed
		}
		const int16_t r = (state & 7) + d;
		// r = 2 + n*4 = exactly at a state
		state = (r & 3);
		if(r > 2) {
			if(state == 0) {
				// leaning right
				state = 4 | STATE_LEAN;
			} else if(state == 3) {
				// leaning right
				state |= STATE_LEAN;
			}
		} else if(state < 2) {
			// leaning left
			state |= STATE_LEAN;
		}
		return (r - (state & 7)) / 4;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline int8_t lean(void) const {
		return (state & STATE_LEAN) ? fraction() : 0;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline int8_t fraction(void) const {
		return (state & 7) - 2;
	}

	void reset(void) {
		enc->reset();
		state = uint8_t(enc->fraction() + 2 - notchPos) & 3;
	}

	NotchedEncoder_impl(EncoderT *enc)
		: enc(enc)
		, state(0)
	{
		reset();
	}
};

#undef STATE_LEAN

template <uint8_t notchPos, typename EncoderT>
[[gnu::always_inline,nodiscard]]
inline NotchedEncoder_impl<
	EncoderT,
	notchPos
> _MakeNotchedEncoder(EncoderT *enc) {
	return NotchedEncoder_impl<EncoderT, notchPos>(enc);
}

#define MakeNotchedEncoder(enc, notchPos) _MakeNotchedEncoder<notchPos>(enc)

#endif

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

#ifndef FONTRENDERER_H_INCLUDED
#define FONTRENDERER_H_INCLUDED

#include <ProgMem.h>

#include "ext.h"

#define INF_WIDTH 0x7FFF

template <typename Bitmask>
class FontRenderer {
	Bitmask *tgt;
	int16_t x;
	int16_t xlim;
	int16_t cx;
	int16_t cy;
	int16_t lnHeight;
	int8_t yShift;

	uint8_t wordbuffer[8];
	uint8_t wordlen;
	int16_t wordw;
	bool softhyphen;

	template <typename Font>
	void dump(const Font &f, uint8_t lim) {
		for(uint8_t i = 0; i < lim; ++ i) {
			uint16_t cw = (
				f.render(*tgt, wordbuffer[i], cx, cy + yShift) +
				f.spacing()
			);
			cx += cw;
			wordw -= cw;
		}
		lnHeight = ext::max2(lnHeight, int16_t(f.line_height()));
		wordlen -= lim;
		memcpy(wordbuffer, &wordbuffer[lim], wordlen);
	}

	[[gnu::pure,nodiscard]]
	inline uint8_t latest_char(void) const {
		return (wordlen == 0) ? 0 : wordbuffer[wordlen-1];
	}

public:
//	[[gnu::nonnull]] // causes false-positive in MakeFontRenderer
	constexpr FontRenderer(
		Bitmask *target,
		int16_t xPos,
		int16_t yPos,
		uint16_t width = INF_WIDTH,
		int16_t indent = 0
	)
		: tgt(target)
		, x(xPos)
		, xlim((width == INF_WIDTH) ? INF_WIDTH : (xPos + width))
		, cx(xPos + indent)
		, cy(yPos)
		, lnHeight(0)
		, yShift(0)
		, wordbuffer()
		, wordlen(0)
		, wordw(0)
	{}

	void move(
		int16_t xPos,
		int16_t yPos,
		uint16_t width = INF_WIDTH,
		int16_t indent = 0
	) {
		x = xPos;
		xlim = (width == INF_WIDTH) ? INF_WIDTH : (xPos + width);
		cx = xPos + indent;
		cy = yPos;
	}

	[[gnu::always_inline]]
	void set_yshift(int8_t y) {
		yShift = y;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	int16_t cursor_x(void) const {
		return cx;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	int16_t cursor_y(void) const {
		return cy;
	}

	template <typename Font>
	void end_section(const Font &f) {
		if(wordlen != 0) {
			dump(f, wordlen);
			wordw = 0; // Not required, but be safe
			softhyphen = false;
		}
	}

	template <typename Font>
	void print_part(const Font &f, uint8_t c) {
		static PROGMEM const char OPTIONAL_RENDER[] = " \r\n\t";
		static PROGMEM const char WRAP_BEFORE[] = "([{<#";
		static PROGMEM const char WRAP_AFTER[] = ",:;?!)]}>/\\|%";
		static PROGMEM const char WRAP_AFTER_EXCL_NUM[] = ".-";

		int16_t cw = f.measure(c) + f.spacing();

		if(strchr_P(OPTIONAL_RENDER, c) != nullptr) {
			end_section(f);

			int16_t next;
			if(c == '\t') {
				next = f.next_tabstop(cx - x) + x;
			} else {
				next = cx + cw;
			}
			next = ext::min2(next, xlim);
			if(cx + cw <= next) {
				f.render(*tgt, c, cx, cy);
			}

			switch(c) {
			case '\r':
				cx = x;
				break;
			case '\n':
				cx = x;
				cy += ext::max2(lnHeight, int16_t(f.line_height()));
				lnHeight = 0;
				break;
			default:
				cx = next;
			}
			return;
		}

		if(
			strchr_P(WRAP_BEFORE, c) != nullptr ||
			( // special case
				strchr_P(WRAP_AFTER_EXCL_NUM, latest_char()) != nullptr &&
				(c < '0' || c > '9')
			)
		) {
			end_section(f);
		}

		if(f.char_supported(c)) {
			if(cx + wordw + cw > xlim) {
				if(x + wordw + cw > xlim) {
					end_section(f);
					softhyphen = true;
				}
				if(softhyphen && cx + int16_t(f.measure('-')) <= xlim) {
					f.render(*tgt, '-', cx, cy);
				}
				cx = x;
				cy += lnHeight;
				lnHeight = 0;
			}

			// TODO: better forced-wrapping
			if(wordlen >= sizeof(wordbuffer)) {
				dump(f, 4);
				softhyphen = true;
			}

			wordbuffer[wordlen++] = c;
			wordw += cw;
		}

		if(strchr_P(WRAP_AFTER, c) != nullptr) {
			end_section(f);
		}
	}

	template <typename Font>
	void print(const Font &f, char c) {
		print_part(f, uint8_t(c));
		end_section(f);
	}

private:
	template <typename Font, typename String>
	void _print(const Font &f, String message) {
		if(!message) {
			return;
		}
		char c;
		for(String p = message; (c = p[0]) != '\0'; p += 1) {
			print_part(f, uint8_t(c));
		}
		end_section(f);
	}

public:
	template <typename Font>
	[[gnu::always_inline]]
	inline void print(const Font &f, const char *message) {
		return _print(f, message);
	}

	template <typename Font>
	[[gnu::always_inline]]
	inline void print(const Font &f, ProgMem<char> message) {
		return _print(f, message);
	}

	template <typename Font, typename T>
	void print_number(const Font &f, T n, uint8_t minDigits = 1) {
		T r = n;
		if(r < 0) {
			print_part(f, '-');
			r = -r;
		}
		uint8_t c = 0;
		char buf[11]; // uint32_t has max value of 4294967295
		uint8_t lim = sizeof(buf) - 1;
		memset(buf, '0', lim);
		while(r > 0 && c < lim) {
			buf[lim-c-1] = char((r % 10) + '0');
			r /= 10;
			++ c;
		}
		c = ext::min2(ext::max2(c, minDigits), lim);
		buf[lim] = '\0';
		print(f, &buf[lim-c]);
	}

	template <typename Font, typename T>
	void print_number_base(
		const Font &f,
		T n,
		uint8_t base,
		uint8_t minDigits = 1
	) {
		T r = n;
		if(r < 0) {
			print_part(f, '-');
			r = -r;
		}

		if(base == 1) {
			while(r --) {
				print(f, '1');
			}
		} else {
			uint8_t c = 0;
			char buf[33]; // 32-bit int in base 2
			uint8_t lim = sizeof(buf) - 1;
			memset(buf, '0', lim);
			while(r > 0 && c < lim) {
				uint8_t d = (r % base);
				char digit;
				if(d < 10) {
					digit = char(d + '0');
				} else if(d < 36) {
					digit = char(d + 'A' - 10);
				} else {
					digit = char(d + 'a' - 36);
				}
				buf[lim-c-1] = digit;
				r /= base;
				++ c;
			}
			c = ext::min2(ext::max2(c, minDigits), lim);
			buf[lim] = '\0';
			print(f, &buf[lim-c]);
		}
	}

	template <typename Font>
	void print_number(
		const Font &f,
		float n,
		uint8_t decimalPlaces = 0,
		uint8_t minDigits = 1
	) {
		float r = n;
		if(r < 0) {
			print_part(f, '-');
			r = -r;
		}
		uint32_t integer = uint32_t(r);
		print_number(f, integer, minDigits);
		if(decimalPlaces == 0) {
			return;
		}
		print(f, '.');
		r -= integer;
		for(int8_t i = 0; i < decimalPlaces; ++ i) {
			r *= 10;
		}
		// TODO: add 0.5 for rounding (but needs integer part to be altered too)
		print_number(f, uint32_t(r), decimalPlaces);
	}
};

template <typename Bitmask>
[[gnu::nonnull,nodiscard]]
FontRenderer<Bitmask> MakeFontRenderer(
	Bitmask *target,
	int16_t xPos,
	int16_t yPos,
	uint16_t width = INF_WIDTH,
	int16_t indent = 0
) {
	return FontRenderer<Bitmask>(target, xPos, yPos, width, indent);
}

#undef INF_WIDTH

#endif

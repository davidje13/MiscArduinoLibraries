/*
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

#include "CompressedStrings.h"

#include <CSNA2.h>
#include <ProgMem.h>

#define WIDE_DATA
#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>

template <typename Printer>
void showCharRange(Printer &printer, uint8_t start, uint8_t end) {
	auto hex = ProgMemString("0123456789ABCDEF");

	printer.set_justification(CSNA2::Justification::CENTRE);

	printer.print(DOUBLE_SPACE);
	for(uint8_t i = start; i < end; ++ i) {
		printer.print(' ');
		printer.print(hex[i]);
		printer.print('_');
	}
	printer.linefeed();

	for(uint8_t j = 0x00; j < 0x10; ++ j) {
		printer.print('_');
		printer.print(hex[j]);
		for(uint8_t i = start; i < end; ++ i) {
			printer.print(DOUBLE_SPACE);
			printer.print(char((i << 4) | j));
		}
		printer.linefeed();
		delay(200);
	}

	printer.set_justification(CSNA2::Justification::LEFT);
}

template <typename Printer, typename T>
void showFullCharset(Printer &printer, T name, CSNA2::Charset c) {
	printer.awake();
	printer.set_charset(c);

	printer.linefeed();
	printer.print(name);
	printer.linefeed();

	showCharRange(printer, 0x2, 0x8);
}

template <typename Printer, typename T>
void showCodepage(Printer &printer, T name, CSNA2::Codepage c) {
	printer.awake();
	printer.set_codepage(c);

	printer.linefeed();
	printer.print(name);
	printer.linefeed();

	showCharRange(printer, 0x8, 0x10);
}

template <typename Printer, typename T>
void showCharset(Printer &printer, T name, CSNA2::Charset c) {
	printer.awake();
	printer.print(name);
	printer.linefeed();
	printer.set_charset(c);
	printer.print(CHARSET_SAMPLE);
	delay(400);
}

template <typename Printer>
void showUserCharset(Printer &printer) {
	printer.awake();

	// Annoyingly, all custom chars must be specified at once.
	// Specifying individual characters will clear the previous definitions.
	// To help reduce memory use, a lambda can be used to draw each character
	// as it is needed:
	Bitmask18<12,24> bitmask;
	printer.set_user_chars('a', 6, [&bitmask] (char c) {
		bitmask.clear();
		Pattern p;
		BlendMode m;
		switch(c) {
		case 'a': p = PATTERN_GRID;   m = BlendMode::ON;  break;
		case 'b': p = PATTERN_GRID;   m = BlendMode::OFF; break;
		case 'c': p = PATTERN_FILL75; m = BlendMode::ON;  break;
		case 'd': p = PATTERN_FILL75; m = BlendMode::OFF; break;
		case 'e': p = PATTERN_BRICKS; m = BlendMode::ON;  break;
		case 'f': p = PATTERN_BRICKS; m = BlendMode::OFF; break;
		}
		bitmask.fill_rect(0, 0, 12, 24, m, p);
		return bitmask;
	});

	printer.awake();
	printer.print(CHARSET_CUSTOM_TITLE);
	printer.set_user_chars(true);
	printer.print(CHARSET_CUSTOM);
	printer.set_user_chars(false);
	printer.print(CHARSET_NORMAL);
	delay(800);
}

template <typename Printer>
void demoCharset(Printer &printer) {
	showTitle(printer, TITLE_CHARSET);

	showFullCharset(printer, CHARSET_USA, CSNA2::Charset::USA);
	showCharset(printer, CHARSET_FRANCE, CSNA2::Charset::FRANCE);
	showCharset(printer, CHARSET_GERMANY, CSNA2::Charset::GERMANY);
	showCharset(printer, CHARSET_UK, CSNA2::Charset::UK);
	showCharset(printer, CHARSET_DENMARK1, CSNA2::Charset::DENMARK1);
	showCharset(printer, CHARSET_SWEDEN, CSNA2::Charset::SWEDEN);
	showCharset(printer, CHARSET_ITALY, CSNA2::Charset::ITALY);
	showCharset(printer, CHARSET_SPAIN1, CSNA2::Charset::SPAIN1);
	showCharset(printer, CHARSET_JAPAN, CSNA2::Charset::JAPAN);
	showCharset(printer, CHARSET_NORWAY, CSNA2::Charset::NORWAY);
	showCharset(printer, CHARSET_DENMARK2, CSNA2::Charset::DENMARK2);
	showCharset(printer, CHARSET_SPAIN2, CSNA2::Charset::SPAIN2);
	showCharset(printer, CHARSET_LATIN_AMERICA, CSNA2::Charset::LATIN_AMERICA);
	showCharset(printer, CHARSET_KOREA, CSNA2::Charset::KOREA);
	showCharset(printer, CHARSET_SLOVENIA, CSNA2::Charset::SLOVENIA);
	showCharset(printer, CHARSET_CHINA, CSNA2::Charset::CHINA);
	showUserCharset(printer);

	// Charset changes can be woven with printing commands:
	printer.awake();
	printer.set_charset(CSNA2::Charset::USA);
	printer.print(CHARSET_WORLD_TOUR_USA);
	printer.set_charset(CSNA2::Charset::UK);
	printer.print(CHARSET_WORLD_TOUR_UK);
	printer.set_charset(CSNA2::Charset::CHINA);
	printer.print(CHARSET_WORLD_TOUR_CHINA);
	printer.set_charset(CSNA2::Charset::USA);
	delay(200);

	showCodepage(printer, CODEPAGE_CP437, CSNA2::Codepage::CP437);
	showCodepage(printer, CODEPAGE_CP720, CSNA2::Codepage::CP720);
}

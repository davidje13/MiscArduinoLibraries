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

#include <CSNA2.h>
#include <ProgMem.h>

#define WIDE_DATA
#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>

static PROGMEM const char HEX_CHARS[] = "0123456789ABCDEF";

template <typename Printer>
void showCharRange(Printer &printer, uint8_t start, uint8_t end) {
	auto hex = MakeProgMem(HEX_CHARS);

	printer.set_justification(CSNA2::Justification::CENTRE);

	printer.print(ProgMemString("  "));
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
			printer.print(ProgMemString("  "));
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
	printer.print(ProgMemString(
		"  23=\x23 24=\x24 40=\x40 5B=\x5B 5C=\x5C 5D=\x5D\n"
		"  5E=\x5E 60=\x60 7B=\x7B 7C=\x7C 7D=\x7D 7E=\x7E\n"
	));
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
	printer.print(ProgMemString("\nCustom characters\n"));
	printer.set_user_chars(true);
	printer.print(ProgMemString("CUSTOM: abcdefxyz\n"));
	printer.set_user_chars(false);
	printer.print(ProgMemString("NORMAL: abcdefxyz\n"));
	delay(800);
}

template <typename Printer>
void demoCharset(Printer &printer) {
	showTitle(printer, ProgMemString("Character Sets!"));

	showFullCharset(printer, ProgMemString("USA"), CSNA2::Charset::USA);
	showCharset(printer, ProgMemString("FRANCE"), CSNA2::Charset::FRANCE);
	showCharset(printer, ProgMemString("GERMANY"), CSNA2::Charset::GERMANY);
	showCharset(printer, ProgMemString("UK"), CSNA2::Charset::UK);
	showCharset(printer, ProgMemString("DENMARK1"), CSNA2::Charset::DENMARK1);
	showCharset(printer, ProgMemString("SWEDEN"), CSNA2::Charset::SWEDEN);
	showCharset(printer, ProgMemString("ITALY"), CSNA2::Charset::ITALY);
	showCharset(printer, ProgMemString("SPAIN1"), CSNA2::Charset::SPAIN1);
	showCharset(printer, ProgMemString("JAPAN"), CSNA2::Charset::JAPAN);
	showCharset(printer, ProgMemString("NORWAY"), CSNA2::Charset::NORWAY);
	showCharset(printer, ProgMemString("DENMARK2"), CSNA2::Charset::DENMARK2);
	showCharset(printer, ProgMemString("SPAIN2"), CSNA2::Charset::SPAIN2);
	showCharset(printer, ProgMemString("LATIN_AMERICA"), CSNA2::Charset::LATIN_AMERICA);
	showCharset(printer, ProgMemString("KOREA"), CSNA2::Charset::KOREA);
	showCharset(printer, ProgMemString("SLOVENIA / CROATIA"), CSNA2::Charset::SLOVENIA);
	showCharset(printer, ProgMemString("CHINA"), CSNA2::Charset::CHINA);
	showUserCharset(printer);

	// Charset changes can be woven with printing commands:
	printer.awake();
	printer.set_charset(CSNA2::Charset::USA);
	printer.print(ProgMemString("\nWorld Tour! USD $ GBP "));
	printer.set_charset(CSNA2::Charset::UK);
	printer.print(ProgMemString("\x23 RMB "));
	printer.set_charset(CSNA2::Charset::CHINA);
	printer.print(ProgMemString("\x24\n"));
	printer.set_charset(CSNA2::Charset::USA);
	delay(200);

	showCodepage(printer, ProgMemString("CP437"), CSNA2::Codepage::CP437);
	showCodepage(printer, ProgMemString("CP720"), CSNA2::Codepage::CP720);
}

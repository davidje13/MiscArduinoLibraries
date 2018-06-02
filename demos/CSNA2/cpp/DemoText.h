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

template <typename Printer>
void demoText(Printer &printer) {
	showTitle(printer, TITLE_TEXT);

	printer.print(TEXT_INTRO);

	printer.set_justification(CSNA2::Justification::CENTRE);
	printer.print(TEXT_CENTRE);
	printer.set_justification(CSNA2::Justification::RIGHT);
	printer.print(TEXT_RIGHT);
	printer.set_justification(CSNA2::Justification::LEFT);

	printer.print(TEXT_NORMAL);
	printer.set_bold(true);
	printer.print(TEXT_BOLD);
	printer.set_bold(false);
	printer.set_doublestrike(true);
	printer.print(TEXT_DBLSTRIKE);
	printer.set_bold(true);
	printer.print(TEXT_BOTH);
	printer.set_doublestrike(false);
	printer.set_bold(false);
	printer.linefeed();

	printer.print(TEXT_NORMAL);
	printer.set_underline(true);
	printer.print(TEXT_UNDERLINE);
	printer.set_underline(CSNA2::Underline::THICK);
	printer.print(TEXT_DBLUNDERLINE);
	printer.set_underline(false);
	printer.linefeed();

	printer.print(TEXT_NORMAL);
	printer.set_inverted(true);
	printer.print(TEXT_INVERTED);
	printer.set_inverted(false);
	printer.set_strikeout(true);
	printer.print(TEXT_STRIKEOUT);
	printer.set_strikeout(false);
	printer.linefeed();

	delay(1000);

	printer.set_rotation(CSNA2::Rotation::CW_90);
	printer.print(TEXT_CW_90);
	printer.linefeed();
	printer.set_rotation(CSNA2::Rotation::CCW_90);
	printer.print(TEXT_CCW_90);
	printer.linefeed();
	printer.set_rotation(CSNA2::Rotation::REVERSED);
	printer.print(TEXT_REVERSED);
	printer.linefeed();
	printer.set_rotation(CSNA2::Rotation::NORMAL);

	delay(1000);

	for(uint8_t h = 1; h < 5; ++ h) {
		printer.set_character_size(1, h);
		for(uint8_t w = 1; w < 5; ++ w) {
			printer.print(char('0' + w));
			printer.print('x');
			printer.print(char('0' + h));
			printer.set_character_width(w + 1);
		}
		printer.linefeed();
	}

	delay(1000);

	printer.set_character_size(1, 1);

	printer.set_font(CSNA2::Font::B_9_17);
	printer.print(TEXT_ALTFONT);
	printer.set_font(CSNA2::Font::A_12_24);

	printer.set_character_spacing(2 * 8);
	printer.print(TEXT_WIDE);
	printer.set_character_spacing(0);

	printer.print(TEXT_X0);
	printer.set_x(100);
	printer.print(TEXT_X100);
	printer.set_x(200);
	printer.print(TEXT_X200);
	printer.linefeed();

	printer.print(TEXT_X0);
	printer.set_x(100);
	printer.print(TEXT_X100);
	printer.set_x(85);
	printer.print(TEXT_X85);
	printer.linefeed();

	delay(2000);
}

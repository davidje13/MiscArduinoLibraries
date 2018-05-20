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

template <typename Printer>
void demoText(Printer &printer) {
	showTitle(printer, ProgMemString("Text!"));

	printer.print(ProgMemString(
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
		"Suspendisse vitae auctor est. "
		"Aliquam scelerisque scelerisque erat sit amet vulputate."
		"\n"
	));

	printer.set_justification(CSNA2::Justification::CENTRE);
	printer.print(ProgMemString("centre\n"));
	printer.set_justification(CSNA2::Justification::RIGHT);
	printer.print(ProgMemString("right\n"));
	printer.set_justification(CSNA2::Justification::LEFT);

	printer.print(ProgMemString("normal "));
	printer.set_bold(true);
	printer.print(ProgMemString("bold "));
	printer.set_bold(false);
	printer.set_doublestrike(true);
	printer.print(ProgMemString("dblstrike "));
	printer.set_bold(true);
	printer.print(ProgMemString("both "));
	printer.set_doublestrike(false);
	printer.set_bold(false);
	printer.linefeed();

	printer.print(ProgMemString("normal "));
	printer.set_underline(true);
	printer.print(ProgMemString("underline "));
	printer.set_underline(CSNA2::Underline::THICK);
	printer.print(ProgMemString("dblunderline "));
	printer.set_underline(false);
	printer.linefeed();

	printer.print(ProgMemString("normal "));
	printer.set_inverted(true);
	printer.print(ProgMemString("inverted "));
	printer.set_inverted(false);
	printer.set_strikeout(true);
	printer.print(ProgMemString("strikeout"));
	printer.set_strikeout(false);
	printer.linefeed();

	delay(1000);

	printer.set_rotation(CSNA2::Rotation::CW_90);
	printer.print(ProgMemString("CW_90"));
	printer.linefeed();
	printer.set_rotation(CSNA2::Rotation::CCW_90);
	printer.print(ProgMemString("CCW_90"));
	printer.linefeed();
	printer.set_rotation(CSNA2::Rotation::REVERSED);
	printer.print(ProgMemString("REVERSED"));
	printer.linefeed();
	printer.set_rotation(CSNA2::Rotation::NORMAL);

	printer.set_character_size(1, 1);
	printer.print(ProgMemString("1x1"));
	printer.set_character_width(2);
	printer.print(ProgMemString("2x1"));
	printer.set_character_width(3);
	printer.print(ProgMemString("3x1"));
	printer.set_character_width(4);
	printer.print(ProgMemString("4x1\n"));

	printer.set_character_size(1, 2);
	printer.print(ProgMemString("1x2"));
	printer.set_character_width(2);
	printer.print(ProgMemString("2x2"));
	printer.set_character_width(3);
	printer.print(ProgMemString("3x2"));
	printer.set_character_width(4);
	printer.print(ProgMemString("4x2\n"));

	printer.set_character_size(1, 3);
	printer.print(ProgMemString("1x3"));
	printer.set_character_width(2);
	printer.print(ProgMemString("2x3"));
	printer.set_character_width(3);
	printer.print(ProgMemString("3x3"));
	printer.set_character_width(4);
	printer.print(ProgMemString("4x3\n"));

	printer.set_character_size(1, 4);
	printer.print(ProgMemString("1x4"));
	printer.set_character_width(2);
	printer.print(ProgMemString("2x4"));
	printer.set_character_width(3);
	printer.print(ProgMemString("3x4"));
	printer.set_character_width(4);
	printer.print(ProgMemString("4x4\n"));

	delay(1000);

	printer.set_character_size(1, 1);

	printer.set_font(CSNA2::Font::B_9_17);
	printer.print(ProgMemString("Alternative font\n"));
	printer.set_font(CSNA2::Font::A_12_24);

	printer.set_character_spacing(2 * 8);
	printer.print(ProgMemString("WIDE SPACING\n"));
	printer.set_character_spacing(0);

	printer.print(ProgMemString("x0"));
	printer.set_x(100);
	printer.print(ProgMemString("x100"));
	printer.set_x(200);
	printer.print(ProgMemString("x200"));
	printer.linefeed();

	printer.print(ProgMemString("x0"));
	printer.set_x(100);
	printer.print(ProgMemString("x100"));
	printer.set_x(85);
	printer.print(ProgMemString("x85"));
	printer.linefeed();

	delay(2000);
}

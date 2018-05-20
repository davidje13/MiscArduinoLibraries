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

#include "DemoUtils.h"
#include "Logo.h"

#include <CSNA2.h>
#include <ProgMem.h>
#include <StaticBitmask18.h>
#include <SolidFill.h>

template <typename Printer>
void demoBitmaps(Printer &printer) {
	showTitle(printer, ProgMemString("Bitmaps!"));

	auto bitmask = MakeStaticBitmask18(SolidFill(true), 64, 32);

	printer.awake();
	printer.print("print_bitmask81msb\n");
	printer.print_bitmask81msb(bitmask);
	delay(500);

	printer.awake();
	printer.print("\nprint_bitmask81msb_small\n");
	printer.print_bitmask81msb_small(bitmask);
	delay(500);

	printer.awake();
	printer.print("\nprint_bitmask81msb_fullwidth\n");
	printer.print_bitmask81msb_fullwidth(bitmask); // lower quality
	delay(500);

	printer.awake();
	printer.print("\nprint_bitmask81lsb_fullwidth\n");
	printer.print_bitmask81lsb_fullwidth(bitmask); // lower quality
	delay(500);

	printer.awake();
	printer.print("\nprint_bitmask18\n");
	printer.print_bitmask18(bitmask); // lower quality for large images
	delay(500);

	printer.awake();
	printer.print("\nprint_bitmask18_lines\n");
	printer.print_bitmask18_lines(bitmask); // fails for large images (why??)
	delay(500);

	printer.awake();
	printer.print("\nLarge ProgMem image:\n");
	auto logo = MakeStaticBitmask18(MakeProgMem(LOGO), 256, 256);
	printer.print_bitmask18(logo, false, false);
	printer.linefeed();

	delay(2000);
}

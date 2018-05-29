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
#include "CompressedFace.h"
#include "CompressedShip.h"

#include <CSNA2.h>
#include <ProgMem.h>
#include <SolidFill.h>
#include <Decompressor.h>
#include <DecompressorPointer.h>
#include <StaticBitmask81.h>

template <typename Printer>
void showFace(Printer &printer) {
	printer.awake();
	printer.print(ProgMemString("\nSimple compressed ProgMem image:\n"));
	// window size must be at least as large as
	// the configured value during compression
	auto faceDecompressor = MakeDecompressor<128>(
		MakeProgMem(COMPRESSED_FACE)
	);
	auto face = MakeStaticBitmask81(
		MakeDecompressorPointer(&faceDecompressor),
		256,
		256
	);
	printer.print_bitmask81msb(face);
	printer.linefeed();
	delay(1000);
}

template <typename Printer>
void showShip(Printer &printer) {
//	printer.awake();
//	printer.print(ProgMemString("\nLarge ProgMem image:\n"));
//	auto rawShip = MakeStaticBitmask81(MakeProgMem(SHIP), 384, 224);
//	printer.print_bitmask81lsb_fullwidth(rawShip);
//	printer.linefeed();
//	delay(2000);

	printer.awake();
	printer.print(ProgMemString("\nLarge compressed ProgMem image:\n"));
	// window size must be at least as large as
	// the configured value during compression
	auto shipDecompressor = MakeDecompressor<128>(
		MakeProgMem(COMPRESSED_SHIP)
	);
	auto ship = MakeStaticBitmask81(
		MakeDecompressorPointer(&shipDecompressor),
		384,
		224
	);
	printer.print_bitmask81lsb_fullwidth(ship);
	printer.linefeed();
	delay(1000);
}

template <typename Printer>
void demoBitmaps(Printer &printer) {
	showTitle(printer, ProgMemString("Bitmaps!"));

	auto bitmask = MakeStaticBitmask81(SolidFill(true), 64, 32);

	printer.awake();
	printer.print(ProgMemString("print_bitmask81msb\n"));
	printer.print_bitmask81msb(bitmask);
	delay(500);

	printer.awake();
	printer.print(ProgMemString("\nprint_bitmask81msb_small\n"));
	printer.print_bitmask81msb_small(bitmask);
	delay(500);

	printer.awake();
	printer.print(ProgMemString("\nprint_bitmask81msb_fullwidth\n"));
	printer.print_bitmask81msb_fullwidth(bitmask); // lower quality
	delay(500);

	printer.awake();
	printer.print(ProgMemString("\nprint_bitmask81lsb_fullwidth\n"));
	printer.print_bitmask81lsb_fullwidth(bitmask); // lower quality
	delay(500);

	printer.awake();
	printer.print(ProgMemString("\nprint_bitmask18\n"));
	printer.print_bitmask18(bitmask); // lower quality for large images
	delay(500);

	printer.awake();
	printer.print(ProgMemString("\nprint_bitmask18_lines\n"));
	printer.print_bitmask18_lines(bitmask); // fails for large images (why??)
	delay(500);

	showFace(printer);
	showShip(printer);
}

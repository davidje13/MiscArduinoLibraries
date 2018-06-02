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
#include "CompressedStrings.h"

#include <CSNA2.h>
#include <ProgMem.h>
#include <SolidFill.h>
#include <Decompressor.h>
#include <DecompressorPointer.h>
#include <StaticBitmask81.h>

template <typename Printer>
void showFace(Printer &printer) {
	printer.awake();
	printer.print(IMAGE_COMPRESSED_FACE);
	// window size must be at least as large as
	// the configured value during compression
	auto faceDecompressor = MakeFaceDecompressor();
	auto face = MakeCompressedFaceBitmask(&faceDecompressor);
	printer.print_bitmask81msb(face);
	printer.linefeed();
	delay(1000);
}

template <typename Printer>
void showShip(Printer &printer) {
//	printer.awake();
//	printer.print(ProgMemString("\nLarge ProgMem image:\n"));
//	auto rawShip = MakeShipBitmask();
//	printer.print_bitmask81lsb_fullwidth(rawShip);
//	printer.linefeed();
//	delay(2000);

	printer.awake();
	printer.print(IMAGE_COMPRESSED_SHIP);
	// window size must be at least as large as
	// the configured value during compression
	auto shipDecompressor = MakeShipDecompressor();
	auto ship = MakeCompressedShipBitmask(&shipDecompressor);
	printer.print_bitmask81lsb_fullwidth(ship);
	printer.linefeed();
	delay(1000);
}

template <typename Printer>
void demoBitmaps(Printer &printer) {
	showTitle(printer, TITLE_BITMAPS);

	auto bitmask = MakeStaticBitmask81(SolidFill(true), 64, 32);

	printer.awake();
	printer.print(BITMAPS_BITMASK81MSB);
	printer.print_bitmask81msb(bitmask);
	delay(500);

	printer.awake();
	printer.print(BITMAPS_BITMASK81MSB_SMALL);
	printer.print_bitmask81msb_small(bitmask);
	delay(500);

	printer.awake();
	printer.print(BITMAPS_BITMASK81MSB_FULLWIDTH);
	printer.print_bitmask81msb_fullwidth(bitmask); // lower quality
	delay(500);

	printer.awake();
	printer.print(BITMAPS_BITMASK81LSB_FULLWIDTH);
	printer.print_bitmask81lsb_fullwidth(bitmask); // lower quality
	delay(500);

	printer.awake();
	printer.print(BITMAPS_BITMASK18);
	printer.print_bitmask18(bitmask); // lower quality for large images
	delay(500);

	printer.awake();
	printer.print(BITMAPS_BITMASK18_LINES);
	printer.print_bitmask18_lines(bitmask); // fails for large images (why??)
	delay(500);

	showFace(printer);
	showShip(printer);
}

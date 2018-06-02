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
void demoBarcodes(Printer &printer) {
	showTitle(printer, TITLE_BARCODES);

	printer.awake();
	printer.set_barcode_text(false, true);
	printer.print(BARCODE_UPCA);
	printer.print_barcode_upca(BARCODE_UPCA_SAMPLE);
	delay(800);

	printer.awake();
	printer.set_barcode_thickness(2);
	printer.print(BARCODE_UPCA_NARROW);
	printer.print_barcode_upca(BARCODE_UPCA_SAMPLE);
	delay(800);

	printer.awake();
	printer.set_barcode_height(40); // 0.5cm
	printer.print(BARCODE_UPCA_SHORT);
	printer.print_barcode_upca(BARCODE_UPCA_SAMPLE);
	delay(800);

	printer.awake();
	printer.print(BARCODE_UPCE);
	// UPCE is a 0-omitted version of UPCA
	// Printer takes values in UPCA format and converts them:
	//  first digit (number system) can be 0 or 1 (all others are rejected)
	//  remaining digits must match one of the following patterns:
	//   **x0000*** (x can be 0-2)
	//   ***00000**
	//   ****00000*
	//   *****0000x (x can be 5-9)
	printer.print_barcode_upce(BARCODE_UPCE_SAMPLE);
	delay(800);

	printer.awake();
	printer.print(BARCODE_JAN13);
	printer.print_barcode_jan13(BARCODE_JAN13_SAMPLE);
	delay(800);

	printer.awake();
	printer.print(BARCODE_JAN8);
	printer.print_barcode_jan8(BARCODE_JAN8_SAMPLE);
	delay(800);

	printer.awake();
	printer.set_barcode_thickness(4);
	printer.print(BARCODE_JAN8_WIDE);
	printer.print_barcode_jan8(BARCODE_JAN8_SAMPLE);
	delay(800);

	printer.awake();
	printer.set_barcode_thickness(2);
	printer.print(BARCODE_CODE39);
	printer.print_barcode_code39_regular(BARCODE_CODE39_SAMPLE); // varlen
	delay(800);

	printer.awake();
	printer.print(BARCODE_ITF);
	printer.print_barcode_itf(BARCODE_ITF_SAMPLE); // varlen, even number of chars
	delay(800);

	printer.awake();
	printer.print(BARCODE_CODABAR);
	printer.print_barcode_codabar(BARCODE_CODABAR_SAMPLE); // varlen
	delay(800);

	printer.awake();
	printer.print(BARCODE_CODE93);
	printer.print_barcode_code93(BARCODE_CODE93_SAMPLE); // varlen
	delay(800);

	printer.awake();
	printer.print(BARCODE_CODE128);
	// spec sheet claims { escapes are needed, but this seems to be false
//	printer.print_barcode_code128(ProgMemString(CODE128_CODEA "hello! " CODE128_BRACE "0}")); // varlen
	printer.print_barcode_code128(BARCODE_CODE128_SAMPLE); // varlen
	delay(800);
}

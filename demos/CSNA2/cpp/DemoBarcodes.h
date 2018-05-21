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
void demoBarcodes(Printer &printer) {
	showTitle(printer, ProgMemString("Barcodes!"));

	printer.awake();
	printer.set_barcode_text(false, true);
	printer.print("UPCA\n");
	printer.print_barcode_upca("01234567890");
	delay(800);

	printer.awake();
	printer.set_barcode_thickness(2);
	printer.print("UPCA (narrow)\n");
	printer.print_barcode_upca("01234567890");
	delay(800);

	printer.awake();
	printer.set_barcode_height(40); // 0.5cm
	printer.print("UPCA (short)\n");
	printer.print_barcode_upca("01234567890");
	delay(800);

	printer.awake();
	printer.print("UPCE\n");
	// UPCE is a 0-omitted version of UPCA
	// Printer takes values in UPCA format and converts them:
	//  first digit (number system) can be 0 or 1 (all others are rejected)
	//  remaining digits must match one of the following patterns:
	//   **x0000*** (x can be 0-2)
	//   ***00000**
	//   ****00000*
	//   *****0000x (x can be 5-9)
	printer.print_barcode_upce("01230000045");
	delay(800);

	printer.awake();
	printer.print("JAN13 / EAN13\n");
	printer.print_barcode_jan13("001234567890");
	delay(800);

	printer.awake();
	printer.print("JAN8 / EAN8\n");
	printer.print_barcode_jan8("0123456");
	delay(800);

	printer.awake();
	printer.set_barcode_thickness(4);
	printer.print("JAN8 / EAN8 (wide)\n");
	printer.print_barcode_jan8("0123456");
	delay(800);

	printer.awake();
	printer.set_barcode_thickness(2);
	printer.print("CODE39 regular\n");
	printer.print_barcode_code39_regular("HELLO"); // varlen
	delay(800);

	printer.awake();
	printer.print("ITF\n");
	printer.print_barcode_itf("0123456789"); // varlen, even number of chars
	delay(800);

	printer.awake();
	printer.print("CODABAR\n");
	printer.print_barcode_codabar("0123456789ABCD"); // varlen
	delay(800);

	printer.awake();
	printer.print("CODE93\n");
	printer.print_barcode_code93("HELLO 1/2"); // varlen
	delay(800);

	printer.awake();
	printer.print("CODE128\n");
	// spec sheet claims { escapes are needed, but this seems to be false
//	printer.print_barcode_code128(CODE128_CODEA "hello! " CODE128_BRACE "0}"); // varlen
	printer.print_barcode_code128("hello! {0}"); // varlen
	delay(800);
}

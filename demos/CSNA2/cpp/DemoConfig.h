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

#include <CSNA2.h>

template <typename Printer>
void demoConfig(Printer &printer) {
	showTitle(printer, ProgMemString("Configuration!"));

	auto test = ProgMemString("\xB0\xB0\xB1\xB1\xB2\xB2\xDB\xDB\n");

	printer.awake();
	printer.configure(CSNA2::Configuration::FACTORY);
	printer.print(ProgMemString("FACTORY MODE    "));
	printer.print(test);

	delay(1000);

	printer.awake();
	printer.configure(CSNA2::Configuration::ECO);
	printer.print(ProgMemString("ECO MODE        "));
	printer.print(test);

	delay(1000);

	printer.awake();
	printer.configure(CSNA2::Configuration::PRECISION);
	printer.print(ProgMemString("PRECISION MODE  "));
	printer.print(test);

	delay(1000);

	printer.awake();
	printer.configure(CSNA2::Configuration::FAST);
	printer.print(ProgMemString("FAST MODE       "));
	printer.print(test);

	delay(1000);

	printer.configure(CSNA2::Configuration::FACTORY);
}

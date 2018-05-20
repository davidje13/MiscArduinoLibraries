#ifndef DEMOUTILS_H_INCLUDED
#define DEMOUTILS_H_INCLUDED

#include <CSNA2.h>

template <typename Printer, typename String>
void showTitle(Printer &printer, String title) {
	printer.soft_reset();
	printer.set_justification(CSNA2::Justification::CENTRE);
	printer.set_underline(CSNA2::Underline::THICK);
	printer.linefeed();
	printer.print(title);
	printer.linefeed();
	printer.set_justification(CSNA2::Justification::LEFT);
	printer.set_underline(false);
	printer.linefeed();

	delay(500);
}

#endif

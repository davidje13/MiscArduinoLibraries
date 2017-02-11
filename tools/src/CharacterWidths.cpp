/*
 * PNG font width calculator.
 *
 * Takes an input PNG image and generates a C-style array declaration which
 * represents the widths of each character. Character boundaries are determined
 * by checking for ((luminosity + 32) % 192) > 64, which allows characters to
 * alternate in brightness (background/foreground of 0/192 to 64/256).
 *
 * Usage:
 *  ./CharacterWidths <image_file>
 *
 * Dependencies: boost (GIL, GIL IO), libpng
 *
 * @author David Evans
 * @license Public Domain
 */

#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <iostream>
#include <ios>
#include <iomanip>
#include <stdlib.h>

int main(int argc, const char *const *argv) {
	if(argc < 2) {
		return EXIT_FAILURE;
	}
	boost::gil::gray8_image_t img;
	boost::gil::png_read_and_convert_image(argv[1], img);

	auto imgView = boost::gil::const_view(img);
	int w = imgView.width();
	bool lastV = false;
	int lastX = 0;
	for(int x = 0; x <= w; ++ x) {
		bool v = (x == w) ? !lastV : (((imgView(x, 0) + 32) % 192) > 64);
		if(x != lastX && v != lastV) {
			std::cout
				<< "0x"
				<< std::hex
				<< std::setfill('0')
				<< std::setw(2)
				<< (x - lastX)
				<< ", ";
			lastX = x;
		}
		lastV = v;
	}

	return EXIT_SUCCESS;
}

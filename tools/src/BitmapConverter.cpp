/*
 * PNG to bitmask conversion program.
 *
 * Takes an input PNG image and generates a C-style array declaration which
 * represents it, in Bitmap18 format: each byte represents a 1x8 slice, LSB
 * is top-most pixel, bytes arranged left-to-right, top-to-bottom.
 *
 * Usage:
 *  ./BitmapConverter <image_file>
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
	for(int y = 0; y < imgView.height(); y += 8) {
		for(int x = 0; x < imgView.width(); ++ x) {
			unsigned char value = 0x00;

			for(int yy = 0; yy < 8; ++ yy) {
				if(imgView(x, y + yy) > 128) {
					value |= 1 << yy;
				}
			}

			std::cout
				<< "0x"
				<< std::hex
				<< std::setfill('0')
				<< std::setw(2)
				<< int(value)
				<< ", ";
		}
	}

	return EXIT_SUCCESS;
}

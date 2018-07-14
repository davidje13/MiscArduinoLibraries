/*
 * PNG to bitmask conversion program.
 *
 * Takes an input PNG image and generates a C-style array declaration which
 * represents it, in Bitmask18 or Bitmask81 format: each byte represents a 1x8
 * or 8x1 slice, LSB is top-left-most pixel, bytes arranged left-to-right,
 * top-to-bottom.
 *
 * Usage:
 *  ./BitmapConverter <image_file> <18/81>
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
#include <stdlib.h>

int main(int argc, const char *const *argv) {
	if(argc < 3) {
		return EXIT_FAILURE;
	}
	boost::gil::gray8_image_t img;
	boost::gil::png_read_and_convert_image(argv[1], img);

	const auto imgView = boost::gil::const_view(img);
	const auto w = imgView.width();
	const auto h = imgView.height();

	std::string format(argv[2]);
	if(format == "18") {
		for(int y = 0; y < h; y += 8) {
			for(int x = 0; x < w; ++ x) {
				unsigned char value = 0x00;

				for(int yy = 0; yy < 8 && y + yy < h; ++ yy) {
					if(imgView(x, y + yy) > 128) {
						value |= 1 << yy;
					}
				}

				std::cout << value;
			}
		}
	} else if(format == "81") {
		for(int y = 0; y < h; ++ y) {
			for(int x = 0; x < w; x += 8) {
				unsigned char value = 0x00;

				for(int xx = 0; xx < 8 && x + xx < w; ++ xx) {
					if(imgView(x + xx, y) > 128) {
						value |= 1 << xx;
					}
				}

				std::cout << value;
			}
		}
	} else {
		std::cerr << "Unknown format " << format << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

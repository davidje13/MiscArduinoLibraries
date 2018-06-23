/*
 * Luminous intentity calculation program.
 *
 * Computes a lookup table converting perceived intensity into LED duty cycle.
 *
 * Usage:
 *  ./LuminousIntensity <wavelength_nm> <peak_intensity_mcd>
 *
 * Dependencies: none
 *
 * @author David Evans
 * @license Public Domain
 */

#include <iostream>
#include <math.h>
#include <stdlib.h>

int main(int argc, const char *const *argv) {
	if(argc < 3) {
		return EXIT_FAILURE;
	}

	double wavelength = atof(argv[1]); // nanometres
	double peakIntensity = atof(argv[2]) * 1000; // milli-candels

	// TODO: actually use the inputs!
	// https://en.wikipedia.org/wiki/CIE_1931_color_space
	// https://en.wikipedia.org/wiki/CIELUV
	// https://en.wikipedia.org/wiki/Luminous_intensity
	// https://en.wikipedia.org/wiki/Luminosity_function
	// http://www.kayelaby.npl.co.uk/general_physics/2_5/2_5_3.html
	// https://ledshield.wordpress.com/2012/11/13/led-brightness-to-your-eye-gamma-correction-no/

	// gamma approximation from
	// https://learn.adafruit.com/led-tricks-gamma-correction/the-longer-fix
	double gamma = 2.8;
	for(int i = 0; i < 256; ++ i) {
		double v = i / 255.0;
		double corrected = pow(v, gamma);
		// use ceiling so that only 0 is fully off (avoid glitchy appearance)
		std::cout << static_cast<unsigned char>(ceil(corrected * 255.0));
	}

	return EXIT_SUCCESS;
}

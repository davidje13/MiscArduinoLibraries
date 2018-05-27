/*
 * Convert a bytestream into C-compatible hex codes.
 *
 * Dependencies: none
 *
 * @author David Evans
 * @license Public Domain
 */

#include <iostream>
#include <iomanip>
#include <stdlib.h>

int main() {
	char value;
	while(std::cin.get(value)) {
		std::cout
			<< "0x"
			<< std::hex
			<< std::setfill('0')
			<< std::setw(2)
			<< int(static_cast<unsigned char>(value))
			<< ", ";
	}
	return EXIT_SUCCESS;
}

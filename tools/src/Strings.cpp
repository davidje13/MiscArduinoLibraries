/*
 * Strings program.
 *
 * Combines strings into a single block and provides indices for each string.
 *
 * Usage:
 *  ./Strings <index_output_file>
 *
 * Dependencies: none
 *
 * @author David Evans
 * @license Public Domain
 */

#include "strings.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <stdlib.h>

void pack_strings(
	const std::vector<stringdef> &strings,
	std::vector<uint8_t> &data,
	std::vector<std::size_t> &indices
) {
	for(const stringdef &d : strings) {
		const auto pos = std::search(
			data.begin(), data.end(),
			d.value.begin(), d.value.end()
		);
		if(pos == data.end()) {
			indices.push_back(data.size());
			data.insert(data.end(), d.value.begin(), d.value.end());
		} else {
			indices.push_back(pos - data.begin());
		}
	}
}

int main(int argc, const char *const *argv) {
	if(argc < 2) {
		return EXIT_FAILURE;
	}

	std::string indexFile = argv[1];
	std::vector<stringdef> strings = read_all_strings(std::cin);
	std::vector<uint8_t> data;
	std::vector<std::size_t> indices;
	pack_strings(strings, data, indices);

	for(uint8_t v : data) {
		std::cout << v;
	}

	if(!indexFile.empty()) {
		std::fstream indexStream(indexFile, std::fstream::out);
		for(std::size_t i = 0; i < strings.size(); ++ i) {
			indexStream
				<< strings[i].name
				<< " "
				<< indices[i]
				<< std::endl;
		}
		indexStream.close();
	}

	return EXIT_SUCCESS;
}

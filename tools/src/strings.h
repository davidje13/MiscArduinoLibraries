/*
 * Strings file reader
 *
 * @author David Evans
 * @license Public Domain
 */

#ifndef STRINGS_H_INCLUDED
#define STRINGS_H_INCLUDED

#include <istream>
#include <vector>

class stringdef {
public:
	std::string name;
	std::vector<uint8_t> value;

	bool valid(void) const {
		return !name.empty();
	}
};

uint8_t fromHex(char c) {
	if(c >= '0' && c <= '9') {
		return uint8_t(c - '0');
	} else if(c >= 'a' && c <= 'f') {
		return uint8_t(c - 'a' + 10);
	} else if(c >= 'A' && c <= 'F') {
		return uint8_t(c - 'A' + 10);
	} else {
		return 0;
	}
}

stringdef read_string_entry(const std::string &ln) {
	stringdef result;

	std::size_t p = ln.find("=");
	if(ln[0] == '#' || p == std::string::npos) {
		return result;
	}

	result.name = ln.substr(0, p);
	++ p;
	while(ln[p] == ' ') {
		++ p;
	}
	bool quoted = (ln[p] == '"');
	for(std::size_t b = p + (quoted ? 1 : 0); b < ln.size();) {
		const char c = ln[b];
		if(quoted && c == '"') {
			break;
		}
		if(c == '\\') {
			const char e = ln[b+1];
			switch(e) {
			case 0:
				break;
			case 'n':
				result.value.push_back(uint8_t('\n'));
				break;
			case 'x':
				if(b + 4 <= ln.size()) {
					result.value.push_back(
						(fromHex(ln[b+2]) << 4) |
						fromHex(ln[b+3])
					);
				}
				b += 2;
				break;
			default:
				result.value.push_back(uint8_t(e));
				break;
			}
			b += 2;
		} else {
			result.value.push_back(uint8_t(c));
			++ b;
		}
	}
	result.value.push_back(0);

	return result;
}

std::vector<stringdef> read_all_strings(std::istream &source) {
	std::vector<stringdef> result;

	for(std::string ln; std::getline(source, ln);) {
		stringdef d = read_string_entry(ln);
		if(d.valid()) {
			result.push_back(std::move(d));
		}
	}

	return result;
}

#endif

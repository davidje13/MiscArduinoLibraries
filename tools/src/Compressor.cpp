/*
 * Data stream compression program.
 *
 * Compresses the input data stream. Uses a DEFLATE-like algorithm, but
 * focussed on minimal RAM requirements for decompression.
 *
 * Usage:
 *  ./Compressor [windowSize]
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
#include <queue>
#include <unordered_map>
#include <stdlib.h>

class bitwriter {
	std::ostream *target;
	uint8_t bits;
	uint8_t sz;
	std::size_t count;

public:
	bitwriter(std::ostream *target)
		: target(target)
		, bits(0x00)
		, sz(0)
		, count(0)
	{}

	std::size_t size(void) const {
		return count;
	}

	std::size_t size_bits(void) const {
		return (count << 3) | sz;
	}

	void push(bool bit) {
		bits = (bits << 1) | (bit ? 1 : 0);
		if((++ sz) == 8) {
			(*target) << bits;
			++ count;
			sz = 0;
		}
	}

	void push(unsigned int value, uint8_t bitcount) {
		// TODO: this could be more efficient
		for(uint8_t i = bitcount; (i --) > 0;) {
			push(value & (1 << i));
		}
	}

	void close(void) {
		if(sz > 0) {
			push(0, 8 - sz);
		}
	}
};

class bitsizer {
	std::size_t sz;

public:
	bitsizer(void) : sz(0) {}

	std::size_t size(void) const {
		return sz >> 3;
	}

	std::size_t size_bits(void) const {
		return sz;
	}

	void push(bool) {
		++ sz;
	}

	void push(unsigned int, uint8_t bitcount) {
		sz += bitcount;
	}

	void close(void) {
		sz = (sz + 7) & ~7;
	}
};

std::vector<uint8_t> read_all(std::istream &in) {
	std::vector<uint8_t> result;

	char value;
	while(in.get(value)) {
		result.push_back(uint8_t(value));
	}

	return result;
}

void optimise_strings(std::vector<stringdef> &strings) {
	for(auto i = strings.begin(); i != strings.end(); ++ i) {
		for(auto j = strings.begin(); j != i; ++ j) {
			if(j->value == i->value) {
				stringdef d = std::move(*i);
				strings.erase(i);
				d.value.clear();
				strings.insert(j, std::move(d));
				break;
			}
		}
	}
}

template <typename LookbackFn, typename RawByteFn>
void compress_duplicates(
	std::vector<uint8_t> bytes,
	std::size_t minDist,
	std::size_t maxDist,
	std::size_t minLen,
	std::size_t maxLen,
	LookbackFn lookbackFn,
	RawByteFn rawByteFn
) {
	for(std::size_t i = 0; i < bytes.size();) {
		std::size_t bestLength = minLen;
		std::size_t bestDist = 0;
		for(std::size_t d = minDist; d <= maxDist; ++ d) {
			std::size_t l;
			for(l = 0; l <= maxLen && i + l < bytes.size(); ++ l) {
				std::size_t p = i + (l % d);
				if(((p < d) ? 0 : bytes[p - d]) != bytes[i + l]) {
					break;
				}
			}
			if(l > bestLength) {
				bestLength = l;
				bestDist = d;
				if(l == maxLen || i + l == bytes.size()) {
					break;
				}
			}
		}

		if(bestDist != 0) {
			lookbackFn(bestDist, bestLength);
			i += bestLength;
		} else {
			rawByteFn(bytes[i]);
			++ i;
		}
	}
}

template <typename T>
class huffnode {
public:
	std::size_t frequency;
	const huffnode<T> *child1;
	const huffnode<T> *child2;
	T value;

	huffnode(T value, std::size_t frequency)
		: frequency(frequency)
		, child1(nullptr)
		, child2(nullptr)
		, value(value)
	{}

	huffnode(const huffnode<T> *child1, const huffnode<T> *child2)
		: frequency(child1->frequency + child2->frequency)
		, child1(child1)
		, child2(child2)
		, value()
	{}
};

template <typename T>
class huffnodeptr_cmp {
public:
	bool operator()(const huffnode<T> *a, const huffnode<T> *b) const {
		return a->frequency > b->frequency;
	}
};

template <typename T>
class codebook {
	typedef uint16_t bits_t;
	static constexpr uint8_t maxBits = 15;

	std::vector<bits_t> bits;
	std::vector<uint8_t> sizes;
	uint8_t maxSize;

	void populate_sizes(const huffnode<T> &node, uint8_t len) {
		if(len > maxBits) {
			// TODO: try to generate non-optimal trees in this situation
			throw std::string("Cannot encode: huffman tree is too skewed");
		}
		if(node.child1 == nullptr) {
			sizes[node.value] = len;
			maxSize = std::max(maxSize, len);
		} else {
			populate_sizes(*node.child1, len + 1);
			populate_sizes(*node.child2, len + 1);
		}
	}

	void populate_bits(void) {
		// Thanks, https://en.wikipedia.org/wiki/Canonical_Huffman_code
		bits_t b = 0;
		for(uint8_t l = 1; l <= maxSize; ++ l) {
			for(T v = 0; v < symbol_count(); ++ v) {
				if(sizes[v] == l) {
					bits[v] = b;
					++ b;
				}
			}
			b <<= 1;
		}
	}

	static T get_max(const std::unordered_map<T, std::size_t> frequencies) {
		T maxValue = 0;
		for(const auto &i : frequencies) {
			const std::size_t f = i.second;
			if(f > 0) {
				maxValue = std::max(maxValue, i.first);
			}
		}
		return maxValue;
	}

	static const huffnode<T> *calculate_huffman(
		const std::unordered_map<T, std::size_t> frequencies,
		std::vector<huffnode<T>> &nodes
	) {
		// ensure nodes list memory never moves
		nodes.reserve(frequencies.size() * 2 - 1);

		std::priority_queue<
			const huffnode<T>*,
			std::vector<const huffnode<T>*>,
			huffnodeptr_cmp<T>
		> queue;

		for(const auto &i : frequencies) {
			const std::size_t f = i.second;
			if(f > 0) {
				nodes.emplace_back(i.first, f);
				queue.push(&nodes.back());
			}
		}

		while(queue.size() > 1) {
			const huffnode<T> *a = queue.top();
			queue.pop();
			const huffnode<T> *b = queue.top();
			queue.pop();

			nodes.emplace_back(a, b);
			queue.push(&nodes.back());
		}

		return queue.top();
	}

public:
	codebook(const std::unordered_map<T, std::size_t> frequencies)
		: bits()
		, sizes()
		, maxSize(0)
	{
		T maxValue = get_max(frequencies);
		sizes.resize(maxValue + 1, 0);
		bits.resize(maxValue + 1, 0);

		std::vector<huffnode<T>> nodes;
		populate_sizes(*calculate_huffman(frequencies, nodes), 0);

		populate_bits();
	}

	T symbol_count(void) const {
		return sizes.size();
	}

	void debug_value(std::ostream &target, const T &v) const {
		bits_t value = bits.at(v);
		uint8_t size = sizes.at(v);
		for(uint8_t i = size; (i --) > 0;) {
			target << ((value & (1 << i)) ? '1' : '0');
		}
	}

	void debug(std::ostream &target) const {
		target << "TABLE:" << std::endl;
		for(T v = 0; v < symbol_count(); ++ v) {
			target << "  " << int(v) << ": ";
			debug_value(target, v);
			target << std::endl;
		}
	}

	template <typename Writer>
	void write(Writer &target) const {
		uint8_t bitsPerSymbol = 4;

		// store total symbol count
		target.push(sizes.size(), 10);

		// now store the number of bits used by each entry in order:
		for(auto size : sizes) {
			target.push(size, bitsPerSymbol);
		}
	}

	template <typename Writer>
	void write_coded(Writer &target, const T &value) const {
		target.push(bits.at(value), sizes.at(value));
	}
};

template <typename T>
codebook<T> make_codebook(const std::vector<T> &values) {
	std::unordered_map<T, std::size_t> frequencies;
	for(const T &v : values) {
		++ frequencies[v];
	}
	return codebook<T>(frequencies);
}

struct opts_t {
	std::size_t maxDist;
	std::size_t minDist;
	std::size_t minLen;
	std::size_t zeroDist;
	std::size_t zeroLen;
	bool splitMarker;
	bool distFirst;
};

template <typename Writer>
codebook<uint16_t> compress(
	Writer &writer,
	std::vector<std::size_t> &zeroPositions,
	const std::vector<std::vector<uint8_t>> &byteBlocks,
	const opts_t &opts
) {
	std::size_t maxLen = opts.zeroLen + 255;
	if(opts.distFirst) {
		maxLen += opts.maxDist - opts.zeroDist;
	}

	std::vector<uint16_t> values;
	zeroPositions.clear();
	for(const auto &bytes : byteBlocks) {
		zeroPositions.push_back(values.size());
		compress_duplicates(
			bytes,
			opts.minDist,
			opts.maxDist,
			opts.minLen,
			maxLen,
			[&values, &opts] (std::size_t dist, std::size_t len) {
				if(opts.splitMarker) {
					values.push_back(0);
					values.push_back(dist - opts.zeroDist);
					values.push_back(len - opts.zeroLen);
				} else if(opts.distFirst) {
					values.push_back(256 + dist - opts.zeroDist);
					values.push_back(len - opts.zeroLen);
				} else {
					values.push_back(256 + len - opts.zeroLen);
					values.push_back(dist - opts.zeroDist);
				}
			},
			[&values, &opts] (uint8_t c) {
				if(opts.splitMarker) {
					values.push_back(c + 1);
				} else {
					values.push_back(c);
				}
			}
		);
	}

	auto table = make_codebook(values);
	writer.push(opts.splitMarker);
	writer.push(opts.distFirst);
	table.write(writer);

	std::size_t section = 0;
	for(std::size_t i = 0; i < values.size(); ++ i) {
		while(i == zeroPositions[section]) {
			zeroPositions[section] = writer.size_bits();
			++ section;
		}
		table.write_coded(writer, values[i]);
	}

	return table;
}

template <typename T>
void inc(T &value, int skip) {
	value = (value & ~(skip - 1)) + skip;
}

std::vector<std::size_t> suggest_window_sizes(std::size_t limit) {
	std::vector<std::size_t> windowSizes;
	windowSizes.push_back(0);
	if(limit > 0) {
		windowSizes.push_back(1);
		for(std::size_t sz = 2; sz < limit; sz += 2) {
			windowSizes.push_back(sz);
		}
		windowSizes.push_back(limit);
	}
	return windowSizes;
}

opts_t pick_compression_options(
	const std::vector<std::vector<uint8_t>> &byteBlocks,
	std::size_t maxWindowSize
) {
	opts_t bestOpts;
	std::size_t bestSize = std::size_t(-1);

	opts_t opts;

	// fixed settings (not stored in data)
	opts.zeroDist = 1;
	opts.zeroLen = 2;

	std::vector<std::size_t> zeroPositions;

	// brute-force best settings
	std::vector<std::size_t> windowSizes = suggest_window_sizes(maxWindowSize);
	for(opts.minDist = opts.zeroDist; opts.minDist <= 1; ++ opts.minDist) {
		for(opts.minLen = opts.zeroLen; opts.minLen <= 3; ++ opts.minLen) {
			for(std::size_t windowSize : windowSizes) {
				opts.maxDist = windowSize;
				for(int flags = 0; flags < 3; ++ flags) {
					opts.splitMarker = (flags & 0x02);
					opts.distFirst = (flags & 0x01);

					bitsizer writer;
					compress(writer, zeroPositions, byteBlocks, opts);
					writer.close();

					if(writer.size() < bestSize) {
						bestOpts = opts;
						bestSize = writer.size();
					}
				}
			}
		}
	}
	return bestOpts;
}

int main(int argc, const char *const *argv) {
	std::vector<std::vector<uint8_t>> byteBlocks;

	if(argc < 2) {
		return EXIT_FAILURE;
	}

	std::size_t maxWindowSize = std::size_t(atoi(argv[1]));

	std::vector<stringdef> strings;
	std::string indexFile;
	if(argc > 3 && std::string("--strings") == argv[2]) {
		indexFile = argv[3];
		strings = read_all_strings(std::cin);
		optimise_strings(strings);
		for(const stringdef &string : strings) {
			byteBlocks.push_back(string.value);
		}
	} else {
		byteBlocks.push_back(read_all(std::cin));
	}

	std::size_t totalBytesIn = 0;
	for(const auto &bytes : byteBlocks) {
		totalBytesIn += bytes.size();
	}
	std::cerr
		<< "Original bits:      " << (totalBytesIn * 8)
		<< " (parts: " << byteBlocks.size() << ")"
		<< std::endl;

	if(totalBytesIn == 0) {
		std::cerr << "No data!" << std::endl;
		return EXIT_FAILURE;
	}

	const opts_t opts = pick_compression_options(byteBlocks, maxWindowSize);

	bitwriter writer(&std::cout);
	std::vector<std::size_t> zeroPositions;
	auto table = compress(writer, zeroPositions, byteBlocks, opts);
	writer.close();

	if(!indexFile.empty()) {
		std::fstream indexStream(indexFile, std::fstream::out);
		for(std::size_t i = 0; i < strings.size(); ++ i) {
			indexStream
				<< strings[i].name
				<< " "
				<< zeroPositions[i]
				<< std::endl;
		}
		indexStream.close();
	}

//	table.debug(std::cerr);

	std::cerr
		<< "Compressed bits:    " << (writer.size() * 8)
		<< " (window: " << opts.maxDist
		<< ", min-dist: " << opts.minDist
		<< ", min-len: " << opts.minLen
		<< ", mode: " << (
			opts.splitMarker ? "split" :
			opts.distFirst ? "dist-len" :
			"len-dist"
		)
		<< ", symbols: " << table.symbol_count()
		<< ")"
		<< std::endl;

	return EXIT_SUCCESS;
}

/*
 * Compression algorithm based on overlapping characters in a list of input
 * words.
 *
 * Takes input as a newline-separated list of words on stdin. Stops reading
 * at first blank line.
 *
 * Dependencies: none
 *
 * Originally written for:
 * http://codegolf.stackexchange.com/questions/87311/compounding-english
 *
 * @author David Evans
 * @license Public Domain
 */

#include <iostream>
#include <string>
#include <vector>

std::size_t calcOverlap(const std::string &a, const std::string &b, std::size_t limit, std::size_t minimal) {
	std::size_t la = a.size();
	for(std::size_t p = std::min(std::min(la, b.size()), limit + 1); -- p > minimal; ) {
		if(a.compare(la - p, p, b, 0, p) == 0) {
			return p;
		}
	}
	return 0;
}

int main() {
	std::vector<std::string> words;

	// Load all words from input
	while(true) {
		std::string word;
		std::getline(std::cin, word);
		if(word.empty()) {
			break;
		}
		words.push_back(word);
	}

	std::cerr
		<< "Input word count: " << words.size() << std::endl;

	// Remove all fully subsumed words

	for(auto p = words.begin(); p != words.end(); ) {
		bool subsumed = false;
		for(auto i = words.begin(); i != words.end(); ++ i) {
			if(i == p) {
				continue;
			}
			if(i->find(*p) != std::string::npos) {
				subsumed = true;
				break;
			}
		}
		if(subsumed) {
			p = words.erase(p);
		} else {
			++ p;
		}
	}

	std::cerr
		<< "After subsuming checks: " << words.size()
		<< std::endl;

	// Sort words longest-to-shortest (not necessary but doesn't hurt. Makes finding maxlen a tiny bit easier)
	std::sort(words.begin(), words.end(), [](const std::string &a, const std::string &b) {
		return a.size() > b.size();
	});

	std::size_t maxlen = words.front().size();

	// Repeatedly combine most-compatible words until there is only one left
	std::size_t bestPossible = maxlen - 1;
	while(words.size() > 1) {
		auto bestA = words.begin();
		auto bestB = -- words.end();
		std::size_t bestOverlap = 0;
		for(auto p = ++ words.begin(), e = words.end(); p != e; ++ p) {
			if(p->size() - 1 <= bestOverlap) {
				continue;
			}
			for(auto q = words.begin(); q != p; ++ q) {
				std::size_t overlap = calcOverlap(*p, *q, bestPossible, bestOverlap);
				if(overlap > bestOverlap) {
					bestA = p;
					bestB = q;
					bestOverlap = overlap;
				}
				overlap = calcOverlap(*q, *p, bestPossible, bestOverlap);
				if(overlap > bestOverlap) {
					bestA = q;
					bestB = p;
					bestOverlap = overlap;
				}
			}
			if(bestOverlap == bestPossible) {
				break;
			}
		}
		std::string newStr = std::move(*bestA);
		newStr.append(*bestB, bestOverlap, std::string::npos);

		if(bestA == -- words.end()) {
			words.pop_back();
			*bestB = std::move(words.back());
			words.pop_back();
		} else {
			*bestB = std::move(words.back());
			words.pop_back();
			*bestA = std::move(words.back());
			words.pop_back();
		}

		// No need to check for newly subsumed words; since we choose the
		// longest overlap every time and removed subsumed words in advance,
		// we can never get new subsumed words

		std::cerr
			<< "Words remaining: " << (words.size() + 1)
			<< " Latest combination: (" << bestOverlap << ") " << newStr
			<< std::endl;

		words.push_back(std::move(newStr));
		bestPossible = bestOverlap; // Merging existing words will never make longer merges possible
	}

	std::string result = words.front();

	std::cout
		<< result
		<< std::endl;
	std::cerr
		<< "Word size: " << result.size()
		<< std::endl;
	return 0;
}

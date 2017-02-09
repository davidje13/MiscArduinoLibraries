/*
 * Written in 2017 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef PATTERN_H_INCLUDED
#define PATTERN_H_INCLUDED

#include "PatternData.h"

#include <ProgMem.h>

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

typedef const uint8_t* Pattern;
//typedef ProgMem<uint8_t> Pattern; // Uses more program space (inferior compilation); why?

[[gnu::pure,nodiscard,gnu::always_inline]]
inline uint8_t PatternCol(Pattern p, uint8_t x) {
	return ProgMem<uint8_t>(p)[x & 3];
}

#define PATTERN_FILL75  PATTERN_FILL75_11
#define PATTERN_FILL87  PATTERN_FILL87_22
#define PATTERN_FILL94  PATTERN_FILL94_41
#define PATTERN_GRID    PATTERN_GRID_41
#define PATTERN_GRID_SM PATTERN_GRID_SM_22
#define PATTERN_HATCH   PATTERN_HATCH_32

#endif

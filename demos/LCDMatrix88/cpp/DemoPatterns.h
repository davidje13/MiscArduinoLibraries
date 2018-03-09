/*
 * Written in 2018 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef DEMOPATTERNS_H_INCLUDED
#define DEMOPATTERNS_H_INCLUDED

#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>
#include <ProgMem.h>

struct PatternDetails {
	BlendMode blend;
	Pattern pattern;
};

template <typename Bitmask>
void demoPatternSwatch(
	Bitmask &bitmask,
	PatternDetails a
) {
	int w = bitmask.width();
	int h = bitmask.height();

	bitmask.clear();
	bitmask.fill_rect(
		0, 0, w, h,
		a.blend, a.pattern
	);
}

template <typename Display>
void demoPatterns(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;
	
	const static PROGMEM PatternDetails samples[] = {
		{BlendMode::ON, PATTERN_ON},
		{BlendMode::ON, PATTERN_GRID},
		{BlendMode::ON, PATTERN_GRID_SM},
		{BlendMode::ON, PATTERN_HLINES_SM},
		{BlendMode::ON, PATTERN_VLINES_SM},
		{BlendMode::ON, PATTERN_FILL94},
		{BlendMode::ON, PATTERN_FILL75},
		{BlendMode::ON, PATTERN_CHECKER},
		{BlendMode::ON, PATTERN_FILL87},
		{BlendMode::OFF, PATTERN_FILL94},
		{BlendMode::OFF, PATTERN_GRID},
		{BlendMode::OFF, PATTERN_FILL75},
		{BlendMode::OFF, PATTERN_GRID_SM},
		{BlendMode::OFF, PATTERN_FILL87},
		{BlendMode::OFF, PATTERN_HLINES_2}
	};
	ProgMem<PatternDetails> samplesP(samples);

	for(uint8_t i = 0; i < sizeof(samples) / sizeof(*samples); ++ i) {
		demoPatternSwatch(bitmask, samplesP[i]);
		for(uint8_t r = 0; r < 10; ++ r) {
			display.send(bitmask);
		}
	}
}

#endif

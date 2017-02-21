/*
 * These demo sources are based on work from Hobbytronics, in turn based on
 * work from Sparkfun. The sources have been significantly re-written to work
 * with the alternative SSD1306 library and implement general improvements in
 * structure and capability. See the accompanying LICENSE file for details.
 *
 * These demo sources are released under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEMOPATTERNS_H_INCLUDED
#define DEMOPATTERNS_H_INCLUDED

#include "DemoUtils.h"

#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>
#include <ProgMem.h>

struct PatternDetails {
	BlendMode blend;
	Pattern pattern;
};

template <typename Display, typename Bitmask>
void demoPatternSwatch(
	Display &display,
	Bitmask &bitmask,
	PatternDetails a,
	PatternDetails b
) {
	int w = bitmask.width();
	int h = bitmask.height();
	int margin = 4;

	bitmask.clear();
	bitmask.fill_rect(
		margin, margin, w / 2 - margin * 2, h - margin * 2,
		a.blend, a.pattern
	);
	bitmask.fill_ellipse(
		w / 2 + margin, margin, w / 2 - margin * 2, h - margin * 2,
		b.blend, b.pattern
	);

	display.send(bitmask);
}

template <typename Display>
void demoSwatches(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;
	
	const static PROGMEM PatternDetails samples[] = {
		{BlendMode::ON, PATTERN_ON},
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

	for(uint8_t i = 0; i < sizeof(samples) / sizeof(*samples); i += 2) {
		demoPatternSwatch(display, bitmask, samplesP[i], samplesP[i+1]);
		delay((i == 0) ? 200 : 800);
	}
}

template <typename Display, typename Bitmask>
void demoPatternShade(
	Display &display,
	Bitmask &bitmask,
	ProgMem<PatternDetails> details
) {
	int w = bitmask.width();
	int h = bitmask.height();
	uint32_t t0 = micros();
	uint8_t fps = display.fps();
	for(uint8_t i = 0; i < 20; ++ i) {
		PatternDetails detail = details[i & 3];
		bitmask.fill_rect(0, 0, w, h, detail.blend, detail.pattern);
		display.send(bitmask);

		// try to clamp to vertical blank
		// (pin for detecting this isn't exposed by hardware, so we have to
		// guess based on the expected framerate)
		int32_t d = t0 + (uint32_t(i) * 1000000) / fps;
		d -= micros();
		if(d > 0) {
			delayMicroseconds(d);
		}
	}
}

template <typename Display>
void demoShades(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;

	const static PROGMEM PatternDetails shades[] = {
		{BlendMode::OFF, PATTERN_ON},
		{BlendMode::OFF, PATTERN_ON},
		{BlendMode::OFF, PATTERN_ON},
		{BlendMode::OFF, PATTERN_ON},

		{BlendMode::OFF, PATTERN_FILL94_11},
		{BlendMode::OFF, PATTERN_FILL94_32},
		{BlendMode::OFF, PATTERN_FILL94_23},
		{BlendMode::OFF, PATTERN_FILL94_44},

		{BlendMode::OFF, PATTERN_FILL87_11},
		{BlendMode::OFF, PATTERN_FILL87_22},
		{BlendMode::OFF, PATTERN_FILL87_31},
		{BlendMode::OFF, PATTERN_FILL87_42},

		{BlendMode::OFF, PATTERN_FILL75_11},
		{BlendMode::OFF, PATTERN_FILL75_12},
		{BlendMode::OFF, PATTERN_FILL75_21},
		{BlendMode::OFF, PATTERN_FILL75_22},

		{BlendMode::ON,  PATTERN_CHECKER},
		{BlendMode::OFF, PATTERN_CHECKER},
		{BlendMode::ON,  PATTERN_CHECKER},
		{BlendMode::OFF, PATTERN_CHECKER},

		{BlendMode::ON,  PATTERN_FILL75_11},
		{BlendMode::ON,  PATTERN_FILL75_12},
		{BlendMode::ON,  PATTERN_FILL75_21},
		{BlendMode::ON,  PATTERN_FILL75_22},

		{BlendMode::ON,  PATTERN_FILL87_11},
		{BlendMode::ON,  PATTERN_FILL87_22},
		{BlendMode::ON,  PATTERN_FILL87_31},
		{BlendMode::ON,  PATTERN_FILL87_42},

		{BlendMode::ON,  PATTERN_FILL94_11},
		{BlendMode::ON,  PATTERN_FILL94_32},
		{BlendMode::ON,  PATTERN_FILL94_23},
		{BlendMode::ON,  PATTERN_FILL94_44},

		{BlendMode::ON,  PATTERN_ON},
		{BlendMode::ON,  PATTERN_ON},
		{BlendMode::ON,  PATTERN_ON},
		{BlendMode::ON,  PATTERN_ON}
	};
	ProgMem<PatternDetails> shadesP(shades);
	uint8_t nShades = sizeof(shades) / (sizeof(*shades) * 4);

	for(uint8_t i = 0; i < nShades; ++ i) {
		demoPatternShade(display, bitmask, shadesP + (i * 4));
	}
	for(uint8_t i = nShades; (i --) > 0;) {
		demoPatternShade(display, bitmask, shadesP + (i * 4));
	}
}

template <typename Display>
void demoPatterns(Display &display) {
	showTitle(display, ProgMemString("Patterns!"));
	delay(1500);

	demoSwatches(display);
	demoShades(display);
}

template <typename Display>
void testSendPart(Display &display) {
	Bitmask18<32,32> bitmask1;
	Bitmask18<30,30> bitmask2;
	Bitmask18<4,4> bitmask3;
	display.send(true);

	bitmask1.fill_rect(0, 0, 32, 32, BlendMode::ON, PATTERN_HATCH);
	bitmask2.fill_rect(0, 0, 30, 30, BlendMode::ON, PATTERN_BRICKS);
	bitmask3.fill_rect(0, 0, 4, 4, BlendMode::ON, PATTERN_CHECKER);

	for(int y = 8; y < 24; ++ y) {
		for(int x = 4; x < 12; ++ x) {
			display.send(true);
			display.send(bitmask1, x, y);
			display.send(bitmask2, x + 48, y);
			display.send(bitmask3, x + 96, y);
			delay(200);
		}
		++ y;
		for(int x = 12; (x --) > 4;) {
			display.send(true);
			display.send(bitmask1, x, y);
			display.send(bitmask2, x + 48, y);
			display.send(bitmask3, x + 96, y);
			delay(200);
		}
	}
}

#endif

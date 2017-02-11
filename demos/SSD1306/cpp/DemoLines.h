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

#ifndef DEMOLINES_H_INCLUDED
#define DEMOLINES_H_INCLUDED

#include "DemoUtils.h"

#include <Bitmask18.h>
#include <FastMath.h>

template <typename Display>
void demoLines(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;

	printTitle(bitmask, ProgMemString("Lines!"));
	display.send(bitmask);
	delay(1500);

	bitmask.clear();

	uint8_t cx = bitmask.width() / 2;
	uint8_t cy = bitmask.height() / 2;
	uint8_t radius = min(cx, cy) - 2;

	uint8_t shift = 4;
	uint16_t mcx = (cx << shift) + (1 << shift) / 2;
	uint16_t mcy = (cy << shift) + (1 << shift) / 2;

	for(uint8_t i = 0; i < 4; ++ i) {
		BlendMode m = (i % 2) ? BlendMode::OFF : BlendMode::ON;
		for(uint8_t p = 0; p < 72; ++ p) {
			uint8_t angle = (p * 256) / 72;
			bitmask.line(
				mcx, mcy,
				mcx + cos8_mult_shift(angle, radius, shift),
				mcy + sin8_mult_shift(angle, radius, shift),
				m, PATTERN_ON, shift
			);
			display.send(bitmask);
			delay(1);
		}
	}
}

template <typename Display>
void testLines(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;

	uint8_t cx = bitmask.width() / 2;
	uint8_t cy = bitmask.height() / 2;
	uint8_t radius = min(cx, cy) / 2;

	uint8_t shift = 4;
	uint16_t mcx = (cx << shift) + (1 << shift) / 2;
	uint16_t mcy = (cy << shift) + (1 << shift) / 2;

	for(uint16_t p = 0; p < 2000; ++ p) {
		uint8_t angle = (uint32_t(p) * 256) / 2000;
		bitmask.clear();
		bitmask.line(
			mcx, mcy,
			mcx + cos8_mult_shift(angle, radius, shift),
			mcy + sin8_mult_shift(angle, radius, shift),
			BlendMode::ON, PATTERN_ON, shift
		);
		display.send(bitmask);
		delay(1);
	}
}

#endif

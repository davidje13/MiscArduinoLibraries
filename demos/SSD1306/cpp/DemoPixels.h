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

#ifndef DEMOPIXELS_H_INCLUDED
#define DEMOPIXELS_H_INCLUDED

#include "DemoUtils.h"
#include "Strings.h"

#include <Bitmask18.h>

template <typename Display>
void demoPixels(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;

	printTitle(bitmask, TITLE_PIXELS);
	display.send(bitmask);
	delay(1500);

	uint8_t w = bitmask.width();
	uint8_t h = bitmask.height();

	for(uint16_t i = 0; i < 2048; ++ i) {
		uint8_t x = random(w);
		uint8_t y = random(h);
		bitmask.pixel_on(x, y);
		display.send_part(bitmask, x, y, 1, 1);
	}
}

#endif

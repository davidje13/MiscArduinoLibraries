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

#ifndef DEMOBITMAPS_H_INCLUDED
#define DEMOBITMAPS_H_INCLUDED

#include "DemoUtils.h"

#include <StaticBitmask18.h>
#include <ProgMem.h>

#include "logo.h"

template <typename Display>
void demoBitmaps(Display &display) {
	showTitle(display, ProgMemString("Bitmaps!"));
	delay(1500);

	display.send(WrapStaticBitmask18(MakeProgMem(LOGO), 128, 64));
	delay(1500);
}

template <typename Display>
void demoCommands(Display &display) {
	showTitle(display, ProgMemString("Commands!"));
	delay(1500);

	auto logo = WrapStaticBitmask18(MakeProgMem(LOGO), 128, 64);

	display.send(logo);
	delay(500);

	display.set_x_flip(true);
	display.send(logo);
	delay(500);
	display.set_y_flip(true);
	delay(500);
	display.set_x_flip(false);
	display.send(logo);
	delay(500);
	display.set_y_flip(false);
	delay(500);

	for(uint8_t i = 0x80; i < 0xFF; ++ i) {
		display.set_contrast(i);
		delay(2);
	}
	for(uint8_t i = 0xFF; i > 0x00; -- i) {
		display.set_contrast(i);
		delay(2);
	}
	for(uint8_t i = 0x00; i < 0x7F; ++ i) {
		display.set_contrast(i);
		delay(2);
	}
	display.set_contrast(0x7F);

	delay(500);

	display.marquee(
		2, // framerate
		1, // xspeed
		1, // yspeed
		0,                          // horizontal_top_page,
		display.height_bytes() - 1, // horizontal_base_page,
		0,                          // vertical_top,
		display.height()            // vertical_base
	);
	delay(4000);
	display.stop_marquee();

	display.set_fs_white(true);
	for(int i = 64; i >= 16; i -= 2) {
		display.set_displayed_height(i);
		display.set_vertical_offset_b(-(64 - i) / 2);
		delay(100);
	}
	display.send(false);
	display.set_fs_white(false);
	display.set_displayed_height(64);
	display.set_vertical_offset_b(0);
}

#endif

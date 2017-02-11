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

#ifndef DEMOUTILS_H_INCLUDED
#define DEMOUTILS_H_INCLUDED

#include "Font1.h"

#include <Bitmask18.h>
#include <Font.h>
#include <FontRenderer.h>

template <typename Bitmask, typename String>
void printTitle(Bitmask &bitmask, String title) {
	Font f1(F1DATA, F1IMG, F1MASK);

	uint16_t w = f1.measure(title);
	uint16_t h = f1.height();

	auto r = MakeFontRenderer(
		&bitmask,
		(bitmask.width() - w) / 2, (bitmask.height() - h) / 2,
		w + 1
	);

	bitmask.clear();
	r.print(f1, title);
}

template <typename Display, typename String>
void showTitle(Display &display, String title) {
//	Bitmask18<display.width(),16> bitmask;
	Bitmask18<display.width(),display.height()> bitmask;

	printTitle(bitmask, title);
	if(
		bitmask.height() < display.height() ||
		bitmask.width() < display.width()
	) {
		display.send(false);
	}
	display.send(
		bitmask,
		(display.width() - bitmask.width()) / 2,
		(display.height() - bitmask.height()) / 2
	);
}

template <typename Display, typename T>
void showNumber(Display &display, T value) {
	Bitmask18<display.width(),display.height()> bitmask;

	Font f1(F1DATA, F1IMG, F1MASK);

	uint16_t h = f1.height();

	auto r = MakeFontRenderer(
		&bitmask,
		10, (bitmask.height() - h) / 2,
		bitmask.width() - 20
	);

	bitmask.clear();
	r.print_number(f1, value);
	display.send(
		bitmask,
		(display.width() - bitmask.width()) / 2,
		(display.height() - bitmask.height()) / 2
	);
}

#endif

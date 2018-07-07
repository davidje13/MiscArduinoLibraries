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

#ifndef DEMOTEXT_H_INCLUDED
#define DEMOTEXT_H_INCLUDED

#include "DemoUtils.h"
#include "Font0.h"
#include "Font1.h"
#include "Font2.h"
#include "Font3.h"
#include "Strings.h"

#include <Bitmask18.h>
#include <Font.h>
#include <FontVariable.h>
#include <FontRenderer.h>

template <typename Display, typename Bitmask>
void demoCharacters(Display &display, Bitmask &bitmask, const Font &f) {
	bitmask.clear();
	auto r = MakeFontRenderer(&bitmask, 0, 0, bitmask.width(), 0);
	for(uint8_t i = f.first_supported_char(); i <= f.last_supported_char(); ++ i) {
		r.print(f, char(i));
		if(r.cursor_y() + f.line_height() > bitmask.height()) {
			delay(500);
			bitmask.clear();
			r.move(0, 0, bitmask.width(), 0);
			r.print(f, char(i));
		}
		display.send(bitmask);
		delay(10);
	}
	delay(500);
}

template <typename Display, typename Bitmask, typename Message>
void demoMessage(
	Display &display,
	Bitmask &bitmask,
	const Font &f1,
	const Font &f2,
	Message message
) {
	auto r = MakeFontRenderer(&bitmask, 0, 0, bitmask.width(), 0);
	bitmask.clear();
	r.print(f1, message);
	r.print(f1, '\n');
	r.print(f2, message);
	display.send(bitmask);
	delay(1000);
}

template <typename Display>
void demoText(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;

	printTitle(bitmask, TITLE_TEXT);
	display.send(bitmask);
	delay(1500);

	auto f0 = MakeFont0();
	auto f1 = MakeFont1();
	auto f2 = MakeFont2();
	auto f3 = MakeFont3();
	auto fVar = MakeFontVariable();

	demoCharacters(display, bitmask, f0);
	demoCharacters(display, bitmask, fVar);

	{
		demoMessage(display, bitmask, f1, fVar, TEXT_SAMPLE_1);
		demoMessage(display, bitmask, f1, fVar, TEXT_SAMPLE_2);
		demoMessage(display, bitmask, f1, fVar, TEXT_SAMPLE_3);

		bitmask.clear();
		auto r = MakeFontRenderer(&bitmask, 0, 0, bitmask.width(), 0);
		for(uint8_t i = 0x20; i <= 0x40; ++ i) {
			r.print(f1, char(i));
		}
		r.print(f1, TEXT_FINAL_CHARS);
		display.send(bitmask);
		delay(1000);
	}

	for(int i = 0; i < 25; ++ i) {
		bitmask.clear();

		auto r = MakeFontRenderer(&bitmask, 0, 0, bitmask.width(), 0);
		r.set_yshift(4);
		r.print(f0, TEXT_A0);
		r.set_yshift(0);
		r.print_number(f2, analogRead(A0), 4);
		r.set_yshift(8);
		r.print(f0, TEXT_PER1024);
		r.set_yshift(0);

		r.move(0, 20, bitmask.width(), 0);
		r.set_yshift(4);
		r.print(f0, TEXT_A1);
		r.set_yshift(0);
		r.print_number(f2, analogRead(A1), 4);
		r.set_yshift(8);
		r.print(f0, TEXT_PER1024);
		r.set_yshift(0);

		r.move(0, 40, bitmask.width(), 0);
		r.set_yshift(4);
		r.print(f0, TEXT_A2);
		r.set_yshift(0);
		r.print_number(f2, analogRead(A2), 4);
		r.set_yshift(8);
		r.print(f0, TEXT_PER1024);
		r.set_yshift(0);

		display.send(bitmask);
		delay(100);
	}

	{
		uint32_t countdown = 3000;
		auto r = MakeFontRenderer(&bitmask, 0, 0, bitmask.width(), 0);
		uint32_t start = millis();
		while(true) {
			uint32_t posn = millis() - start;
			if(posn > countdown) {
				break;
			}
			uint16_t remainingCents = (countdown - posn) / 10;
			bitmask.clear();
			r.move(40, 5, bitmask.width() - 50, 0);
			r.print_number(f3, remainingCents / 100, 2);
			r.set_yshift(16);
			r.print(f1, ':');
			r.set_yshift(0);
			r.print_number(f3, remainingCents % 100, 2);
			display.send(bitmask);
			delay(1);
		}
	}
}

template <typename Display, typename Bitmask>
static void _testText(Display &display, Bitmask &bitmask, int x, int y) {
	auto f0 = MakeFont0();
	auto f1 = MakeFont1();
	auto f2 = MakeFont2();
	auto f3 = MakeFont3();

	bitmask.clear();
//	bitmask.fill_rect(0, 0, bitmask.width(), bitmask.height(), BlendMode::ON, PATTERN_CHECKER);

	auto r = MakeFontRenderer(&bitmask, x, y, bitmask.width() - x, 0);
	r.print(f0, TEXT_F0_SHORT);
	r.print(f1, TEXT_F1_SHORT);
	r.print(f2, TEXT_F2_SHORT);
	r.print(f3, TEXT_F3_SHORT);
	display.send(bitmask);
//	delay(500);
}

template <typename Display>
void testText(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;

	for(int y = 0; y < 8; ++ y) {
		for(int x = 8; x < 16; ++ x) {
			_testText(display, bitmask, x, y);
		}
		++ y;
		for(int x = 16; (x --) > 8;) {
			_testText(display, bitmask, x, y);
		}
	}
}

#endif

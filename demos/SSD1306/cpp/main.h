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

/*
 * Hardware connections:
 *
 *   GND -- GND
 *    +V -- 3.3V
 *   DAT -- D11 (MOSI)
 *   CLK -- D13 (SCK)
 *    DC -- D* (any available pin; set below)
 *   RST -- D* (any available pin; set below)
 *    CS -- D* (any available pin; set below)
 */

#include <SSD1306.h>
#include <ArduinoPin.h>
#include <ArduinoAnalogPin.h>
#include <ArduinoSPI.h>
#include <DemoBitmaps.h>
#include <DemoPixels.h>
#include <DemoLines.h>
#include <DemoPong.h>
#include <DemoPatterns.h>
#include <DemoText.h>
#include <DemoClock.h>
#include <Demo3D.h>

// These can be set to any available pin
#define OLED_PIN_CS  FixedArduinoPin<10>()
#define OLED_PIN_RST FixedArduinoPin<7>()
#define OLED_PIN_DC  FixedArduinoPin<9>()

// To play PONG, attach potentiometers to A0 and A1
#define PONG_POT_L FixedArduinoAnalogPin<0>()
#define PONG_POT_R FixedArduinoAnalogPin<1>()

__attribute__((section(".noinit")))
static volatile uint16_t randomMemory;

void setup(void) {
	randomSeed(
		(analogRead(0) << 0) ^
		(analogRead(1) << 1) ^
		(analogRead(2) << 2) ^
		(analogRead(3) << 3) ^
		(ADCW << 6) ^ // internal temperature sensor
		randomMemory
	);
	randomMemory = random();

	auto oled = MakeSSD1306<128,64>(
		ArduinoSPI(),
		OLED_PIN_CS,
		OLED_PIN_RST,
		OLED_PIN_DC
	);
	oled.set_on(true);

	while(true) {
		demoBitmaps(oled);
		demoCommands(oled);
		demoPixels(oled);
		demoLines(oled);
		demoPong(oled, PONG_POT_L, PONG_POT_R);
		demoPatterns(oled);
		demoText(oled);
		demoClock(oled);
		demo3D(oled);
	}
}

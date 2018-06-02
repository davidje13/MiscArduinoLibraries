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

#ifndef DEMOSHAPES_H_INCLUDED
#define DEMOSHAPES_H_INCLUDED

#include "DemoUtils.h"
#include "Font0.h"
#include "Strings.h"

#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>
#include <SceneRenderer.h>

template <typename Display, typename BatLPinT, typename BatRPinT>
void demoPong(Display &display, BatLPinT batL, BatRPinT batR) {
	Bitmask18<display.width(),display.height()> bitmask;

	printTitle(bitmask, TITLE_PONG);
	display.send(bitmask);
	delay(1500);

	int w = bitmask.width();
	int h = bitmask.height();

	int shift = 6;
	vector2d<int16_t> pBall((w / 2) << shift, (h / 2) << shift);
	vector2d<int16_t> vBall(0, 0);
	uint8_t scoreL = 0;
	uint8_t scoreR = 0;

	int ballSize = 4;

	int batSize = 15;
	int batWidth = 3;
	int batLift = 2;
	uint8_t respawnDelay = 15;
	uint8_t startDelay = 10;
	uint8_t maxScore = 3;

	int batLPos = h / 2;
	int batRPos = h / 2;
	int16_t topBounce = (ballSize / 2) << shift;
	int16_t baseBounce = (h - ballSize / 2) << shift;
	int16_t leftBounce = (batLift + batWidth + ballSize / 2) << shift;
	int16_t rightBounce = (w - batLift - batWidth - ballSize / 2) << shift;
	bool hadChance = false;
	uint8_t respawn = startDelay;
	bool startRight = random(2);

	auto f0 = MakeFont0();

	while(true) {
		if(batL.supports_analog_input()) {
			batLPos = batL.read_analog(h - batSize) + batSize / 2;
		} else {
			batLPos = pBall.y >> shift;
		}
		if(batR.supports_analog_input()) {
			batRPos = batR.read_analog(h - batSize) + batSize / 2;
		} else {
			batRPos = pBall.y >> shift;
		}
		pBall += vBall;
		if(pBall.y < topBounce && vBall.y < 0) {
			pBall.y = topBounce * 2 - pBall.y;
			// y *= -1.1
			vBall.y = -vBall.y;
			vBall.y += vBall.y / 10;
		} else if(pBall.y > baseBounce && vBall.y > 0) {
			pBall.y = baseBounce * 2 - pBall.y;
			// y *= -1.1
			vBall.y = -vBall.y;
			vBall.y += vBall.y / 10;
		}
		if(pBall.x < leftBounce && vBall.x < 0) {
			if(
				!hadChance &&
				(pBall.y >> shift) >= batLPos - (batSize + ballSize) / 2 &&
				(pBall.y >> shift) < batLPos + (batSize + ballSize) / 2
			) {
				pBall.x = leftBounce * 2 - pBall.x;
				// x *= -1.1
				vBall.x = -vBall.x;
				vBall.x += vBall.x / 10;
			}
			hadChance = true;
		} else if(pBall.x > rightBounce && vBall.x > 0) {
			if(
				!hadChance &&
				(pBall.y >> shift) >= batRPos - (batSize + ballSize) / 2 &&
				(pBall.y >> shift) < batRPos + (batSize + ballSize) / 2
			) {
				pBall.x = rightBounce * 2 - pBall.x;
				// x *= -1.1
				vBall.x = -vBall.x;
				vBall.x += vBall.x / 10;
			}
			hadChance = true;
		} else {
			hadChance = false;
		}

		if(respawn > 0) {
			-- respawn;
			if(respawn == startDelay) {
				if(scoreL >= maxScore || scoreR >= maxScore) {
					// Game over
					return;
				}
				pBall.x = (w / 2) << shift;
				pBall.y = (h / 2) << shift;
				vBall.x = 0;
				vBall.y = 0;
			} else if(respawn == 0) {
				vBall.x = (1 << shift) * (startRight ? 1 : -1);
				vBall.y = (1 << shift) * (random(2) ? 1 : -1);
			}
		} else if(pBall.x < 0) {
			++ scoreR;
			startRight = true;
			respawn = respawnDelay + startDelay;
		} else if(pBall.x > (w << shift)) {
			++ scoreL;
			startRight = false;
			respawn = respawnDelay + startDelay;
		}

		bitmask.clear(false);
		bitmask.fill_rect(
			w / 2 - 1, 0, 2, h,
			BlendMode::ON, PATTERN_CHECKER
		);
		char scoreLCh = '0' + scoreL;
		f0.render(bitmask, scoreLCh, w / 2 - f0.measure(scoreLCh) - 2, 0);
		f0.render(bitmask, char('0' + scoreR), w / 2 + 2, 0);
		bitmask.fill_ellipse(
			(pBall.x >> shift) - ballSize / 2,
			(pBall.y >> shift) - ballSize / 2,
			ballSize, ballSize,
			BlendMode::XOR
		);
		bitmask.fill_rect(
			batLift, batLPos - batSize / 2,
			batWidth, batSize
		);
		bitmask.fill_rect(
			w - batLift - batWidth, batRPos - batSize / 2,
			batWidth, batSize
		);
		display.send(bitmask);
		delay(1000 / 50);
	}
}

template <typename Display, typename Bitmask>
static void _testEllipses(Display &display, Bitmask &bitmask, int x, int y) {
	int cx1 = (bitmask.width() * 1) / 8;
	int cx2 = (bitmask.width() * 3) / 8;
	int cx3 = (bitmask.width() * 5) / 8;
	int cx4 = (bitmask.width() * 7) / 8;
	int cy = bitmask.height() / 2;
	int cyT = bitmask.height() / 8;
	int cyB = (bitmask.height() * 7) / 8;

	bitmask.clear();

	bitmask.outline_ellipse(cx1 - x / 2, cy - y / 2, x, y, 1, BlendMode::ON);
	bitmask.outline_ellipse(cx1 - x / 2, cyT - 1, x, 3, 1, BlendMode::ON);
	bitmask.outline_ellipse(cx1 - x / 2, cyB - 2, x, 5, 1, BlendMode::ON);
	bitmask.outline_ellipse(cx2 - x / 2, cy - y / 2, x, y, 1, BlendMode::XOR);
	bitmask.outline_ellipse(cx2 - x / 2, cyT - 1, x, 3, 1, BlendMode::XOR);
	bitmask.outline_ellipse(cx2 - x / 2, cyB - 2, x, 5, 1, BlendMode::XOR);
	bitmask.fill_ellipse(cx3 - x / 2, cy - y / 2, x, y, BlendMode::ON);
	bitmask.fill_ellipse(cx3 - x / 2, cyT - 1, x, 3, BlendMode::ON);
	bitmask.fill_ellipse(cx3 - x / 2, cyB - 2, x, 5, BlendMode::ON);
//	bitmask.fill_rect(cx4 - x / 2, cy - y / 2, x, y, BlendMode::ON);
	bitmask.fill_ellipse(cx4 - x / 2, cy - y / 2, x, y, BlendMode::XOR);
	bitmask.fill_ellipse(cx4 - x / 2, cyT - 1, x, 3, BlendMode::XOR);
	bitmask.fill_ellipse(cx4 - x / 2, cyB - 2, x, 5, BlendMode::XOR);

	display.send(bitmask);
	delay(50);
}

template <typename Display>
void testEllipses(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;

	for(int x = 3; x < 32; x += 2) {
		int y;
		for(y = 3; y < 32; y += 2) {
			_testEllipses(display, bitmask, x, y);
		}
		x += 2;
		for(; y > 1; y -= 2) {
			_testEllipses(display, bitmask, x, y);
		}
	}

	for(int x = 2; x < 32; x += 2) {
		int y;
		for(y = 2; y < 32; y += 2) {
			_testEllipses(display, bitmask, x, y);
		}
		x += 2;
		for(; y > 1; y -= 2) {
			_testEllipses(display, bitmask, x, y);
		}
	}
}

#endif

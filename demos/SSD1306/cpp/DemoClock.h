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

#ifndef DEMOCLOCK_H_INCLUDED
#define DEMOCLOCK_H_INCLUDED

#include "DemoUtils.h"

#include <Bitmask18.h>
#include <BlendMode.h>
#include <Font.h>
#include <Font0.h>
#include <FastMath.h>

__attribute__((section(".noinit")))
static volatile uint16_t clock_time;

class TimeKeeperModeSharp {
public:
	[[gnu::always_inline,gnu::const,nodiscard]]
	static inline uint8_t second_adjustment(uint16_t, uint16_t) {
		return 0;
	}
};

class TimeKeeperModeSweep {
public:
	[[gnu::always_inline,gnu::const,nodiscard]]
	static inline uint8_t second_adjustment(uint16_t pos, uint16_t lim) {
		return (pos * 256) / lim;
	}
};

template <int bounceFrac = 10, uint8_t velocity = 6>
class TimeKeeperModeBounce {
public:
	[[gnu::always_inline,gnu::const,nodiscard]]
	static inline uint8_t second_adjustment(uint16_t pos, uint16_t lim) {
		uint16_t B = lim / bounceFrac;
		if(pos < B * 2) {
			return (uint32_t(B * 2 - pos) * pos * velocity * 128) / (B * lim);
		} else {
			uint32_t t = pos;
			for(uint8_t i = 1; i < velocity; ++ i) {
				t = (t * pos) / lim;
			}
			return (t * 256) / lim;
		}
	}
};

template <
	typename Mode = TimeKeeperModeSharp,
	typename time_t = uint16_t,
	typename timedelta_t = time_t,
	uint16_t modulo = 43200
>
class TimeKeeper {
	volatile uint16_t *t;
	timedelta_t s;
	timedelta_t overflow;
	time_t nextTm;

public:
	[[gnu::always_inline]]
	inline TimeKeeper(
		volatile uint16_t *time,
		timedelta_t tickSpeed = 1000
	)
		: t(time)
		, s(tickSpeed)
		, overflow(0)
		, nextTm(0)
	{
		*t %= modulo;
	}

	[[gnu::always_inline]]
	inline void begin(time_t now) {
		nextTm = now + s;
		overflow = 0;
	}

	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline uint16_t current_time(void) const {
		return *t;
	}

	template <typename T>
	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline T hour(T range) const {
		return ((current_time() / 3600) * range) / 12;
	}

	template <typename T>
	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline T hour_smooth(T range) const {
		return (current_time() * range) / 43200;
	}

	template <typename T>
	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline T minute(T range) const {
		return (((current_time() / 60) % 60) * range) / 60;
	}

	template <typename T>
	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline T minute_smooth(T range) const {
		return ((current_time() % 3600) * range) / 3600;
	}

	template <typename T>
	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline T second(T range) const {
		return ((current_time() % 60) * range) / 60;
	}

	template <typename T>
	[[gnu::always_inline,gnu::pure,nodiscard]]
	inline T second_smooth(T range) const {
		return ((
			((current_time() % 60) << 8) |
			Mode::second_adjustment(overflow, s)
		) * range) / (60 * 256);
	}

	bool update_time(time_t now) {
		overflow = now + s - nextTm;
		if(overflow < s) {
			return false;
		}

		++ (*t);
		if((*t) >= modulo) {
			(*t) -= modulo;
		}
		nextTm += s;
		overflow -= s;
		return true;
	}
};

class ClockFace {
	uint8_t cx;
	uint8_t cy;
	uint8_t rad;
	uint8_t numRad;
	uint8_t radSec;
	uint8_t radMin;
	uint8_t radHour;
	uint8_t thkHour;

public:
	ClockFace(
		uint8_t centerX,
		uint8_t centerY,
		uint8_t radius
	)
		: cx(centerX)
		, cy(centerY)
		, rad(radius)
		, numRad(rad - 8 / 2 - 2)
		, radSec(numRad - 8 / 2 - 1)
		, radMin(radSec - 3)
		, radHour((radMin * 6) / 10)
		, thkHour(3)
	{}

	template <typename Bitmask>
	void render_back(Bitmask &bitmask) const {
		// Extracting the font makes the executable much larger (>1kB)
		auto f = MakeFont0(BlendMode::OFF);

		uint8_t diameter = rad * 2 + 1;
		bitmask.fill_ellipse(
			cx - rad,
			cy - rad,
			diameter,
			diameter,
			BlendMode::ON,
			PATTERN_ON
		);
		bitmask.outline_ellipse(
			cx - rad,
			cy - rad,
			diameter,
			diameter,
			1,
			BlendMode::OFF,
			PATTERN_ON
		);

		int w12 = f.measure(ProgMemString("12"));
		int w3 = f.measure('3');
		int w6 = f.measure('6');
		int w9 = f.measure('9');
		int numHalfHeight = f.height() / 2;

		f.render(bitmask, ProgMemString("12"), cx - w12 / 2, cy - numRad - numHalfHeight);
		f.render(bitmask, '3', cx + numRad - w3 / 2, cy - numHalfHeight);
		f.render(bitmask, '6', cx - w6 / 2, cy + numRad - numHalfHeight);
		f.render(bitmask, '9', cx - numRad - w9 / 2, cy - numHalfHeight);
	}

	template <typename Bitmask, typename TK>
	void render_hands(Bitmask &bitmask, const TK &tk) const {
		uint8_t shift = 4;
		uint16_t mcx = (cx << shift) + (1 << shift) / 2;
		uint16_t mcy = (cy << shift) + (1 << shift) / 2;
		uint16_t angle;

		angle = tk.hour_smooth(65536);
		bitmask.fill_triangle(
			mcx + sin16_mult_shift(angle + 43691, thkHour, shift),
			mcy - cos16_mult_shift(angle + 43691, thkHour, shift),
			mcx + sin16_mult_shift(angle - 43691, thkHour, shift),
			mcy - cos16_mult_shift(angle - 43691, thkHour, shift),
			mcx + sin16_mult_shift(angle, radHour, shift),
			mcy - cos16_mult_shift(angle, radHour, shift),
			BlendMode::OFF, PATTERN_ON, shift
		);

		angle = tk.minute_smooth(65536);
		bitmask.line(
			mcx, mcy,
			mcx + sin16_mult_shift(angle, radMin, shift),
			mcy - cos16_mult_shift(angle, radMin, shift),
			BlendMode::OFF, PATTERN_ON, shift
		);

		angle = tk.second_smooth(65536);
		bitmask.line(
			mcx, mcy,
			mcx + sin16_mult_shift(angle, radSec, shift),
			mcy - cos16_mult_shift(angle, radSec, shift),
			BlendMode::XOR, PATTERN_ON, shift
		);
	}

	template <typename Bitmask, typename TK>
	[[gnu::always_inline]]
	inline void render(Bitmask &bitmask, const TK &tk) const {
		render_back(bitmask);
		render_hands(bitmask, tk);
	}
};

template <typename Display>
void demoClock(Display &display) {
	Bitmask18<display.width(),display.height()> bitmask;

	printTitle(bitmask, ProgMemString("Clock!"));
	display.send(bitmask);
	delay(1500);

	uint8_t w = bitmask.width();
	uint8_t h = bitmask.height();

	bitmask.fill_rect(0, 0, w, h, BlendMode::ON, PATTERN_CHECKER);
	display.send(bitmask);

	TimeKeeper<TimeKeeperModeBounce<>,uint8_t> tk(&clock_time, 1000 >> 3);
	ClockFace face(
		w / 2, h / 2,
		min(w, h) / 2 - 3
	);

	tk.begin(millis() >> 3);
	for(int ticks = 0; ticks < 10;) {
		face.render(bitmask, tk);
		display.send_part(bitmask, 32, 0, 64, 64);
		delay(1);
		if(tk.update_time(millis() >> 3)) {
			++ ticks;
		}
	}
}

#endif

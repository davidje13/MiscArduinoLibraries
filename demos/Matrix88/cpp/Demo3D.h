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

#ifndef DEMOCUBE_H_INCLUDED
#define DEMOCUBE_H_INCLUDED

#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>
#include <SceneRenderer.h>
#include <Vector.h>
#include <Matrix.h>
#include <Quaternion.h>
#include <ProgMem.h>

/*
 *  x-> y^ z.
 *  2-----6
 *  |\   /|
 *  | 3-7 |
 *  | | | |
 *  | 1-5 |
 *  |/   \|
 *  0-----4
 */

static PROGMEM const int16_t CUBE_VERTICES[] = {
	-1, -1, -1,
	-1, -1,  1,
	-1,  1, -1,
	-1,  1,  1,
	 1, -1, -1,
	 1, -1,  1,
	 1,  1, -1,
	 1,  1,  1
};

static PROGMEM const uint8_t CUBE_LINES[] = {
	0, 1,
	1, 3,
	3, 2,
	2, 0,
	4, 5,
	5, 7,
	7, 6,
	6, 4,
	0, 4,
	1, 5,
	2, 6,
	3, 7
};

static PROGMEM const uint8_t CUBE_TRIANGLES[] = {
	0, 3, 1, 2, // Left
	0, 2, 3, 2,

	2, 7, 3, 4, // Top
	2, 6, 7, 4,

	4, 5, 7, 2, // Right
	4, 7, 6, 2,

	4, 1, 5, 4, // Bottom
	4, 0, 1, 4,

	4, 2, 0, 0, // Front
	4, 6, 2, 0,

	1, 7, 5, 0, // Back
	1, 3, 7, 0
};

template <typename Display>
void demo3D(Display &display) {
	// Render at high resolution. When displaying, the extra data will appear
	// as grayscale (provides antialiasing)
	constexpr uint8_t res = 4;
	constexpr uint8_t w = display.width() * res;
	constexpr uint8_t h = display.height() * res;

	Bitmask18<w, h> bitmask;
	auto verts = MakeProgMem(CUBE_VERTICES);
	auto lines = MakeProgMem(CUBE_LINES);
	auto tris = MakeProgMem(CUBE_TRIANGLES);
	uint16_t nVerts = sizeof(CUBE_VERTICES) / (sizeof(*CUBE_VERTICES) * 3);
	uint16_t nTris = sizeof(CUBE_TRIANGLES) / (sizeof(*CUBE_TRIANGLES) * 3);

	int cx = bitmask.width() / 2;
	int cy = bitmask.height() / 2;
	int zoom = bitmask.height();

	uint8_t shift = 2;
	uint8_t matShift = 8;

	const Material materials[] = {
		{RenderMode::FILL, BlendMode::ON,  PATTERN_ON},
		{RenderMode::FILL, BlendMode::ON,  PATTERN_FILL75},
		{RenderMode::FILL, BlendMode::ON,  PATTERN_CHECKER},
		{RenderMode::FILL, BlendMode::OFF, PATTERN_FILL75},
		{RenderMode::FILL, BlendMode::OFF, PATTERN_ON}
	};

	// Shaded
	for(uint32_t t0 = micros(), time = 0;
		time < 512;
		time = (micros() - t0) >> 14
	) {
		matrix4<int32_t> M = (quaternion<int32_t>(
			vector3d<int32_t>(154, 205, 0), // 0.6, 0.8
			uint8_t((time * 256) / 256),
			(1 << matShift)
		).toMatrix() /= (1 << matShift));

		M.v[11] = 4 << matShift;
		for(uint8_t j = 0; j < 4; ++ j) {
			M.v[j  ] = (M.v[j  ] * zoom + cx * M.v[j+8]) * (1 << shift);
			M.v[j+4] = (M.v[j+4] * zoom + cy * M.v[j+8]) * (1 << shift);
		}

		renderScene(
			display, bitmask,
			verts, nVerts,
			lines, 0,
			tris, nTris,
			M,
			materials,
			0, 0, w, h,
			shift
		);
	}
}

#endif

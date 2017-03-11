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

#include "DemoUtils.h"

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
	0, 3, 1, 0, // Left
	0, 2, 3, 0,

	2, 7, 3, 1, // Top
	2, 6, 7, 1,

	4, 5, 7, 0, // Right
	4, 7, 6, 0,

	4, 1, 5, 3, // Bottom
	4, 0, 1, 3,

	4, 2, 0, 2, // Front
	4, 6, 2, 2,

	1, 7, 5, 2, // Back
	1, 3, 7, 2
};

template <typename Display>
void demoCube(Display &display) {
//	Bitmask18<64,8> bitmask;
	Bitmask18<display.width(),display.height()> bitmask;
	auto verts = MakeProgMem(CUBE_VERTICES);
	auto lines = MakeProgMem(CUBE_LINES);
	auto tris = MakeProgMem(CUBE_TRIANGLES);
	uint16_t nVerts = sizeof(CUBE_VERTICES) / (sizeof(*CUBE_VERTICES) * 3);
	uint16_t nLines = sizeof(CUBE_LINES) / (sizeof(*CUBE_LINES) * 2);
	uint16_t nTris = sizeof(CUBE_TRIANGLES) / (sizeof(*CUBE_TRIANGLES) * 3);

	int cx = display.width() / 2;
	int cy = display.height() / 2;
	int zoom = display.height();

	uint8_t shift = 2;
	uint8_t matShift = 8;

	const Material materials[] = {
		{RenderMode::FILL, BlendMode::ON,  PATTERN_ON},
		{RenderMode::FILL, BlendMode::ON,  PATTERN_FILL75},
		{RenderMode::FILL, BlendMode::ON,  PATTERN_CHECKER},
		{RenderMode::FILL, BlendMode::ON,  PATTERN_BRICKS},
		{RenderMode::FILL, BlendMode::OFF, PATTERN_CHECKER},
		{RenderMode::FILL, BlendMode::OFF, PATTERN_FILL75},
		{RenderMode::FILL, BlendMode::OFF, PATTERN_ON}
	};

	// Wireframe
	for(uint32_t t0 = micros(), time = 0;
		time < 256;
		time = (micros() - t0) >> 14
	) {
		matrix4<int32_t> M = (quaternion<int32_t>(
			vector3d<int32_t>(154, 205, 0), // 0.6, 0.8
			uint8_t((time * 256) / 256),
			(1 << matShift)
		).toMatrix() /= (1 << matShift));

		M.v[11] = 5 << matShift;
		for(uint8_t j = 0; j < 4; ++ j) {
			M.v[j  ] = (M.v[j  ] * zoom + cx * M.v[j+8]) * (1 << shift);
			M.v[j+4] = (M.v[j+4] * zoom + cy * M.v[j+8]) * (1 << shift);
		}

		renderScene(
			display, bitmask,
			verts, nVerts,
			lines, nLines,
			tris, 0,
			M,
			materials,
			0, 0, display.width(), display.height(),
			shift
		);
	}

	// Shaded
	for(uint32_t t0 = micros(), time = 0;
		time < 256;
		time = (micros() - t0) >> 14
	) {
		matrix4<int32_t> M = (quaternion<int32_t>(
			vector3d<int32_t>(154, 205, 0), // 0.6, 0.8
			uint8_t((time * 256) / 256),
			(1 << matShift)
		).toMatrix() /= (1 << matShift));

		M.v[11] = 5 << matShift;
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
			0, 0, display.width(), display.height(),
			shift
		);
	}
}

template <typename Display>
void demo3D(Display &display) {
	showTitle(display, ProgMemString("3D!"));
	delay(1500);

	demoCube(display);
}

#endif

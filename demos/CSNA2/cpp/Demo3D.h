/*
 * Written in 2018 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#include "DemoUtils.h"

#define WIDE_DATA
#include <Bitmask18.h>
#include <BlendMode.h>
#include <Pattern.h>
#include <SceneRenderer.h>
#include <Vector.h>
#include <Matrix.h>
#include <Quaternion.h>

#include <CSNA2.h>
#include <ProgMem.h>

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

template <typename Printer>
void demo3D(Printer &printer) {
	showTitle(printer, ProgMemString("3D!"));

	constexpr uint16_t ww = 128;
	constexpr uint16_t hh = 128;
	bool doubleSize = false;

	Bitmask18<ww, 16> bitmask;
	auto verts = MakeProgMem(CUBE_VERTICES);
	auto lines = MakeProgMem(CUBE_LINES);
	auto tris = MakeProgMem(CUBE_TRIANGLES);
	uint16_t nVerts = sizeof(CUBE_VERTICES) / (sizeof(*CUBE_VERTICES) * 3);
	uint16_t nLines = sizeof(CUBE_LINES) / (sizeof(*CUBE_LINES) * 2);
	uint16_t nTris = sizeof(CUBE_TRIANGLES) / (sizeof(*CUBE_TRIANGLES) * 3);

	int cx = ww / 2;
	int cy = hh / 2;
	int zoom = hh;

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

	const uint16_t time = 96;
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

	printer.awake();
	printer.configure(CSNA2::Configuration::PRECISION);
	if(doubleSize) {
		printer.set_character_size(2, 2);
	}
	renderScene(
		printer, bitmask,
		verts, nVerts,
		lines, nLines,
		tris, 0,
		M,
		materials,
		0, 0, ww, hh,
		shift
	);
	printer.linefeed();

	delay(1000);

	printer.awake();
	renderScene(
		printer, bitmask,
		verts, nVerts,
		lines, 0,
		tris, nTris,
		M,
		materials,
		0, 0, ww, hh,
		shift
	);
	printer.linefeed();

	printer.configure(CSNA2::Configuration::FACTORY);
	printer.set_character_size(1, 1);

	delay(1000);
}

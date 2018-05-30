/*
 * SceneRenderer - Basic 3D rendering utilities.
 * Written in 2017 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef SCENERENDERER_H_INCLUDED
#define SCENERENDERER_H_INCLUDED

#include <BlendMode.h>
#include <Pattern.h>
#include <Vector.h>
#include <alloca.h>

#include <ProgMem.h>

#include "ext.h"

enum class RenderMode : uint8_t {
	OUTLINE,
	FILL
};

class Material {
public:
	RenderMode render;
	BlendMode blend;
	Pattern pattern;

	[[gnu::always_inline]]
	constexpr inline Material(RenderMode r, BlendMode b, Pattern p)
		: render(r)
		, blend(b)
		, pattern(p)
	{}
};

template <typename VertT, typename ProjectorT>
void projectScene(
	ProgMem<VertT> vertices,
	uint16_t nVertices,
	const ProjectorT &projector,
	int16_t *target
) {
	for(uint16_t i = 0; i < nVertices; ++ i) {
		uint16_t p = i * 3;
		projector.project(
			vector3d<VertT>(vertices[p], vertices[p+1], vertices[p+2]),
			target[ i<<1   ],
			target[(i<<1)|1]
		);
	}
}

template <typename Bitmask, typename IndexListT>
void renderLines(
	Bitmask &bitmask,
	const int16_t *projVerts,
	IndexListT lines,
	uint16_t nLines,
	const Material*, // TODO (ideally optional to save space)
	uint16_t dx,
	uint16_t dy,
	uint8_t shift
) {
	for(uint16_t n = 0; n < nLines; ++ n) {
		uint16_t v0 = lines[n*2  ] << 1;
		uint16_t v1 = lines[n*2+1] << 1;
		bitmask.line(
			projVerts[v0] - dx, projVerts[v0|1] - dy,
			projVerts[v1] - dx, projVerts[v1|1] - dy,
			BlendMode::ON, PATTERN_ON, shift
		);
	}
}

template <typename Bitmask, typename IndexListT>
void renderTriangles(
	Bitmask &bitmask,
	const int16_t *projVerts,
	IndexListT triangles,
	uint16_t nTriangles,
	const Material *materials,
	uint16_t dx,
	uint16_t dy,
	uint8_t shift
) {
	for(uint16_t n = 0; n < nTriangles; ++ n) {
		uint16_t v0 = triangles[(n<<2)  ] << 1;
		uint16_t v1 = triangles[(n<<2)|1] << 1;
		uint16_t v2 = triangles[(n<<2)|2] << 1;
		const auto &m = materials[triangles[(n<<2)|3]];
		if(m.render == RenderMode::OUTLINE) {
			bitmask.outline_ccw_triangle(
				projVerts[v0] - dx, projVerts[v0|1] - dy,
				projVerts[v1] - dx, projVerts[v1|1] - dy,
				projVerts[v2] - dx, projVerts[v2|1] - dy,
				m.blend, m.pattern, shift
			);
		} else {
			bitmask.fill_ccw_triangle(
				projVerts[v0] - dx, projVerts[v0|1] - dy,
				projVerts[v1] - dx, projVerts[v1|1] - dy,
				projVerts[v2] - dx, projVerts[v2|1] - dy,
				m.blend, m.pattern, shift
			);
		}
	}
}

template <
	typename Display,
	typename Bitmask,
	typename VertT,
	typename LineIndexListT,
	typename TriangleIndexListT,
	typename ProjectorT
>
void renderScene(
	Display &display,
	Bitmask &bitmask,
	ProgMem<VertT> vertices,
	uint16_t nVertices,
	LineIndexListT lines,
	uint16_t nLines,
	TriangleIndexListT triangles,
	uint16_t nTriangles,
	const ProjectorT &projector,
	const Material *materials,
	uint16_t x0,
	uint16_t y0,
	uint16_t w,
	uint16_t h,
	uint8_t shift
) {
	int16_t *projVerts = alloca(nVertices * 2 * sizeof(int16_t));
	projectScene(vertices, nVertices, projector, projVerts);
	for(uint16_t y = 0; y < h; y += bitmask.height()) {
		for(uint16_t x = 0; x < w; x += bitmask.width()) {
			bitmask.clear();
			renderLines(
				bitmask,
				projVerts,
				lines, nLines,
				materials,
				uint16_t(x) << shift,
				uint16_t(y) << shift,
				shift
			);
			renderTriangles(
				bitmask,
				projVerts,
				triangles, nTriangles,
				materials,
				uint16_t(x) << shift,
				uint16_t(y) << shift,
				shift
			);
			display.send(bitmask, x + x0, y + y0);
		}
	}
}

#endif

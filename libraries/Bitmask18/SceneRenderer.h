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
 *
 * Attributions / Acknowledgements: (also noted in source)
 * - Quaternion-to-matrix calculations from euclideanspace.com, written by
 *   Martin John Baker
 */

#ifndef SCENERENDERER_H_INCLUDED
#define SCENERENDERER_H_INCLUDED

#include <BlendMode.h>
#include <Pattern.h>
#include <ProgMem.h>
#include <FastMath.h>
#include <alloca.h>

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

template <typename T>
struct vector2d {
	T x;
	T y;

	vector2d(T xx, T yy) : x(xx), y(yy) {}

	void normalize(T norm = 1) {
		T mult = norm / sqrt(x * x + y * y);
		x *= mult;
		y *= mult;
	}

	vector2d &operator+=(const vector2d<T> &b) {
		x += b.x;
		y += b.y;
		return *this;
	}
};

template <typename T>
struct vector3d {
	T x;
	T y;
	T z;

	vector3d(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

	void normalize(T norm = 1) {
		T mult = norm / sqrt(x * x + y * y + z * z);
		x *= mult;
		y *= mult;
		z *= mult;
	}
};

template <typename T>
struct vector4d {
	T x;
	T y;
	T z;
	T w;

	vector4d(T xx, T yy, T zz, T ww) : x(xx), y(yy), z(zz), w(ww) {}

	void normalize(T norm = 1) {
		T mult = norm / sqrt(x * x + y * y + z * z + w * w);
		x *= mult;
		y *= mult;
		z *= mult;
		w *= mult;
	}
};

template <typename T>
struct matrix4 {
	T v[16];

//	matrix4<T> &operator*=(const matrix4<T> &b) {
//	}

	matrix4<T> &operator*=(T b) {
		for(uint8_t i = 0; i < 16; ++ i) {
			v[i] *= b;
		}
		return *this;
	}

	matrix4<T> &operator/=(T b) {
		for(uint8_t i = 0; i < 16; ++ i) {
			v[i] /= b;
		}
		return *this;
	}

	template <typename TO>
	[[nodiscard]]
	matrix4<TO> toFixed(int shift) const {
		matrix4<TO> r;
		T mult = 1 << shift;
		for(uint8_t i = 0; i < 16; ++ i) {
			r.v[i] = TO(v[i] * mult);
		}
		return r;
	}

	void apply(vector4d<T> &b) const {
		T x = b.x;
		T y = b.y;
		T z = b.z;
		b.x = x * v[ 0] + y * v[ 1] + z * v[ 2] + b.w * v[ 3];
		b.y = x * v[ 4] + y * v[ 5] + z * v[ 6] + b.w * v[ 7];
		b.z = x * v[ 8] + y * v[ 9] + z * v[10] + b.w * v[11];
		b.w = x * v[12] + y * v[13] + z * v[14] + b.w * v[15];
	}

	void apply(vector3d<T> &b) const {
		T x = b.x;
		T y = b.y;
		b.x = x * v[ 0] + y * v[ 1] + b.z * v[ 2] + v[ 3];
		b.y = x * v[ 4] + y * v[ 5] + b.z * v[ 6] + v[ 7];
		b.z = x * v[ 8] + y * v[ 9] + b.z * v[10] + v[11];
	}

	template <typename TI, typename TO>
	void project(const vector3d<TI> &b, TO &outX, TO &outY) const {
		T z  =  b.x * v[ 8] + b.y * v[ 9] + b.z * v[10] + v[11];
		outX = (b.x * v[ 0] + b.y * v[ 1] + b.z * v[ 2] + v[ 3]) / z;
		outY = (b.x * v[ 4] + b.y * v[ 5] + b.z * v[ 6] + v[ 7]) / z;
	}
};

template <typename T>
struct quaternion : public vector4d<T> {
	quaternion(void) : vector4d<T>(1, 0, 0, 0) {}

	[[gnu::always_inline]]
	inline quaternion(vector3d<T> axis, float angle, T norm = 1)
		: vector4d<T>(0, 0, 0, cos(angle * 0.5) * norm)
	{
		T s = sin(angle * 0.5) * norm;
		this->x = axis.x * s;
		this->y = axis.y * s;
		this->z = axis.z * s;
	}

	[[gnu::always_inline]]
	inline quaternion(vector3d<T> axis, uint8_t angle, T norm)
		: vector4d<T>(0, 0, 0, (cos8_fine(angle / 2) * norm) / 128)
	{
		T s = sin8_fine(angle / 2);
		this->x = (axis.x * s) / 128;
		this->y = (axis.y * s) / 128;
		this->z = (axis.z * s) / 128;
	}

	[[nodiscard]]
	matrix4<T> toNormalizedMatrix(void) const {
		// Thanks, http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/
		T xx = this->x * this->x;
		T yy = this->y * this->y;
		T zz = this->z * this->z;
		T ww = this->w * this->w;
		T m = 1.0 / (xx + yy + zz + ww);
		matrix4<T> r;
		r.v[ 0] = ( xx - yy - zz + ww) * m;
		r.v[ 1] = 2 * (this->x * this->y + this->z * this->w) * m;
		r.v[ 2] = 2 * (this->x * this->z - this->y * this->w) * m;
		r.v[ 3] = 0;
		r.v[ 4] = 2 * (this->x * this->y - this->z * this->w) * m;
		r.v[ 5] = (-xx + yy - zz + ww) * m;
		r.v[ 6] = 2 * (this->y * this->z + this->x * this->w) * m;
		r.v[ 7] = 0;
		r.v[ 8] = 2 * (this->x * this->z + this->y * this->w) * m;
		r.v[ 9] = 2 * (this->y * this->z - this->x * this->w) * m;
		r.v[10] = (-xx - yy + zz + ww) * m;
		r.v[11] = 0;
		r.v[12] = 0;
		r.v[13] = 0;
		r.v[14] = 0;
		r.v[15] = 1;
		return r;
	}

	[[nodiscard]]
	matrix4<T> toMatrix(void) const {
		T xx = this->x * this->x;
		T yy = this->y * this->y;
		T zz = this->z * this->z;
		T ww = this->w * this->w;
		matrix4<T> r;
		r.v[ 0] = ( xx - yy - zz + ww);
		r.v[ 1] = 2 * (this->x * this->y + this->z * this->w);
		r.v[ 2] = 2 * (this->x * this->z - this->y * this->w);
		r.v[ 3] = 0;
		r.v[ 4] = 2 * (this->x * this->y - this->z * this->w);
		r.v[ 5] = (-xx + yy - zz + ww);
		r.v[ 6] = 2 * (this->y * this->z + this->x * this->w);
		r.v[ 7] = 0;
		r.v[ 8] = 2 * (this->x * this->z + this->y * this->w);
		r.v[ 9] = 2 * (this->y * this->z - this->x * this->w);
		r.v[10] = (-xx - yy + zz + ww);
		r.v[11] = 0;
		r.v[12] = 0;
		r.v[13] = 0;
		r.v[14] = 0;
		r.v[15] = ( xx + yy + zz + ww);
		return r;
	}
};

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
	inline Material(RenderMode r, BlendMode b, Pattern p)
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

template <typename Bitmask, typename IndexT>
void renderLines(
	Bitmask &bitmask,
	const int16_t *projVerts,
	ProgMem<IndexT> lines,
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

template <typename Bitmask, typename IndexT>
void renderTriangles(
	Bitmask &bitmask,
	const int16_t *projVerts,
	ProgMem<IndexT> triangles,
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
	typename IndexT,
	typename ProjectorT
>
void renderScene(
	Display &display,
	Bitmask &bitmask,
	ProgMem<VertT> vertices,
	uint16_t nVertices,
	ProgMem<IndexT> lines,
	uint16_t nLines,
	ProgMem<IndexT> triangles,
	uint16_t nTriangles,
	const ProjectorT &projector,
	const Material *materials,
	uint8_t x0,
	uint8_t y0,
	uint8_t w,
	uint8_t h,
	uint8_t shift
) {
	int16_t *projVerts = alloca(nVertices * 2 * sizeof(int16_t));
	projectScene(vertices, nVertices, projector, projVerts);
	for(uint8_t y = 0; y < h; y += bitmask.height()) {
		for(uint8_t x = 0; x < w; x += bitmask.width()) {
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

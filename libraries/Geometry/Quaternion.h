/*
 * Quaternion - Simple quaternion data structures.
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

#ifndef QUATERNION_H_INCLUDED
#define QUATERNION_H_INCLUDED

#include "Matrix.h"
#include "Vector.h"

#include <FastMath.h>

#include "ext.h"

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

#endif

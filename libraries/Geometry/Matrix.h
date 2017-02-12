/*
 * Matrix - Simple matrix data structures.
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

#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include "Vector.h"

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

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

#endif

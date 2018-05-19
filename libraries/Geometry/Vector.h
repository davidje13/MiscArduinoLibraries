/*
 * Vector - Simple vector data structures.
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

#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include "ext.h"

template <typename T>
struct vector2d {
	T x;
	T y;

	constexpr vector2d(T xx, T yy) : x(xx), y(yy) {}

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

	constexpr vector3d(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}

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

	constexpr vector4d(T xx, T yy, T zz, T ww) : x(xx), y(yy), z(zz), w(ww) {}

	void normalize(T norm = 1) {
		T mult = norm / sqrt(x * x + y * y + z * z + w * w);
		x *= mult;
		y *= mult;
		z *= mult;
		w *= mult;
	}
};

#endif

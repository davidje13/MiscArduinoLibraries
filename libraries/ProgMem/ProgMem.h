/*
 * ProgMem - PROGMEM wrapping class.
 * Written in 2017 by David Evans
 *
 * Allows use of PROGMEM data without changes to syntax (particularly useful
 * for templated code). Note that some function calls (e.g. memcpy) will still
 * need to be replaced with progmem versions (e.g. memcpy_P).
 *
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#ifndef PROGMEM_H_INCLUDED
#define PROGMEM_H_INCLUDED

#include "ext.h"

#if defined(__AVR__)
#  include <avr/pgmspace.h>
#else
#  define PROGMEM
#  define PSTR(s) s
#  define memcpy_P(a,b,l) memcpy(a,b,l)
#  define strchr_P(a,b) strchr(a,b)
#  define pgm_read_byte_near(x) (*reinterpret_cast<const uint8_t*>(x))
#  define pgm_read_word_near(x) (*reinterpret_cast<const uint16_t*>(x))
#  define pgm_read_dword_near(x) (*reinterpret_cast<const uint32_t*>(x))
#  define pgm_read_float_near(x) (*reinterpret_cast<const float*>(x))
template <typename T>
[[gnu::pure,nodiscard,gnu::always_inline]]
inline void *pgm_read_ptr_near(T *const *x) {
	return *x;
}
template <typename T>
[[gnu::pure,nodiscard,gnu::always_inline]]
inline void *pgm_read_ptr_near(const T *const *x) {
	// Match AVR API: no const safety
	return const_cast<void*>(reinterpret_cast<const void*>(*x));
}
#endif

#define ProgMemString(s) ProgMem<char>(PSTR(s))

// This class achieves 0 overhead in code size and execution time compared to
// storing the pointer and calling pgm_read_* directly, while allowing the type
// to be used more safely and in templates

template <typename T>
[[gnu::const,nodiscard,gnu::always_inline]]
inline T safe_subdata(T v, int n) {
	return T(v ? v + n : nullptr);
}

template <typename T, typename Me>
class ProgMem_Base {
protected:
	const T *p;

public:
	[[gnu::always_inline]]
	inline ProgMem_Base(void) : p(nullptr) {}

	[[gnu::always_inline]]
	inline ProgMem_Base(const T *ptr) : p(ptr) {}

	[[gnu::always_inline]]
	inline ProgMem_Base(const ProgMem_Base<T,Me>&) = default;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline operator bool(void) const {
		return p != nullptr;
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline Me operator+(int n) const {
		return Me(p + n);
	}

	[[gnu::always_inline]]
	inline Me &operator+=(int n) {
		p += n;
		return *(static_cast<Me*>(this));
	}

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline const T *raw(void) const {
		return p;
	}
};

template <typename T, typename Enable = void>
class ProgMem : public ProgMem_Base<T,ProgMem<T>> {
public:
	using ProgMem_Base<T,ProgMem<T>>::ProgMem_Base;

	[[gnu::pure,nodiscard]]
	T operator[](int index) const {
		T target;
		memcpy_P(&target, this->p + index, sizeof(T));
		return target;
	}
};

template <typename T>
class ProgMem<T,ext::enable_if_t<
	ext::is_integral<T>::value && sizeof(T) == 1
>> : public ProgMem_Base<T,ProgMem<T>> {
public:
	using ProgMem_Base<T,ProgMem<T>>::ProgMem_Base;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline T operator[](int index) const {
		return T(pgm_read_byte_near(this->p + index));
	}
};

template <typename T>
class ProgMem<T,ext::enable_if_t<
	ext::is_integral<T>::value && sizeof(T) == 2
>> : public ProgMem_Base<T,ProgMem<T>> {
public:
	using ProgMem_Base<T,ProgMem<T>>::ProgMem_Base;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline T operator[](int index) const {
		return T(pgm_read_word_near(this->p + index));
	}
};

template <typename T>
class ProgMem<T,ext::enable_if_t<
	ext::is_integral<T>::value && sizeof(T) == 4
>> : public ProgMem_Base<T,ProgMem<T>> {
public:
	using ProgMem_Base<T,ProgMem<T>>::ProgMem_Base;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline T operator[](int index) const {
		return T(pgm_read_dword_near(this->p + index));
	}
};

template <>
class ProgMem<float> : public ProgMem_Base<float,ProgMem<float>> {
public:
	using ProgMem_Base<float,ProgMem<float>>::ProgMem_Base;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline float operator[](int index) const {
		return pgm_read_float_near(this->p + index);
	}
};

template <typename T>
class ProgMem<T*> : public ProgMem_Base<T*,ProgMem<T*>> {
public:
	using ProgMem_Base<T*,ProgMem<T*>>::ProgMem_Base;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline T *operator[](int index) const {
		return static_cast<T*>(pgm_read_ptr_near(this->p + index));
	}
};

template <typename T>
[[gnu::always_inline]]
static inline ProgMem<T> MakeProgMem(const T *ptr) {
	return ProgMem<T>(ptr);
}

#endif

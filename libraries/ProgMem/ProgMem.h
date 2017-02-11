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

#include <avr/pgmspace.h>

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

#define ProgMemString(s) ProgMem<char>(PSTR(s))

// Thanks, http://en.cppreference.com/w/cpp/types/enable_if
template<bool B, typename T = void> struct enable_if {};
template<typename T> struct enable_if<true, T> { typedef T type; };
template<bool B, typename T = void> using enable_if_t = typename enable_if<B,T>::type;

// Thanks, http://en.cppreference.com/w/cpp/types/is_same
template<typename T, typename U> struct is_same { static constexpr bool value = false; };
template<typename T> struct is_same<T, T> { static constexpr bool value = true; };

template<typename T> struct is_integral { static constexpr bool value = false; };
template<> struct is_integral<char> { static constexpr bool value = true; };
template<> struct is_integral<uint8_t> { static constexpr bool value = true; };
template<> struct is_integral<int8_t> { static constexpr bool value = true; };
template<> struct is_integral<uint16_t> { static constexpr bool value = true; };
template<> struct is_integral<int16_t> { static constexpr bool value = true; };
template<> struct is_integral<uint32_t> { static constexpr bool value = true; };
template<> struct is_integral<int32_t> { static constexpr bool value = true; };

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
class ProgMem<T,enable_if_t<is_integral<T>::value>> : public ProgMem_Base<T,ProgMem<T>> {
public:
	using ProgMem_Base<T,ProgMem<T>>::ProgMem_Base;

	[[gnu::pure,nodiscard,gnu::always_inline]]
	inline T operator[](int index) const {
		// This will be optimised to a single path at compile-time
		if(sizeof(T) == 1) {
			return pgm_read_byte_near(this->p + index);
		} else if(sizeof(T) == 2) {
			return pgm_read_word_near(this->p + index);
		} else if(sizeof(T) == 4) {
			return pgm_read_dword_near(this->p + index);
		} else {
			T target;
			memcpy_P(&target, this->p + index, sizeof(T));
			return target;
		}
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
		return pgm_read_ptr_near(this->p + index);
	}
};

template <typename T>
[[gnu::always_inline]]
static inline ProgMem<T> WrapProgMem(const T *ptr) {
	return ProgMem<T>(ptr);
}

#endif

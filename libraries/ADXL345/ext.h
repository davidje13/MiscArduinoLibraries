/*
 * ext - Language extensions and common utilities.
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

// This file is copied from /shared/ to all library folders at build time.
// Do not change this file from any location except /shared/ - it will be
// overwritten at the next build.

#ifndef EXT_H_INCLUDED
#define EXT_H_INCLUDED

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

namespace ext {
	// Thanks, http://en.cppreference.com/w/cpp/types/enable_if
	template<bool B, typename T = void> struct enable_if {
	};
	template<typename T> struct enable_if<true, T> {
		typedef T type;
	};
	template<bool B, typename T = void> using enable_if_t =
		typename enable_if<B,T>::type;

	// Thanks, http://en.cppreference.com/w/cpp/types/is_same
	template<typename T, typename U> struct is_same {
		static constexpr bool value = false;
	};
	template<typename T> struct is_same<T, T> {
		static constexpr bool value = true;
	};

	template<typename T> struct is_integral {
		static constexpr bool value = false;
	};
	template<> struct is_integral<char> {
		static constexpr bool value = true;
	};
	template<> struct is_integral<uint8_t> {
		static constexpr bool value = true;
	};
	template<> struct is_integral<int8_t> {
		static constexpr bool value = true;
	};
	template<> struct is_integral<uint16_t> {
		static constexpr bool value = true;
	};
	template<> struct is_integral<int16_t> {
		static constexpr bool value = true;
	};
	template<> struct is_integral<uint32_t> {
		static constexpr bool value = true;
	};
	template<> struct is_integral<int32_t> {
		static constexpr bool value = true;
	};

	// Thanks, http://stackoverflow.com/a/14466705/1180785
	template <typename T>
	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline T &ref(T &o) {
		return o;
	}

	template <typename T>
	[[gnu::const,nodiscard,gnu::always_inline]]
	static inline T &ref(T *o) {
		return *o;
	}

	template <typename T>
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline T max2(T a, T b) {
		return (a < b) ? b : a;
	}

	template <typename T>
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline T min2(T a, T b) {
		return (a < b) ? a : b;
	}

	template <typename T>
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline T max3(T a, T b, T c) {
		return max2(max2(a, b), c);
	}

	template <typename T>
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline T min3(T a, T b, T c) {
		return min2(min2(a, b), c);
	}

	template <typename T>
	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr inline T posmod(T a, T b) {
		return ((a % b) + b) % b;
	}

	// This class is a simplified version of boost's compressed_pair. It is
	// used to allow empty structs to be stored without taking up any memory.
	template <typename OptionalT, typename KnownT>
	class Flattener : public OptionalT {
	public:
		KnownT flattened_value;
		Flattener(OptionalT b, KnownT v) : OptionalT(b), flattened_value(v) {}
	};
}

#endif

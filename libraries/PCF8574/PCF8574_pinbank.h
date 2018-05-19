/*
 * PCF8574_pinbank
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

#ifndef PCF8574_PINBANK_H_INCLUDED
#define PCF8574_PINBANK_H_INCLUDED

template <typename T, uint8_t sz, uint8_t mask = ((1 << sz) - 1)>
class PCF8574_pinbank {
	T *device;
	uint8_t start;

public:
	PCF8574_pinbank(void) = delete;
	constexpr PCF8574_pinbank(T *device, uint8_t start)
		: device(device)
		, start(start)
	{}

	PCF8574_pinbank(const PCF8574_pinbank&) = default;
	PCF8574_pinbank(PCF8574_pinbank&&) = default;

	PCF8574_pinbank &operator=(const PCF8574_pinbank&) = default;
	PCF8574_pinbank &operator=(PCF8574_pinbank&&) = default;

	[[gnu::always_inline]]
	static constexpr inline uint8_t size(void) {
		return sz;
	}

	[[gnu::always_inline]]
	inline void set_fast(uint8_t value) {
		device->set(value << start, mask << start);
	}

	[[gnu::always_inline]]
	inline void set(uint8_t value) {
		set_fast(value);
	}

	[[gnu::always_inline]]
	inline void set_input(bool = false) {
		// when ports are high they are also in an input mode
		set(0xFF);
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		// no concept of dedicated output mode
	}

	[[nodiscard,gnu::always_inline]]
	inline uint8_t get(void) const {
		const uint8_t value = device->get();
		return (value >> start) & mask;
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
		device->begin_batch();
	}

	[[gnu::always_inline]]
	inline void send_batch(void) {
		device->send_batch();
	}

	[[nodiscard,gnu::always_inline]]
	inline PCF8574_pinbank fast(void) {
		return *this;
	}
};

template <
	typename T,
	uint8_t start,
	uint8_t end,
	uint8_t mask = ((1 << end) - 1) ^ ((1 << start) - 1)
>
class PCF8574_fixedpinbank {
	T *device;

public:
	PCF8574_fixedpinbank(void) = delete;
	constexpr PCF8574_fixedpinbank(T *device)
		: device(device)
	{}

	PCF8574_fixedpinbank(const PCF8574_fixedpinbank&) = default;
	PCF8574_fixedpinbank(PCF8574_fixedpinbank&&) = default;

	PCF8574_fixedpinbank &operator=(const PCF8574_fixedpinbank&) = default;
	PCF8574_fixedpinbank &operator=(PCF8574_fixedpinbank&&) = default;

	[[gnu::always_inline]]
	static constexpr inline uint8_t size(void) {
		return end - start;
	}

	[[gnu::always_inline]]
	inline void set_fast(uint8_t value) {
		device->set(value << start, mask);
	}

	[[gnu::always_inline]]
	inline void set(uint8_t value) {
		set_fast(value);
	}

	[[gnu::always_inline]]
	inline void set_input(bool = false) {
		// when ports are high they are also in an input mode
		set(0xFF);
	}

	[[gnu::always_inline]]
	inline void set_output(void) {
		// no concept of dedicated output mode
	}

	[[nodiscard,gnu::always_inline]]
	inline uint8_t get(void) const {
		const uint8_t value = device->get();
		return (value & mask) >> start;
	}

	[[gnu::always_inline]]
	inline void begin_batch(void) {
		device->begin_batch();
	}

	[[gnu::always_inline]]
	inline void send_batch(void) {
		device->send_batch();
	}

	[[nodiscard,gnu::always_inline]]
	inline PCF8574_fixedpinbank fast(void) {
		return *this;
	}
};

#endif

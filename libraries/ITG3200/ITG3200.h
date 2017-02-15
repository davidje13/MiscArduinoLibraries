/*
 * ITG3200 - ITG3200 (gyroscope) communication library.
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

#ifndef ITG3200_H_INCLUDED
#define ITG3200_H_INCLUDED

#include <Wire.h>

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

// Startup time ~20ms, +50ms for zero point to stabilise

// Recommended startup call (better accuracy):
// gyroscope.set_clock_source_sync(ITG3200::ClockSource::X_GYRO);

class ITG3200 {
	static const constexpr uint8_t I2C_ADDR = 0x68;

	enum Register : uint8_t {
		WHO_AM_I            = 0x00, // rw
		// ...
		SAMPLE_RATE_DIVIDER = 0x15, // rw
		DLPF_FULL_SCALE     = 0x16, // rw, see Range, LowPassBandwidth
		INTERRUPT_CONFIG    = 0x17, // rw
		// ...
		INTERRUPT_STATUS    = 0x1A,
		TEMPERATURE_MSB     = 0x1B,
		TEMPERATURE_LSB     = 0x1C,
		GYRO_X_MSB          = 0x1D,
		GYRO_X_LSB          = 0x1E,
		GYRO_Y_MSB          = 0x1F,
		GYRO_Y_LSB          = 0x20,
		GYRO_Z_MSB          = 0x21,
		GYRO_Z_LSB          = 0x22,
		// ...
		POWER_MANAGEMENT    = 0x3E, // rw
	};

	enum Range : uint8_t {
		INITIAL = 0x00, // takes this value on reset, but this is not valid
		R2000   = 0x18 // should be set to this before use (+/-2000deg/s)
	};

public:
	enum class ConnectionStatus : uint8_t {
		CONNECTED = 0,
		REQUEST_ERR_DATA_TOO_LONG = 1,
		REQUEST_ERR_NACK_ADDR = 2,
		REQUEST_ERR_NACK_DATA = 3,
		REQUEST_ERR_OTHER = 4,
		READ_TIMEOUT = 5,
		ID_MISMATCH
	};

	enum class LowPassBandwidth : uint8_t {
		L256_HZ = 0x00, // sample rate = 8kHz
		L188_HZ = 0x01, // sample rate = 1kHz
		L98_HZ  = 0x02, // sample rate = 1kHz
		L42_HZ  = 0x03, // sample rate = 1kHz
		L20_HZ  = 0x04, // sample rate = 1kHz
		L10_HZ  = 0x05, // sample rate = 1kHz
		L5_HZ   = 0x06  // sample rate = 1kHz
	};

	enum class ClockSource : uint8_t {
		BUILTIN              = 0x00, // default, but not recommended
		X_GYRO               = 0x01, // one of these is recommended
		Y_GYRO               = 0x02,
		Z_GYRO               = 0x03,
		EXTERNAL_32768_HZ    = 0x06,
		EXTERNAL_19200000_HZ = 0x05
	};

	enum InterruptCondition : uint8_t { // combinable
		DATA_AVAILABLE = 0x01,
		DEVICE_READY   = 0x04
	};

	enum class InterruptMode : uint8_t {
		PULSE_PIN_LATCH_REG_UNTIL_STATUS_READ = 0x00,
		PULSE_PIN_LATCH_REG_UNTIL_ANY_READ    = 0x10,
		LATCH_PIN_AND_REG_UNTIL_STATUS_READ   = 0x20,
		LATCH_PIN_AND_REG_UNTIL_ANY_READ      = 0x30
	};

	enum class InterruptStates : uint8_t {
		ACTIVE_HIGH_INACTIVE_LOW  = 0x00,
		ACTIVE_LOW_INACTIVE_OPEN  = 0x40,
		ACTIVE_LOW_INACTIVE_HIGH  = 0x80,
		ACTIVE_HIGH_INACTIVE_OPEN = 0xC0
	};

private:
	uint8_t intPin; // TODO: actually use this if available
	uint8_t powerCache;
	bool addrLSB : 4;
	bool hasSetRange : 4;

	enum WireError : uint8_t {
		SUCCESS = 0,
		DATA_TOO_LONG = 1,
		NACK_ADDR = 2,
		NACK_DATA = 3,
		OTHER = 4,
		READ_TIMEOUT = 5
	};

	[[gnu::pure,nodiscard,gnu::always_inline]]
	uint8_t i2c_addr(void) const {
		return I2C_ADDR | addrLSB;
	}

	[[gnu::always_inline]]
	WireError set_register(Register r, uint8_t value) {
		Wire.beginTransmission(i2c_addr());
		Wire.write(r);
		Wire.write(value);
		return WireError(Wire.endTransmission(true));
	}

	WireError read(Register r, uint8_t count, void *buffer, uint16_t maxMicros) {
		Wire.beginTransmission(i2c_addr());
		Wire.write(r);
		WireError err = WireError(Wire.endTransmission(true));
		if(err != SUCCESS) {
			return err;
		}

		Wire.requestFrom(i2c_addr(), count, uint8_t(true));
		uint16_t t0 = micros();
		uint8_t *b = static_cast<uint8_t*>(buffer);
		while(true) {
			while(Wire.available()) {
				*b = Wire.read();
				++ b;
				if((-- count) == 0) {
					return SUCCESS;
				}
			}
			if(uint16_t(micros() - t0) > maxMicros) {
				return READ_TIMEOUT;
			}
		}
	}

	bool check_interrupt(InterruptCondition condition) {
		uint8_t state;
		if(read(INTERRUPT_STATUS, 1, &state, 10000) != SUCCESS) {
			return false;
		}
		return (state & uint8_t(condition)) != 0;
	}

	void check_config(void) {
		if(!hasSetRange) {
			set_register(DLPF_FULL_SCALE, uint8_t(LowPassBandwidth::L256_HZ) | R2000);
			hasSetRange = true;
		}
	}

	[[gnu::const,nodiscard]]
	static ClockSource safe_clock_source(
		ClockSource cs,
		bool nox,
		bool noy,
		bool noz
	) {
		// Disabling a gyroscope also disables its timer
		if(cs == ClockSource::X_GYRO && nox) {
			return (!noy) ? ClockSource::Y_GYRO
			     : (!noz) ? ClockSource::Z_GYRO
			     : ClockSource::BUILTIN;
		}
		if(cs == ClockSource::Y_GYRO && noy) {
			return (!nox) ? ClockSource::X_GYRO
			     : (!noz) ? ClockSource::Z_GYRO
			     : ClockSource::BUILTIN;
		}
		if(cs == ClockSource::Z_GYRO && noz) {
			return (!nox) ? ClockSource::X_GYRO
			     : (!noy) ? ClockSource::Y_GYRO
			     : ClockSource::BUILTIN;
		}
		return cs;
	}

public:
	class reading {
		int16_t xx;
		int16_t yy;
		int16_t zz;
		int16_t temp;

	public:
		static const constexpr uint16_t RAW_DIVISOR = 8;
		static const constexpr uint16_t RAW_MULTIPLIER = 115;

		[[gnu::always_inline]]
		reading(void) : xx(0), yy(0), zz(0), temp(0x7FFF) {}

		[[gnu::always_inline]]
		reading(int16_t xv, int16_t yv, int16_t zv, int16_t tv)
			: xx(xv)
			, yy(yv)
			, zz(zv)
			, temp(tv)
		{}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		bool is_valid(void) const {
			return temp != 0x7FFF;
		}

		/**
		 * = deg/s * 14.375 = deg/s * 115 / 8
		 * actual scale factor = +/- 6%
		 * accuracy over temperature changes = 10%
		 */
		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t raw_x(void) const {
			return xx;
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t raw_y(void) const {
			return yy;
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t raw_z(void) const {
			return zz;
		}

		/**
		 * accuracy = +/- 1 deg C
		 */
		[[gnu::pure,nodiscard,gnu::always_inline]]
		int32_t temperature_millicelsius(void) const {
			return ((int32_t(temp) + 23000) * 1000) / 280;
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t raw_temperature(void) const {
			return temp;
		}
	};

private:
	[[nodiscard,gnu::always_inline]]
	static reading make_reading(const uint8_t *buffer) {
		return reading(
			(buffer[2] << 8) | buffer[3],
			(buffer[4] << 8) | buffer[5],
			(buffer[6] << 8) | buffer[7],
			(buffer[0] << 8) | buffer[1]
		);
	}

public:

	static const constexpr uint8_t NO_PIN = 0xFF;

	ConnectionStatus connection_status(void) {
		uint8_t regID;
		WireError err = read(WHO_AM_I, 1, &regID, 10000);
		if(err != SUCCESS) {
			return ConnectionStatus(err);
		}
		if(regID != i2c_addr()) {
			return ConnectionStatus::ID_MISMATCH;
		}
		return ConnectionStatus::CONNECTED;
	}

	/**
	 * Re-sync local configuration with configuration of the actual device
	 * (useful if another master may have changed the remote configuration)
	 */
	void check_configuration(void) {
		uint8_t output;
		read(POWER_MANAGEMENT, 1, &output, 10000);
		powerCache = output & 0x3F;
	}

	void set_sample_rate_divider_raw(uint8_t value) {
		set_register(SAMPLE_RATE_DIVIDER, value);
	}

	void set_sample_rate_divider(uint16_t value) {
		set_sample_rate_divider_raw(value - 1);
	}

	void set_filter_bandwidth(LowPassBandwidth bandwidth) {
		set_register(DLPF_FULL_SCALE, uint8_t(bandwidth) | R2000);
		hasSetRange = true;
	}

	void configure_interrupt(
		InterruptCondition conditions,
		InterruptMode mode      = InterruptMode::PULSE_PIN_LATCH_REG_UNTIL_STATUS_READ,
		InterruptStates pinMode = InterruptStates::ACTIVE_HIGH_INACTIVE_LOW
	) {
		set_register(INTERRUPT_CONFIG,
			uint8_t(pinMode) |
			uint8_t(mode) |
			uint8_t(conditions)
		);
	}

	void sleep(void) {
		powerCache |= 0x40; // sleep flag
		set_register(POWER_MANAGEMENT, powerCache); // sleep
	}

	void awake(void) {
		powerCache &= ~0x40;
		set_register(POWER_MANAGEMENT, powerCache);
		// takes a moment to wake up before communication works properly
		delayMicroseconds(1000);
	}

	bool sleeping(void) const {
		return powerCache & 0x40;
	}

	bool set_clock_source_async(ClockSource cs) {
		check_configuration();
		uint8_t newPower = (powerCache & 0x38) | uint8_t(safe_clock_source(
			cs, (powerCache & 0x20), (powerCache & 0x10), (powerCache & 0x08)
		));
		if(powerCache == newPower) {
			return false;
		}
		powerCache = newPower;
		set_register(POWER_MANAGEMENT, powerCache);
		return true;
	}

	void set_clock_source_sync(ClockSource cs) {
		// TODO: ensure DEVICE_READY is in interrupt conditions, or this will hang
		if(!set_clock_source_async(cs)) {
			return;
		}
		while(!check_interrupt(InterruptCondition::DEVICE_READY)) {
		}
	}

	void disable_gyros(bool disable_x, bool disable_y, bool disable_z) {
		ClockSource cs = ClockSource(powerCache & 0x07);
		powerCache = (
			(disable_x * 0x20) |
			(disable_y * 0x10) |
			(disable_z * 0x08) |
			uint8_t(safe_clock_source(cs, disable_x, disable_y, disable_z))
		);
		set_register(POWER_MANAGEMENT, powerCache);
	}

	reading next_reading(void) {
		// TODO: ensure DATA_AVAILABLE is in interrupt conditions, or this will hang
		check_config();
		uint8_t buffer[8];
		while(true) {
			if(check_interrupt(InterruptCondition::DATA_AVAILABLE)) {
				if(read(TEMPERATURE_MSB, 8, buffer, 10000) != SUCCESS) {
					continue;
				}
				return make_reading(buffer);
			}
		}
	}

	bool peek_reading(reading &output, uint16_t maxMicros = 0) {
		// TODO: ensure DATA_AVAILABLE is in interrupt conditions, or this will hang
		check_config();
		uint8_t buffer[9];
		uint16_t t0 = micros();
		while(true) {
			if(check_interrupt(InterruptCondition::DATA_AVAILABLE)) {
				if(read(TEMPERATURE_MSB, 8, buffer, 10000) != SUCCESS) {
					return false;
				}
				output = make_reading(buffer);
				return true;
			}
			if(uint16_t(micros() - t0) > maxMicros) {
				return false;
			}
		}
	}

	reading current_reading(bool checkInterrupt = false) {
		check_config();
		uint8_t buffer[9];
		if(checkInterrupt) {
			if(read(INTERRUPT_STATUS, 9, buffer, 10000) != SUCCESS) {
				return reading();
			}
		} else {
			buffer[0] = 0x00;
			if(read(TEMPERATURE_MSB, 8, buffer + 1, 10000) != SUCCESS) {
				return reading();
			}
		}

		return make_reading(buffer + 1);
	}

	void reset(void) {
		set_register(POWER_MANAGEMENT, 0x80); // reset
		powerCache = 0x00;

		// Enable all interupts so we can poll them later
		configure_interrupt(InterruptCondition(
			InterruptCondition::DATA_AVAILABLE |
			InterruptCondition::DEVICE_READY
		));

		// Set R2000 (default setting is bad according to spec)
		set_filter_bandwidth(LowPassBandwidth::L256_HZ);
	}

	ITG3200(bool AD0 = false, uint8_t interrupt = NO_PIN)
		: intPin(interrupt)
		, powerCache(0x00) // Assume default
		, addrLSB(AD0)
		, hasSetRange(false)
	{
		// Device supports up to 400kHz
		Wire.setClock(400000);
		if(intPin != NO_PIN) {
			pinMode(intPin, INPUT);
		}
		if(connection_status() == ConnectionStatus::CONNECTED) {
			reset();
		}
	}

	~ITG3200(void) {
	}
};

#endif

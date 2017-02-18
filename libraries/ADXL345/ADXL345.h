/*
 * ADXL345 - ADXL345 (accelerometer) communication library.
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

#ifndef ADXL345_H_INCLUDED
#define ADXL345_H_INCLUDED

#include <Wire.h>

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

class ADXL345 {
public:
	static const constexpr uint8_t NO_PIN = 0xFF;

	enum class ConnectionStatus : uint8_t {
		CONNECTED = 0,
		REQUEST_ERR_DATA_TOO_LONG = 1,
		REQUEST_ERR_NACK_ADDR = 2,
		REQUEST_ERR_NACK_DATA = 3,
		REQUEST_ERR_OTHER = 4,
		READ_TIMEOUT,
		ID_MISMATCH
	};

	enum class Voltage : uint8_t {
		V2_0,
		V2_5,
		V3_3,
		V3_6
	};

	enum class Axes : uint8_t {
		NONE = 0x00,
		X    = 0x04,
		Y    = 0x02,
		Z    = 0x01,
		XY   = X|Y,
		YZ   = Y|Z,
		XZ   = X|Z,
		XYZ  = X|Y|Z
	};

	enum class Rate : uint8_t {
		R0_10_HZ = 0x00,
		R0_20_HZ = 0x01,
		R0_39_HZ = 0x02,
		R0_78_HZ = 0x03,
		R1_56_HZ = 0x04,
		R3_13_HZ = 0x05,
		R6_25_HZ = 0x06,
		R12_5_HZ = 0x07, // low power capable
		R25_HZ   = 0x08, // low power capable
		R50_HZ   = 0x09, // low power capable
		R100_HZ  = 0x0A, // low power capable, default
		R200_HZ  = 0x0B, // low power capable
		R400_HZ  = 0x0C, // low power capable
		R800_HZ  = 0x0D,
		R1600_HZ = 0x0E,
		R3200_HZ = 0x0F
	};

	enum class SleepRate : uint8_t {
		R8_HZ = 0x00,
		R4_HZ = 0x01,
		R2_HZ = 0x02,
		R1_HZ = 0x03
	};

	enum class InterruptPin : uint8_t {
		NONE,
		INTERRUPT1,
		INTERRUPT2
	};

	class reading {
		int16_t xx;
		int16_t yy;
		int16_t zz;

	public:
		[[gnu::always_inline]]
		reading(void) : xx(0x7FFF), yy(0x7FFF), zz(0x7FFF) {}

		[[gnu::always_inline]]
		reading(int16_t xv, int16_t yv, int16_t zv)
			: xx(xv)
			, yy(yv)
			, zz(zv)
		{}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		bool is_valid(void) const {
			return xx != 0x7FFF;
		}

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

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t x_millig(void) const {
			return xx * 4; // TODO: correct value may be 3.9
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t y_millig(void) const {
			return yy * 4; // TODO: correct value may be 3.9
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t z_millig(void) const {
			return zz * 4; // TODO: correct value may be 3.9
		}
	};

private:
	enum Register : uint8_t {
		DEVID                 = 0x00,
		// ...
		TAP_THRESHOLD         = 0x1D, // rw
		OFFSET_X              = 0x1E, // rw
		OFFSET_Y              = 0x1F, // rw
		OFFSET_Z              = 0x20, // rw
		TAP_DURATION          = 0x21, // rw
		TAP_LATENCY           = 0x22, // rw
		TAP_WINDOW            = 0x23, // rw
		ACTIVITY_THRESHOLD    = 0x24, // rw
		INACTIVITY_THRESHOLD  = 0x25, // rw
		INACTIVITY_TIME       = 0x26, // rw
		ACTIVITY_AXIS_CONTROL = 0x27, // rw
		FREEFALL_THRESHOLD    = 0x28, // rw
		FREEFALL_TIME         = 0x29, // rw
		TAP_AXIS_CONTROL      = 0x2A, // rw
		ACT_TAP_STATUS        = 0x2B, // r
		BW_RATE               = 0x2C, // rw
		POWER_CONTROL         = 0x2D, // rw
		INT_ENABLE            = 0x2E, // rw
		INT_MAP               = 0x2F, // rw
		INT_SOURCE            = 0x30, // r
		DATA_FORMAT           = 0x31, // rw
		DATA_X0               = 0x32, // r
		DATA_X1               = 0x33, // r
		DATA_Y0               = 0x34, // r
		DATA_Y1               = 0x35, // r
		DATA_Z0               = 0x36, // r
		DATA_Z1               = 0x37, // r
		FIFO_CONTROL          = 0x38, // rw
		FIFO_STATUS           = 0x39  // r
	};

	enum WireError : uint8_t {
		SUCCESS = 0,
		DATA_TOO_LONG = 1,
		NACK_ADDR = 2,
		NACK_DATA = 3,
		OTHER = 4
	};

	enum Range : uint8_t {
		R2  = 0x00,
		R4  = 0x01,
		R8  = 0x02,
		R16 = 0x03
	};

	enum Interrupt : uint8_t {
		OVERRUN    = 0x01,
		WATERMARK  = 0x02,
		FREEFALL   = 0x04,
		INACTIVITY = 0x08,
		ACTIVITY   = 0x10,
		DOUBLE_TAP = 0x20,
		SINGLE_TAP = 0x40,
		DATA_READY = 0x80
	};

	enum PowerCtl : uint8_t {
		PCTL_SLEEP      = 0x04,
		PCTL_MEASURE    = 0x08,
		PCTL_AUTO_SLEEP = 0x10,
		PCTL_LINK       = 0x20
	};

	uint8_t int1Pin;
	uint8_t int2Pin;
	uint8_t actTapStatus; // unused 0x80 is commandeered for activity status
	uint8_t intStatus;
	uint8_t intEnable;
	uint8_t intMap;
	uint8_t power; // unused 0x80 is commandeered for interrupts low config
	// also 0x40 is commandeered for alternative address config

	[[gnu::pure,nodiscard,gnu::always_inline]]
	uint8_t i2c_addr(void) const {
		return (power & 0x40) ? 0x53 : 0x1D;
	}

	[[gnu::always_inline]]
	WireError set_register(Register r) {
		Wire.beginTransmission(i2c_addr());
		Wire.write(r);
		return WireError(Wire.endTransmission(true));
	}

	[[gnu::always_inline]]
	WireError set_register(Register r, uint8_t *values, uint8_t count) {
		Wire.beginTransmission(i2c_addr());
		Wire.write(r);
		for(uint8_t i = 0; i < count; ++ i) {
			Wire.write(values[i]);
		}
		return WireError(Wire.endTransmission(true));
	}

	[[gnu::always_inline]]
	WireError set_register(Register r, uint8_t value) {
		return set_register(r, &value, 1);
	}

	bool read(uint8_t count, void *buffer, uint16_t maxMicros) {
		Wire.requestFrom(i2c_addr(), count, uint8_t(true));
		uint16_t t0 = micros();
		uint8_t *b = static_cast<uint8_t*>(buffer);
		while(true) {
			while(Wire.available()) {
				*b = Wire.read();
				++ b;
				if((-- count) == 0) {
					return true;
				}
			}
			if(uint16_t(micros() - t0) > maxMicros) {
				return false;
			}
		}
	}

	bool setIntConfig(Interrupt i, InterruptPin pin) {
		switch(pin) {
		case InterruptPin::NONE:
			if(!(intEnable & i)) {
				return false;
			}
			intEnable &= ~i;
			return true;
		case InterruptPin::INTERRUPT1:
			if((intEnable & i) && !(intMap & i)) {
				return false;
			}
			intEnable |= i;
			intMap &= ~i;
			return true;
		case InterruptPin::INTERRUPT2:
			if((intEnable & i) && (intMap & i)) {
				return false;
			}
			intEnable |= i;
			intMap |= i;
			return true;
		}
		__builtin_unreachable();
	}

	void sendIntConfig(bool includeMap) {
		// INT_MAP should be set before INT_ENABLE, so these can't be batched :(
		if(includeMap) {
			set_register(INT_MAP, intMap);
		}
		set_register(INT_ENABLE, intEnable);
	}

	bool checkIntStatus(Interrupt i) {
		bool match = (intStatus & i);
		intStatus &= ~i;
		return match;
	}

	void set_df(bool selftest) {
		// doesn't appear to be any disadvantage to using full-res
		// data, right-aligned, at maximum range all the time
		set_register(DATA_FORMAT,
			(selftest * 0x80) |
			((power & 0x80) ? 0x20 : 0x00) |
			0x08 |
			Range::R16
		);
	}

	reading grab_reading(void) {
		if(set_register(DATA_X0) != SUCCESS) {
			return reading();
		}
		uint8_t data[6];
		if(!read(6, data, 10000)) {
			return reading();
		}
		return reading(
			(data[1] << 8) | data[0],
			(data[3] << 8) | data[2],
			(data[5] << 8) | data[4]
		);
		// Note: should be at least 5us between readings when using FIFO
		// (not a realistic concern here; sending the register address on I2C
		// takes longer anyway)
	}

public:
	ConnectionStatus connection_status(void) {
		WireError err = set_register(DEVID);
		if(err != SUCCESS) {
			return ConnectionStatus(err);
		}
		uint8_t devid;
		if(!read(1, &devid, 10000)) {
			return ConnectionStatus::READ_TIMEOUT;
		}
		if(devid != 0xE5) {
			return ConnectionStatus::ID_MISMATCH;
		}
		return ConnectionStatus::CONNECTED;
	}

	void set_sample_rate(Rate rate, bool low_power = false) {
		if(rate < Rate::R12_5_HZ || rate > Rate::R400_HZ) {
			// low power mode does not save power outside this range
			low_power = false;
		}
		set_register(BW_RATE, (low_power ? 0x10 : 0x00) | uint8_t(rate));
	}

	void sleep(SleepRate rate = SleepRate::R8_HZ) {
		power = (power & ~0x03) | PCTL_SLEEP | uint8_t(rate);
		set_register(POWER_CONTROL, power & 0x3F);
	}

	void awake(void) {
		power &= ~PCTL_SLEEP;
		set_register(POWER_CONTROL, power & 0x3F);
	}

	void set_on(bool on) {
		if(on) {
			power |= PCTL_MEASURE;
			set_df(false);
		} else {
			power &= ~PCTL_MEASURE;
		}
		set_register(POWER_CONTROL, power & 0x3F);
	}

	void set_offsets(int16_t x_millig, int16_t y_millig, int16_t z_millig) {
		uint8_t config[] = {
			uint8_t((x_millig * 5) / 78),
			uint8_t((y_millig * 5) / 78),
			uint8_t((z_millig * 5) / 78)
		};
		set_register(OFFSET_X, config, 3);
	}

	void configure_double_tap_detection(
		uint16_t min_millig,
		uint32_t max_micros,
		uint32_t double_min_sep_micros,
		uint32_t double_max_sep_micros,
		bool rapid_tap_invalidates_double_tap = true, // aka supress bit
		Axes included_axes = Axes::XYZ,
		InterruptPin pinSingle = InterruptPin::INTERRUPT1,
		InterruptPin pinDouble = InterruptPin::INTERRUPT1
	) {
		// Disable interrupts before reconfiguring
		if(
			setIntConfig(SINGLE_TAP, InterruptPin::NONE) |
			setIntConfig(DOUBLE_TAP, InterruptPin::NONE)
		) {
			sendIntConfig(false);
		}

		if(
			max_micros == 0 ||
			included_axes == Axes::NONE ||
			(pinSingle == InterruptPin::NONE && pinDouble == InterruptPin::NONE)
		) {
			return;
		}

		bool enDouble = (
			pinDouble != InterruptPin::NONE &&
			double_max_sep_micros > double_min_sep_micros
		);

		uint8_t config[] = {
			uint8_t(max_micros / 625),
			uint8_t(double_min_sep_micros / 1250),
			uint8_t((double_max_sep_micros - double_min_sep_micros) / 1250)
		};
		set_register(TAP_THRESHOLD, (min_millig * 2) / 125);
		set_register(TAP_DURATION, config, enDouble ? 3 : 1);
		set_register(TAP_AXIS_CONTROL,
			(rapid_tap_invalidates_double_tap * 0x08) |
			uint8_t(included_axes)
		);

		// Enable interrupts
		setIntConfig(SINGLE_TAP, pinSingle);
		setIntConfig(DOUBLE_TAP, enDouble ? pinDouble : InterruptPin::NONE);
		sendIntConfig(true);
	}

	[[gnu::always_inline]]
	void configure_single_tap_detection(
		uint16_t min_millig,
		uint32_t max_micros,
		Axes included_axes = Axes::XYZ,
		InterruptPin pin = InterruptPin::INTERRUPT1
	) {
		configure_double_tap_detection(
			min_millig,
			max_micros,
			0,
			0,
			true,
			included_axes,
			pin,
			InterruptPin::NONE
		);
	}

	[[gnu::always_inline]]
	void disable_tap_detection(void) {
		configure_double_tap_detection(
			0, 0, 0, 0, true, Axes::XYZ,
			InterruptPin::NONE, InterruptPin::NONE
		);
	}

	void configure_freefall_detection(
		uint16_t max_millig,
		uint16_t min_millis,
		InterruptPin pin = InterruptPin::INTERRUPT1
	) {
		// Disable interrupts before reconfiguring
		if(setIntConfig(FREEFALL, InterruptPin::NONE)) {
			sendIntConfig(false);
		}

		if(max_millig == 0 || pin == InterruptPin::NONE) {
			return;
		}

		uint8_t config[] = {
			uint8_t((max_millig * 2) / 125),
			uint8_t(min_millis / 5)
		};
		set_register(FREEFALL_THRESHOLD, config, 2);

		// Enable interrupts
		setIntConfig(FREEFALL, pin);
		sendIntConfig(true);
	}

	[[gnu::always_inline]]
	void disable_freefall_detection(void) {
		configure_freefall_detection(0, 0, InterruptPin::NONE);
	}

	void configure_activity_detection(
		uint16_t activity_min_millig,
		uint16_t inactivity_max_millig,
		uint8_t inactivity_min_seconds,
		Axes activity_axes = Axes::XYZ,
		Axes inactivity_axes = Axes::XYZ,
		bool auto_sleep = false,
		bool activity_relative = true,
		bool inactivity_relative = true,
		bool mutually_exclusive = true,
		InterruptPin activityPin = InterruptPin::INTERRUPT1,
		InterruptPin inactivityPin = InterruptPin::INTERRUPT1
	) {
		// Disable interrupts before reconfiguring
		if(
			setIntConfig(ACTIVITY, InterruptPin::NONE) |
			setIntConfig(INACTIVITY, InterruptPin::NONE)
		) {
			sendIntConfig(false);
		}

		if(
			activityPin == InterruptPin::NONE &&
			inactivityPin == InterruptPin::NONE
		) {
			return;
		}

		uint8_t config[] = {
			uint8_t((activity_min_millig * 2) / 125),
			uint8_t((inactivity_max_millig * 2) / 125),
			inactivity_min_seconds,
			uint8_t(
				(activity_relative * 0x80) |
				(uint8_t(activity_axes) << 4) |
				(inactivity_relative * 0x08) |
				uint8_t(inactivity_axes)
			)
		};
		set_register(ACTIVITY_THRESHOLD, config, 4);

		bool needsCycle = false;
		if(auto_sleep) {
			power |= (PCTL_AUTO_SLEEP | PCTL_LINK); // required by chip
		} else if(mutually_exclusive) {
			power |= PCTL_LINK;
			if(power & PCTL_AUTO_SLEEP) {
				power &= ~PCTL_AUTO_SLEEP;
				needsCycle = true;
			}
		} else {
			if(power & PCTL_LINK) {
				needsCycle = true;
			}
			power &= ~(PCTL_LINK | PCTL_AUTO_SLEEP);
		}
		if(power & PCTL_MEASURE) {
			if(needsCycle) {
				// Momentarily set standby mode to avoid noise
				set_register(POWER_CONTROL, power & 0x37);
			}
			set_register(POWER_CONTROL, power & 0x3F);
		}

		// Enable interrupts
		setIntConfig(ACTIVITY, activityPin);
		setIntConfig(INACTIVITY, inactivityPin);
		sendIntConfig(true);
	}

	[[gnu::always_inline]]
	void disable_activity_detection(void) {
		configure_activity_detection(
			0, 0, 0, Axes::XYZ, Axes::XYZ, false, true, true, true,
			InterruptPin::NONE, InterruptPin::NONE
		);
	}

	void configure_fifo_bypass(void) {
		set_register(FIFO_CONTROL, 0x00);
	}

	void configure_fifo(uint8_t samples) {
		set_register(FIFO_CONTROL, 0x40 | min(samples, 31));
	}

	void configure_fifo_stream(uint8_t watermarkTriggerPoint) {
		set_register(FIFO_CONTROL, 0x80 | min(watermarkTriggerPoint, 31));
	}

	void configure_fifo_trigger(InterruptPin pin, uint8_t samplesBefore) {
		set_register(FIFO_CONTROL,
			0xC0 |
			((pin == InterruptPin::INTERRUPT2) ? 0x20 : 0x00) |
			min(samplesBefore, 31)
		);
	}

	void configure_interrupt_data_ready(InterruptPin pin) {
		// this interrupt will be in the register no matter what, but here we
		// can configure how it affects the INTERRUPT* pins
		if(setIntConfig(DATA_READY, pin)) {
			sendIntConfig(true);
		}
	}

	void configure_interrupt_watermark(InterruptPin pin) {
		// this interrupt will be in the register no matter what, but here we
		// can configure how it affects the INTERRUPT* pins
		if(setIntConfig(WATERMARK, pin)) {
			sendIntConfig(true);
		}
	}

	void configure_interrupt_overrun(InterruptPin pin) {
		// this interrupt will be in the register no matter what, but here we
		// can configure how it affects the INTERRUPT* pins
		if(setIntConfig(OVERRUN, pin)) {
			sendIntConfig(true);
		}
	}

	void poll_device(void) {
		// Must read ACT_TAP_STATUS before INT_SOURCE, or it may be clobbered
		// with new data
		uint8_t ats;
		set_register(ACT_TAP_STATUS);
		read(1, &ats, 10000);

		uint8_t is;
		set_register(INT_SOURCE);
		read(1, &is, 10000);
		intStatus = (intStatus & ~(
			Interrupt::DATA_READY |
			Interrupt::WATERMARK |
			Interrupt::OVERRUN
		)) | is;
		if(is & Interrupt::ACTIVITY) {
			actTapStatus = (actTapStatus & ~0xF0) | (ats & 0x70) | 0x80;
			intStatus &= ~Interrupt::INACTIVITY;
		} else if(is & Interrupt::INACTIVITY) {
			actTapStatus = (actTapStatus & ~0xF0);
			intStatus &= ~Interrupt::ACTIVITY;
		}
		if(is & (Interrupt::SINGLE_TAP | Interrupt::DOUBLE_TAP)) {
			actTapStatus = (actTapStatus & ~0x07) | (ats & 0x07);
		}
		actTapStatus = (actTapStatus & ~0x08) | (ats & 0x08); // sleep bit
	}

	void clear_interrupts(bool onDevice = false) {
		if(onDevice) {
			poll_device();
		}
		intStatus = 0x00;
	}

	bool has_detected_freefall(void) {
		return checkIntStatus(Interrupt::FREEFALL);
	}

	bool has_detected_tap(void) {
		return checkIntStatus(Interrupt::SINGLE_TAP);
	}

	bool has_detected_double_tap(void) {
		return checkIntStatus(Interrupt::DOUBLE_TAP);
	}

	bool has_detected_activity(void) {
		return checkIntStatus(Interrupt::ACTIVITY);
	}

	bool has_detected_inactivity(void) {
		return checkIntStatus(Interrupt::INACTIVITY);
	}

	bool has_detected_overrun(void) {
		return intStatus & Interrupt::OVERRUN;
	}

	bool has_detected_watermark(void) {
		return intStatus & Interrupt::WATERMARK;
	}

	bool is_active(void) const {
		return actTapStatus & 0x80;
	}

	bool is_on(void) const {
		return (power & PCTL_MEASURE);
	}

	bool is_sleeping(void) const {
		return (power & PCTL_SLEEP) || (actTapStatus & 0x08);
	}

	Axes latest_activity_axes(void) const {
		return Axes((actTapStatus & 0x70) >> 4);
	}

	Axes latest_tap_axes(void) const {
		return Axes(actTapStatus & 0x07);
	}

	reading next_reading(void) {
		if(power & PCTL_SLEEP) {
			// TODO: sleep mode disables DATA_READY, but still produces data
			// at a configured rate. Ideally should identify how long to wait
			return grab_reading();
		}
		while(true) {
			// TODO: might be worth performing 8-byte readings from INT_SOURCE
			// to get flags and data simultaneously (but only advantageous if
			// data is expected to be ready almost-always)
			if(checkIntStatus(Interrupt::DATA_READY)) {
				return grab_reading();
			}
			poll_device();
		}
	}

	bool peek_reading(reading &output, uint16_t maxMicros = 0) {
		if(power & PCTL_SLEEP) {
			// TODO: sleep mode disables DATA_READY, but still produces data
			// at a configured rate. Ideally should identify how long to wait
			output = grab_reading();
			return true;
		}
		uint16_t t0 = micros();
		while(true) {
			if(checkIntStatus(Interrupt::DATA_READY)) {
				output = grab_reading();
				return true;
			}
			if(uint16_t(micros() - t0) > maxMicros) {
				return false;
			}
			poll_device();
		}
	}

	uint8_t get_buffered_readings(
		reading *output,
		uint8_t maxReadings = 33,
		bool onlyIfTriggered = false
	) {
		if(maxReadings == 0) {
			return 0;
		}
		uint8_t fifoSize;
		set_register(FIFO_STATUS);
		read(1, &fifoSize, 10000);
		if(onlyIfTriggered && !(fifoSize & 0x80)) {
			return 0;
		}
		fifoSize = min((fifoSize & 0x3F) + 1, maxReadings);

		for(uint8_t i = 0; i < fifoSize; ++ i) {
			output[i] = grab_reading();
		}
		return fifoSize;
	}

	reading current_reading(void) {
		return grab_reading();
	}

	reading accumulate_readings(uint8_t samples) {
		int16_t x = 0;
		int16_t y = 0;
		int16_t z = 0;
		uint8_t n = 0;
		while(n < samples) {
			reading r = next_reading();
			if(r.is_valid()) {
				x += r.raw_x();
				y += r.raw_y();
				z += r.raw_z();
				++ n;
			}
		}
		return reading(x, y, z);
	}

	bool test(Voltage voltage) {
		// Store old config
		uint8_t oldBW;
		uint8_t oldFIFO;
		set_register(BW_RATE);
		read(1, &oldBW, 10000);
		set_register(FIFO_CONTROL);
		read(1, &oldFIFO, 10000);

		// Run test
		uint8_t config[] = {
			uint8_t(Rate::R100_HZ),
			PCTL_MEASURE,
			0x00
		};
		set_register(BW_RATE, config, 3);
		set_register(FIFO_CONTROL, 0x00);

		set_df(false);
		accumulate_readings(4); // discard 4 samples while settling
		reading forceOff = accumulate_readings(16);

		set_df(true);
		accumulate_readings(4); // discard 4 samples while settling
		reading forceOn = accumulate_readings(16);

		set_df(false);
		accumulate_readings(4); // discard 4 samples while settling

		// Reset to normal operation
		config[0] = oldBW;
		config[1] = power & 0x3F;
		config[2] = intEnable;
		set_register(BW_RATE, config, 3);
		set_register(FIFO_CONTROL, oldFIFO);

		// Analyse results
		int16_t dx = (forceOn.raw_x() - forceOff.raw_x()) / 16;
		int16_t dy = (forceOff.raw_y() - forceOn.raw_y()) / 16;
		int16_t dz = (forceOn.raw_z() - forceOff.raw_z()) / 16;

		switch(voltage) {
		case Voltage::V2_0: // multipliers: xy = 0.64, z = 0.8
			return (
				dx >= 32 && dx <= 346 &&
				dy >= 32 && dy <= 346 &&
				dz >= 60 && dz <= 700
			);
		case Voltage::V2_5: // multipliers: xyz = 1
			return (
				dx >= 50 && dx <= 540 &&
				dy >= 50 && dy <= 540 &&
				dz >= 75 && dz <= 875
			);
		case Voltage::V3_3: // multipliers: xy = 1.77, z = 1.47
			return (
				dx >= 88 && dx <= 956 &&
				dy >= 88 && dy <= 956 &&
				dz >= 110 && dz <= 1286
			);
		case Voltage::V3_6: // multipliers: xy = 2.11, z = 1.69
			return (
				dx >= 105 && dx <= 1140 &&
				dy >= 105 && dy <= 1140 &&
				dz >= 126 && dz <= 1479
			);
		}
		__builtin_unreachable();
	}

	ADXL345(
		bool altAddress = true,
		uint8_t int1 = NO_PIN,
		uint8_t int2 = NO_PIN,
		uint8_t activeIntMode = HIGH
	)
		: int1Pin(int1)
		, int2Pin(int2)
		, actTapStatus(0x00)
		, intStatus(0x00)
		, intEnable(0x00)
		, intMap(0x00)
		, power(((activeIntMode == LOW) * 0x80) | (altAddress * 0x40))
	{
		// Device supports up to 400kHz
		Wire.setClock(400000);
		if(int1Pin != NO_PIN) {
			pinMode(int1Pin, INPUT);
		}
		if(int2Pin != NO_PIN) {
			pinMode(int2Pin, INPUT);
		}
	}

	~ADXL345(void) {
		set_on(false);
	}
};

#endif

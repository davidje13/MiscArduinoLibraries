/*
 * HMC5883L - HMC5883L (compass) communication library.
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

#ifndef HMC5883L_H_INCLUDED
#define HMC5883L_H_INCLUDED

// If the newer nodiscard attribute is available, use it
#ifdef __has_cpp_attribute
#  if !__has_cpp_attribute(nodiscard)
#    define nodiscard gnu::warn_unused_result
#  endif
#else
#  define nodiscard gnu::warn_unused_result
#endif

template <
	typename TwiT,
	typename DRDYPinT
>
class HMC5883L {
	// This class is a simplified version of boost's compressed_pair. It is used
	// to allow empty structs to be stored without taking up any memory.
	template <typename OptionalT, typename KnownT>
	class Flattener : public OptionalT {
	public:
		KnownT flattened_value;
		Flattener(OptionalT b, KnownT v) : OptionalT(b), flattened_value(v) {}
	};

public:
	enum class ConnectionStatus : uint8_t {
		CONNECTED = 0,
		REQUEST_ERR_DATA_TOO_LONG = 1,
		REQUEST_ERR_NACK_ADDR = 2,
		REQUEST_ERR_NACK_DATA = 3,
		REQUEST_ERR_OTHER = 4,
		READ_TIMEOUT,
		ID_MISMATCH
	};

	enum class Averaging : uint8_t {
		A1 = 0x00, // Default
		A2 = 0x20,
		A4 = 0x40,
		A8 = 0x60
	};

	enum class Rate : uint8_t {
		R0_75_HZ = 0x00,
		R1_5_HZ  = 0x04,
		R3_HZ    = 0x08,
		R7_5_HZ  = 0x0C,
		R15_HZ   = 0x10, // Default
		R30_HZ   = 0x14,
		R75_HZ   = 0x18,
		R160_HZ  = 0xFF
	};

	[[gnu::const,nodiscard]]
	static uint16_t rate_period_millis(Rate rate) {
		switch(rate) {
		case Rate::R0_75_HZ:
			return 1334;
		case Rate::R1_5_HZ:
			return 667;
		case Rate::R3_HZ:
			return 334;
		case Rate::R7_5_HZ:
			return 134;
		case Rate::R15_HZ:
			return 67;
		case Rate::R30_HZ:
			return 34;
		case Rate::R75_HZ:
			return 14;
		case Rate::R160_HZ:
			return 6;
		}
		__builtin_unreachable();
	}

	enum class Gain : uint8_t {
		G1370 = 0x00, // Range: 0.88Ga
		G1090 = 0x20, // Range: 1.3Ga, Default
		G820  = 0x40, // Range: 1.9Ga
		G660  = 0x60, // Range: 2.5Ga
		G440  = 0x80, // Range: 4.0Ga
		G390  = 0xA0, // Range: 4.7Ga
		G330  = 0xC0, // Range: 5.6Ga
		G230  = 0xE0  // Range: 8.1Ga
	};

	[[gnu::const,nodiscard]]
	static uint16_t gain_value(Gain gain) {
		switch(gain) {
		case Gain::G1370:
			return 1370;
		case Gain::G1090:
			return 1090;
		case Gain::G820:
			return 820;
		case Gain::G660:
			return 660;
		case Gain::G440:
			return 440;
		case Gain::G390:
			return 390;
		case Gain::G330:
			return 330;
		case Gain::G230:
			return 230;
		}
		__builtin_unreachable();
	}

	[[gnu::const,nodiscard]]
	static Gain gain_for_range(uint16_t range_milligauss) {
		if(range_milligauss <= 880) {
			return Gain::G1370;
		} else if(range_milligauss <= 1300) {
			return Gain::G1090;
		} else if(range_milligauss <= 1900) {
			return Gain::G820;
		} else if(range_milligauss <= 2500) {
			return Gain::G660;
		} else if(range_milligauss <= 4000) {
			return Gain::G440;
		} else if(range_milligauss <= 4700) {
			return Gain::G390;
		} else if(range_milligauss <= 5600) {
			return Gain::G330;
		} else {
			return Gain::G230;
		}
	}

	class reading {
		static const constexpr uint8_t NO_DATA = 0xFE;
		static const constexpr uint8_t SATURATED = 0xFF;

		int16_t xx;
		int16_t yy;
		int16_t zz;
		uint8_t status;

	public:
		[[gnu::always_inline]]
		reading(void) : xx(0), yy(0), zz(0), status(NO_DATA) {}

		[[gnu::always_inline]]
		reading(int16_t xv, int16_t yv, int16_t zv, Gain gain)
			: xx(xv)
			, yy(yv)
			, zz(zv)
			, status(int8_t(gain))
		{
			if(xx == -4096 || yy == -4096 || zz == -4096) {
				status = SATURATED;
			}
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		bool is_valid(void) const {
			return status != SATURATED && status != NO_DATA;
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		bool is_saturated(void) const {
			return status == SATURATED;
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		uint16_t gain(void) const {
			if(!is_valid()) {
				return 0;
			}
			return gain_value(Gain(status));
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t x(void) const {
			return xx;
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t y(void) const {
			return yy;
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t z(void) const {
			return zz;
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t x_milligauss(void) const {
			if(!is_valid()) {
				return 0;
			}
			return (xx * 1000) / gain();
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t y_milligauss(void) const {
			if(!is_valid()) {
				return 0;
			}
			return (yy * 1000) / gain();
		}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		int16_t z_milligauss(void) const {
			if(!is_valid()) {
				return 0;
			}
			return (zz * 1000) / gain();
		}

		[[gnu::always_inline]]
		void set(int16_t x, int16_t y, int16_t z) {
			xx = x;
			yy = y;
			zz = z;
		}
	};

private:
	enum TestMode : uint8_t {
		NO_BIAS       = 0x00, // Default
		POSITIVE_BIAS = 0x01, // Applies ~+1.1Ga bias for testing purposes
		NEGATIVE_BIAS = 0x02  // Applies ~-1.1Ga bias for testing purposes
	};

	enum Mode : uint8_t {
		CONTINUOUS_MEASUREMENT = 0x00,
		SINGLE_MEASUREMENT     = 0x01, // Default
		IDLE_A                 = 0x02,
		IDLE_B                 = 0x03
		// register set to 0x81 internally when SINGLE_MEASUREMENT completed
	};

	enum Status : uint8_t {
		DATA_READY  = 0x01, // virtual DRDY pin (bit on = pin low)
		DATA_LOCKED = 0x02
	};

	enum Register : uint8_t {
		CONFIGURATION_A = 0x00, // rw, see Averaging, Rate, TestMode
		CONFIGURATION_B = 0x01, // rw, see Gain
		MODE            = 0x02, // rw, see Mode
		DATA_X_MSB      = 0x03, // range: -2048 - 2047, or -4096 on overflow
		DATA_X_LSB      = 0x04,
		DATA_Z_MSB      = 0x05,
		DATA_Z_LSB      = 0x06,
		DATA_Y_MSB      = 0x07,
		DATA_Y_LSB      = 0x08,
		STATUS          = 0x09, // see Status
		IDENTITY_A      = 0x0A, // Fixed value: 0x48 'H'
		IDENTITY_B      = 0x0B, // Fixed value: 0x34 '4'
		IDENTITY_C      = 0x0C  // Fixed value: 0x33 '3'
	};

	enum class ReqState : uint8_t {
		NONE,
		NOPOLL,
		POLLING,
		POLLING_INT,
		POLLING_INT_GOT_DATA
	};

	static const constexpr uint8_t CONFA_RATE_MASK = 0x1C;
	static HMC5883L<TwiT, DRDYPinT> *interruptTarget;

	static bool test_ok(int32_t value, int32_t gain) {
		if(value * gain_value(Gain::G390) > (575 * gain)) {
			return false;
		}
		if(value * gain_value(Gain::G390) < (243 * gain)) {
			return false;
		}
		return true;
	}

	volatile ReqState state;
	Flattener<TwiT,Gain> twiComm;
#define gainCache twiComm.flattened_value
	Flattener<DRDYPinT, uint8_t> drdyPin;
#define confACache drdyPin.flattened_value
	uint8_t currentRegister : 4;
	bool singleSample : 1;
	bool rapidSamples : 1;
	bool gainChanged : 1;
	bool confASent : 1;

	[[gnu::always_inline]]
	void interrupted(void) {
		state = ReqState::POLLING_INT_GOT_DATA;
	}

	static void interrupted_global(void) {
		if(interruptTarget != nullptr) {
			interruptTarget->interrupted();
		}
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr uint8_t i2c_addr(void) {
		return 0x1E;
	}

	[[gnu::const,nodiscard,gnu::always_inline]]
	static constexpr uint32_t i2c_speed(void) {
		return 400000; // Device supports up to 400kHz
	}

	[[gnu::always_inline]]
	typename TwiT::Error set_register(Register r) {
		currentRegister = r;
		twiComm.begin_transmission(i2c_addr(), i2c_speed());
		twiComm.write(r);
		return twiComm.end_transmission();
	}

	[[gnu::always_inline]]
	typename TwiT::Error set_register(Register r, uint8_t value) {
		currentRegister = r + 1;
		twiComm.begin_transmission(i2c_addr(), i2c_speed());
		twiComm.write(r);
		twiComm.write(value);
		return twiComm.end_transmission();
	}

	bool read(uint8_t count, void *buffer, uint16_t maxMicros) {
		twiComm.request_from(i2c_addr(), count, i2c_speed());
		uint16_t t0 = micros();
		uint8_t *b = static_cast<uint8_t*>(buffer);
		currentRegister += count;
		while(true) {
			while(twiComm.available()) {
				*b = twiComm.read();
				++ b;
				if((-- count) == 0) {
					return true;
				}
			}
			if(uint16_t(micros() - t0) > maxMicros) {
				currentRegister = 0xF; // Unknown state
				return false;
			}
		}
	}

	void begin_polling(void) {
		if(drdyPin.supports_interrupts()) {
			interruptTarget = this;
			state = ReqState::POLLING_INT;
			drdyPin.set_interrupt_on_falling(&interrupted_global);
		} else {
			state = ReqState::POLLING;
		}
	}

	void end_polling(void) {
		singleSample = false;
		rapidSamples = false;
		ReqState s = state; // avoid volatile
		if(s == ReqState::POLLING_INT || s == ReqState::POLLING_INT_GOT_DATA) {
			drdyPin.remove_interrupt();
			interruptTarget = nullptr;
		}
		state = ReqState::NONE;
	}

	bool status_data_locked(void) {
		if(set_register(STATUS) != TwiT::Error::SUCCESS) {
			return false;
		}
		uint8_t status = 0x00;
		read(1, &status, 1000);
		return (status & DATA_LOCKED) != 0;
	}

	bool status_data_ready(void) {
		if(set_register(STATUS) != TwiT::Error::SUCCESS) {
			return false;
		}
		uint8_t status = 0x00;
		read(1, &status, 1000);
		return (status & DATA_READY) != 0;
	}

	bool check_ready(void) {
		ReqState s = state; // avoid volatile
		if(s == ReqState::POLLING_INT_GOT_DATA) {
			state = ReqState::POLLING_INT;
			return true;
		}
		if(
			s == ReqState::NONE ||
			s == ReqState::NOPOLL ||
			s == ReqState::POLLING_INT
		) {
			return false;
		}
		if(drdyPin.exists()) {
			if(drdyPin.read_digital() == false) {
				return true;
			}
		} else if(status_data_ready()) {
			return true;
		}
		return false;
	}

	void set_conf_a(uint8_t c) {
		if(confACache != c) {
			confACache = c;
			confASent = false;
		}
	}

	void set_rate(Rate rate) {
		set_conf_a((confACache & ~CONFA_RATE_MASK) | uint8_t(rate));
	}

	void send_conf_a(void) {
		if(!confASent) {
			if(
				set_register(CONFIGURATION_A, confACache) ==
				TwiT::Error::SUCCESS
			) {
				confASent = true;
			}
		}
	}

	void set_mode(Mode mode) {
		send_conf_a();
		gainChanged = false;
		set_register(MODE, mode);
	}

	reading grab_reading(void) {
		if(currentRegister != DATA_X_MSB) {
			if(set_register(DATA_X_MSB) != TwiT::Error::SUCCESS) {
				return reading();
			}
		}
		uint8_t buffer[6];
		if(!read(6, buffer, 10000)) {
			return reading();
		}
		if(gainChanged) {
			// TODO: sample will have been using old gain
			gainChanged = false;
		}

		// Automatically loops back
		currentRegister = DATA_X_MSB;

		if(rapidSamples) {
			set_mode(SINGLE_MEASUREMENT);
		} else if(singleSample) {
			end_polling();
		}

		return reading(
			(buffer[0] << 8) | buffer[1],
			(buffer[4] << 8) | buffer[5],
			(buffer[2] << 8) | buffer[3],
			gainCache
		);
	}

public:
	class calibrator {
	public:
		class calibration {
		public:
			uint16_t x;
			uint16_t y;
			uint16_t z;

			calibration(void) : x(1), y(1), z(1) {}

			calibration(uint16_t xx, uint16_t yy, uint16_t zz)
				: x(xx)
				, y(yy)
				, z(zz)
			{}
		};

	private:
		calibration truth;
		calibration current;
		bool bias;

	public:
		calibrator(void) : truth(), current(), bias(true) {}

		[[gnu::pure,nodiscard,gnu::always_inline]]
		bool is_positive_bias(void) const {
			return bias;
		}

		void set_truth(const calibration &c, bool positive_bias) {
			truth = c;
			current = truth;
			bias = positive_bias;
		}

		void recalibrate(const calibration &c) {
			current = c;
		}

		void decalibrate(void) {
			current = truth;
		}

		void apply(reading &r) const {
			r.set(
				(r.x() * int32_t(truth.x)) / current.x,
				(r.y() * int32_t(truth.y)) / current.y,
				(r.z() * int32_t(truth.z)) / current.z
			);
		}
	};

	ConnectionStatus connection_status(void) {
		auto err = set_register(IDENTITY_A);
		if(err != TwiT::Error::SUCCESS) {
			return ConnectionStatus(err);
		}
		uint8_t buffer[3];
		if(!read(3, buffer, 10000)) {
			return ConnectionStatus::READ_TIMEOUT;
		}
		if(buffer[0] != 0x48 || buffer[1] != 0x34 || buffer[2] != 0x33) {
			return ConnectionStatus::ID_MISMATCH;
		}
		return ConnectionStatus::CONNECTED;
	}

	/**
	 * Re-sync local configuration with configuration of the actual device
	 * (useful if another master may have changed the remote configuration)
	 */
	void check_configuration(void) {
		set_register(CONFIGURATION_A);
		uint8_t output[2];
		read(2, output, 10000);
		confACache = output[0];
		gainCache = Gain(output[1]);
		confASent = true;
	}

	void set_averaging(Averaging averaging) {
		set_conf_a(
			(confACache & CONFA_RATE_MASK) |
			uint8_t(averaging) |
			uint8_t(NO_BIAS)
		);
		if(state != ReqState::NONE) {
			send_conf_a();
		}
	}

	void set_gain(Gain gain) {
		if(gain == gainCache) {
			return;
		}
		set_register(CONFIGURATION_B, uint8_t(gain));
		gainChanged = true;
		gainCache = gain;
	}

	void stop(void) {
		if(state != ReqState::NONE) {
			set_register(MODE, IDLE_A);
			end_polling();
		}
	}

	reading next_reading(void) {
		ReqState s = state; // avoid volatile
		if(s == ReqState::NONE || s == ReqState::NOPOLL) {
			read_continuous_async(Rate::R75_HZ);
		}

		uint16_t t0 = millis();
		uint16_t limit = (singleSample
			? 6
			: rate_period_millis(Rate(confACache & CONFA_RATE_MASK))
		);
		while(uint16_t(millis() - t0) < limit) {
			if(check_ready()) {
				break;
			}
		}

		return grab_reading();
	}

	bool peek_reading(reading &output, uint16_t maxMicros = 0) {
		uint16_t t0 = micros();
		while(true) {
			if(check_ready()) {
				output = grab_reading();
				return true;
			}
			if(uint16_t(micros() - t0) > maxMicros) {
				return false;
			}
		}
	}

	reading current_reading(void) {
		if(state == ReqState::NONE) {
			rapidSamples = false;
			singleSample = false;
			state = ReqState::NOPOLL;
			set_rate(Rate::R75_HZ);
			set_mode(CONTINUOUS_MEASUREMENT);
			delay(6); // Wait for first sample to populate
		}
		return grab_reading();
	}

	void read_async(void) {
		singleSample = true;
		rapidSamples = false;
		begin_polling();
		set_mode(SINGLE_MEASUREMENT);
	}

	void read_continuous_async(Rate rate) {
		begin_polling();
		if(rate == Rate::R160_HZ) {
			singleSample = true;
			rapidSamples = true;
			set_mode(SINGLE_MEASUREMENT);
		} else {
			singleSample = false;
			rapidSamples = false;
			set_rate(rate);
			set_mode(CONTINUOUS_MEASUREMENT);
		}
	}

	[[nodiscard,gnu::always_inline]]
	reading read_sync(void) {
		read_async();
		return next_reading();
	}

	[[nodiscard]]
	bool record_calibration(
		typename calibrator::calibration &output,
		bool positive_bias = true
	) {
		stop();
		Gain gain = Gain::G390;
		TestMode mode = positive_bias ? POSITIVE_BIAS : NEGATIVE_BIAS;
		set_register(CONFIGURATION_A,
			uint8_t(Averaging::A8) |
			uint8_t(Rate::R75_HZ) |
			mode
		);
		set_register(CONFIGURATION_B, uint8_t(gain));
		set_register(MODE, CONTINUOUS_MEASUREMENT);
		delay(6);
		uint8_t buffer[6];
		bool success = false;
		while(true) {
			if(!read(6, buffer, 10000)) {
				break;
			}
			int16_t x = (buffer[0] << 8) | buffer[1];
			int16_t y = (buffer[4] << 8) | buffer[5];
			int16_t z = (buffer[2] << 8) | buffer[3];
			int16_t g = gain_value(gain);
			if(mode == NEGATIVE_BIAS) {
				x = -x;
				y = -y;
				z = -z;
			}
			if(test_ok(x, g) && test_ok(y, g) && test_ok(z, g)) {
				output.x = (x * gain_value(Gain::G390)) / g;
				output.y = (y * gain_value(Gain::G390)) / g;
				output.z = (z * gain_value(Gain::G390)) / g;
				success = true;
				break;
			}
			if(gain == Gain::G230) {
				break;
			}
			gain = Gain(uint8_t(gain) + 0x20); // 0x20 = LSB for gain
			set_register(CONFIGURATION_B, uint8_t(gain));
			delay(14);
			// Skip next reading (will have old gain)
			if(!read(6, buffer, 10000)) {
				break;
			}
			delay(14);
		}
		// Reset to original settings
		confASent = false;
		send_conf_a();
		gainChanged = (gain != gainCache);
		if(gainChanged) {
			set_register(CONFIGURATION_B, uint8_t(gainCache));
		}
		set_register(MODE, IDLE_A);
		return success;
	}

	bool record_calibration(calibrator &c, bool positive_bias = true) {
		typename calibrator::calibration current;
		if(!record_calibration(current, positive_bias)) {
			return false;
		}
		c.set_truth(current, positive_bias);
		return true;
	}

	bool recalibrate(calibrator &c) {
		typename calibrator::calibration current;
		if(!record_calibration(current, c.is_positive_bias())) {
			return false;
		}
		c.recalibrate(current);
		return true;
	}

	[[nodiscard,gnu::always_inline]]
	bool test(bool positive_bias = true) {
		typename calibrator::calibration c;
		return record_calibration(c, positive_bias);
	}

	HMC5883L(TwiT twi, DRDYPinT drdy)
		: state(ReqState::NONE)
		, twiComm(twi, Gain::G1090) // gainCache - Assume default
		, drdyPin(
			drdy,
			// confACache - Assume default
			uint8_t(Averaging::A1) |
			uint8_t(Rate::R15_HZ) |
			uint8_t(NO_BIAS)
		)
		, currentRegister(0xF) // Assume unknown
		, singleSample(false)
		, rapidSamples(false)
		, gainChanged(false)
		, confASent(false)
	{
		drdyPin.set_input();
	}

	~HMC5883L(void) {
		stop();
	}

#undef gainCache
#undef confACache
};

template <
	typename TwiT,
	typename DRDYPinT
>
HMC5883L<TwiT, DRDYPinT> *HMC5883L<TwiT, DRDYPinT>::interruptTarget = nullptr;

template <
	typename TwiT,
	typename DRDYPinT
>
[[gnu::always_inline]]
inline HMC5883L<TwiT, DRDYPinT> MakeHMC5883L(
	TwiT twi,
	DRDYPinT drdy // optional (use VoidPin to omit)
) {
	return HMC5883L<TwiT, DRDYPinT>(
		twi,
		drdy
	);
}

#endif

/*
 * IRReceiver - Records and interprets received IR signals.
 * Written in 2018 by David Evans
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

#ifndef IRRECEIVER_H_INCLUDED
#define IRRECEIVER_H_INCLUDED

#include "IR.h"
#include "ext.h"

#define STRICT_HEADER 0x20

class IRReceiver {
public:
	static constexpr uint16_t NO_TIMEOUT = 0xFFFF;

	enum class Activation : uint8_t {
		ACTIVE_LOW  = 0x00,
		ACTIVE_HIGH = 0x01
	};

	enum class ReturnPreference : uint8_t {
		EARLIEST = 0x00,
		LATEST   = 0x10
	};

protected:
	IRReceiver(void) = default;
	IRReceiver(const IRReceiver&) = delete;
	IRReceiver(IRReceiver&&) = default;

	IRReceiver &operator=(const IRReceiver&) = delete;
	IRReceiver &operator=(IRReceiver&&) = delete;
};

template <typename DataPinT, typename StorageT>
class SyncIRReceiver_impl : public IRReceiver {
	ext::Flattener<DataPinT,uint8_t> dataPin;
#define state dataPin.flattened_value
	StorageT current;

	template <typename DPT>
	const StorageT *receive_command(DPT &dat, uint16_t t0) {
		const bool active = (state & uint8_t(Activation::ACTIVE_HIGH));

		// wait for header to end
		while(dat.read_digital() == active) {
			if(uint16_t(micros()) - t0 > StorageT::max_header_thresh()) {
				// Header is too long to be a real command
				return nullptr;
			}
		}
		uint16_t t1 = micros();

		// Store header length
		current.reset(t1 - t0);

		// Store inactive-active pairs
		while(true) {
			// Inactive
			while(dat.read_digital() != active) {
				if(uint16_t(micros()) - t1 > StorageT::max_silence_thresh()) {
					// Silent for too long; command must have finished
					return &current;
				}
			}
			uint16_t t2 = micros();

			// Active
			while(dat.read_digital() == active) {
				if(uint16_t(micros()) - t2 > StorageT::max_data_thresh()) {
					// Another command has begun; ours must have finished
					return &current;
				}
			}
			uint16_t t3 = micros();
			current.add(t2 - t1);
			current.add(t3 - t2);
			t1 = t3;
		}
	}

public:
	[[gnu::always_inline]]
	inline void set_strict_header(bool check) {
		state = (state & ~STRICT_HEADER) | (check * STRICT_HEADER);
	}

	const StorageT *await_command(uint16_t timeoutMillis = NO_TIMEOUT) {
		const uint8_t headerThresh = ((state & STRICT_HEADER)
			? uint8_t(StorageT::min_header_thresh() / 50)
			: 0
		);
		const bool active = (state & uint8_t(Activation::ACTIVE_HIGH));

		auto dat = dataPin.fast();
		uint32_t begin = millis();

		// Wait for command to begin
		do {
			if(dat.read_digital() != active) {
				continue;
			}

			uint16_t t0 = micros();
			// Is this a header?
			bool valid = true;
			for(uint8_t i = headerThresh; (i --) > 0;) {
				delayMicroseconds(50);
				if(dat.read_digital() != active) {
					valid = false;
					break;
				}
			}
			if(valid) {
				return receive_command(dat, t0);
			}
		} while(
			timeoutMillis == NO_TIMEOUT ||
			millis() - begin < timeoutMillis
		);

		// Timed out
		return nullptr;
	}

	[[gnu::always_inline]]
	inline void invalidate(const StorageT*) {
		// Provided for compatibility with asynchronous API
		// (calling this method gives no actual advantage when using the
		// synchronous version)
		current->reset(0);
	}

	SyncIRReceiver_impl(DataPinT dat, Activation act = Activation::ACTIVE_LOW)
		: dataPin(dat, uint8_t(act) | STRICT_HEADER)
		, current()
	{
		dataPin.set_input();
	}

	SyncIRReceiver_impl(SyncIRReceiver_impl &&b)
		: dataPin(static_cast<ext::Flattener<DataPinT,uint8_t>&&>(b.dataPin))
		, current(static_cast<StorageT&&>(b.current))
	{}

#undef state
};

#define RUNNING 0x80
#define LATCHED 0x40
#define LATCH_PROTECT 0x08
#define STATE_BITS 0x06

template <typename DataPinT, typename StorageT>
class AsyncIRReceiver_impl : public IRReceiver {
	static AsyncIRReceiver_impl<DataPinT,StorageT> *interruptTarget;

	static void update_global(void) {
		interruptTarget->update();
	}

	ext::Flattener<DataPinT,volatile uint8_t> dataPin;
#define state dataPin.flattened_value
	uint16_t lastTm;
	StorageT *volatile current;
	StorageT *volatile latched;

	[[gnu::always_inline]]
	inline void set_state(uint8_t st) {
		state = (state & ~STATE_BITS) | st;
	}

	void latch(void) {
		if(state & LATCH_PROTECT) {
			return;
		}
		if(!(state & LATCHED) || (state & uint8_t(ReturnPreference::LATEST))) {
			StorageT *t = current;
			current = latched;
			latched = t;
			state |= LATCHED;
		}
	}

	void check_header(uint16_t tm) {
		const uint16_t duration = tm - lastTm;
		if(
			(state & STRICT_HEADER) &&
			duration < StorageT::min_header_thresh()
		) {
			// Header is too short; probably data from a missed header
			set_state(0);
		} else if(duration <= StorageT::max_header_thresh()) {
			current->reset(duration);
			set_state(4);
			lastTm = tm;
		} else {
			// Header is too long to be a real command
			set_state(0);
		}
	}

	void update(void) {
		const bool active = (state & uint8_t(Activation::ACTIVE_HIGH));
		const bool pin = (dataPin.read_digital() == active);
		const uint16_t tm = micros();
		const uint16_t duration = tm - lastTm;
		switch((state & STATE_BITS) | pin) {
		// Waiting for header to begin
		case 0: break;
		case 1: // Header has begun
			set_state(2);
			lastTm = tm;
			break;

		// Waiting for possible header to end
		case 2: // Possible header has ended
			check_header(tm);
			break;
		case 3: break;

		// Waiting for next activity to begin, or silence timeout
		case 4: // Possible silence timeout
			if(duration > StorageT::max_silence_thresh()) {
				// Silent for too long; command must have finished
				latch();
				set_state(0);
			}
			break;
		case 5: // Next activity has begun, possibly after silence timeout
			if(duration > StorageT::max_silence_thresh()) {
				latch();
				// Assume new activity is a header
				set_state(2);
				lastTm = tm;
			} else {
				current->add(duration);
				set_state(6);
				lastTm = tm;
			}
			break;

		// Waiting for current activity to end, or to be revealed to be a header
		case 6: // Current activity ended, but possibly is a header
			if(duration > StorageT::max_data_thresh()) {
				// Another command has begun; ours must have finished
				current->pop(); // pop latest silence
				latch();
				check_header(tm);
			} else {
				current->add(duration);
				set_state(4);
				lastTm = tm;
			}
			break;
		case 7: // Possible header
			if(duration > StorageT::max_data_thresh()) {
				// Another command has begun; ours must have finished
				current->pop(); // pop latest silence
				latch();
				set_state(2); // keep current lastTm (data is ongoing)
			}
			break;
		}
	}

public:
	[[gnu::always_inline]]
	inline void set_strict_header(bool check) {
		uint8_t oldSREG = SREG;
		cli();
		state = (state & ~STRICT_HEADER) | (check * STRICT_HEADER);
		SREG = oldSREG;
	}

	[[gnu::always_inline]]
	inline void set_return_preference(ReturnPreference pref) {
		uint8_t oldSREG = SREG;
		cli();
		state = (state & ~uint8_t(ReturnPreference::LATEST)) | uint8_t(pref);
		SREG = oldSREG;
	}

	[[gnu::always_inline]]
	inline void invalidate(const StorageT *ptr) {
		uint8_t oldSREG = SREG;
		cli();
		if(ptr == latched) {
			state &= ~LATCH_PROTECT;
		}
		latched->reset(0);
		SREG = oldSREG;
	}

	[[gnu::always_inline]]
	inline void start(void) {
		if(!(state & RUNNING)) {
			state = (state & ~STATE_BITS) | RUNNING;
			interruptTarget = this;
			dataPin.set_interrupt_on_change(&update_global);
		}
	}

	[[gnu::always_inline]]
	inline void stop(void) {
		if(state & RUNNING) {
			dataPin.remove_interrupt();
			interruptTarget = nullptr;
			state &= ~RUNNING;
		}
	}

	const StorageT *next_command(void) {
		uint8_t oldSREG = SREG;
		cli();

		state &= ~LATCH_PROTECT;
		if(!(state & LATCHED)) {
			update();
		}

		StorageT *v;
		if(state & LATCHED) {
			v = latched;
			state = (state & ~LATCHED) | LATCH_PROTECT;
		} else {
			v = nullptr;
		}

		SREG = oldSREG;
		return v;
	}

	const StorageT *await_command(uint16_t timeoutMillis = NO_TIMEOUT) {
		uint32_t begin = millis();

		// Wait for command to latch
		do {
			const StorageT *v = next_command();
			if(v) {
				return v;
			}
		} while(
			timeoutMillis == NO_TIMEOUT ||
			millis() - begin < timeoutMillis
		);

		// Timed out
		return nullptr;
	}

	AsyncIRReceiver_impl(DataPinT dat, Activation act = Activation::ACTIVE_LOW)
		: dataPin(dat,
			uint8_t(act) |
			STRICT_HEADER |
			uint8_t(ReturnPreference::EARLIEST)
		)
		, lastTm(0)
		, current(new StorageT())
		, latched(new StorageT())
	{
		dataPin.set_input();
	}

	AsyncIRReceiver_impl(AsyncIRReceiver_impl &&b)
		: dataPin(static_cast<ext::Flattener<
			DataPinT,
			volatile uint8_t
		>&&>(b.dataPin))
		, lastTm(b.lastTm)
		, current(b.current)
		, latched(b.latched)
	{
		if(b.state & RUNNING) {
			interruptTarget = this;
			b.state &= ~RUNNING;
		}
		b.current = nullptr;
		b.latched = nullptr;
	}

	~AsyncIRReceiver_impl(void) {
		stop();
		if(current) {
			delete current;
		}
		if(latched) {
			delete latched;
		}
	}

#undef state
};

#undef RUNNING
#undef LATCHED
#undef STRICT_HEADER
#undef LATCH_PROTECT
#undef STATE_BITS

template <typename DataPinT, typename StorageT>
AsyncIRReceiver_impl<DataPinT,StorageT> *
AsyncIRReceiver_impl<DataPinT,StorageT>::interruptTarget = nullptr;

template <typename DataPinT, typename StorageT = IRCommand>
[[gnu::always_inline,nodiscard]]
inline SyncIRReceiver_impl<DataPinT,StorageT> MakeSynchronousIRReceiver(
	DataPinT dat,
	IRReceiver::Activation act = IRReceiver::Activation::ACTIVE_LOW
) {
	return SyncIRReceiver_impl<DataPinT,StorageT>(dat, act);
}

template <typename DataPinT, typename StorageT = IRCommand>
[[gnu::always_inline,nodiscard]]
inline AsyncIRReceiver_impl<DataPinT,StorageT> MakeAsynchronousIRReceiver(
	DataPinT dat,
	IRReceiver::Activation act = IRReceiver::Activation::ACTIVE_LOW
) {
	return AsyncIRReceiver_impl<DataPinT,StorageT>(dat, act);
}

#endif

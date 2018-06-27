#ifndef MOCK_TWI_MASTER_H_INCLUDED
#define MOCK_TWI_MASTER_H_INCLUDED

#include "BaseMock.h"

class TWIMaster {
public:
	enum Error : uint8_t {
		SUCCESS = 50,
		DATA_TOO_LONG = 60,
		NACK_ADDR = 70,
		NACK_DATA = 80,
		OTHER = 90
	};

	class Mock {
	public:
		uint8_t transmission_write_buffer[32] = {};
		uint8_t transmission_write_count = 0;
		uint32_t set_max_clock_last_hz = 0;
		bool set_max_clock_last_force = false;
		uint8_t transmission_stop_call_count = 0;
		Error transmission_stop_return_value = SUCCESS;
		uint8_t transmission_restart_call_count = 0;
		Error transmission_restart_return_value = SUCCESS;
		uint8_t begin_transmission_last_address = 0;
		uint8_t send_last_address = 0;
		uint8_t send_last_value = 0;
		Error send_return_value = SUCCESS;
		uint8_t request_from_last_address = 0;
		uint8_t request_from_last_count = 0;
		bool request_from_last_stop = false;
		bool request_read_return_value = false;
		uint8_t request_available_return_value = 0;
		uint8_t request_read_buffer[32] = {};
		uint8_t request_read_count = 0;
		uint16_t request_read_last_maxMicros = 0;

		TWIMaster implementation(void);
	};

private:
	Mock *mock;
	TWIMaster(Mock *m) : mock(m) {}

	friend class Mock;

public:
	static constexpr uint8_t max_write_bytes(void) {
		return 32;
	}

	class Transmission {
		Mock *mock;
		Transmission(Mock *m) : mock(m) {}

		friend class TWIMaster;

	public:
		Transmission(const Transmission&) = delete;
		Transmission(Transmission&&) = default;

		void write(uint8_t data) {
			mock->transmission_write_buffer[mock->transmission_write_count ++] = data;
		}

		void write(const uint8_t *data, uint8_t count) {
			for(uint8_t i = 0; i < count; ++ i) {
				mock->transmission_write_buffer[mock->transmission_write_count ++] = data[i];
			}
		}

		Error stop(void) {
			++ mock->transmission_stop_call_count;
			return mock->transmission_stop_return_value;
		}

		Error restart(void) {
			++ mock->transmission_restart_call_count;
			return mock->transmission_restart_return_value;
		}
	};

	class Request {
		Mock *mock;
		Request(Mock *m) : mock(m) {}

		friend class TWIMaster;

	public:
		Request(const Request&) = delete;
		Request(Request&&) = default;

		uint8_t available(void) const {
			return mock->request_available_return_value;
		}

		uint8_t read(void) {
			return mock->request_read_buffer[mock->request_read_count ++];
		}

		uint8_t peek(void) const {
			return mock->request_read_buffer[mock->request_read_count];
		}

		bool read(void *buffer, uint8_t count, uint16_t maxMicros) {
			mock->request_read_last_maxMicros = maxMicros;
			uint8_t *b = static_cast<uint8_t*>(buffer);
			for(uint8_t i = 0; i < count; ++ i) {
				b[i] = mock->request_read_buffer[mock->request_read_count ++];
			}
			return mock->request_read_return_value;
		}
	};

	void set_max_clock(uint32_t hz, bool force = false) {
		mock->set_max_clock_last_hz = hz;
		mock->set_max_clock_last_force = force;
	}

	Transmission begin_transmission(uint8_t address) {
		mock->begin_transmission_last_address = address;
		return Transmission(mock);
	}

	Error send(uint8_t address, uint8_t value) {
		mock->send_last_address = address;
		mock->send_last_value = value;
		return mock->send_return_value;
	}

	Request request_from(uint8_t address, uint8_t count, bool stop = true) {
		mock->request_from_last_address = address;
		mock->request_from_last_count = count;
		mock->request_from_last_stop = stop;
		return Request(mock);
	}

	bool request_from(
		uint8_t address,
		void *buffer,
		uint8_t count,
		uint16_t maxMicros,
		bool stop = true
	) {
		mock->request_from_last_address = address;
		uint8_t *b = static_cast<uint8_t*>(buffer);
		for(uint8_t i = 0; i < count; ++ i) {
			b[i] = mock->request_read_buffer[mock->request_read_count ++];
		}
		mock->request_from_last_count = count;
		mock->request_read_last_maxMicros = maxMicros;
		mock->request_from_last_stop = stop;
		return mock->request_read_return_value;
	}
};

TWIMaster TWIMaster::Mock::implementation(void) {
	return TWIMaster(this);
}

#include "EndMocks.h"

#endif

#include <boost/test/unit_test.hpp>

#include <MockMicros.h>
#include <MockPin.h>
#include <MockTWIMaster.h>

#include "../ADXL345.h"

BOOST_AUTO_TEST_SUITE(ADXL345_test)

BOOST_AUTO_TEST_CASE(connection_status_checks_id) {
	TWIMaster::Mock mockTWI;
	mockTWI.send_return_value = TWIMaster::SUCCESS;
	mockTWI.request_read_return_value = TWIMaster::SUCCESS;
	mockTWI.request_read_buffer[0] = 0xE5;

	auto o = MakeADXL345(
		mockTWI.implementation(),
		Pin::Mock().implementation(),
		Pin::Mock().implementation(),
		true
	);
	BOOST_CHECK_EQUAL(
		uint8_t(o.connection_status()),
		uint8_t(ADXL345::ConnectionStatus::CONNECTED)
	);

	BOOST_CHECK_EQUAL(mockTWI.send_last_address, 0x53);
	BOOST_CHECK_EQUAL(mockTWI.send_last_hz, 400000);
	BOOST_CHECK_EQUAL(mockTWI.send_last_value, 0x00); // DEVID

	BOOST_CHECK_EQUAL(mockTWI.request_from_last_address, 0x53);
	BOOST_CHECK_EQUAL(mockTWI.request_from_last_hz, 400000);
	BOOST_CHECK_EQUAL(mockTWI.request_from_last_count, 1);
	BOOST_CHECK_EQUAL(mockTWI.request_read_last_maxMicros, 10000);
}

BOOST_AUTO_TEST_SUITE_END()

#include <boost/test/unit_test.hpp>

#include "../Bitmask18.h"

BOOST_AUTO_TEST_SUITE(Bitmask18_test)

BOOST_AUTO_TEST_CASE(const_getters) {
	Bitmask18<5,7> bitmask;
	BOOST_CHECK_EQUAL(bitmask.width(), 5);
	BOOST_CHECK_EQUAL(bitmask.height(), 7);
	BOOST_CHECK_EQUAL(bitmask.height_bytes(), 1);
	BOOST_CHECK_EQUAL(bitmask.raw_step(), 5);
}

BOOST_AUTO_TEST_CASE(begins_zeroed) {
	Bitmask18<16,16> bitmask;
	for(int i = 0; i < 32; ++ i) {
		BOOST_CHECK_EQUAL(bitmask.raw()[i], 0x00);
	}
}

BOOST_AUTO_TEST_CASE(clear_int) {
	Bitmask18<16,16> bitmask;
	bitmask.clear(0x68);
	for(int i = 0; i < 32; ++ i) {
		BOOST_CHECK_EQUAL(bitmask.raw()[i], 0x68);
	}
}

BOOST_AUTO_TEST_CASE(clear_blendmode) {
	Bitmask18<16,16> bitmask;
	bitmask.clear(BlendMode::ON);
	for(int i = 0; i < 32; ++ i) {
		BOOST_CHECK_EQUAL(bitmask.raw()[i], 0xFF);
	}
}

BOOST_AUTO_TEST_CASE(pixel_on) {
	Bitmask18<8,8> bitmask;
	bitmask.pixel_on(2, 2);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x04);
}

BOOST_AUTO_TEST_CASE(pixel_off) {
	Bitmask18<8,8> bitmask;
	bitmask.clear(BlendMode::ON);
	bitmask.pixel_off(2, 2);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0xFB);
}

BOOST_AUTO_TEST_CASE(pixel_toggle) {
	Bitmask18<8,8> bitmask;
	bitmask.pixel_toggle(2, 2);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x04);
	bitmask.pixel_toggle(2, 2);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x00);
}

BOOST_AUTO_TEST_CASE(pixel_set) {
	Bitmask18<8,8> bitmask;
	bitmask.set_pixel(2, 2, BlendMode::ON, PATTERN_ON);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x04);
	bitmask.set_pixel(2, 2, BlendMode::OFF, PATTERN_ON);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x00);
	bitmask.set_pixel(2, 2, BlendMode::XOR, PATTERN_ON);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x04);
	bitmask.set_pixel(2, 2, BlendMode::XOR, PATTERN_ON);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x00);
	bitmask.set_pixel(2, 2, BlendMode::NOP, PATTERN_ON);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x00);
}

BOOST_AUTO_TEST_CASE(fill_rect_small) {
	Bitmask18<16,16> bitmask;
	bitmask.fill_rect(2, 2, 4, 4, BlendMode::ON, PATTERN_ON);
	BOOST_CHECK_EQUAL(bitmask.raw()[0], 0x00);
	BOOST_CHECK_EQUAL(bitmask.raw()[1], 0x00);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x3C);
	BOOST_CHECK_EQUAL(bitmask.raw()[3], 0x3C);
	BOOST_CHECK_EQUAL(bitmask.raw()[4], 0x3C);
	BOOST_CHECK_EQUAL(bitmask.raw()[5], 0x3C);
	for(int i = 6; i < 32; ++ i) {
		BOOST_CHECK_EQUAL(bitmask.raw()[i], 0x00);
	}
}

BOOST_AUTO_TEST_CASE(fill_rect_clip) {
	Bitmask18<16,16> bitmask;
	bitmask.fill_rect(-2, -2, 19, 19, BlendMode::ON, PATTERN_ON);
	for(int i = 6; i < 32; ++ i) {
		BOOST_CHECK_EQUAL(bitmask.raw()[i], 0xFF);
	}
}

BOOST_AUTO_TEST_CASE(outline_rect) {
	Bitmask18<16,16> bitmask;
	bitmask.outline_rect(2, 2, 4, 4, 1, BlendMode::ON, PATTERN_ON);
	BOOST_CHECK_EQUAL(bitmask.raw()[0], 0x00);
	BOOST_CHECK_EQUAL(bitmask.raw()[1], 0x00);
	BOOST_CHECK_EQUAL(bitmask.raw()[2], 0x3C);
	BOOST_CHECK_EQUAL(bitmask.raw()[3], 0x24);
	BOOST_CHECK_EQUAL(bitmask.raw()[4], 0x24);
	BOOST_CHECK_EQUAL(bitmask.raw()[5], 0x3C);
	for(int i = 6; i < 32; ++ i) {
		BOOST_CHECK_EQUAL(bitmask.raw()[i], 0x00);
	}
}

BOOST_AUTO_TEST_CASE(outline_rect_clip) {
	Bitmask18<16,16> bitmask;
	bitmask.outline_rect(-2, -2, 19, 19, 1, BlendMode::ON, PATTERN_ON);
	for(int i = 6; i < 32; ++ i) {
		BOOST_CHECK_EQUAL(bitmask.raw()[i], 0x00);
	}
}

BOOST_AUTO_TEST_CASE(render_fs_bitmap) {
	const uint8_t fsbmp[] = {
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
	};
	Bitmask18<16,16> bitmask;
	bitmask.render_fs_bitmap(fsbmp);
	for(int i = 0; i < 32; ++ i) {
		BOOST_CHECK_EQUAL(bitmask.raw()[i], 0x20 + i);
	}
}

BOOST_AUTO_TEST_CASE(render_fs_bitmap_progmem) {
	static PROGMEM const uint8_t fsbmp[] = {
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F
	};
	Bitmask18<16,16> bitmask;
	bitmask.render_fs_bitmap(WrapProgMem(fsbmp));
	for(int i = 0; i < 32; ++ i) {
		BOOST_CHECK_EQUAL(bitmask.raw()[i], 0x60 + i);
	}
}

BOOST_AUTO_TEST_CASE(render_bitmap_tiny) {
	const uint8_t bmp[] = {0xF1, 0xF3, 0xF2, 0xF3};
	Bitmask18<8,8> bitmask;

	const uint32_t checkA[] = {
		0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
		0x001000, 0x003000, 0x002000, 0x003000,
		0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
	};

	const uint32_t checkB[] = {
		0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF,
		0xFFDFFF, 0xFFFFFF, 0xFFEFFF, 0xFFFFFF,
		0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 0xFFFFFF
	};

	for(int x = -3; x < 10; ++ x) {
		for(int y = -3; y < 10; ++ y) {
			bitmask.clear(BlendMode::OFF);
			bitmask.render_bitmap(bmp, nullptr, x, y, 4, 2, 4, BlendMode::ON);
			for(int i = 0; i < 8; ++ i) {
				BOOST_CHECK_EQUAL(bitmask.raw()[i], uint8_t(checkA[i+9-x] >> (12 - y)));
			}
			bitmask.clear(BlendMode::ON);
			bitmask.render_bitmap(bmp, nullptr, x, y, 4, 2, 4, BlendMode::ON);
			for(int i = 0; i < 8; ++ i) {
				BOOST_CHECK_EQUAL(bitmask.raw()[i], uint8_t(checkB[i+9-x] >> (12 - y)));
			}
		}
	}
}

// TODO: test large bitmaps, ellipses, triangles, lines

BOOST_AUTO_TEST_SUITE_END()

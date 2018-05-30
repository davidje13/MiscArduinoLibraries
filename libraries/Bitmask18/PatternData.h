#ifndef PATTERN_DATA_H_INCLUDED
#define PATTERN_DATA_H_INCLUDED

// Auto-generated from PatternData.h.gen

#include <ProgMem.h>

#ifdef NO_PATTERNS

static PROGMEM const uint8_t RAW_PATTERN_DATA[] = {
	0xFF, 0xFF, 0xFF, 0xFF
};

#define PATTERN_ON  (RAW_PATTERN_DATA + 0)

#elifdef REDUCED_PATTERNS

static PROGMEM const uint8_t RAW_PATTERN_DATA[] = {
	0x11, 0x11, 0x11, 0xFF, 0xAA, 0xFF, 0xAA, 0xFF,
	0xFF, 0xFF, 0xFF, 0xEE, 0x00, 0xFF, 0x00, 0xFF,
	0xDD, 0xFF, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xBB,
	0xEE, 0xBB, 0xEE, 0xBB, 0x55, 0xFF, 0x55, 0xFF,
	0x55, 0xAA, 0x55, 0xAA, 0xAA, 0xAA, 0xAA
};

#define PATTERN_CHECKER     (RAW_PATTERN_DATA + 32)
#define PATTERN_FILL75_11   (RAW_PATTERN_DATA + 24)
#define PATTERN_FILL75_12   (RAW_PATTERN_DATA + 19)
#define PATTERN_FILL75_21   (RAW_PATTERN_DATA + 23)
#define PATTERN_FILL75_22   (RAW_PATTERN_DATA + 18)
#define PATTERN_FILL87_22   (RAW_PATTERN_DATA + 15)
#define PATTERN_FILL94_41   (RAW_PATTERN_DATA + 8)
#define PATTERN_GRID_41     (RAW_PATTERN_DATA + 0)
#define PATTERN_GRID_SM_11  (RAW_PATTERN_DATA + 29)
#define PATTERN_GRID_SM_12  (RAW_PATTERN_DATA + 3)
#define PATTERN_GRID_SM_21  (RAW_PATTERN_DATA + 28)
#define PATTERN_GRID_SM_22  (RAW_PATTERN_DATA + 4)
#define PATTERN_HLINES_SM   (RAW_PATTERN_DATA + 35)
#define PATTERN_ON          (RAW_PATTERN_DATA + 7)
#define PATTERN_VLINES_SM   (RAW_PATTERN_DATA + 12)

#else

static PROGMEM const uint8_t RAW_PATTERN_DATA[] = {
	0xEE, 0xDD, 0xBB, 0x77, 0xEE, 0xDD, 0xBB, 0xEE,
	0xBB, 0xEE, 0xBB, 0xFF, 0xEE, 0xFF, 0xBB, 0xFF,
	0xEE, 0x77, 0xBB, 0xDD, 0xEE, 0x77, 0xBB, 0xCC,
	0x66, 0x33, 0x99, 0xCC, 0xFF, 0x22, 0x22, 0x22,
	0x22, 0xFF, 0x22, 0x22, 0xFF, 0xDD, 0xFF, 0x77,
	0xFF, 0xDD, 0xFF, 0xFF, 0xFF, 0xFF, 0xEE, 0xFF,
	0xFF, 0xFF, 0xDD, 0xFF, 0xFF, 0xFF, 0xBB, 0xFF,
	0xFF, 0xFF, 0x77, 0xFF, 0xFF, 0xFF, 0x11, 0x11,
	0x11, 0x11, 0xFF, 0x11, 0x11, 0x55, 0xFF, 0x55,
	0xFF, 0x55, 0x88, 0x55, 0x22, 0x55, 0x88, 0x55,
	0xDD, 0x55, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0x33,
	0x33, 0x33, 0x33, 0x66, 0xCC, 0x99, 0x33, 0x66,
	0x66, 0x66, 0x66, 0x44, 0x44, 0x44, 0xFF, 0x44,
	0x44, 0x44, 0x44, 0x88, 0xFF, 0x88, 0x88, 0x88,
	0x88, 0xFF, 0x88, 0x00, 0xFF, 0x00, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x55, 0xAA,
	0x55, 0xAA, 0x44, 0xAA, 0x11, 0xAA, 0x44, 0xAA,
	0xFF, 0xAA, 0xFF, 0xAA, 0xAA, 0xAA, 0xAA
};

#define PATTERN_BRICKS              (RAW_PATTERN_DATA + 79)
#define PATTERN_CHECKER             (RAW_PATTERN_DATA + 126)
#define PATTERN_FILL75_11           (RAW_PATTERN_DATA + 7)
#define PATTERN_FILL75_12           (RAW_PATTERN_DATA + 83)
#define PATTERN_FILL75_21           (RAW_PATTERN_DATA + 6)
#define PATTERN_FILL75_22           (RAW_PATTERN_DATA + 82)
#define PATTERN_FILL87_11           (RAW_PATTERN_DATA + 12)
#define PATTERN_FILL87_12           (RAW_PATTERN_DATA + 37)
#define PATTERN_FILL87_21           (RAW_PATTERN_DATA + 11)
#define PATTERN_FILL87_22           (RAW_PATTERN_DATA + 36)
#define PATTERN_FILL87_31           (RAW_PATTERN_DATA + 10)
#define PATTERN_FILL87_32           (RAW_PATTERN_DATA + 39)
#define PATTERN_FILL87_41           (RAW_PATTERN_DATA + 13)
#define PATTERN_FILL87_42           (RAW_PATTERN_DATA + 38)
#define PATTERN_FILL94_11           (RAW_PATTERN_DATA + 46)
#define PATTERN_FILL94_12           (RAW_PATTERN_DATA + 41)
#define PATTERN_FILL94_13           (RAW_PATTERN_DATA + 54)
#define PATTERN_FILL94_14           (RAW_PATTERN_DATA + 58)
#define PATTERN_FILL94_21           (RAW_PATTERN_DATA + 45)
#define PATTERN_FILL94_22           (RAW_PATTERN_DATA + 40)
#define PATTERN_FILL94_23           (RAW_PATTERN_DATA + 53)
#define PATTERN_FILL94_24           (RAW_PATTERN_DATA + 57)
#define PATTERN_FILL94_31           (RAW_PATTERN_DATA + 44)
#define PATTERN_FILL94_32           (RAW_PATTERN_DATA + 48)
#define PATTERN_FILL94_33           (RAW_PATTERN_DATA + 52)
#define PATTERN_FILL94_34           (RAW_PATTERN_DATA + 56)
#define PATTERN_FILL94_41           (RAW_PATTERN_DATA + 43)
#define PATTERN_FILL94_42           (RAW_PATTERN_DATA + 47)
#define PATTERN_FILL94_43           (RAW_PATTERN_DATA + 51)
#define PATTERN_FILL94_44           (RAW_PATTERN_DATA + 55)
#define PATTERN_GRID_11             (RAW_PATTERN_DATA + 61)
#define PATTERN_GRID_12             (RAW_PATTERN_DATA + 28)
#define PATTERN_GRID_13             (RAW_PATTERN_DATA + 102)
#define PATTERN_GRID_14             (RAW_PATTERN_DATA + 108)
#define PATTERN_GRID_21             (RAW_PATTERN_DATA + 65)
#define PATTERN_GRID_22             (RAW_PATTERN_DATA + 32)
#define PATTERN_GRID_23             (RAW_PATTERN_DATA + 101)
#define PATTERN_GRID_24             (RAW_PATTERN_DATA + 107)
#define PATTERN_GRID_31             (RAW_PATTERN_DATA + 64)
#define PATTERN_GRID_32             (RAW_PATTERN_DATA + 31)
#define PATTERN_GRID_33             (RAW_PATTERN_DATA + 100)
#define PATTERN_GRID_34             (RAW_PATTERN_DATA + 111)
#define PATTERN_GRID_41             (RAW_PATTERN_DATA + 63)
#define PATTERN_GRID_42             (RAW_PATTERN_DATA + 30)
#define PATTERN_GRID_43             (RAW_PATTERN_DATA + 99)
#define PATTERN_GRID_44             (RAW_PATTERN_DATA + 110)
#define PATTERN_GRID_SM_11          (RAW_PATTERN_DATA + 70)
#define PATTERN_GRID_SM_12          (RAW_PATTERN_DATA + 136)
#define PATTERN_GRID_SM_21          (RAW_PATTERN_DATA + 69)
#define PATTERN_GRID_SM_22          (RAW_PATTERN_DATA + 135)
#define PATTERN_HATCH_11            (RAW_PATTERN_DATA + 132)
#define PATTERN_HATCH_12            (RAW_PATTERN_DATA + 76)
#define PATTERN_HATCH_21            (RAW_PATTERN_DATA + 131)
#define PATTERN_HATCH_22            (RAW_PATTERN_DATA + 75)
#define PATTERN_HATCH_31            (RAW_PATTERN_DATA + 130)
#define PATTERN_HATCH_32            (RAW_PATTERN_DATA + 74)
#define PATTERN_HATCH_41            (RAW_PATTERN_DATA + 129)
#define PATTERN_HATCH_42            (RAW_PATTERN_DATA + 73)
#define PATTERN_HLINES_1            (RAW_PATTERN_DATA + 62)
#define PATTERN_HLINES_2            (RAW_PATTERN_DATA + 29)
#define PATTERN_HLINES_3            (RAW_PATTERN_DATA + 103)
#define PATTERN_HLINES_4            (RAW_PATTERN_DATA + 109)
#define PATTERN_HLINES_SM           (RAW_PATTERN_DATA + 139)
#define PATTERN_HLINES_WIDE_1       (RAW_PATTERN_DATA + 87)
#define PATTERN_HLINES_WIDE_2       (RAW_PATTERN_DATA + 95)
#define PATTERN_ON                  (RAW_PATTERN_DATA + 42)
#define PATTERN_STRIPE_DOWN_1       (RAW_PATTERN_DATA + 17)
#define PATTERN_STRIPE_DOWN_2       (RAW_PATTERN_DATA + 16)
#define PATTERN_STRIPE_DOWN_3       (RAW_PATTERN_DATA + 19)
#define PATTERN_STRIPE_DOWN_4       (RAW_PATTERN_DATA + 18)
#define PATTERN_STRIPE_DOWN_WIDE_1  (RAW_PATTERN_DATA + 24)
#define PATTERN_STRIPE_DOWN_WIDE_2  (RAW_PATTERN_DATA + 23)
#define PATTERN_STRIPE_UP_1         (RAW_PATTERN_DATA + 0)
#define PATTERN_STRIPE_UP_2         (RAW_PATTERN_DATA + 3)
#define PATTERN_STRIPE_UP_3         (RAW_PATTERN_DATA + 2)
#define PATTERN_STRIPE_UP_4         (RAW_PATTERN_DATA + 1)
#define PATTERN_STRIPE_UP_WIDE_1    (RAW_PATTERN_DATA + 92)
#define PATTERN_STRIPE_UP_WIDE_2    (RAW_PATTERN_DATA + 91)
#define PATTERN_VLINES_1            (RAW_PATTERN_DATA + 119)
#define PATTERN_VLINES_2            (RAW_PATTERN_DATA + 122)
#define PATTERN_VLINES_3            (RAW_PATTERN_DATA + 121)
#define PATTERN_VLINES_4            (RAW_PATTERN_DATA + 120)
#define PATTERN_VLINES_SM           (RAW_PATTERN_DATA + 115)
#define PATTERN_VLINES_WIDE_1       (RAW_PATTERN_DATA + 118)
#define PATTERN_VLINES_WIDE_2       (RAW_PATTERN_DATA + 117)

#endif

#endif

#ifndef PATTERNDATA_H_INCLUDED
#define PATTERNDATA_H_INCLUDED

#include <ProgMem.h>

// Auto-generated by PatternData.gen.sh

#ifdef NO_PATTERNS

static PROGMEM const uint8_t PATTERNS[] = {
	0xFF, 0xFF, 0xFF, 0xFF
};

#define PATTERN_ON  (PATTERNS + 0)

#elifdef REDUCED_PATTERNS

static PROGMEM const uint8_t PATTERNS[] = {
	0x11, 0x11, 0x11, 0xFF, 0xAA, 0xFF, 0xAA, 0xFF,
	0xFF, 0xFF, 0xFF, 0xEE, 0x00, 0xFF, 0x00, 0xFF,
	0xDD, 0xFF, 0x77, 0xDD, 0x77, 0xDD, 0x77, 0xBB,
	0xEE, 0xBB, 0xEE, 0xBB, 0x55, 0xFF, 0x55, 0xFF,
	0x55, 0xAA, 0x55, 0xAA, 0xAA, 0xAA, 0xAA
};

#define PATTERN_CHECKER     (PATTERNS + 32)
#define PATTERN_FILL75_11   (PATTERNS + 24)
#define PATTERN_FILL75_12   (PATTERNS + 19)
#define PATTERN_FILL75_21   (PATTERNS + 23)
#define PATTERN_FILL75_22   (PATTERNS + 18)
#define PATTERN_FILL87_22   (PATTERNS + 15)
#define PATTERN_FILL94_41   (PATTERNS + 8)
#define PATTERN_GRID_41     (PATTERNS + 0)
#define PATTERN_GRID_SM_11  (PATTERNS + 29)
#define PATTERN_GRID_SM_12  (PATTERNS + 3)
#define PATTERN_GRID_SM_21  (PATTERNS + 28)
#define PATTERN_GRID_SM_22  (PATTERNS + 4)
#define PATTERN_HLINES_SM   (PATTERNS + 35)
#define PATTERN_ON          (PATTERNS + 7)
#define PATTERN_VLINES_SM   (PATTERNS + 12)

#else

static PROGMEM const uint8_t PATTERNS[] = {
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

#define PATTERN_BRICKS              (PATTERNS + 79)
#define PATTERN_CHECKER             (PATTERNS + 126)
#define PATTERN_FILL75_11           (PATTERNS + 7)
#define PATTERN_FILL75_12           (PATTERNS + 83)
#define PATTERN_FILL75_21           (PATTERNS + 6)
#define PATTERN_FILL75_22           (PATTERNS + 82)
#define PATTERN_FILL87_11           (PATTERNS + 12)
#define PATTERN_FILL87_12           (PATTERNS + 37)
#define PATTERN_FILL87_21           (PATTERNS + 11)
#define PATTERN_FILL87_22           (PATTERNS + 36)
#define PATTERN_FILL87_31           (PATTERNS + 10)
#define PATTERN_FILL87_32           (PATTERNS + 39)
#define PATTERN_FILL87_41           (PATTERNS + 13)
#define PATTERN_FILL87_42           (PATTERNS + 38)
#define PATTERN_FILL94_11           (PATTERNS + 46)
#define PATTERN_FILL94_12           (PATTERNS + 41)
#define PATTERN_FILL94_13           (PATTERNS + 54)
#define PATTERN_FILL94_14           (PATTERNS + 58)
#define PATTERN_FILL94_21           (PATTERNS + 45)
#define PATTERN_FILL94_22           (PATTERNS + 40)
#define PATTERN_FILL94_23           (PATTERNS + 53)
#define PATTERN_FILL94_24           (PATTERNS + 57)
#define PATTERN_FILL94_31           (PATTERNS + 44)
#define PATTERN_FILL94_32           (PATTERNS + 48)
#define PATTERN_FILL94_33           (PATTERNS + 52)
#define PATTERN_FILL94_34           (PATTERNS + 56)
#define PATTERN_FILL94_41           (PATTERNS + 43)
#define PATTERN_FILL94_42           (PATTERNS + 47)
#define PATTERN_FILL94_43           (PATTERNS + 51)
#define PATTERN_FILL94_44           (PATTERNS + 55)
#define PATTERN_GRID_11             (PATTERNS + 61)
#define PATTERN_GRID_12             (PATTERNS + 28)
#define PATTERN_GRID_13             (PATTERNS + 102)
#define PATTERN_GRID_14             (PATTERNS + 108)
#define PATTERN_GRID_21             (PATTERNS + 65)
#define PATTERN_GRID_22             (PATTERNS + 32)
#define PATTERN_GRID_23             (PATTERNS + 101)
#define PATTERN_GRID_24             (PATTERNS + 107)
#define PATTERN_GRID_31             (PATTERNS + 64)
#define PATTERN_GRID_32             (PATTERNS + 31)
#define PATTERN_GRID_33             (PATTERNS + 100)
#define PATTERN_GRID_34             (PATTERNS + 111)
#define PATTERN_GRID_41             (PATTERNS + 63)
#define PATTERN_GRID_42             (PATTERNS + 30)
#define PATTERN_GRID_43             (PATTERNS + 99)
#define PATTERN_GRID_44             (PATTERNS + 110)
#define PATTERN_GRID_SM_11          (PATTERNS + 70)
#define PATTERN_GRID_SM_12          (PATTERNS + 136)
#define PATTERN_GRID_SM_21          (PATTERNS + 69)
#define PATTERN_GRID_SM_22          (PATTERNS + 135)
#define PATTERN_HATCH_11            (PATTERNS + 132)
#define PATTERN_HATCH_12            (PATTERNS + 76)
#define PATTERN_HATCH_21            (PATTERNS + 131)
#define PATTERN_HATCH_22            (PATTERNS + 75)
#define PATTERN_HATCH_31            (PATTERNS + 130)
#define PATTERN_HATCH_32            (PATTERNS + 74)
#define PATTERN_HATCH_41            (PATTERNS + 129)
#define PATTERN_HATCH_42            (PATTERNS + 73)
#define PATTERN_HLINES_1            (PATTERNS + 62)
#define PATTERN_HLINES_2            (PATTERNS + 29)
#define PATTERN_HLINES_3            (PATTERNS + 103)
#define PATTERN_HLINES_4            (PATTERNS + 109)
#define PATTERN_HLINES_SM           (PATTERNS + 139)
#define PATTERN_HLINES_WIDE_1       (PATTERNS + 87)
#define PATTERN_HLINES_WIDE_2       (PATTERNS + 95)
#define PATTERN_ON                  (PATTERNS + 42)
#define PATTERN_STRIPE_DOWN_1       (PATTERNS + 17)
#define PATTERN_STRIPE_DOWN_2       (PATTERNS + 16)
#define PATTERN_STRIPE_DOWN_3       (PATTERNS + 19)
#define PATTERN_STRIPE_DOWN_4       (PATTERNS + 18)
#define PATTERN_STRIPE_DOWN_WIDE_1  (PATTERNS + 24)
#define PATTERN_STRIPE_DOWN_WIDE_2  (PATTERNS + 23)
#define PATTERN_STRIPE_UP_1         (PATTERNS + 0)
#define PATTERN_STRIPE_UP_2         (PATTERNS + 3)
#define PATTERN_STRIPE_UP_3         (PATTERNS + 2)
#define PATTERN_STRIPE_UP_4         (PATTERNS + 1)
#define PATTERN_STRIPE_UP_WIDE_1    (PATTERNS + 92)
#define PATTERN_STRIPE_UP_WIDE_2    (PATTERNS + 91)
#define PATTERN_VLINES_1            (PATTERNS + 119)
#define PATTERN_VLINES_2            (PATTERNS + 122)
#define PATTERN_VLINES_3            (PATTERNS + 121)
#define PATTERN_VLINES_4            (PATTERNS + 120)
#define PATTERN_VLINES_SM           (PATTERNS + 115)
#define PATTERN_VLINES_WIDE_1       (PATTERNS + 118)
#define PATTERN_VLINES_WIDE_2       (PATTERNS + 117)

#endif

#endif

/******************************************************************************
This file was imported from the MicroView library, written by GeekAmmo
(https://github.com/geekammo/MicroView-Arduino-Library), and released under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

// This data has been adapted to the format used by the Bitmask18 Font class.

#ifndef FONT2_H_INCLUDED
#define FONT2_H_INCLUDED

#include <ProgMem.h>

// sevenseg
static PROGMEM const uint8_t F2DATA[] = {
	// data section defines:
	//  FONTHEIGHT
	//  LINEHEIGHT
	//  ASCII START CHAR (inclusive)
	//  ASCII END CHAR (inclusive)
	//  DATA LAYOUT:
	//    0x00 = 1 bitmap image, chars from left-to-right
	//    0x01 = 1 bitmap image per character
	//    0x02 = 1 2D bitmap image, chars left-to-right, top-to-bottom, with width given below
	//  WIDTHS:
	//    layout 0x00: one int per char
	//    layout 0x01: one int for all chars
	//    layout 0x02: one int for all chars then one for # chars per line
	16, 16, 0x2E, 0x39, 0x02,
	10, 12,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x78, 0xFC, 0x02, 0x03, 0x03, 0x03, 0x03, 0x02, 0xFC, 0x78, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0x00, 0x00, 0x02, 0x83, 0x83, 0x83, 0x83, 0x02,
	0xFC, 0x78, 0x00, 0x00, 0x02, 0x83, 0x83, 0x83, 0x83, 0x02, 0xFC, 0x78, 0x7E, 0xFF, 0x00, 0x80,
	0x80, 0x80, 0x80, 0x00, 0xFF, 0x7E, 0x78, 0xFC, 0x02, 0x83, 0x83, 0x83, 0x83, 0x02, 0x00, 0x00,
	0x78, 0xFC, 0x02, 0x83, 0x83, 0x83, 0x83, 0x02, 0x00, 0x00, 0x00, 0x02, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x02, 0xFC, 0x78, 0x78, 0xFC, 0x02, 0x83, 0x83, 0x83, 0x83, 0x02, 0xFC, 0x78, 0x78, 0xFC,
	0x02, 0x83, 0x83, 0x83, 0x83, 0x02, 0xFC, 0x78, 0x00, 0x00, 0x00, 0x60, 0xF0, 0xF0, 0x60, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x3F, 0x40, 0xC0,
	0xC0, 0xC0, 0xC0, 0x40, 0x3F, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E,
	0x1C, 0x3E, 0x41, 0xC1, 0xC1, 0xC1, 0xC1, 0x41, 0x00, 0x00, 0x00, 0x00, 0x41, 0xC1, 0xC1, 0xC1,
	0xC1, 0x41, 0x3E, 0x1C, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0x7E, 0x00, 0x00,
	0x41, 0xC1, 0xC1, 0xC1, 0xC1, 0x41, 0x3E, 0x1C, 0x1C, 0x3E, 0x41, 0xC1, 0xC1, 0xC1, 0xC1, 0x41,
	0x3E, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7E, 0x1C, 0x3E, 0x41, 0xC1,
	0xC1, 0xC1, 0xC1, 0x41, 0x3E, 0x1C, 0x00, 0x00, 0x41, 0xC1, 0xC1, 0xC1, 0xC1, 0x41, 0x3E, 0x1C
};
#define F2IMG (F2DATA + 7)
#define F2MASK (nullptr)

#endif

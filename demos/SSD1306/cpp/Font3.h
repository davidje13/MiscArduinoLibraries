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

#ifndef FONT3_H_INCLUDED
#define FONT3_H_INCLUDED

#include <ProgMem.h>

// large-num
static PROGMEM const uint8_t F3DATA[] = {
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
	48, 48, 0x30, 0x3A, 0x02,
	12, 11,
	0x00, 0xC0, 0xF8, 0x7C, 0x3E, 0x3E, 0xFC, 0xF8, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0,
	0x78, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x7C, 0x3C, 0x3E, 0x3E, 0xFE, 0xFC,
	0xE0, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x3E, 0x3E, 0x3E, 0xFE, 0xF8, 0xC0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x3E,
	0x3E, 0x3E, 0x3E, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF0, 0xFC, 0x3E, 0x3E, 0x3E,
	0xFC, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E, 0xFE, 0xFE, 0x00, 0x00,
	0x00, 0x00, 0xC0, 0xF8, 0xFE, 0x3E, 0x7E, 0xFC, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFC,
	0x7E, 0x3E, 0xFE, 0xF8, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xC0, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xF0, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0xF9, 0xFF, 0xFF, 0xF0, 0x00, 0x00,
	0x00, 0x00, 0x07, 0x03, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x1F, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x3F,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFC,
	0x7F, 0x03, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00,
	0x00, 0xFE, 0xFF, 0x03, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x3F, 0x7F, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00,
	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFE, 0x1F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xF8,
	0xFC, 0xFF, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFE, 0x3F, 0x03, 0x00, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x3F, 0x3E, 0x7E, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x80, 0xF0, 0x7C, 0x7C, 0xF8, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x80, 0xF8, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x9F, 0xFF, 0xF8, 0xFE, 0x1F,
	0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xC0, 0xFC,
	0x7F, 0x03, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFE, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x0F, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0xF8, 0xFF, 0xE7, 0xE0,
	0xE0, 0xE0, 0xFF, 0xFF, 0xE0, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF,
	0xFE, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFC, 0x3F,
	0x03, 0x03, 0x1F, 0xFF, 0xFC, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x3E, 0x3E, 0x0F, 0x01,
	0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x07, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFE, 0x0F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xFF, 0x7F, 0x00, 0x00, 0x00,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFF, 0xFF, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xC0, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x80,
	0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x80, 0xFC, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1F, 0x3F, 0x7C, 0x7C, 0x3F, 0x1F, 0x03, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x7C, 0x7C, 0x7C, 0x7F, 0x7F, 0x7C, 0x7C, 0x7C, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x7C,
	0x7C, 0x7C, 0x7C, 0x7C, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x7E, 0x7C, 0x7C, 0x7E, 0x1F, 0x07,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x1F, 0x3E, 0x7C, 0x7C, 0x3E, 0x1F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1F,
	0x7F, 0x7C, 0x7C, 0x3F, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x1F, 0x3F, 0x7E, 0x7C, 0x7E, 0x3F, 0x1F, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x3E, 0x7C, 0x7C, 0x7E, 0x3F, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define F3IMG (F3DATA + 7)
#define F3MASK (nullptr)

#endif

/*
 * Written in 2018 by David Evans
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

/*
 * Hardware connections:
 *
 *        GND -- GND
 *         +V -- 3.3V
 *   DAT (D1) -- D11 (MOSI)
 *   CLK (D0) -- D13 (SCK)
 *         DC -- D* (any available pin; set below)
 *        RST -- D* (any available pin; set below)
 *         CS -- D* (any available pin; set below)
 */

#include "Menu.h"
#include "TextField.h"
#include "GameTetris.h"
#include "GameInvaders.h"
#include "Strings.h"

#include <ArduinoPin.h>
#include <ArduinoAnalogPin.h>
#include <ArduinoSPI.h>
#include <ArduinoTone.h>
#include <ProgMem.h>

#include <IRReceiver.h>

#include <RotaryEncoder.h>
#include <NotchedEncoder.h>
#include <DebouncedButton.h>

#include <ArduinoTWIMaster.h>
#include <24LC256.h>

#include <SSD1306.h>
#include <Bitmask18.h>
#include <Font.h>
#include <FontVariable.h>
#include <FontFixed.h>

FixedArduinoPin<10> oledCsPin;
FixedArduinoPin<7> oledRstPin;
FixedArduinoPin<9> oledDcPin;

FixedArduinoPin<3> irPin;

FixedArduinoPin<2> rotAPin;
FixedArduinoPin<8> rotBPin;
FixedArduinoPin<12> btnPin;

FixedArduinoAnalogPin<0> potL;
FixedArduinoAnalogPin<1> potR;

ArduinoTone<6> bleeper;

auto ir = MakeAsynchronousIRReceiver(irPin);
auto encoder = MakeInterruptRotaryEncoder(rotAPin, rotBPin, true);
auto notchedEncoder = MakeNotchedEncoder(&encoder, 2);
auto btn = MakeDebouncedButton(btnPin, false);

enum class IRAction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	SELECT,
	BACK,

	COUNT
};

#define MAX_REMOTE_BTNS 4

template <typename input_t, uint8_t actionCount>
class ActionHandler {
	typedef uint8_t action_t;

	input_t triggers[actionCount][MAX_REMOTE_BTNS];
	input_t continuations[actionCount][MAX_REMOTE_BTNS];

	action_t lastAction;
	uint32_t lastTm;
	bool repeatWaiting;

	[[gnu::pure,nodiscard]]
	static bool matchesAny(const input_t *choices, input_t check) {
		for(uint8_t j = 0; j < MAX_REMOTE_BTNS; ++ j) {
			if(choices[j] == check) {
				return true;
			}
		}
		return false;
	}

public:
	static constexpr action_t NO_ACTION = action_t(-1);
	static constexpr uint16_t repeatDelay = 200;

	action_t handle(input_t input) {
		uint32_t tm = millis();
		bool canRepeat = (uint32_t(tm - lastTm) >= repeatDelay);
		if(input) {
			if(lastAction != NO_ACTION) {
				if(matchesAny(continuations[lastAction], input)) {
					if(canRepeat) {
						repeatWaiting = false;
						lastTm = tm;
						return lastAction;
					} else {
						repeatWaiting = true;
						return NO_ACTION;
					}
				}
			}
			for(action_t i = 0; i < actionCount; ++ i) {
				if(matchesAny(triggers[i], input)) {
					repeatWaiting = false;
					lastAction = i;
					lastTm = tm;
					return i;
				}
			}
		}
		if(repeatWaiting && canRepeat) {
			repeatWaiting = false;
			lastTm = tm;
			return lastAction;
		}
		lastAction = NO_ACTION;
		return NO_ACTION;
	}

	void add(input_t input, action_t action) {
	}

	void remove(input_t input, action_t action) {
	}

	void clear(action_t action) {
	}
};

ActionHandler<uint64_t, uint8_t(IRAction::COUNT)> act;


//ArduinoTWIMaster twi;
//auto eeprom = Make24LC256(twi, 0x0);

template <typename Display>
int showMenu(Display &display, ProgMem<char> *items, uint8_t itemCount) {
	auto menu = MakeMenu([&items, itemCount] (int16_t index) {
		if(index < 0 || index >= itemCount) {
			return ProgMem<char>(nullptr);
		} else {
			return items[index];
		}
	});

	notchedEncoder.reset();
	btn.reset();

	Bitmask18<display.width(),display.height()> bitmask;
	auto font = MakeFontVariable();
	while(true) {
		if(btn.released()) {
			return menu.index();
		}
		menu.scroll(notchedEncoder, true);

		bitmask.clear(false);
		menu.render(
			bitmask, font,
			24,
			(display.height() - 8) / 2,
			display.width() - 24 * 2,
			8
		);
		display.send(bitmask);
		delay(10);
	}
}

template <typename Display>
void showRemoteListener(Display &display, int action, ProgMem<char> name) {
	// TODO:
	// - listen for remote signals
	// - filter out if already known
	// - check for conflicts with other actions
	// - store as an init action
	// - listen for immediate repetition signals
	// - if different from initial signal, store as a follup action

	// need a UI for clear all actions and return to previous menu
	// (also show name of current action in UI)
	(void) display;
	(void) action;
	(void) name;
}

template <typename Display>
void showRemoteUI(Display &display) {
	uint8_t itemCount = 0;
	ProgMem<char> items[16];
	items[itemCount++] = REMOTE_UP;
	items[itemCount++] = REMOTE_DOWN;
	items[itemCount++] = REMOTE_LEFT;
	items[itemCount++] = REMOTE_RIGHT;
	items[itemCount++] = REMOTE_SELECT;
	items[itemCount++] = REMOTE_BACK;
	items[itemCount++] = SAVE;

	while(true) {
		int action = showMenu(display, items, itemCount);
		if(action == 6) {
			// TODO: save
			return;
		}
		showRemoteListener(display, action, items[action]);
	}
}

template <typename Display>
void showOptions(Display &display) {
	uint8_t itemCount = 0;
	ProgMem<char> items[16];
	items[itemCount++] = OPTION_ORIENTATION;
	items[itemCount++] = OPTION_SCROLL;
	items[itemCount++] = OPTION_REMOTE;
	items[itemCount++] = SAVE;

	while(true) {
		int action = showMenu(display, items, itemCount);
		switch(action) {
		case 0:
			// TODO
			break;
		case 1:
			// TODO
			break;
		case 2:
			showRemoteUI(display);
			break;
		case 3:
			// TODO: save
			return;
		}
	}
}

template <typename Display>
void showNameSelect(Display &display) {
	constexpr uint8_t maxLen = 8;

	char input[maxLen] = {};

	auto field = MakeTextField(input, maxLen, LETTER_SELECTION);

	notchedEncoder.reset();
	btn.reset();
	bool navigating = false;

	Bitmask18<display.width(),display.height()> bitmask;
	auto font = MakeFontFixed();
	while(true) {
		if(btn.released()) {
			navigating = !navigating;
			field.reset_fractions();
		}

		if(navigating) {
			field.navigate(notchedEncoder, true);
		} else {
			field.spin(notchedEncoder, true);
		}

		bitmask.clear(false);
		field.render(
			bitmask, font,
			!navigating,
			(display.width() - 9 * maxLen) / 2,
			(display.height() - 8) / 2,
			9,
			8
		);
		display.send(bitmask);
		delay(10);
	}
}

template <typename Display>
void addHighscore(
	Display &display,
	uint8_t identifier,
	ProgMem<char> name,
	uint16_t score
) {
	(void) display;
	(void) identifier;
	(void) name;
	(void) score;
	// TODO
}

template <typename Display>
void showMainMenu(Display &display) {
	uint8_t itemCount = 0;

	ProgMem<char> items[16];
	items[itemCount++] = MENU_INVADERS;
	items[itemCount++] = MENU_TETRIS;
	items[itemCount++] = MENU_NAME;
	items[itemCount++] = MENU_OPTIONS;

	while(true) {
		switch(showMenu(display, items, itemCount)) {
		case 0: // Space Invaders
			addHighscore(
				display,
				0,
				NAME_INVADERS,
				playInvaders(display, &notchedEncoder, &btn, &bleeper)
			);
			break;
		case 1: // Tetris
			addHighscore(
				display,
				1,
				NAME_TETRIS,
				playTetris(display, &notchedEncoder, &btn)
			);
			break;
		case 2: // Enter Name
			showNameSelect(display);
			break;
		case 3: // Options
			showOptions(display);
			break;
		}
	}
}

__attribute__((section(".noinit")))
static volatile uint16_t randomMemory;

void setup(void) {
	randomSeed(
		(analogRead(0) << 0) ^
		(analogRead(1) << 1) ^
		(analogRead(2) << 2) ^
		(analogRead(3) << 3) ^
		(ADCW << 6) ^ // internal temperature sensor
		randomMemory
	);
	randomMemory = random();

	// To allow easy attachment of an IR receiver,
	// set pin 4 to GND (allows direct attachment on 3, 4 and VCC)
	FixedArduinoPin<4>().set_output();

	auto oled = MakeSSD1306<128,64>(
		ArduinoSPI(),
		oledCsPin,
		oledRstPin,
		oledDcPin
	);

	oled.set_on(true);
	oled.set_x_flip(true);
	oled.set_y_flip(true);

	showMainMenu(oled);
}

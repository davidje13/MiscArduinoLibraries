#ifndef MOCK_ARDUINO_PIN_H_INCLUDED
#define MOCK_ARDUINO_PIN_H_INCLUDED

#include <BaseMock.h>

#define NOT_ON_TIMER 4
#define NOT_AN_INTERRUPT 7
#define HIGH 40
#define LOW 20
#define OUTPUT 25
#define INPUT 45
#define INPUT_PULLUP 47
#define RISING 33
#define FALLING 35
#define CHANGE 37

#define digitalPinToTimer(x) (uint8_t(((x) == 2) ? 9 : NOT_ON_TIMER))
#define digitalPinToInterrupt(x) (uint8_t(((x) == 5) ? 3 : NOT_AN_INTERRUPT))

#define digitalPinToPort(x) (uint8_t((x) + 12))
#define digitalPinToBitMask(x) (uint8_t(1 << ((x) % 8)))

static uint32_t pulseIn(int, int, uint32_t = 1000000) {
	return 0;
}

static uint8_t portOutputRegisterValues[256] = {};
static uint8_t portModeRegisterValues[256] = {};
#define portOutputRegister(x) (portOutputRegisterValues + (x % 256))
#define portModeRegister(x) (portModeRegisterValues + (x % 256))

static uint8_t digitalReadLastPin = 0;
static uint8_t digitalReadReturnValue = 0;
MOCK uint8_t digitalRead(uint8_t pin) {
	digitalReadLastPin = pin;
	return digitalReadReturnValue;
}

static uint8_t digitalWriteLastPin = 0;
static uint8_t digitalWriteLastValue = 0;
MOCK void digitalWrite(uint8_t pin, uint8_t value) {
	digitalWriteLastPin = pin;
	digitalWriteLastValue = value;
}

static uint8_t analogReadLastPin = 0;
static uint16_t analogReadReturnValue = 0;
MOCK uint16_t analogRead(uint8_t pin) {
	analogReadLastPin = pin;
	return analogReadReturnValue;
}

static uint8_t analogWriteLastPin = 0;
static uint8_t analogWriteLastValue = 0;
MOCK void analogWrite(uint8_t pin, uint8_t value) {
	analogWriteLastPin = pin;
	analogWriteLastValue = value;
}

static uint8_t pinModeLastPin = 0;
static uint8_t pinModeLastMode = 0;
MOCK void pinMode(uint8_t pin, uint8_t mode) {
	pinModeLastPin = pin;
	pinModeLastMode = mode;
}

typedef void (*interruptFunc)(void);

static uint8_t attachInterruptLastInt = 0;
static interruptFunc attachInterruptLastFunc = nullptr;
static uint8_t attachInterruptLastMode = 0;
MOCK void attachInterrupt(uint8_t interrupt, interruptFunc func, uint8_t mode) {
	attachInterruptLastInt = interrupt;
	attachInterruptLastFunc = func;
	attachInterruptLastMode = mode;
}

static uint8_t detachInterruptLastInt = 0;
MOCK void detachInterrupt(uint8_t interrupt) {
	detachInterruptLastInt = interrupt;
}

#include <EndMocks.h>

#endif

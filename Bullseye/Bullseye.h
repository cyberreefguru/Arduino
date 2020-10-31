// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _LedStar_H_
#define _LedStar_H_

#define TIMER_INTERRUPT_DEBUG      1

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include <ESP8266TimerInterrupt.h>


// Witty Specific Pins
#define PIN_LDR 		(char)A0
#define PIN_BUTTON 		4  // GPIO4
#define PIN_LED_RED		15 // GPIO15
#define PIN_LED_GREEN	13 // GPIO13
#define PIN_LED_BLUE	12 // GPIO12
#define PIN_LED_BUILTIN	2  // GPIO2

// Bullseye Specific Pins
#define PIN_NEOPIXELS	14 // GPIO14
#define PIN_INPUT_SW1	16 // GPIO16
#define PIN_INPUT_SW2	5  // GPIO5

#define NUM_PIXELS 50
#define HALF NUM_PIXELS/2

// Animation constants
#define DOWN 	0
#define UP		1

#define RING_1		0
#define RING_2		1
#define RING_3		2
#define RING_4		3
#define RING_ALL	4

// Neopixel colors
#define WHITE	0x00FFFFFF
#define BLACK	0x00000000
#define RED		0x00FF0000
#define BLUE	0x000000FF
#define GREEN	0x0000FF00
#define MAGENTA	RED | BLUE
#define CYAN	BLUE | GREEN
#define YELLOW	RED | GREEN

#define ORANGE	0xFF3300
#define PURPLE	0x990099

// Interrupt constants
#define TIMER_INTERVAL_MS			100
#define DEBOUNCE_TIME               25
#define LONG_BUTTON_PRESS_TIME_MS   100
#define DEBUG_ISR                   0

#define ACTIVE_HIGH		1
#define ACTIVE_LOW		0

boolean fadeRings(uint8_t reps, uint32_t colorRing1, uint32_t colorRing2, uint32_t colorRing3, uint32_t colorRing4, uint32_t fadeDuration, uint32_t holdDuration);
boolean fadeRings(uint8_t reps, uint32_t color, uint32_t fadeDuration, uint32_t holdDuration);
boolean flashRingConcentric(uint8_t reps, uint32_t startColor, uint32_t color1, uint32_t color2, uint32_t holdDuration, uint32_t loopHoldDuration);


void writeRing(uint8_t ring, uint32_t color, uint8_t show);

boolean fade(uint8_t direction, uint32_t time, uint32_t color);
boolean fade(uint8_t direction, uint32_t time, uint32_t color, uint8_t start, uint8_t end);

boolean sparkle(uint32_t duration, uint32_t onTime, int32_t offTime, uint32_t onColor, uint32_t offColor);
boolean sparkle(uint32_t duration, uint32_t onTime, int32_t offTime, uint32_t onColor, uint32_t offColor, uint8_t start, uint8_t end);

boolean random(uint32_t delay, uint32_t onColor, uint32_t offColor);
boolean random(uint32_t delay, uint32_t onColor, uint32_t offColor, uint8_t start, uint8_t end);

boolean sequence(uint32_t wait, uint32_t onColor, uint32_t offColor, uint8_t clear);
boolean sequence(uint32_t wait, uint32_t onColor, uint32_t offColor, uint8_t start, uint8_t end, uint8_t clear);

void writeColor(uint32_t color, uint8_t show);
void writeColor(uint32_t color, uint8_t start, uint8_t end, uint8_t show);

boolean isCommandAvailable();
void setCommandAvailable(boolean b);

boolean commandWait(uint32_t wait);
void worker();

void ICACHE_RAM_ATTR isrActiveHighRisingEdge();
void ICACHE_RAM_ATTR isrActiveHighFallingEdge();

void ICACHE_RAM_ATTR isrActiveLowRisingEdge();
void ICACHE_RAM_ATTR isrActiveLowFallingEdge();

void ICACHE_RAM_ATTR isrTimer();

void ICACHE_RAM_ATTR isrInput1();
void ICACHE_RAM_ATTR isrInput2();
void ICACHE_RAM_ATTR isrInput3();

enum MODE { Start, Incoming, Landed };
MODE gMode = Start;

enum COLORS {White=WHITE, Black=BLACK, Red=RED, Green=GREEN, Blue=BLUE, Magenta=MAGENTA, Cyan=CYAN, Yellow=YELLOW, Orange=ORANGE, Purple=PURPLE };
COLORS gHeartBeatColor = Blue;

// Flag indicating a command has arrived
uint8_t gCommand = 0;

// Global value of LED brightness
uint8_t gBrightness = 0;

typedef void (*isrPtr)();

struct DebounceInterruptConfig
{
	uint8_t pin;
	uint8_t config;
	uint8_t state;
	uint8_t changed;
	uint8_t processing;
	uint32_t time;
	isrPtr handler;

};

volatile bool			ledOn = false;
volatile uint8_t 		hbCounter = 0;

#define ISR_SIZE	3
volatile DebounceInterruptConfig isrConfig[ISR_SIZE] =
{
	{PIN_BUTTON, ACTIVE_LOW, 0, false, false, 0, &isrInput1},
	{PIN_INPUT_SW1, ACTIVE_HIGH, 0, false, false, 0, &isrInput2},
	{PIN_INPUT_SW2, ACTIVE_LOW, 0, false, false, 0, &isrInput3}
};

//Do not add code below this line
#endif /* _LedStar_H_ */

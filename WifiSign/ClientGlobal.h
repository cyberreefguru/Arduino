/*
 * Global.h
 *
 *  Created on: Oct 30, 2016
 *      Author: tsasala
 */

#ifndef __CLIENT_GLOBAL_H_
#define __CLIENT_GLOBAL_H_


#define __DEBUG
#define __ESP_YIELD

#define MY_LEDS		STRING
//#define MY_LEDS		STRIP

#define MY_CONTROLLER	WS2812
//#define MY_CONTROLLER	NEOPIXELS

#define MY_COLOR_ORDER	GRB
//#define MY_COLOR_ORDER	RGB

#define MY_COLOR_CORRECTION	TypicalPixelString
//#define MY_COLOR_CORRECTION	TypicalLEDStrip


//enum StatusEnum { Unknown=0, None, Booting, Waiting, Processing, Configuring, Uploading, Ok, Error, Reset };
enum ComponentEnum {Config=0, Queue, Wifi, Driver, General };

// What LED string are we dealing with
#ifdef __LED_STRING
#define MY_CONTROLLER	WS2812
#endif

#ifdef __LED_STRIP
#define CONTROLLER	NEOPIXEL
#endif


#endif /* __CLIENT_GLOBAL_H_ */

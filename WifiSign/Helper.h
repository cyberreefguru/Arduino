/*
 * Helper.h
 *
 *  Created on: Oct 24, 2015
 *      Author: tsasala
 */

#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "ClientGlobal.h"
//#include "WifiWrapper.h"

#define ON		true
#define OFF		false

#define ERROR_CONFIG_TIME		100
#define ERROR_WIRELESS_TIME		200
#define ERROR_QUEUE_TIME		50
#define ERROR_DRIVER_TIME		25
#define ERROR_GENERAL_TIME		400

class Helper
{
public:
	Helper();
	static void workYield();
	static void delayWorker(uint32_t time);
	static void delayYield(uint32_t time);
	static void error();
	static void dumpBuffer(uint8_t *buf, uint8_t len);
	static void toggleLedTimed(uint32_t time);
	static void toggleLed();
	static void setLed(uint8_t b);
	static uint8_t getLed();

	static int8_t readChar(boolean b);
	static int16_t readInt(uint8_t *b, uint8_t len);
	static int16_t readString(uint8_t *b, uint8_t len);
	static void readAll();
	static boolean toString(char *buf, IPAddress ip);

};


//end of add your includes here
#ifdef __cplusplus
extern "C"
{
#endif

extern void worker();
extern boolean isCommandAvailable();
extern boolean commandDelay(uint32_t time);
extern void yield();

#ifdef __cplusplus
} // extern "C"
#endif


/*
 * MatrixWrapper.h
 *
 *  Created on: Jan 15, 2021
 *      Author: tsasala
 */

#pragma once

#include "Arduino.h"
#include <Adafruit_Protomatter.h>
#include "Command.h"
#include "FileSystemWrapper.h"

//#include <Fonts/FreeSansBold9pt7b.h>
//#include <Fonts/Org_01.h>
//#include "DialogBold8.h"
//#include "OpenSans8.h"
#include "DejaVuSans10.h"
//#include "NimbusSans.h"
//#include "SignImages.h"

class MatrixWrapper
{

public:
	MatrixWrapper();
	boolean initialize();

	void drawPixel(uint8_t x, uint8_t y, uint32_t color, boolean show);
	uint16_t getPixel(uint8_t x, uint8_t y) const;

	void fillScreen(uint16_t color, boolean show);
	void drawLine(uint8_t x, uint8_t y, uint8_t h, uint32_t color, DirectionEnum direction, boolean show);
	void drawLine(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint32_t color, boolean show);
	void drawVLine(uint8_t x, uint8_t y, uint8_t h, uint32_t color, boolean show);
	void drawHLine(uint8_t x, uint8_t y, uint8_t w, uint32_t color, boolean show);
	void drawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color, boolean show);
	void drawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color, boolean show);
	void drawCircle(uint8_t x0, uint8_t y0, uint8_t r, uint32_t color, boolean show);
	void drawFilledCircle(uint8_t x0, uint8_t y0, uint8_t r, uint32_t color, boolean show);
	void drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint32_t color, boolean show);
	void drawFilledTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint32_t color, boolean show);
	void drawRoundRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t radius, uint32_t color, boolean show);
	void drawFilledRoundRect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, uint8_t radius, uint32_t color, boolean show);

	void print(uint8_t x, uint8_t y, uint32_t color, char* b, boolean show);
    void print(uint8_t x, uint8_t y, uint32_t color, const __FlashStringHelper *, boolean show);

	void printLine(uint8_t x, uint8_t y, uint32_t color, char* b, boolean show);
    void printLine(uint8_t x, uint8_t y, uint32_t color, const __FlashStringHelper *, boolean show);

    void scrollText(uint8_t x, uint8_t y, uint32_t fg, uint32_t speed, int16_t repeat, DirectionEnum direction, const __FlashStringHelper *);
    void scrollText(uint8_t x, uint8_t y, uint32_t fg, uint32_t bg, uint32_t speed, int16_t repeat, DirectionEnum direction, const __FlashStringHelper *);
    void scrollText(uint8_t x, uint8_t y, uint32_t fg, uint32_t speed, int16_t repeat, DirectionEnum direction, uint8_t *);
    void scrollText(uint8_t x, uint8_t y, uint32_t fg, uint32_t bg, uint32_t speed, int16_t repeat, DirectionEnum direction, uint8_t *);

    // TODO - fix image index code
    void drawImage(ImageEnum index, boolean show);
    void drawImage(char *fileName, boolean show);
    void drawImage(char *filename, uint8_t x, uint8_t y, boolean show);

    void animate(AnimateEnum index, uint32_t delay);

	void setIntensity(uint8_t i);
	void show();
	void clear();

	void test();
	void testFull();
	void testText();
	void printString(const char *s, int16_t x, int16_t y,	int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);

	uint8_t getWidth();
	uint8_t getHeight();

protected:
	uint16_t mapColor(uint32_t color);
	float fract(float x);
	float mix(float a, float b, float t);
	float step(float e, float x);
	uint32_t hsv2rgb(int h, double s, double v);

private:
	uint8_t fontHeight;
	uint8_t fontWidth;
	uint8_t fontLines;
	uint8_t fontChars;
	Adafruit_ImageReader iReader;
	FileSystemWrapper fileSys;
	Adafruit_Image image;


};


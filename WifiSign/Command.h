/*
 * Command.h
 *
 *  Created on: Jan 14, 2021
 *      Author: tsasala
 */

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "ClientGlobal.h"
#include "Helper.h"

#define CMD_BUFFER_SIZE		512
#define CMD_STRING_SIZE		128

// Defines JSON keys for command values
#define	KEY_CMD						"cmd"
#define KEY_STRING					"t"
#define KEY_X						"x"
#define KEY_Y						"y"
#define KEY_X1						"x1"
#define KEY_Y1						"y1"
#define KEY_X2						"x2"
#define KEY_Y2						"y2"
#define KEY_WIDTH					"w"
#define KEY_HEIGHT					"h"
#define KEY_RADIUS					"r"
#define KEY_COLOR					"c"
#define KEY_FOREGROUND_COLOR		"fg"
#define KEY_BACKGROUND_COLOR		"bg"
#define KEY_STROKE_SIZE				"st"
#define KEY_STROKE_STYLE			"ss"
#define KEY_STROKE_COLOR			"sc"
#define KEY_SPEED					"sp"
#define KEY_REPEAT					"r"
#define KEY_DIRECTION				"d"
#define KEY_INTENSITY				"i"
#define KEY_SHOW					"s"
#define KEY_ON_DURATION				"don"
#define KEY_OFF_DURATION			"doff"
#define KEY_INDEX					"i"
#define KEY_STATUS					"status"

// Sample command using all the fields -- NOT all the values are valid!
// {\"cmd\": 2, \"t\": \"test string\", \"x\":1, "\y":2, \"x1\":111, "\y1":112, \"x2\":211, "\y2":212, \"w\":3, \"h\":4, "r":5, \"c\":6, \"fg\":601, \"bg\":602, \"st\":7, \"ss\":8, \"sc\":9, \"sp\":10, \"r\":11, \"d\":12, \"i\":13, \"s\":14, \"don\":15, \"doff\":16, \"i\":17}
// {\"cmd\": 22, \"t\": \"%s\", \"x\":0, "\y":31, \"c\":0xffffff, \"sp\":10, \"d\":1}
// {\"cmd\": 22, \"t\": \"%s\", \"x\":0, "\y":31, \"fg":0xffffff, \"bg":0x00ff00, \"sp\":10, \"d\":1}


// Commands
enum CommandEnum { Clear=0x01, Show=0x02, SetPixel=0x10, Text=0x11, Line=0x012, Rectangle=0x13, Circle=0x14, RandomFill=0x15, Scroll=0x16, SetImage=0x17, Animate=0x18, SetIntensity=0x30, Response=0x50 };

// Return Codes
enum ResponseCodesEnum {Success=0x00, General_Failure=0x01, Unknown_Argument=0x02, Out_of_Bounds=0x03 };

// Attribute Enums
enum DirectionEnum {Left=0x00, Right=0x01, Up=0x02, Down=0x03};
enum StrokeStyleEnum {None=0x00, Solid=0x01, DoubleLine=0x02, Dash=0x03, ShortDash=0x04, LongDash=0x05 };
enum AnimateEnum {Record=0x00};
enum ImageEnum {RecordRed=0,RecordWhite=1};


/**
 * Class definition
 */
class Command
{
public:
	Command();

	// Functions
	void initialize();
	boolean parse(uint8_t *b);
	boolean buildCommand(uint8_t *b);
	boolean buildResponse(uint8_t *b);
	void dump();

	// Getters and Setters
	CommandEnum getCommand() const;
	void setCommand(CommandEnum command);

	uint8_t* getText();
	void setText(uint8_t* s);
	uint8_t getX() const;
	void setX(uint8_t x);
	uint8_t getY() const;
	void setY(uint8_t y);
	uint8_t getX1() const;
	void setX1(uint8_t x);
	uint8_t getY1() const;
	void setY1(uint8_t y);
	uint8_t getX2() const;
	void setX2(uint8_t x);
	uint8_t getY2() const;
	void setY2(uint8_t y);
	uint8_t getWidth() const;
	void setWidth(uint8_t l);
	uint8_t getHeight() const;
	void setHeight(uint8_t h);
	uint8_t getRadius() const;
	void setRadius(uint8_t r);
	uint32_t getColor() const;
	void setColor(uint32_t c);
	uint32_t getForegroundColor() const;
	void setForegroundColor(uint32_t c);
	uint32_t getBackgroundColor() const;
	void setBackgroundColor(uint32_t c);
	uint8_t getStrokeSize() const;
	void setStrokeSize(uint8_t s);
	StrokeStyleEnum getStrokeStyle() const;
	void setStrokeStyle(StrokeStyleEnum s);
	uint32_t getStrokeColor() const;
	void setStrokeColor(uint32_t s);
	uint8_t getSpeed() const;
	void setSpeed(uint8_t s);
	int16_t getRepeat() const;
	void setRepeat(int16_t r);
	DirectionEnum getDirection() const;
	void setDirection(DirectionEnum d);
	uint8_t getIntensity() const;
	void setIntensity(uint8_t i);
	boolean getShow() const;
	void setShow(boolean show);
	uint32_t getOffDuration() const;
	void setOffDuration(uint32_t offDuration);
	uint32_t getOnDuration() const;
	void setOnDuration(uint32_t onDuration);
	uint8_t getIndex() const;
	void setIndex(uint8_t i);
	ResponseCodesEnum getStatus() const;
	void setStatus(ResponseCodesEnum s);

private:
	CommandEnum command;
	uint8_t text[CMD_STRING_SIZE];
	uint8_t x;
	uint8_t y;
	uint8_t x1;
	uint8_t y1;
	uint8_t x2;
	uint8_t y2;
	uint8_t width;
	uint8_t height;
	uint8_t radius;
	uint32_t fg;
	uint32_t bg;
	uint8_t strokeSize;
	StrokeStyleEnum strokeStyle;
	uint32_t strokeColor;
	uint8_t speed;
	uint8_t repeat;
	DirectionEnum direction;
	uint8_t intensity;
	boolean show;
	uint32_t onDuration;
	uint32_t offDuration;
	uint8_t index;

	ResponseCodesEnum status;
};

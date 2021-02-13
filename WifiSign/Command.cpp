/*
 * Command.cpp
 *
 *  Created on: Jan 14, 2021
 *      Author: tsasala
 */

#include "Command.h"


/**
 * Constructor - initializes object to defaults
 *
 */
Command::Command()
{
	initialize();
}

/**
 * Initializes the object to defaults
 *
 */
void Command::initialize()
{
	command = Clear;
	memset(text, 0, CMD_STRING_SIZE);
	x = 0;
	y = 0;
	x1 = 0;
	y1 = 0;
	x2 = 0;
	y2 = 0;
	width = 0;
	height = 0;
	radius = 0;
	fg = 0;
	bg = 0;
	strokeSize = 0;
	strokeStyle = None;
	strokeColor = 0;
	speed = 0;
	repeat = 0;
	direction = Left;
	intensity = 0;
	show = false;
	onDuration = 0;
	offDuration = 0;
	index=0;

} // end initialize

/**
 * Parses buffer into object
 *
 */
boolean Command::parse(uint8_t* b)
{
	boolean status = false;

#ifdef __DEBUG
	Serial.println("Parsing buffer...");
#endif

	StaticJsonDocument<CMD_BUFFER_SIZE> doc;
	auto error = deserializeJson(doc, (char *)b);
	if (error)
	{
	    Serial.print(F("deserializeJson() failed with code: "));
	    Serial.println(error.c_str());
	}
	else
	{
#ifdef __DEBUG
		Serial.println("Successful parse; pulling values...");
#endif
		command = doc[KEY_CMD];
		strlcpy((char *)text,doc[KEY_STRING],sizeof(text));
		x = doc[KEY_X];
		y = doc[KEY_Y];
		x1 = doc[KEY_X1];
		y1 = doc[KEY_Y1];
		x2 = doc[KEY_X2];
		y2 = doc[KEY_Y2];
		width = doc[KEY_WIDTH];
		height = doc[KEY_HEIGHT];
		radius = doc[KEY_RADIUS];
		fg = doc[KEY_FOREGROUND_COLOR];
		bg = doc[KEY_BACKGROUND_COLOR];
		strokeSize = doc[KEY_STROKE_SIZE];
		strokeStyle = doc[KEY_STROKE_STYLE];
		strokeColor = doc[KEY_STROKE_COLOR];
		speed = doc[KEY_SPEED];
		repeat = doc[KEY_REPEAT];
		direction = doc[KEY_DIRECTION];
		intensity = doc[KEY_INTENSITY];
		show = doc[KEY_SHOW];
		onDuration = doc[KEY_ON_DURATION];
		offDuration = doc[KEY_OFF_DURATION];
		index = doc[KEY_INDEX];
		status = true;
	}

	return status;

} // end parse

boolean Command::buildCommand(uint8_t *buffer)
{
	boolean status = false;

	StaticJsonDocument<CMD_BUFFER_SIZE> doc;
	doc.clear();
	doc[KEY_CMD] = command;
	doc[KEY_STRING] = text;
	doc[KEY_X] = x;
	doc[KEY_Y] = y;
	doc[KEY_X1] = x1;
	doc[KEY_Y1] = y1;
	doc[KEY_X2] = x2;
	doc[KEY_Y2] = y2;
	doc[KEY_WIDTH] = width;
	doc[KEY_HEIGHT] = height;
	doc[KEY_RADIUS] = radius;
	doc[KEY_FOREGROUND_COLOR] = fg;
	doc[KEY_BACKGROUND_COLOR] = bg;
	doc[KEY_STROKE_SIZE] = strokeSize;
	doc[KEY_STROKE_STYLE] = strokeStyle;
	doc[KEY_STROKE_COLOR] = strokeColor;
	doc[KEY_SPEED] = speed;
	doc[KEY_REPEAT]= repeat;
	doc[KEY_DIRECTION] = direction;
	doc[KEY_INTENSITY] = intensity;
	doc[KEY_SHOW] = show;
	doc[KEY_ON_DURATION] = onDuration;
	doc[KEY_OFF_DURATION] = offDuration;
	doc[KEY_INDEX] = index;

	if( serializeJson(doc, (char *)buffer, CMD_BUFFER_SIZE) == 0)
	{
	    Serial.println(F("serializeJson() failed"));
	}
	else
	{
#ifdef DEBUG
		Serial.println(F("Serialize command success."))
#endif
		status = true;
	}

	return status;
}

/**
 * Builds the response to a command
 */
boolean Command::buildResponse(uint8_t *buffer)
{
	boolean status = false;

	StaticJsonDocument<CMD_BUFFER_SIZE> doc;
	doc.clear();
	doc[KEY_CMD] = Response;
	doc[KEY_STATUS] = Success;

	if( serializeJson(doc, (char *)buffer, CMD_BUFFER_SIZE) == 0)
	{
	    Serial.println(F("serializeJson() failed"));
	}
	else
	{
#ifdef DEBUG
		Serial.println(F("Serialize command response success."))
#endif
		status = true;
	}

	return status;
}


void Command::dump()
{
#ifdef __DEBUG
	Serial.print("command: ");
	Serial.print( command, HEX );
	Serial.print(", string: ");
	Serial.print( (char *)text );
	Serial.print(", x: ");
	Serial.print( x );
	Serial.print(", y: ");
	Serial.print( y );
	Serial.print(", x1: ");
	Serial.print( x1 );
	Serial.print(", y1: ");
	Serial.print( y1 );
	Serial.print(", x2: ");
	Serial.print( x2 );
	Serial.print(", y2: ");
	Serial.print( y2 );
	Serial.print(", width: ");
	Serial.print( width );
	Serial.print(", w: ");
	Serial.print( height );
	Serial.print(", r: ");
	Serial.print( radius );
	Serial.print(", fg: ");
	Serial.print( fg );
	Serial.print(", bg: ");
	Serial.print( bg );
	Serial.print(", size: ");
	Serial.print( strokeSize );
	Serial.print(", style: ");
	Serial.print( strokeStyle );
	Serial.print(", color: ");
	Serial.print( strokeColor );
	Serial.print(", speed: ");
	Serial.print( speed );
	Serial.print(", repeat: ");
	Serial.print( repeat );
	Serial.print(", direction: ");
	Serial.print( direction );
	Serial.print(", intensity: ");
	Serial.print( intensity );
	Serial.print(", show: ");
	Serial.print( show );
	Serial.print(", onDuration: ");
	Serial.print( onDuration );
	Serial.print(", offDuration: ");
	Serial.print( offDuration );
	Serial.print(", index: ");
	Serial.print( index );

	Serial.print(", status: ");
	Serial.print( status );
	Serial.println();

#endif
}

CommandEnum Command::getCommand() const
{
	return command;
}

void Command::setCommand(CommandEnum command)
{
	this->command = command;
}

uint8_t* Command::getText()
{
	return text;
}

void Command::setText(uint8_t* s)
{
	strlcpy((char *)text, (char *)s, sizeof(text));
}

uint8_t Command::getX() const
{
	return x;
}

void Command::setX(uint8_t x)
{
	this->x = x;
}

uint8_t Command::getY() const
{
	return y;
}

void Command::setY(uint8_t y)
{
	this->y = y;
}

uint8_t Command::getX1() const
{
	return x1;
}

void Command::setX1(uint8_t x)
{
	this->x1 = x;
}

uint8_t Command::getY1() const
{
	return y1;
}

void Command::setY1(uint8_t y)
{
	this->y1 = y;
}

uint8_t Command::getX2() const
{
	return x2;
}

void Command::setX2(uint8_t x)
{
	this->x2 = x;
}

uint8_t Command::getY2() const
{
	return y2;
}

void Command::setY2(uint8_t y)
{
	this->y2 = y;
}

uint8_t Command::getWidth() const
{
	return width;
}

void Command::setWidth(uint8_t w)
{
	this->width = w;
}

uint8_t Command::getHeight() const
{
	return height;
}

void Command::setHeight(uint8_t h)
{
	this->height = h;
}

uint8_t Command::getRadius() const
{
	return radius;
}

void Command::setRadius(uint8_t r)
{
	this->radius = r;
}

uint32_t Command::getForegroundColor() const
{
	return fg;
}

void Command::setForegroundColor(uint32_t c)
{
	this->fg = c;
}

uint32_t Command::getBackgroundColor() const
{
	return bg;
}

void Command::setBackgroundColor(uint32_t c)
{
	this->bg = c;
}

uint8_t Command::getStrokeSize() const
{
	return strokeSize;
}

void Command::setStrokeSize(uint8_t s)
{
	this->strokeSize = s;
}

StrokeStyleEnum Command::getStrokeStyle() const
{
	return strokeStyle;
}

void Command::setStrokeStyle(StrokeStyleEnum s)
{
	this->strokeStyle = s;
}

uint32_t Command::getStrokeColor() const
{
	return strokeColor;
}

void Command::setStrokeColor(uint32_t s)
{
	this->strokeColor = s;
}

uint8_t Command::getSpeed() const
{
	return speed;
}

void Command::setSpeed(uint8_t s)
{
	this->speed = s;
}

int16_t Command::getRepeat() const
{
	return repeat;
}

void Command::setRepeat(int16_t repeat)
{
	this->repeat = repeat;
}

DirectionEnum Command::getDirection() const
{
	return direction;
}

void Command::setDirection(DirectionEnum direction)
{
	this->direction = direction;
}

uint8_t Command::getIntensity() const
{
	return intensity;
}

void Command::setIntensity(uint8_t i)
{
	this->intensity = i;
}

boolean Command::getShow() const
{
	return show;
}

void Command::setShow(boolean show)
{
	this->show = show;
}

uint32_t Command::getOffDuration() const
{
	return offDuration;
}

void Command::setOffDuration(uint32_t offDuration)
{
	this->offDuration = offDuration;
}

uint32_t Command::getOnDuration() const
{
	return onDuration;
}

void Command::setOnDuration(uint32_t onDuration)
{
	this->onDuration = onDuration;
}

uint8_t Command::getIndex() const
{
	return index;
}

void Command::setIndex(uint8_t i)
{
	this->index = i;
}

ResponseCodesEnum Command::getStatus() const
{
	return status;
}

void Command::setStatus(ResponseCodesEnum s)
{
	status = s;
}

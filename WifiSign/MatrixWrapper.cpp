/*
 * MatrixWrapper.cpp
 *
 *  Created on: Jan 15, 2021
 *      Author: tsasala
 */

#include "MatrixWrapper.h"


uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;

Adafruit_Protomatter matrix(64, 4, 1, rgbPins, 4, addrPins, clockPin, latchPin, oePin, false);

uint16_t* SignImages[] = { (uint16_t*)Record_Red_Bitmap, (uint16_t*)Record_White_Bitmap };


/**
 * Constructor
 */
MatrixWrapper::MatrixWrapper()
{
	fontHeight = 0;
	fontWidth = 0;
	fontLines = 0;
	fontChars = 0;
//	iReader = new Adafruit_ImageReader();
//	fileSys = new FatFileSystem();
//	image = new Adafruit_Image();
}


boolean MatrixWrapper::initialize()
{
	boolean status = false;

	Serial.println("Initializing matrix...");
	ProtomatterStatus s = matrix.begin();
	Serial.print("Matrix status: ");
	Serial.println((int)s);
	if(s == PROTOMATTER_OK)
	{
		matrix.setTextWrap(false);

//		matrix.setFont(&FreeSansBold9pt7b);
//		matrix.setFont(&Dialog_bold_8);
//		matrix.setFont(&Nimbus_Sans_L_Regular_Condensed_8);
//		matrix.setFont(&Nimbus_Sans_L_Bold_10);
//		matrix.setFont(&Org_01);
		matrix.setFont(&DejaVu_Sans_Mono_10);

		// Calculate font size
		int16_t x=0, y=0, x1=0, y1=0;
		uint16_t h=0, w=0;

		matrix.getTextBounds(F("M"), x, y, &x1, &y1, &w, &h);
//		Serial.print("x=");
//		Serial.print(x);
//		Serial.print(", y=");
//		Serial.print(y);
//		Serial.print(", x1=");
//		Serial.print(x1);
//		Serial.print(", y1=");
//		Serial.print(y1);
//		Serial.print(", w=");
//		Serial.print(w);
//		Serial.print(", h=");
//		Serial.print(h);

		fontHeight = h;
		fontWidth = w;
		fontLines = matrix.height()/h;
		fontChars = matrix.width()/w;

//		Serial.print(", l=");
//		Serial.print(fontLines);
//		Serial.print(", c=");
//		Serial.print(fontChars);
//		Serial.println();

		if( fileSys.initialize() )
		{
			iReader.setFileSystem( fileSys.getFileSystem() );
			status = true;
		}
	}


	return status;

}

void MatrixWrapper::drawPixel(uint8_t x, uint8_t y, uint32_t color, boolean s)
{
	matrix.drawPixel(x, y, mapColor(color));
	if(s) show();
}

uint16_t MatrixWrapper::getPixel(uint8_t x, uint8_t y) const
{
	return matrix.getPixel(x, y);
}

void MatrixWrapper::fillScreen(uint16_t color, boolean s)
{
	matrix.fillScreen(mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawLine(uint8_t x, uint8_t y, uint8_t h, uint32_t color, DirectionEnum direction, boolean s)
{
	if(h == 0)
	{
		h=1;
	}
	if( direction == Up || direction == Down )
	{
		matrix.drawFastVLine(x, y, h, mapColor(color));
	}
	else if( direction == Left || direction == Right )
	{
		matrix.drawFastHLine(x, y, h, mapColor(color));
	}
	else
	{
		Serial.print(F("ERROR - Unknown direction value:"));
		Serial.println( direction, HEX);
	}
	if(s) show();
}

void MatrixWrapper::drawLine(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint32_t color, boolean s)
{
	matrix.drawLine(x, y, x1, y1, mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawVLine(uint8_t x, uint8_t y, uint8_t h, uint32_t color, boolean s)
{
	if(h == 0)
	{
		h=1;
	}
	matrix.drawFastVLine(x, y, h, mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawHLine(uint8_t x, uint8_t y, uint8_t w, uint32_t color, boolean s)
{
	if(w == 0)
	{
		w=1;
	}
	matrix.drawFastHLine(x, y, w, mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color, boolean s)
{
	if(h == 0)
	{
		h=1;
	}
	if(w == 0)
	{
		w=1;
	}
	matrix.drawRect(x, y, w, h, mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint32_t color, boolean s)
{
	if(h == 0)
	{
		h=1;
	}
	if(w == 0)
	{
		w=1;
	}
	matrix.fillRect(x, y, w, h, mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawCircle(uint8_t x, uint8_t y, uint8_t r, uint32_t color, boolean s)
{
	if(r == 0)
	{
		r=1;
	}
	matrix.drawCircle(x, y, r, mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawFilledCircle(uint8_t x, uint8_t y, uint8_t r, uint32_t color, boolean s)
{
	if(r == 0)
	{
		r=1;
	}
	matrix.fillCircle(x, y, r, mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint32_t color, boolean s)
{
	matrix.drawTriangle(x0, y0, x1, y1, x2, y2, mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawFilledTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint32_t color, boolean s)
{
	matrix.fillTriangle(x0, y0, x1, y1, x2, y2, mapColor(color));
	if(s) show();
}

void MatrixWrapper::drawRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint32_t c, boolean s)
{
	if(h == 0)
	{
		h=1;
	}
	if(w == 0)
	{
		w=1;
	}
	if(r == 0)
	{
		r=1;
	}
	matrix.drawRoundRect(x, y, w, h, r, mapColor(c));
	if(s) show();
}

void MatrixWrapper::drawFilledRoundRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, uint32_t c, boolean s)
{
	if(h == 0)
	{
		h=1;
	}
	if(w == 0)
	{
		w=1;
	}
	if(r == 0)
	{
		r=1;
	}
	matrix.fillRoundRect(x, y, w, h, r, mapColor(c));
	if(s) show();
}

void MatrixWrapper::printLine(uint8_t x, uint8_t y, uint32_t c, char *b, boolean s)
{

	int16_t x1=0, y1=0;
	uint16_t w=0, h=0, x2=0, y2=0;

	matrix.setTextColor( mapColor(c) );
	matrix.getTextBounds(b, x, y, &x1, &y1, &w, &h);

//	Serial.print("x=");
//	Serial.print(x);
//	Serial.print(", y=");
//	Serial.print(y);
//	Serial.print(", x1=");
//	Serial.print(x1);
//	Serial.print(", y1=");
//	Serial.print(y1);
//	Serial.print(", w=");
//	Serial.print(w);
//	Serial.print(", h=");
//	Serial.print(h);

	x2 = x*fontWidth;
	y2 = (y+1)*fontHeight;
//	Serial.print(", x2=");
//	Serial.print(x2);
//	Serial.print(", y2=");
//	Serial.println(y2);

	matrix.setCursor( x2, y2 );
	matrix.print(b);
	if(s) show();
}

void MatrixWrapper::printLine(uint8_t x, uint8_t y, uint32_t c, const __FlashStringHelper *s, boolean ss)
{
	int16_t x1=0, y1=0;
	uint16_t w=0, h=0, x2=0, y2=0;

	matrix.setTextColor( mapColor(c) );
	matrix.getTextBounds(s, x, y, &x1, &y1, &w, &h);
	x2 = x*fontWidth;
	y2 = (y+1)*fontHeight;
	matrix.setCursor( x2, y2 );
	matrix.print(s);
	if(ss) show();
}

void MatrixWrapper::print(uint8_t x, uint8_t y, uint32_t c, char *b, boolean s)
{
	int16_t x1=0, y1=0;
	uint16_t w=0, h=0;
	matrix.getTextBounds((const char*)b, x, y, &x1, &y1, &w, &h);
//	Serial.print("\nText String: '");
//	Serial.print(b);
//	Serial.println("'");
//	Serial.print("x=");
//	Serial.print(x);
//	Serial.print(", y=");
//	Serial.print(y);
//	Serial.print(", x1=");
//	Serial.print(x1);
//	Serial.print(", y1=");
//	Serial.print(y1);
//	Serial.print(", w=");
//	Serial.print(w);
//	Serial.print(", h=");
//	Serial.println(h);

	matrix.setTextColor( mapColor(c) );
	matrix.setCursor(x, y);
	matrix.print(b);
	if(s) show();
}

void MatrixWrapper::print(uint8_t x, uint8_t y, uint32_t c, const __FlashStringHelper *s, boolean ss)
{
	int16_t x1=0, y1=0;
	uint16_t w=0, h=0;
	matrix.getTextBounds(s, x, y, &x1, &y1, &w, &h);
//	Serial.print("\nText String: '");
//	Serial.print(s);
//	Serial.println("'");
//	Serial.print("x=");
//	Serial.print(x);
//	Serial.print(", y=");
//	Serial.print(y);
//	Serial.print(", x1=");
//	Serial.print(x1);
//	Serial.print(", y1=");
//	Serial.print(y1);
//	Serial.print(", w=");
//	Serial.print(w);
//	Serial.print(", h=");
//	Serial.println(h);

	matrix.setTextColor( mapColor(c) );
	matrix.setCursor(x, y);
	matrix.print(s);
	if(ss) show();
}

void MatrixWrapper::setIntensity(uint8_t i)
{
	if(i==0)
	{
		fillScreen(0, true);
	}
	else
	{

	}
}

void MatrixWrapper::show()
{
	matrix.show();
}

void MatrixWrapper::clear()
{
	matrix.fillScreen(mapColor(0)); // can simply call fill rect too
	matrix.show();
}

uint8_t MatrixWrapper::getWidth()
{
	return matrix.width();
}

uint8_t MatrixWrapper::getHeight()
{
	return matrix.height();
}


void MatrixWrapper::testFull()
{
	Serial.println(F("Full matrix test: BEGIN"));

	test();

	for(uint8_t i=0; i<7; i++)
	{
		drawImage(RecordRed, true);
		delay(250);
		drawImage(RecordWhite, true);
		delay(250);
	}
	clear();

	drawLine(0,  matrix.height()-1, 5, 0x00ff0000, Left, false);
	printLine(0, 0, 0x00FF0000, F("TestM00"), false);

	drawLine(7,  matrix.height()-1, 5, 0x00ff00, Left, false);
	printLine(1, 1, 0x00ff00, F("TestM01"), false);

	drawLine(14,  matrix.height()-1, 5, 0x00ff, Left, false);
	printLine(2, 2, 0x00FF, F("TestM02"), false);

	drawLine(21,  matrix.height()-1, 5, 0x00ffff, Left, false);
	printLine(3, 3, 0x00FFFF, F("TestM03"), false);

	show();
	delay(1000);
	clear();

	matrix.setCursor(0,7);
	matrix.print(F("Test-00"));
	matrix.setCursor(1,14);
	matrix.print(F("Test-01"));
	matrix.setCursor(2,21);
	matrix.print(F("Test-02"));
	matrix.setCursor(3,28);
	matrix.print(F("Test-03"));

	show();
	delay(1000);
	clear();

	scrollText(0,  0,  0x00ff0000, 50, 1, Left, F("TestLeft") );
	delay(1000);
	clear();

	scrollText(0,  5,  0x000000ff, 50, 1, Right, F("TestRight") );
	delay(1000);
	clear();

	scrollText(20,  0,  0x0000ff00, 50, 1, Down, F("TestDown") );
	delay(1000);
	clear();

	scrollText(10,  0,  0x00ffff00, 50, 1, Up, F("TestUp") );
	delay(1000);
	clear();

	Serial.println(F("Full matrix test: END"));

}

void MatrixWrapper::test()
{
	Serial.println(F("Matrix Test: BEGIN"));

	uint32_t c = 0;
	float hue = 0;
	for(uint8_t i=0; i<matrix.height(); i++)
	{
		c = hsv2rgb(hue,  1.0,  1.0);
		drawLine(0, i, 64, c, Left, false);
		hue += 60;
		if(hue > 360)
		{
			hue=0;
		}
		show();
		delay(50);
	}
	delay(1000);
	clear();

	Serial.println(F("Matrix Test: END"));

}

void MatrixWrapper::printString(const char *s, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
	matrix.getTextBounds(s, x, y, x1, y1, w, h);
	Serial.print("String: '");
	Serial.print(s);
	Serial.println("'");

	Serial.print("x=");
	Serial.print(x);
	Serial.print(", y=");
	Serial.print(y);
	Serial.print(", x1=");
	Serial.print(*x1);
	Serial.print(", y1=");
	Serial.print(*y1);
	Serial.print(", w=");
	Serial.print(*w);
	Serial.print(", h=");
	Serial.println(*h);

}

void MatrixWrapper::testText()
{
	Serial.println(F("Matrix Test: BEGIN"));


	int16_t x1=0, y1=0;
	uint16_t w=0, h=0;
	uint8_t x = 0, y=0;

	printString((const char*)F("WIFI MQTT Recording"),x,y,&x1,&y1,&w,&h);

	x1 = y1 = 0;
	w = h = 0;
	printString((const char*)F("WIFI MQTT Recording 192.168.30.113"),x,y,&x1,&y1,&w,&h);

	x1 = y1 = 0;
	w = h = 0;
	printString((const char*)F("WIFI: 192.168.30.113"),x,y,&x1,&y1,&w,&h);

	x1 = y1 = 0;
	w = h = 0;
	printString((const char*)F("MQTT: 192.168.30.113"),x,y,&x1,&y1,&w,&h);

	x1 = y1 = 0;
	w = h = 0;
	printString((const char*)F("()#jqt|"),x,y,&x1,&y1,&w,&h);


	Serial.println(F("Matrix Test: END"));

}


void MatrixWrapper::scrollText(uint8_t x, uint8_t y, uint32_t fg, uint32_t speed, int16_t repeat, DirectionEnum direction, uint8_t *s)
{
	scrollText(x, y, fg, 0x0, speed, repeat, direction, (__FlashStringHelper*)s );
}
void MatrixWrapper::scrollText(uint8_t x, uint8_t y, uint32_t fg, uint32_t bg, uint32_t speed, int16_t repeat, DirectionEnum direction, uint8_t *s)
{
	scrollText(x, y, fg, bg, speed, repeat, direction, (__FlashStringHelper*)s );
}

void MatrixWrapper::scrollText(uint8_t x, uint8_t y, uint32_t fg, uint32_t speed, int16_t repeat, DirectionEnum direction, const __FlashStringHelper *s)
{
	scrollText(x, y, fg, 0x0, speed, repeat, direction, (__FlashStringHelper*)s );
}


void MatrixWrapper::scrollText(uint8_t x, uint8_t y, uint32_t fg, uint32_t bg, uint32_t speed, int16_t repeat, DirectionEnum direction, const __FlashStringHelper *s)
{

	int16_t x1=0, y1=0;
	uint16_t w=0, h=0;
	int16_t minx=0, miny=0;
	int16_t xo = 0;
	int16_t yo = 0;

	matrix.getTextBounds(s, 0, 0, &minx, &miny, &w, &h);
	xo = -(w+minx);
	yo = -(h+miny);

//	Serial.print("bounds x=");
//	Serial.print(0);
//	Serial.print(", y=");
//	Serial.print(0);
//	Serial.print(", minx=");
//	Serial.print(minx);
//	Serial.print(", miny=");
//	Serial.print(miny);
//	Serial.print(", w=");
//	Serial.print(w);
//	Serial.print(", h=");
//	Serial.print(h);
//	Serial.print(", xo=");
//	Serial.print(xo);
//	Serial.print(", yo=");
//	Serial.print(yo);
//	Serial.println();

	uint16_t scrollCount = 0;
	uint16_t mwidth=matrix.width();
	uint16_t mheight=matrix.height();
	int16_t r = repeat;

	x1=y1=0;
	w=h=0;
	matrix.getTextBounds(s, x, y, &x1, &y1, &w, &h);

	xo = -(w+minx);
	yo = -(h+miny);


//	Serial.print("\nScroll String: '");
//	Serial.print(s);
//	Serial.println("'");
//
//	Serial.print("bounds x=");
//	Serial.print(x);
//	Serial.print(", y=");
//	Serial.print(y);
//	Serial.print(", x1=");
//	Serial.print(x1);
//	Serial.print(", y1=");
//	Serial.print(y1);
//	Serial.print(", w=");
//	Serial.print(w);
//	Serial.print(", h=");
//	Serial.print(h);
//	Serial.print(", xo=");
//	Serial.print(xo);
//	Serial.print(", yo=");
//	Serial.print(yo);
//	Serial.println();

	matrix.setTextColor( mapColor(fg) );

	// NOTE - rect draws from top left coordinates, text draws from bottom left.  Need to compensate by adjusting x,y for text
	int16_t rsx=x, rsy=y;
	int16_t tsx=x, tsy=y;
	int16_t rh=h, rw=w;
	int16_t th=h, tw=w;

	if( direction == Left )
	{
		rsx = 0; // rect start x
		rsy = y; // rect start y
		rh = h; // rect height
		rw = mwidth; // rect width

		tsx = mwidth; // text start x
		tsy = y+h+yo; // start y offset by height of text
		th = h; // text height
		tw = w; // text width
		scrollCount = mwidth + w; // length to scroll is the size of string + size of the matrix (starts with a blank matrix)
		do
		{
			for(uint16_t i=0; i<scrollCount; i++)
			{
				drawFilledRectangle(rsx, rsy, rw, rh, bg, false);
				matrix.setCursor(tsx-i,tsy);
				matrix.print(s);
				show();
				if(commandDelay(speed)) break;
			}
			if( r > 0 )
			{
				r--;
			}
		}
		while( !isCommandAvailable() || (r > 0 && repeat > 0) );
	}
	else if( direction == Right )
	{
		// TODO - FONT HEIGHT STILL NOT WORKING
		rsx = 0; // rect start x - ignore passed in x value since we're scrolling across the entire screen
		rsy = y; // rect start y - start at passed in y
		rh = h; // rect height is string height in pixels
		rw = mwidth; // rect width is whole screen

		// Custom fonts render from baseline, not top left
		// So we need to adjust the start point
		tsx = -w; // text start x
		tsy = y+h+yo; // text start y - move y down by height of text
		th = h; // text height
		tw = w; // text width
		scrollCount = mwidth + w; // length to scroll is the size of string + size of the matrix (starts with a blank matrix)

//		Serial.print("rect(rsx=");
//		Serial.print(rsx);
//		Serial.print(", rsy=");
//		Serial.print(rsy);
//		Serial.print(", rh=");
//		Serial.print(rh);
//		Serial.print(", rw=");
//		Serial.print(rw);
//		Serial.print(") text(tsx=");
//		Serial.print(tsx);
//		Serial.print(", tsy=");
//		Serial.print(tsy);
//		Serial.print(", th=");
//		Serial.print(th);
//		Serial.print(", tw=");
//		Serial.print(tw);
//		Serial.println();

		do
		{
			for(uint16_t i=0; i<scrollCount; i++)
			{
				drawFilledRectangle(rsx, rsy, rw, rh, bg, false);
				matrix.setCursor(i+tsx,tsy);
				matrix.print(s);
				show();
				if(commandDelay(speed)) break;
			}
			if( repeat > 0 )
			{
				r--;
			}
		}
		while( (isCommandAvailable()==false) && (r > 0 || repeat == 0) );
	}
	else if( direction == Up )
	{
		rsx = x; // rect start x
		rsy = 0; // rect start y
		rh = mheight; // rect height
		rw = w; // rect width

		tsx = x; // text start x
		tsy = mheight+h; // start y offset by height of text
		th = h; // text height
		tw = w; // text width
		scrollCount = h+mheight;

		do
		{
			for(uint16_t i=0; i<scrollCount; i++)
			{
				drawFilledRectangle(rsx, rsy, rw, rh, bg, false);
				matrix.setCursor(tsx,mheight-i);
				matrix.print(s);
				show();
				if(commandDelay(speed)) break;
			}
			if( r > 0 )
			{
				r--;
			}
		}
		while( !isCommandAvailable() || (r > 0 && repeat > 0) );
	}
	else if( direction == Down )
	{
		rsx = x; // rect start x
		rsy = 0; // rect start y
		rh = mheight; // rect height
		rw = w; // rect width

		tsx = x; // text start x
		tsy = 0; // start y offset by height of text
		th = h; // text height
		tw = w; // text width
		scrollCount = h+mheight;

		do
		{
			for(uint16_t i=0; i<scrollCount; i++)
			{
				drawFilledRectangle(rsx, rsy, rw, rh, bg, false);
				matrix.setCursor(x,i);
				matrix.print(s);
				show();
				if(commandDelay(speed)) break;
			}
			if( r > 0 )
			{
				r--;
			}
		}
		while( !isCommandAvailable() || (r > 0 && repeat > 0) );
	}
}

void MatrixWrapper::drawImage(char *fileName, boolean show)
{
	drawImage(fileName, 0, 0, show);
}

void MatrixWrapper::drawImage(char *fileName, uint8_t x, uint8_t y, boolean s)
{
	//clear();
//	Serial.print(F("Loading image: "));
//	Serial.println(fileName );

	ImageReturnCode stat = iReader.loadBMP(fileName, image);
	if( stat == IMAGE_SUCCESS)
	{
//		Serial.println(F("Loaded image!"));
		GFXcanvas16 *canvas = (GFXcanvas16*)image.getCanvas();
		matrix.drawRGBBitmap(x, y, canvas->getBuffer(), canvas->width(), canvas->height());
		if(s) show();
	}
	else
	{
		Serial.print(F("ERROR - Unable to load image: "));
		iReader.printStatus(stat);
	}
	if(s) show();
}


void MatrixWrapper::drawImage(ImageEnum index, boolean s)
{
	clear();
	matrix.drawRGBBitmap(0, 0, SignImages[index], 64, 32);
	if(s) show();
}

void MatrixWrapper::animate(AnimateEnum index, uint32_t delay)
{
	boolean flag = false;

	Serial.println("Animate Command: BEGIN");
	switch(index)
	{
		case Record:
//			Serial.print(F("Index: "));
//			Serial.print( index );
//			Serial.print(", delay: ");
//			Serial.println( delay );
			while(!flag)
			{
				drawImage(RecordRed, true);
				flag = commandDelay( delay);
				if(flag) break;
				drawImage(RecordWhite, true);
				flag = commandDelay( delay);
				if(flag) break;
			}
			clear();
			break;
		default:
			break;

	}
	Serial.println("Animate Command: END");
}


uint16_t MatrixWrapper::mapColor(uint32_t color)
{
	return (( (color>>16) & 0xF8) << 8) | (( (color>>8) & 0xFC) << 3) | ( (color&0xff) >> 3);
}

uint32_t MatrixWrapper::hsv2rgb(int h, double s, double v)
{
	uint32_t c;

	  //this is the algorithm to convert from RGB to HSV
	  double r=0;
	  double g=0;
	  double b=0;

	  int i=(int)floor(h/60.0);
	  double f = h/60.0 - i;
	  double pv = v * (1 - s);
	  double qv = v * (1 - s*f);
	  double tv = v * (1 - s * (1 - f));

	  switch (i)
	  {
	  case 0:
	    r = v;
	    g = tv;
	    b = pv;
	    break;
	  case 1:
	    r = qv;
	    g = v;
	    b = pv;
	    break;
	  case 2:
	    r = pv;
	    g = v;
	    b = tv;
	    break;
	  case 3:
	    r = pv;
	    g = qv;
	    b = v;
	    break;
	  case 4:
	    r = tv;
	    g = pv;
	    b = v;
	    break;
	  case 5:
	    r = v;
	    g = pv;
	    b = qv;
	    break;
	  }

	  //set each component to a integer value between 0 and 255
	  int red=constrain((int)255*r,0,255);
	  int green=constrain((int)255*g,0,255);
	  int blue=constrain((int)255*b,0,255);

	  c = (red<<16)|(green<<8)|(blue);

	return c;
}

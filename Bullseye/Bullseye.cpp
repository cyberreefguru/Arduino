// Do not remove the include below
#include "Bullseye.h"


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_NEOPIXELS, NEO_RGB);

ESP8266Timer ITimer;

//The setup function is called once at startup of the sketch
void setup()
{

	Serial.begin(115200);
	Serial.println("Setup Start");

	pinMode(PIN_LED_BUILTIN, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);
	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_BLUE, OUTPUT);

	pinMode(PIN_LDR, INPUT);
	pinMode(PIN_BUTTON, INPUT);
	pinMode(PIN_INPUT_SW1, INPUT_PULLUP);
	pinMode(PIN_INPUT_SW2, INPUT_PULLUP);

	// Turn LEDs Off
	digitalWrite(PIN_LED_RED, LOW);
	digitalWrite(PIN_LED_GREEN, LOW);
	digitalWrite(PIN_LED_BLUE, LOW);
	digitalWrite(PIN_LED_BUILTIN, HIGH);

	// Load current state of the switch pins
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		// set current state of pin
		isrConfig[i].state = digitalRead(isrConfig[i].pin);

		// Initialize interrupt for button; will call ISR when switch input goes low
		if( isrConfig[i].config == ACTIVE_LOW )
		{
			attachInterrupt(digitalPinToInterrupt( isrConfig[i].pin), isrActiveLowFallingEdge, FALLING); // Button pulled high, press brings it low
		}
		else if( isrConfig[i].config == ACTIVE_HIGH )
		{
			attachInterrupt(digitalPinToInterrupt( isrConfig[i].pin), isrActiveHighFallingEdge, FALLING); // Button pulled high, press brings it low
		}

		Serial.printf("Pin[%i]=%i, State=%i\n", i, isrConfig[i].pin, isrConfig[i].state);
	}

	// Initialize timer interrupt; calls timer ISR every 100ms.
	if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, isrTimer)) // Interval param in microsecs; multiply by 1000 since constant is in ms
		Serial.println("Starting  ITimer OK, millis() = " + String(millis()));
	else
		Serial.println("Can't set ITimer. Select another freq. or interval");

	// Initialize randomizer
	randomSeed( analogRead(PIN_LDR) );

	// Initialize Neopixels
	pixels.begin();
	pixels.setBrightness(255);
	writeColor(WHITE, false); // set LED color to white, but don't show

	gBrightness = 128;
	gMode = Start;
	gHeartBeatColor = Blue;

	Serial.println("Setup Complete");

}

// The loop function is called in an endless loop
void loop()
{
	switch( gMode )
	{
		case Start:
			if( fadeRings(1, ORANGE, 10, 2000) ) break;
			if( fadeRings(1, PURPLE, 10, 2000) ) break;
			break;
		case Incoming:
//			pixels.setBrightness(gBrightness);
//			if( sequence(25, RED, WHITE, 0, NUM_PIXELS-1, false) ) break;
//			if( sequence(25, WHITE, RED, NUM_PIXELS-1, 0, false) ) break;
//			if( sequence(10, RED, WHITE, 0, NUM_PIXELS-1, false) ) break;
//			if( sequence(10, WHITE, RED, 0, NUM_PIXELS-1, false) ) break;
			pixels.setBrightness(gBrightness);
			if( flashRingConcentric( 1, WHITE, RED, WHITE, 50, 500) ) break;
			break;
		case Landed:
			gBrightness=128;
			pixels.setBrightness(gBrightness);
			writeColor(WHITE, true);
//			if( flashRingConcentric( 1, WHITE, RED, WHITE, 50, 500) ) break;
//			if( flashRingConcentric( 5, WHITE, ORANGE, PURPLE, 100, 500) ) break;

			break;
		default:
			break;
	}
	if( isCommandAvailable() )
	{
		// Set the display mode. Simple state machine that advances each button press;
		switch( gMode )
		{
			case Start:
				gMode = Incoming;
				gHeartBeatColor = Green;
				break;
			case Incoming:
				gMode = Landed;
				gHeartBeatColor = Red;
				break;
			case Landed:
				gMode = Start;
				gHeartBeatColor = Blue;
				break;
			default:
				gMode = Start;
				gHeartBeatColor = Blue;
				break;
		}
		setCommandAvailable( false );
	}

} // end loop

//boolean fadeRings(uint8_t reps, uint32_t colorRing1, uint32_t colorRing2, uint32_t colorRing3, uint32_t colorRing4, uint32_t fadeDuration, uint32_t holdDuration)
//{
//	return false;
//}


boolean fadeRings(uint8_t reps, uint32_t color, uint32_t fadeDuration, uint32_t holdDuration)
{
	for(uint8_t i=0; i<reps; i++)
	{
		if( fade(UP, fadeDuration, color) ) return true;
		if( commandWait( holdDuration) ) return true;
		if( fade(DOWN, fadeDuration, color)) return true;
	}
	return false;

}

boolean flashRingConcentric(uint8_t reps, uint32_t startColor, uint32_t color1, uint32_t color2, uint32_t holdDuration, uint32_t loopHoldDuration)
{
	boolean cmd = isCommandAvailable();

	if( !cmd )
	{
		pixels.setBrightness(gBrightness);
		writeColor(startColor, true);
		for(uint8_t i=0; i<reps; i++)
		{
			cmd = true;

			writeRing(RING_1, color1, true);
			if(commandWait(holdDuration)) break;

			writeRing(RING_2, color1, true);
			if(commandWait(holdDuration)) break;

			writeRing(RING_3, color1, true);
			if(commandWait(holdDuration)) break;

			writeRing(RING_4, color1, true);
			if(commandWait(holdDuration)) break;

			writeRing(RING_4, color2, true);
			if(commandWait(holdDuration)) break;

			writeRing(RING_3, color2, true);
			if(commandWait(holdDuration)) break;

			writeRing(RING_2, color2, true);
			if(commandWait(holdDuration)) break;

			writeRing(RING_1, color2, true);
			if(commandWait(loopHoldDuration)) break;

			cmd = false;
		}
	} // end cmd

	return cmd;

} // end

/**
 * Interrupt service routine that catches the falling edge of a pin interrupt for active low inputs.
 *
 * This starts the de-bounce timer for the input.
 *
 */
void ICACHE_RAM_ATTR isrActiveLowFallingEdge()
{
	unsigned long currentTime = millis();
	uint8_t v = 0;

	Serial.println("** ACTIVE LOW FALLING EDGE - START **");

	// Loop through all pins to see which ones changed
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		Serial.printf("LOW FALL: Pin[%i], State=%i, Current=%i, Change=%i, Processing=%i\n", i, isrConfig[i].state, digitalRead(isrConfig[i].pin), isrConfig[i].changed, isrConfig[i].processing);
	}

	// Loop through all pins to see which ones changed
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		// Skip processing this pin if it is active high
		if( isrConfig[i].config == ACTIVE_HIGH )
			continue;

		// read current value of port
		v = digitalRead( isrConfig[i].pin );

		// See if we've already started the timer
		if( isrConfig[i].processing == false && v == 0)
		{
			Serial.printf("Starting timer[%i]\n", i);
			// timer not started; start it and set ISR to be rising
			isrConfig[i].processing = true;
			isrConfig[i].changed = false;
			isrConfig[i].time = currentTime;
			isrConfig[i].state = v;
			attachInterrupt(digitalPinToInterrupt( isrConfig[i].pin ), isrActiveLowRisingEdge, RISING);
		}

	} // end for

	Serial.println("**  ACTIVE LOW FALLING EDGE - COMPLETE **\n");
}

/**
 * Interrupt service routine that catches the rising edge of a pin interrupt
 *
 * This should be the end of the timer/bounce cycle
 */
void ICACHE_RAM_ATTR isrActiveLowRisingEdge()
{
	unsigned long currentTime = millis();
	uint8_t v = 0;

	Serial.println("** ACTIVE HIGH RISING EDGE - BEGIN **");

	// Loop through all pins to see which ones changed
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		Serial.printf("LOW RISE: Pin[%i], State=%i, Current=%i, Change=%i, Processing=%i\n", i, isrConfig[i].state, digitalRead(isrConfig[i].pin), isrConfig[i].changed, isrConfig[i].processing);
	}

	// Loop through all pins to see which ones changed
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		// Skip processing this pin if it is active high
		if(  isrConfig[i].config == ACTIVE_HIGH )
			continue;

		// read current value of port
		v = digitalRead( isrConfig[i].pin );

		// See if we've already started the timer
		if( isrConfig[i].processing == true && v == 1 )
		{
			// Timer expired; we have valid input
			if( currentTime > (isrConfig[i].time + DEBOUNCE_TIME ) )
			{
				// Timer expired; we have valid input
				isrConfig[i].changed = true;
			}
			else
			{
				// Pulse too short, skip as input
				isrConfig[i].changed = false;
			}
			// Reset rest of parameters
			isrConfig[i].processing = false;
			isrConfig[i].time = currentTime;
			isrConfig[i].state = v;
			attachInterrupt(digitalPinToInterrupt( isrConfig[i].pin ), isrActiveLowFallingEdge, FALLING);

		}
	}

	Serial.println("** ACTIVE LOW RISING EDGE - COMPLETE **\n");

}

/**
 * Interrupt service routine that catches the rising edge of a pin interrupt
 *
 */
void ICACHE_RAM_ATTR isrActiveHighRisingEdge()
{
	unsigned long currentTime = millis();
	uint8_t v = 0;

	Serial.println("** ACTIVE HIGH RISING EDGE - BEGIN **");

	// Loop through all pins to see which ones changed
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		Serial.printf("HIGH FALL: Pin[%i], State=%i, Current=%i, Change=%i, Processing=%i\n", i, isrConfig[i].state, digitalRead(isrConfig[i].pin), isrConfig[i].changed, isrConfig[i].processing);
	}

	// Loop through all pins to see which ones changed
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		// Skip processing this pin if it is active high
		if( isrConfig[i].config == ACTIVE_LOW )
			continue;

		// read current value of port
		v = digitalRead( isrConfig[i].pin );

		// See if we've already started the timer
		if( isrConfig[i].processing == false && v == 1)
		{
			// timer not started; start it and set ISR to be rising
			isrConfig[i].processing = true;
			isrConfig[i].changed = false;
			isrConfig[i].time = currentTime;
			isrConfig[i].state = v;
			attachInterrupt(digitalPinToInterrupt( isrConfig[i].pin ), isrActiveHighFallingEdge, FALLING);
		}

	} // end for

	Serial.println("** ACTIVE HIGH RISING EDGE - COMPLETE **\n");

}


/**
 * Interrupt service routine that catches the falling edge of a pin interrupt for active low inputs.
 *
 * This starts the de-bounce timer for the input.
 *
 */
void ICACHE_RAM_ATTR isrActiveHighFallingEdge()
{
	unsigned long currentTime = millis();
	uint8_t v = 0;

	Serial.println("** ACTIVE HIGH FALLING EDGE - START **");


	// Loop through all pins to see which ones changed
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		Serial.printf("HIGH RISE: Pin[%i], State=%i, Current=%i, Change=%i, Processing=%i\n", i, isrConfig[i].state, digitalRead(isrConfig[i].pin), isrConfig[i].changed, isrConfig[i].processing);
	}

	// Loop through all pins to see which ones changed
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		// Skip processing this pin if it is active high
		if( isrConfig[i].config == ACTIVE_LOW )
			continue;

		// read current value of port
		v = digitalRead( isrConfig[i].pin );


		// See if we've already started the timer
		if( isrConfig[i].processing == true && v == 0)
		{
			// Timer expired; we have valid input
			if( currentTime > (isrConfig[i].time + DEBOUNCE_TIME ) )
			{
				// Timer expired; we have valid input
				isrConfig[i].changed = true;
			}
			else
			{
				// Pulse too short, skip as input
				isrConfig[i].changed = false;
			}
			isrConfig[i].processing = false;
			isrConfig[i].time = currentTime;
			isrConfig[i].state = v;
			attachInterrupt(digitalPinToInterrupt( isrConfig[i].pin ), isrActiveHighRisingEdge, RISING);

		}
	}

	Serial.println("**  ACTIVE HIGH FALLING EDGE - COMPLETE **\n");
}

/**
 * ISR for timer; execution every 100ms.
 *
 */
void ICACHE_RAM_ATTR isrTimer()
{
	// Update heartbeat LED
	hbCounter++;
	switch(hbCounter)
	{
	case 1:
 		digitalWrite(PIN_LED_RED, ( (gHeartBeatColor>>16) & 0x01) );	// on
 		digitalWrite(PIN_LED_GREEN, ( (gHeartBeatColor>>8) & 0x01) ); 	// on
 		digitalWrite(PIN_LED_BLUE, ( (gHeartBeatColor) & 0x01));  		// on
		break;
	case 3:
 		digitalWrite(PIN_LED_RED, LOW);   // off
 		digitalWrite(PIN_LED_GREEN, LOW); // off
 		digitalWrite(PIN_LED_BLUE, LOW);  // off
		break;
	case 5:
 		digitalWrite(PIN_LED_RED, ( (gHeartBeatColor>>16) & 0x01) );	// on
 		digitalWrite(PIN_LED_GREEN, ( (gHeartBeatColor>>8) & 0x01) ); 	// on
 		digitalWrite(PIN_LED_BLUE, ( (gHeartBeatColor) & 0x01));  		// on
		break;
	case 7:
 		digitalWrite(PIN_LED_RED, LOW);   // off
 		digitalWrite(PIN_LED_GREEN, LOW); // off
 		digitalWrite(PIN_LED_BLUE, LOW);  // off
		break;
	case 14:
		hbCounter=0;
		break;
	}

	// Loop through all pins to see which ones changed
	for(uint8_t i=0; i<ISR_SIZE; i++)
	{
		if( isrConfig[i].changed == true )
		{
			Serial.printf("TIMER_ISR: Pin %i Changed\n", i);

			// reset change button
			isrConfig[i].changed = false;

			// Call appropriate ISR here
			(*isrConfig[i].handler)();

		}
	} // end for ISR_PIN_SIZE

} // end isrTimer


void ICACHE_RAM_ATTR isrInput1()
{
//	Serial.println("\n** ISR INPUT 1 - BEGIN **");

	gCommand = true;

//	if( ledOn )
//	{
//		Serial.printf("Turning LED off\n");
//		digitalWrite(PIN_LED_RED, LOW);
//		ledOn = false;
//	}
//	else
//	{
//		Serial.printf("Turning LED on\n");
//		digitalWrite(PIN_LED_RED, HIGH);
//		ledOn = true;
//	}

//	Serial.println("** ISR INPUT 1 - END **\n");

}

void ICACHE_RAM_ATTR isrInput2()
{
	Serial.println("\n** ISR INPUT 2 **\n");

}

void ICACHE_RAM_ATTR isrInput3()
{
	Serial.println("\n** ISR INPUT 3 **\n");

}

/**
 * Changes the color of the specified ring.
 *
 * @ring - index of which ring to change [RING_1, RING_2, RING_3, RING_4]
 * @color - color to use
 * @show - true - show now; false - do not show (can be used to change multiple rings then show at once)
 */
void writeRing(uint8_t ring, uint32_t color, uint8_t show)
{
	switch(ring)
	{
		case RING_1:
			writeColor(color, 46, 49, show);
			break;
		case RING_2:
			writeColor(color, 35, 45, show);
			break;
		case RING_3:
			writeColor(color, 20, 34, show);
			break;
		case RING_4:
			writeColor(color, 0, 19, show);
			break;
		case RING_ALL:
			writeColor(color, show);
			break;
		default:
			break;
	}
}

/**
 * Lights all LEDs in sequence. If clear = true, turns LED off after lighting.
 */
boolean sequence(uint32_t wait, uint32_t onColor, uint32_t offColor, uint8_t clear)
{
	return sequence(wait, onColor, offColor, 0, NUM_PIXELS-1, clear);
}

/**
 * Lights LEDs in sequence. Starting with start index, ending with end index.  If clear = true, turns LED off after lighting.
 */
boolean sequence(uint32_t wait, uint32_t onColor, uint32_t offColor, uint8_t start, uint8_t end, uint8_t clear)
{
	boolean cmd = isCommandAvailable();

	if( !cmd )
	{
		writeColor( offColor, true);
		if( start < end)
		{
			for(uint8_t i=start; i<=end; i++)
			{
				pixels.setPixelColor(i, onColor);
				pixels.show(); // show color
				cmd = commandWait(wait);
				if( cmd )
				{
					break;
				}
				if( clear == true )
				{
					pixels.setPixelColor(i, offColor);
					pixels.show(); // show color
				}
			}
		}
		else if(start > end)
		{
			uint8_t offset = 0;
			uint8_t index = 0;

			for(uint8_t i=end; i<=start; i++)
			{
				index = start-offset;
				pixels.setPixelColor(index, onColor);
				pixels.show(); // show color
				cmd = commandWait(wait);
				if( cmd )
				{
					break;
				}
				if( clear == true )
				{
					pixels.setPixelColor(index, offColor);
					pixels.show(); // show color
				}
				offset++;
			}
		}
	} // end if cmd

	return cmd;

} // end sequence

/**
 * Randomly turns on all LEDs
 *
 * @wait - time between affecting LED color
 * @onColor
 * @offColor
 *
 */
boolean random(uint32_t wait, uint32_t onColor, uint32_t offColor)
{
	return random( wait, onColor, offColor, 0, NUM_PIXELS);
}


/**
 * Randomly turns on the LEDs
 *
 * @wait
 * @onColor
 * @offColor
 * @start
 * @end
 */
boolean random(uint32_t wait, uint32_t onColor, uint32_t offColor, uint8_t start, uint8_t end)
{
	uint16_t pixel = 0;
	uint8_t count = 0;
	uint8_t numPixels = end - start + 1;
	boolean cmd = isCommandAvailable();

	if( !cmd )
	{
		writeColor( offColor, true);

		while( count < numPixels )
		{
			pixel = (uint8_t)random(0, numPixels-1);
			if(pixels.getPixelColor(pixel) == offColor )
			{
				pixels.setPixelColor(pixel, onColor); // set color
				pixels.show(); // show color
				count++; // increase count
				cmd = commandWait(wait);
				if(cmd)
				{
					break;
				}
			}

		}
	} // end cmd

	return cmd;
}

/**
 *
 */
boolean sparkle(uint32_t duration, uint32_t onTime, int32_t offTime, uint32_t onColor, uint32_t offColor)
{
	return sparkle(duration, onTime, offTime, onColor, offColor, 0, NUM_PIXELS);
}

/**
 *
 */
boolean sparkle(uint32_t duration, uint32_t onTime, int32_t offTime, uint32_t onColor, uint32_t offColor, uint8_t start, uint8_t end)
{
	uint16_t pixel = 0;
	uint32_t now = millis() + duration;
	uint8_t numPixels = end - start + 1;
	boolean cmd = isCommandAvailable();

	if( !cmd )
	{
		writeColor( offColor, true);

		while( millis() < now )
		{
			pixel = (uint8_t)random(0, numPixels-1);
			pixels.setPixelColor(pixel, onColor); // set color
			pixels.show(); // show color

			cmd = commandWait(onTime);
			if( cmd )
			{
				break;
			}
			pixels.setPixelColor(pixel, offColor); // set color
			pixels.show();
			cmd = commandWait(offTime);
			if( cmd )
			{
				break;
			}
		}
	}
	return cmd;
}

/**
 * Fades LED brightness up or down from/to the specified color
 *
 * @direction UP or DOWN
 * @time - time between fade changes (ms)
 * @color to fade from/to
 */
boolean fade(uint8_t direction, uint32_t time, uint32_t color)
{
	uint8_t i;
	boolean cmd = isCommandAvailable();

	if( !cmd )
	{
		for(i=0; i<255; i++)
		{
			writeColor( color, false );
			if( direction == DOWN )
			{
				pixels.setBrightness(255-i);
			}
			else if( direction == UP)
			{
				pixels.setBrightness(i);
			}
			pixels.show();
			cmd = commandWait(time);
			if( cmd )
			{
				break;
			}
		}
	} // end cmd

	return cmd;
}

boolean fade(uint8_t direction, uint32_t time, uint32_t color, uint8_t start, uint8_t end)
{
	uint8_t i;
    uint8_t r = (uint8_t)(color >> 16);
    uint8_t g = (uint8_t)(color >>  8);
    uint8_t b = (uint8_t)color;
    uint8_t brightness;
    boolean cmd = isCommandAvailable();

    if( !cmd )
    {
		for(i=0; i<255; i++)
		{
			for(int pixelIndex = start; pixelIndex < end; pixelIndex++)
			{
				if( direction == DOWN )
				{
					brightness = 255-i;
				}
				else if( direction == UP)
				{
					brightness=i;
				}
				pixels.setPixelColor(pixelIndex, (brightness*r/255) , (brightness*g/255), (brightness*b/255));
			}
			pixels.show();
			cmd = commandWait(time);
			if( cmd )
			{
				break;
			}
		}
    }

    return cmd;
}

/**
 * Writes the specified color to all the LEDs.
 *
 * @color - the color to set
 * @show - if true, shows color immediately.  Can be used to set multiple colors without displaying immediately.
 */
void writeColor(uint32_t color, uint8_t show)
{
	writeColor( color, 0, NUM_PIXELS-1, show);
}


/**
 * Writes the specified color to the LEDs.
 *
 * @color - the color to set
 * @start - start index to set color
 * @end - end index to set color
 * @show - if true, shows color immediately.  Can be used to set multiple colors without displaying immediately.
 */
void writeColor(uint32_t color, uint8_t start, uint8_t end, uint8_t show)
{
	for(uint8_t i=start; i<=end; i++)
	{
		pixels.setPixelColor(i, color);
	}
	if(show)
	{
		pixels.show();
	}
}

/**
 * Returns flag; true=new command is available
 */
boolean isCommandAvailable()
{
	return gCommand;
}

/**
 *
 */
void setCommandAvailable(boolean cmd)
{
	gCommand = cmd;
}

/**
 * Delay function with command check. Delays for the specified amount of time, but checks for incoming
 * commands while delaying. If command is received, breaks the loop.
 *
 * @time - duration to wait
 * @return - true - command received; false - no command
 */
boolean commandWait(uint32_t time)
{
	boolean cmd = isCommandAvailable();
	if( time == 0 ) return cmd;

	if( !cmd ) // if no command, pause
	{
		for (uint32_t i = 0; i < time; i++)
		{
			delay(1); // delay
			worker(); // yield to os and wifi
			cmd = isCommandAvailable();
			if (cmd)
			{
				break;
			}
		}
	}
	return cmd;
}

/**
 * Completes required background tasks for ESP
 */
void worker()
{
	//wifiw.work(); // check for OTA
	yield(); // give time to ESP
	ESP.wdtFeed(); // pump watch dog
	return;
}


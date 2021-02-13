// Do not remove the include below
#include "WifiSign.h"

// Internal Variables
static Configuration config;
static PubSubWrapper pubsubw;
static WifiWrapper wifiw;
static MatrixWrapper matrix;

// Indicates we have a command waiting for processing
static volatile boolean commandAvailable = false;

volatile uint8_t wifiImageCount = 0;
volatile uint8_t mqttImageCount = 0;

boolean initialized = false;
boolean showWifiInit = false;
boolean showMqttInit = false;

// Internal functions
void parseCommand();
boolean initialize();
void startupPause();

/**
 * Setup function
 *
 */
void setup()
{
	// Configure serial port
	Serial.begin(115200);

	// Basic HW setup
	pinMode(LED_BUILTIN, OUTPUT);     // Initialize the  pin as an output

	// This pauses the start process so the user can do things like
	// attach to the serial port and look at debug information :)
	startupPause();

	LOG_INFO(F("WiFI Sign Starting Up"));

	showWifiInit = true;
	showMqttInit = true;

	// Initialize the configuration object; configs stored in Flash
	LOG_DEBUG( F("Initializing configuration...") );
    if( config.initialize(true) == ConfigStatusCode::OK)
    {
    	LOG_DEBUG( F("Configuration initialized:") );
    	LOG_DEBUG( config.toString());
    }
    else
    {
    	runConfigurationPortal();
    }

	// Initialize wifi, pubsub, and LEDs
	if( !initialize() )
	{
		runConfigurationPortal();
		LOG_INFO("Successfully saved configuration; must reboot")
		Helper::error(); // never returns
	}

	initialized = true;
	showWifiInit = false;
	showMqttInit = false;

    Serial.println(F("** Initialization Complete **"));

}

/**
 * Loop function
 *
 */
void loop()
{
	// calls yield and other must run code
	worker();

	if(wifiw.connected() == false )
	{
		LOG_ERROR("Lost WIFI connection!");
		wifiw.initializeWifi(); // TODO - ensure this doesn't screw things up
	}
	if( pubsubw.connected() == false )
	{
		LOG_ERROR("Lost MQTT connection!");
		matrix.clear();
		matrix.print(0, 8, 0x00ff0000, F("Lost"), false);
		matrix.print(0, 17, 0x00ff0000, F("Queue"), false);
		matrix.print(0, 26, 0x00ff0000, F("Connection"), true);
		delay(5000);
		matrix.clear();
		showMqttInit = true;
		if( pubsubw.connect() )
		{
			delay(1000);
			matrix.clear();
			matrix.print(0, 8, 0x0000ff, F("Queue"), false);
			matrix.print(0, 17, 0x0000ff, F("Connected"), true);
			delay(5000);
			matrix.clear();
		}
		else
		{
			matrix.clear();
			matrix.print(0, 8, 0x0000ff, F("Queue"), false);
			matrix.print(0, 17, 0x000ff, F("Connection"), false);
			matrix.print(0, 26, 0x0000ff, F("FAILED"), true);
		}
	}

	// Check if command is available
	if( isCommandAvailable() )
	{
		parseCommand();
	}

} // end loop

/**
 * Initializes the node.  Returns true if node configured properly, false otherwise.
 *
 */
boolean initialize()
{
	boolean configured = false;

	wifiw.setRetryCount(20);
	wifiw.setRetryDelay(250);
	wifiw.setConnectionStatusCallback(wifiConnectionStatusCallback);

	pubsubw.setRetryCount(20);
	pubsubw.setRetryDelay(250);
	pubsubw.setConnectionStatusCallback(mqttConnectionStatusCallback);

	Serial.println(F("Configuring matrix..."));
	if( matrix.initialize() )
	{
		Serial.println(F("Configured matrix."));
		matrix.test();

		// Set up screen
		matrix.drawImage((char *)F("/resources/wifi-grey-0.bmp"), 0, 0, true);
		matrix.print(2, matrix.getHeight()-1, 0x00FFFFFF, F("WIFI"), true);
		matrix.drawImage((char *)F("/resources/wifi-grey-0.bmp"), 32, 0, true);
		matrix.print(33, matrix.getHeight()-1, 0x00FFFFFF, F("MQTT"), true);

		// Initialize WIFI
		Serial.println(F("Configuring wifi..."));
		if( wifiw.initializeWifi(&config) == WifiStatusCode::CONNECTED)
		{
			wifiImageCount = 0;
			matrix.drawImage((char *)F("/resources/wifi-green-4.bmp"), 0, 0, true);

			// Initialize pubsub
			Serial.println(F("Configuring queue..."));
			if( pubsubw.initialize(&config, &wifiw) )
			{
				matrix.drawImage((char *)F("/resources/wifi-purple-4.bmp"), 32, 0, true);

				char ip[16];
				memset(ip, 0, sizeof(char)*16);
				Helper::toString(ip,  wifiw.getIpAddress());

				sprintf((char *)pubsubw.getBuffer(), CMD_IP_ADDRESS, ip, config.getMqttServer(), config.getMqttPort() );
				Serial.println((char *)pubsubw.getBuffer());
				setCommandAvailable(true);

				configured = true;

			}
			else
			{
				LOG_ERROR(F("Failed to configure queue"));
				matrix.drawImage((char *)F("/resources/wifi-red-4.bmp"), 32, 0, true);
			}
		}
		else
		{
			LOG_ERROR(F("Failed to configure WIFI"));
			matrix.drawImage((char *)F("/resources/wifi-red-4.bmp"), 0, 0, true);
		}
	}
	else
	{
		LOG_ERROR(F("Failed to configure matrix"));
	}

	return configured;

} // end initialize


void runConfigurationPortal()
{
    PortalWebServer pws;
    PortalStatusCode status = pws.runCaptivePortal();
    matrix.clear();
    if( status == PortalStatusCode::CONFIG_SUCCESS )
    {
    	// Copy our new configuration to existing configuration
    	pws.setConfiguration(config);

		// Save new config file.
		ConfigStatusCode c = config.write();
		if( c == ConfigStatusCode::OK )
		{
			LOG_INFO(F("Configuration file written."));
			matrix.print(0,  8,  0x0000ff00, F("Success!"), false);
			matrix.print(0,  17,  0x000000ff, F("Please"), true);
			matrix.print(0,  26,  0x000000ff, F("Reboot"), true);
		}
		else
		{
			matrix.print(0,  8,  0x00ff0000, F("Error"), false);
			matrix.print(0,  17,  0x00ff0000, F("Saving"), true);
			LOG_ERROR(F("Error writing config file"));
		}
    }
    else
    {
    	LOG_ERROR("Unable to capture configuration");
		matrix.print(0,  8,  0x00ff0000, F("Error"), false);
		matrix.print(0,  17,  0x00ff0000, F("Configuring"), true);
    }
	Helper::error(); // never returns

}

/**
 * calls others functions while we are not busy.
 * This is required since we disabled interrupts
 * to use the FastLED library.
 *
 */
void worker()
{
	pubsubw.work(); // process queue
	wifiw.work(); // check for OTA

	// check if we are connected - wifi first, then queue
	if( wifiw.connected() )
	{
		if( pubsubw.connected() )
		{
			//statusIndicator.setStatus(Queue, Ok);
		}
		else
		{
			setCommandAvailable(true); // stop on-going processing
			//statusIndicator.setStatus(Queue, Error);
			pubsubw.connect();
		}
	}
	else
	{
		setCommandAvailable(true); // stop on-going processing
		//statusIndicator.setStatus(Wifi, Error);
	}
	return;
}

/**
 * Parses command buffer
 *
 */
void parseCommand()
{
	Command cmd;

	// Reset command available flag
	setCommandAvailable(false);
	//setStatus(Processing);

#ifdef __DEBUG
	Serial.print( millis() );
	Serial.print(F(" - parsing command..."));
#endif

	if( cmd.parse( (uint8_t *)pubsubw.getBuffer() ) )
	{
#ifdef __DEBUG
		cmd.dump();
#endif
		switch(cmd.getCommand())
		{
		case Clear:
			Serial.println(F("Command: Clear"));
			matrix.clear();
			break;
		case Show:
			Serial.println(F("Command: Show"));
			matrix.show();
			break;
		case SetPixel:
			Serial.println(F("Command: SetPixel"));
			matrix.drawPixel(cmd.getX(), cmd.getY(), cmd.getForegroundColor(), cmd.getShow());
			break;
		case Text:
			Serial.println(F("Command: Text"));
			matrix.print(cmd.getX(), cmd.getY(), cmd.getForegroundColor(), (char *)cmd.getText(), cmd.getShow());
			break;
		case Line:
			Serial.println(F("Command: Line"));
			matrix.drawLine(cmd.getX(), cmd.getY(), cmd.getWidth(), cmd.getForegroundColor(), cmd.getDirection(), cmd.getShow());
			break;
		case Rectangle:
			Serial.println(F("Command: Rectangle"));
			matrix.drawRectangle(cmd.getX(), cmd.getY(), cmd.getWidth(), cmd.getHeight(), cmd.getForegroundColor(), cmd.getShow());
			break;
		case Circle:
			Serial.println(F("Command: Circle"));
			matrix.drawCircle(cmd.getX(), cmd.getY(), cmd.getRadius(), cmd.getForegroundColor(), cmd.getShow());
			break;
		case RandomFill:
			Serial.println(F("Command: RandomFill"));
			break;
		case SetIntensity:
			Serial.println(F("Command: SetIntensity"));
			break;
		case Scroll:
			Serial.println(F("Command: Scroll"));
			matrix.scrollText(cmd.getX(),  cmd.getY(),  cmd.getForegroundColor(),  cmd.getBackgroundColor(), cmd.getSpeed(), cmd.getRepeat(), cmd.getDirection(), cmd.getText() );
			break;
		case SetImage:
			Serial.println(F("Command: SetImage"));
			matrix.drawImage((ImageEnum)cmd.getIndex(), cmd.getShow());
			break;
		case Animate:
			Serial.println(F("Command: Animate"));
			matrix.animate( (AnimateEnum)cmd.getIndex(), cmd.getOnDuration());
			break;
		case Response:
			Serial.println(F("Command: Response"));
			break;
		default:
			Serial.println(F("ERROR - UNKNOWN COMMAND"));
			break;
		} // end switch

	} // end if cmd parse = true
	else
	{
		Serial.println(F("ERROR - Unable to parse command"));
	}


	Serial.print( millis() );
	Serial.println(F(" - Command Complete"));

	//setStatus(Waiting);
}

/**
 * Returns flag; true=new command is available
 */
boolean isCommandAvailable()
{
	return commandAvailable;
}

/**
 * Sets command available flag
 *
 */
void setCommandAvailable(boolean flag)
{
	commandAvailable = flag;
}


/**
 * Delay function with command check
 */
boolean commandDelay(uint32_t time)
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
 * Flashes LED during startup to tell user we're alive
 */
void startupPause()
{
	uint8_t i;

	Helper::setLed(OFF);
	for(i=0; i< 20; i++)
	{
		Helper::toggleLed();
		delay(250);
	}
	Helper::setLed(OFF);

}

/**
 * C function that calls class-level callback
 * Admittedly a hack, but it works
 *
 */
void pubsubCallback(char* topic, byte* payload, unsigned int length)
{
	pubsubw.callback(topic, payload, length);

}

void wifiConnectionStatusCallback()
{
	if( showWifiInit )
	{
		LOG_DEBUG1(F("WIFI Connection Status Callback: "), wifiImageCount );
		switch(wifiImageCount)
		{
			case 0:
				// Set image to 1
				matrix.drawImage((char *)"/resources/wifi-green-1.bmp", 0, 0, true);

				// Set next image to 2
				wifiImageCount=1;
				break;
			case 1:
				// Set image to 2
				matrix.drawImage((char *)"/resources/wifi-green-2.bmp",  0, 0, true);

				// Set next image to 3
				wifiImageCount=2;
				break;
			case 2:
				// Set image to 3
				matrix.drawImage((char *)"/resources/wifi-green-3.bmp",  0, 0, true);

				// Set next image to 2
				wifiImageCount=3;
				break;
			case 3:
				// Set image to full
				matrix.drawImage((char *)"/resources/wifi-green-4.bmp",  0, 0, true);

				// set next image to 1
				wifiImageCount=0;
				break;
			default:
				break;
		}
	}
	else
	{
		LOG_WARN("Initialization complete, but WIFI init called");
	}
}

void mqttConnectionStatusCallback()
{
	if( showMqttInit )
	{
		LOG_DEBUG1(F("WIFI Connection Status Callback: "), wifiImageCount );
		switch(wifiImageCount)
		{
			case 0:
				// Set image to 1
				matrix.drawImage((char *)"/resources/wifi-purple-1.bmp", 32, 0, true);

				// Set next image to 2
				wifiImageCount=1;
				break;
			case 1:
				// Set image to 2
				matrix.drawImage((char *)"/resources/wifi-purple-2.bmp",  32, 0, true);

				// Set next image to 3
				wifiImageCount=2;
				break;
			case 2:
				// Set image to 3
				matrix.drawImage((char *)"/resources/wifi-purple-3.bmp",  32, 0, true);

				// Set next image to 2
				wifiImageCount=3;
				break;
			case 3:
				// Set image to full
				matrix.drawImage((char *)"/resources/wifi-purple-4.bmp",  32, 0, true);

				// set next image to 1
				wifiImageCount=0;
				break;
			default:
				break;
		}
	}
}

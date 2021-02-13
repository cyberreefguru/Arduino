/*
 * PortalWebServer.cpp
 *
 *  Created on: Feb 12, 2021
 *      Author: tsasala
 */

#include "PortalWebServer.h"

WiFiServer 	wifiServer(80);

/**
 * Default constructor
 *
 */
PortalWebServer::PortalWebServer()
{
	currentMethod = HttpMethod::GET;
	currentUri = "";
	wifiStatus = WL_IDLE_STATUS;
	myConfig.initialize(false);
}

/**
 * Default destruction
 *
 */
PortalWebServer::~PortalWebServer()
{
}


void PortalWebServer::setConfiguration(Configuration &config)
{
	if( currentParameterSize > 0)
	{
		LOG_DEBUG("Setting parameters");
		config.setVersion( DEFAULT_VERSION );//***
		config.setNodeId( myConfig.getNodeId() );//***
		config.setHostName(  (uint8_t *)myConfig.getHostName() );
		config.setAPName(  (uint8_t *)myConfig.getAPName() ); //***
		config.setWifiSsid(  (uint8_t *)myConfig.getWifiSsid() );
		config.setWifiPassword(  (uint8_t *)myConfig.getWifiPassword() );
		config.setWebPort(  myConfig.getWebPort() ); //***
		config.setWifiTries(  myConfig.getWifiTries() ); //***
		config.setMqttServer(  (uint8_t *)myConfig.getMqttServer() );
		config.setMqttUser(  (uint8_t *)myConfig.getMqttUser() );
		config.setMqttPassword(  (uint8_t *)myConfig.getMqttPassword() );
		config.setMqttPort(  myConfig.getMqttPort() );
		config.setMqttTries(  myConfig.getMqttTries() ); //***
		config.setMyChannel(  (uint8_t *)myConfig.getMyChannel() );
		config.setRegistrationChannel(  (uint8_t *)myConfig.getRegistrationChannel() );
		config.setMyResponseChannel(  (uint8_t *)myConfig.getMyResponseChannel() );
		config.setAllChannel(  (uint8_t *)myConfig.getAllChannel() );
	}
	else
	{
		LOG_ERROR("No parameters to set");
	}
}


PortalStatusCode PortalWebServer::setupPortal()
{
	PortalStatusCode status = PortalStatusCode::FAIL;
	return status;
}


PortalStatusCode PortalWebServer::handlePortalClient()
{
	PortalStatusCode status = PortalStatusCode::FAIL;
	return status;
}


PortalStatusCode PortalWebServer::handlePortalRequest()
{
	PortalStatusCode status = PortalStatusCode::FAIL;
	return status;
}



PortalStatusCode PortalWebServer::setupCaptivePortal()
{
	PortalStatusCode status = PortalStatusCode::FAIL;

	//Initialize serial and wait for port to open:
	LOG_INFO(F("Access Point Web Server with CaptivePortal"));

	// check for the WiFi module:
	if (WiFi.status() == WL_NO_MODULE)
	{
		LOG_ERROR(F("Communication with WiFi module failed!"));
		Helper::error();
	}

	String fv = WiFi.firmwareVersion();
	if (fv < "1.0.0")
	{
		LOG_ERROR(F("Please upgrade the firmware"));
		Helper::error();
	}

	// by default the local IP address of will be 192.168.4.1
	LOG_INFO1(F("Creating access point named: "), DEFAULT_AP_NAME );

	WiFi.disconnect();

	wifiStatus = WiFi.beginAP(DEFAULT_AP_NAME, DEFAULT_AP_CHANNEL); // setup AP
	if (wifiStatus != WL_AP_LISTENING)
	{
		// If we couldn't connect to a WIFI network, and we can't create an AP
		// we have no options - so we lock up
		LOG_ERROR(F("Creating access point failed"));
		status = PortalStatusCode::AP_CREATE_FAILURE;
		return status;
	}
	else
	{
		LOG_DEBUG("Access Point Listening!");
	}

	// Set up DNS server
	dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
	boolean b = dnsServer.start(DEFAULT_DNS_PORT, F("*"), WiFi.localIP());
	if(b)
	{
		LOG_DEBUG(F("DNS server started"));
		wifiServer.begin();

		// you're connected now, so print out the status
		WifiWrapper::printWiFiStatus();

		status = PortalStatusCode::SUCCESS;

		LOG_INFO("DNS and Server started.");

	}
	else
	{
		LOG_ERROR("DNS server not started");
		status = PortalStatusCode::DNS_ERROR;
	}

	return status;

}

/**
 * Sets up and runs captive portal until configuration files is
 * successfully saved.
 *
 */
PortalStatusCode PortalWebServer::runCaptivePortal()
{
	PortalStatusCode status = PortalStatusCode::FAIL;

	LOG_INFO("Setting up captive portal.");
	status = setupCaptivePortal();
	if( status == PortalStatusCode::SUCCESS )
	{
		LOG_INFO("Waiting for client connections.");
		while(1)
		{
			status = handleCaptiveClient();
			if(status == PortalStatusCode::CONFIG_SUCCESS )
			{
				currentClient.stop();
				WiFi.disconnect();
				break;
			}
			else if(status == PortalStatusCode::CONFIG_FAIL)
			{
				LOG_ERROR("Error processing client request");
			}
		}
	}
	else
	{
		LOG_ERROR("Unable to setup captive portal!");
	}

	return status;
}

PortalStatusCode PortalWebServer::handleCaptiveClient()
{
	PortalStatusCode status = PortalStatusCode::FAIL;

	// compare the previous status to the current status
	if (wifiStatus != WiFi.status())
	{
		// it has changed update the variable
		wifiStatus = WiFi.status();

		if (wifiStatus == WL_AP_CONNECTED)
		{
			LOG_INFO(F("Device connected to AP"));
		}
		else
		{
			// a device has disconnected from the AP, and we are back in listening mode
			LOG_INFO(F("Device disconnected from AP"));
		}
	}

	WiFiClient client = wifiServer.available();
	if (client)
	{
		LOG_INFO(F("New Client"));
		currentClient = client;

		while( currentClient.connected() )
		{
			if (currentClient.available())
			{
				boolean parse = parseRequest();
				if (parse == true)
				{

					status = handleCaptiveRequest();

				} // end if parse
				else
				{
					LOG_ERROR(F("Error Parsing Request"));
					status = PortalStatusCode::PARSE_ERROR;
					break;
				}

			} // end if available

		} // end if connected

		// TODO - is this an error condition?
		LOG_INFO("Client disconnected.");

		// Client not connected
		currentClient.stop();
	}
	else
	{
		// Client connected, but no client available
		status = PortalStatusCode::AP_NO_CLIENT;
	}

	return status;
}

/**
 * Handles all requests while in captive portal mode.  Sends
 * request based on URI to correct handler
 *
 */
PortalStatusCode PortalWebServer::handleCaptiveRequest()
{
	LOG_DEBUG("handleCaptiveRequest");

	PortalStatusCode status = PortalStatusCode::FAIL;

	if(currentUri == "/" || currentUri == "/hotspot-detect.html")
	{
		LOG_INFO2("Handle Root - '", currentUri, "'");
		status = handleCaptiveRoot();
	}
	else if(currentUri == "/save_config")
	{
		LOG_INFO2("Handle Config Save - '", currentUri, "'");
		status = handleConfigSave();
	}
	else
	{
		LOG_ERROR1("Unhandled request", currentUri);
		status = PortalStatusCode::UNDEFINED_URL;
		memset(wifiBuffer,0,MAX_WIFI_BUFFER_SIZE);
		snprintf( wifiBuffer, MAX_WIFI_BUFFER_SIZE, "Unhandled Request: %s", currentUri.c_str());
		status = send(404);
	}

	return status;
}

PortalStatusCode PortalWebServer::handleCaptiveRoot()
{
	LOG_DEBUG("handleCaptiveRoot");

	PortalStatusCode status = PortalStatusCode::FAIL;
	int bodySize = strlen(root_body);
	int maxBodySize = bodySize + (32*11)+6;

	if( maxBodySize < MAX_WIFI_BUFFER_SIZE )
	{
		// Clear entire buffer
		memset(wifiBuffer,0,MAX_WIFI_BUFFER_SIZE);
//		myConfig.dump();
		// Build body with real values for place holders
		bodySize = snprintf(wifiBuffer, maxBodySize, root_body,
				myConfig.getNodeId(),
				myConfig.getHostName(),
				myConfig.getAPName(),
				myConfig.getWifiSsid(),
				myConfig.getWifiPassword(),
				myConfig.getWifiTries(),
				myConfig.getMqttServer(),
				myConfig.getMqttPort(),
				myConfig.getMqttUser(),
				myConfig.getMqttPassword(),
				myConfig.getMqttTries(),
				myConfig.getMyChannel(),
				myConfig.getRegistrationChannel(),
				myConfig.getMyResponseChannel(),
				myConfig.getAllChannel());

		if( bodySize > 0 && bodySize < MAX_WIFI_BUFFER_SIZE )
		{
			status = sendResponse(wifiBuffer);
		}
		else
		{
			LOG_ERROR("ERROR - could not create body content");
			status = PortalStatusCode::ERROR_BUILDING_RESPONSE;

		} // end if sprintf size > 0
	}
	else
	{
		LOG_ERROR1("Max Body Size > Max Buffer Size: ", maxBodySize);
		status = PortalStatusCode::OUT_OF_MEMORY;

	} // end if contentSize < buff size

	return status;
}

PortalStatusCode PortalWebServer::handleConfigSave()
{
	LOG_DEBUG("handleConfigSave");

	PortalStatusCode status = PortalStatusCode::FAIL;
	const char* response = config_failed;

	// Pull parameters
	LOG_DEBUG1(F("Parameter Size: "), currentParameterSize);
	if( currentParameterSize > 0 )
	{
		for(uint8_t i=0; i<currentParameterSize; i++)
		{
			LOG_DEBUG4(F("Parameter '"), currentParameters[i].key, F("'='"), currentParameters[i].value, F("'"));

			if(currentParameters[i].key == "nid")
			{
				myConfig.setNodeId( currentParameters[i].value.toInt() );
			}
			else if(currentParameters[i].key == "hn")
			{
				myConfig.setHostName( (uint8_t *)currentParameters[i].value.c_str() );
			}
			else if(currentParameters[i].key == "apn")
			{
				myConfig.setAPName( (uint8_t *)currentParameters[i].value.c_str() );
			}
			else if (currentParameters[i].key == "wssid")
			{
				myConfig.setWifiSsid((uint8_t *)currentParameters[i].value.c_str());
			}
			else if (currentParameters[i].key == "wpass")
			{
				myConfig.setWifiPassword((uint8_t *)currentParameters[i].value.c_str());
			}
			else if (currentParameters[i].key == "wtry")
			{
				myConfig.setWifiTries(currentParameters[i].value.toInt());
			}
			else if (currentParameters[i].key == "mserv")
			{
				myConfig.setMqttServer((uint8_t *)currentParameters[i].value.c_str());
			}
			else if (currentParameters[i].key == "muser")
			{
				myConfig.setMqttUser((uint8_t *)currentParameters[i].value.c_str());
			}
			else if (currentParameters[i].key == "mpass")
			{
				myConfig.setMqttPassword((uint8_t *)currentParameters[i].value.c_str());
			}
			else if (currentParameters[i].key == "mport")
			{
				myConfig.setMqttPort(currentParameters[i].value.toInt());
			}
			else if (currentParameters[i].key == "mtry")
			{
				myConfig.setMqttTries(currentParameters[i].value.toInt());
			}
			else if (currentParameters[i].key == "mch")
			{
				myConfig.setMyChannel((uint8_t *)currentParameters[i].value.c_str());
			}
			else if (currentParameters[i].key == "regch")
			{
				myConfig.setRegistrationChannel((uint8_t *)currentParameters[i].value.c_str());
			}
			else if (currentParameters[i].key == "respch")
			{
				myConfig.setMyResponseChannel((uint8_t *)currentParameters[i].value.c_str());
			}
			else if (currentParameters[i].key == "ach")
			{
				myConfig.setAllChannel((uint8_t *)currentParameters[i].value.c_str());
			}
			else
			{
				LOG_ERROR1(F("Unknown Key: "), currentParameters[i].key)
			}

		} // end for parameters

		// TODO: put some form validation here
		// ports are numbers
		// hostname is all lower case
		// strings are < 32 in length
		// node id <= 255

		// Set status code
		status = PortalStatusCode::CONFIG_SUCCESS;
		response = config_success;

	} // end if param count > 0

	if( sendResponse((char *)response) == PortalStatusCode::SUCCESS)
	{
		LOG_INFO("Successfully captured new configuration parameters");
	}
	else
	{
		LOG_ERROR("Error sending POST response!");
	}

	return status;
}




boolean PortalWebServer::parseRequest()
{
	LOG_DEBUG("parseRequest");
	boolean status = false;

	currentParameterSize = 0;

	// Read first line of HTTP request -- this tells us the type of request
	String req = currentClient.readStringUntil('\r');
	currentClient.readStringUntil('\n');

	LOG_INFO1(F("Client Request - "),req );

	// First line should look like "GET /path HTTP/1.1"
	// Retrieve the "/path" part by finding the spaces
	int addr_start = req.indexOf(' ');
	int addr_end = req.indexOf(' ', addr_start + 1);
	if (addr_start == -1 || addr_end == -1)
	{
		LOG_ERROR("parseRequest: malformed request");
		return false;
	}

	String methodStr = req.substring(0, addr_start);
	String url = req.substring(addr_start + 1, addr_end);

	String searchStr = "";
	int hasSearch = url.indexOf('?');
	if (hasSearch != -1)
	{
		searchStr = urlDecode(url.substring(hasSearch + 1));
		currentUri = url.substring(0, hasSearch);
	}
	else
	{
		currentUri = url;
	}

	currentMethod = HttpMethod::GET;
	if (methodStr == "HEAD")
	{
		currentMethod = HttpMethod::HEAD;
	}
	else if (methodStr == "POST")
	{
		currentMethod = HttpMethod::POST;
	}
	else if (methodStr == "DELETE")
	{
		currentMethod = HttpMethod::DELETE;
	}
	else if (methodStr == "OPTIONS")
	{
		currentMethod = HttpMethod::OPTIONS;
	}
	else if (methodStr == "PUT")
	{
		currentMethod = HttpMethod::PUT;
	}
	else if (methodStr == "PATCH")
	{
		currentMethod = HttpMethod::PATCH;
	}

	LOG_DEBUG4(F("Method: "), methodStr, F(" URI: '"), currentUri, F("'") );

	// below is needed only when POST type request
	if (currentMethod == HttpMethod::POST || currentMethod == HttpMethod::PUT || currentMethod == HttpMethod::PATCH || currentMethod == HttpMethod::DELETE)
	{
		LOG_DEBUG("Processing HTTP POST");

		String boundaryStr;
		String headerName;
		String headerValue;

		boolean isForm = false;
		boolean isEncoded = false;
		uint32_t contentLength = 0;

		//parse headers
		while (1)
		{
			req = currentClient.readStringUntil('\r');
			currentClient.readStringUntil('\n');
			if (req == "")
			{
				break;	//no more headers
			}

			int headerDiv = req.indexOf(':');
			if (headerDiv == -1)
			{
				LOG_ERROR(F("Header missing content demarc of ':'"));
				break;
			}

			headerName = req.substring(0, headerDiv);
			headerValue = req.substring(headerDiv + 1);
			headerValue.trim();

			// TODO: do we want to collect the header values for any reason??

			LOG_DEBUG4(F("Header: '"), headerName, F("' - '"), headerValue, F("'"));

			if (headerName.equalsIgnoreCase(F("Content-Type")))
			{
				using namespace mime;
				if (headerValue.startsWith(mimeTable[txt].mimeType))
				{
					isForm = false;
				}
				else if (headerValue.startsWith(F("application/x-www-form-urlencoded")))
				{
					isForm = false;
					isEncoded = true;
				}
				else if (headerValue.startsWith(F("multipart/")))
				{
					boundaryStr = headerValue.substring(headerValue.indexOf('=') + 1);
					boundaryStr.replace("\"", "");
					isForm = true;
				}
			}
			else if (headerName.equalsIgnoreCase(F("Content-Length")))
			{
				contentLength = headerValue.toInt();
			}


		} // end while gathering headers

		LOG_DEBUG3(F("Encoded: "), isEncoded, F(" Form: "), isForm  );
		if( isEncoded )
		{
			req = currentClient.readStringUntil('\r');
			currentClient.readStringUntil('\n');
			LOG_DEBUG1("Params: ", req);
			if (req != "")
			{
				if( currentParameterSize == 0 )
				{
					int index = 0;
					int ampIndex = 0;
					int equIndex = 0;
					boolean done=false;
					while(done==false)
					{
						equIndex = req.indexOf('=', index);
						if( equIndex != -1 )
						{
							ampIndex = req.indexOf('&', equIndex);
							if( ampIndex == -1 )
							{
								currentParameters[currentParameterSize].key = urlDecode(req.substring(index, equIndex));
								currentParameters[currentParameterSize].value = urlDecode(req.substring(equIndex+1, req.length()));
								done=true;
							}
							else
							{
								currentParameters[currentParameterSize].key = urlDecode(req.substring(index, equIndex));
								currentParameters[currentParameterSize].value = urlDecode(req.substring(equIndex+1, ampIndex));
								index=ampIndex+1;
							}
							//LOG_DEBUG3(F("Param: "), currentParameters[currentParameterSize].key, F(":"), currentParameters[currentParameterSize].value);
							currentParameterSize++;
						}
						else
						{
							done=true;
						}

					}
				}
				else
				{
					LOG_ERROR1(F("Current Parameter Size > 0: "), currentParameterSize);
				}

			}
			else
			{
				LOG_ERROR("Form parameters are missing.");
			}
		}

		if (isForm)
		{
			// Parse URL arguments here
			//parseArguments (searchStr);

			// Parse form elements here
			if (!parseForm(boundaryStr, contentLength))
			{
				return false;
			}
		}
	}
	else
	{
		// GET
		String headerName;
		String headerValue;

		//parse headers
		while (1)
		{
			req = currentClient.readStringUntil('\r');
			currentClient.readStringUntil('\n');
			if (req == "")
			{
				break;	//no more headers
			}

			int headerDiv = req.indexOf(':');
			if (headerDiv == -1)
			{
				break;
			}

			headerName = req.substring(0, headerDiv);
			headerValue = req.substring(headerDiv + 2);

			//_collectHeader(headerName.c_str(), headerValue.c_str());
			LOG_DEBUG4(F("Header: '"), headerName, F("' - '"), headerValue, F("'"));
		}
		// Parse URL arguments here
	}

	currentClient.flush();

	LOG_DEBUG1(F("Request: "), currentUri);
	//LOG_DEBUG1(F("Arguments: "), searchStr);

	status = true;

	return status;

}


/**
 * This method parses an HTML form post where post attributes are
 * separated by boundaries
 */
bool PortalWebServer::parseForm(String boundary, uint32_t len)
{

	LOG_DEBUG1(F("Boundary: "), boundary);
	LOG_DEBUG1(F("Length: "), len);

	String line;
	int retry = 0;

	// Read blank lines
	do
	{
		line = currentClient.readStringUntil('\r');
		++retry;
	} while (line.length() == 0 && retry < 3);

	currentClient.readStringUntil('\n');

	//start reading the form
	if (line == ("--" + boundary))
	{
		while (1)
		{
			String argName;
			String argValue;
			String argType;
			String argFilename;

			bool argIsFile = false;

			line = currentClient.readStringUntil('\r');
			currentClient.readStringUntil('\n');

			if (line.startsWith(F("Content-Disposition")))
			{
				int nameStart = line.indexOf('=');
				if (nameStart != -1)
				{
					// First '=' is the first arg on the line
					// Second '=' is the file name if it exists
					argName = line.substring(nameStart + 2);
					nameStart = argName.indexOf('=');
					if (nameStart == -1)
					{
						// No file name
						argName = argName.substring(0, argName.length() - 1);
					}
					else
					{
						argFilename = argName.substring(nameStart + 2, argName.length() - 1);
						argName = argName.substring(0, argName.indexOf('"'));
						argIsFile = true;

						LOG_DEBUG1(F("PostArg FileName: "), argFilename);

						// we don't need to process files, so skip all that
					}
					LOG_DEBUG1(F("PostArg Name: "), argName);

					argType = F("text/plain");
					line = currentClient.readStringUntil('\r');
					currentClient.readStringUntil('\n');

					if (line.startsWith("Content-Type"))
					{
						argType = line.substring(line.indexOf(':') + 2);
						//skip next line
						currentClient.readStringUntil('\r');
						currentClient.readStringUntil('\n');
					}

					LOG_DEBUG1(F("PostArg Type: "), argType);

					if (!argIsFile)
					{
						while (1)
						{
							line = currentClient.readStringUntil('\r');
							currentClient.readStringUntil('\n');

							if (line.startsWith("--" + boundary))
								break;

							if (argValue.length() > 0)
								argValue += "\n";

							argValue += line;
						}

						LOG_DEBUG1(F("PostArg Value: "), argValue);

						// Save current parameters in args array
						currentParameters[currentParameterSize].key = argName;
						currentParameters[currentParameterSize++].value = argValue;

						// Check if we are done with parameters
						if (line == ("--" + boundary + "--"))
						{
							LOG_DEBUG(F("Done Parsing Parameters"));
							break;
						}
					} // end not a while

				}// end startPos ! -1
				else
				{
					LOG_ERROR(F("Boundary Marker missing Content-Disposition"));
				}
			} // end starts with content-disposition

		} // end while

		return true;
	}

	LOG_DEBUG1(F("Error: line: "), line);

	return false;
}


String PortalWebServer::urlDecode(const String& text)
{
  String decoded    = "";
  char temp[]       = "0x00";
  unsigned int len  = text.length();
  unsigned int i    = 0;

  while (i < len)
  {
    char decodedChar;
    char encodedChar = text.charAt(i++);

    if ((encodedChar == '%') && (i + 1 < len))
    {
      temp[2] = text.charAt(i++);
      temp[3] = text.charAt(i++);

      decodedChar = strtol(temp, NULL, 16);
    }
    else
    {
      if (encodedChar == '+')
      {
        decodedChar = ' ';
      }
      else
      {
        decodedChar = encodedChar;  // normal ascii char
      }
    }

    decoded += decodedChar;
  }

  return decoded;
}

PortalStatusCode PortalWebServer::sendResponse(char *buf)
{
	PortalStatusCode status = PortalStatusCode::FAIL;

	int bodySize = strlen(buf);
	int headSize = strlen(head);
	int contentSize = headSize + bodySize;
	int sent = 0;

	LOG_INFO3(F("Body Size: "), bodySize, F(" Content Size: "), contentSize );

	currentClient.print(F("HTTP/1.1 200 OK\r\n"));
	currentClient.print(F("Content-type: text/html\r\n"));
	currentClient.print(F("Connection: closed\r\n"));
	currentClient.print(F("Content-Length: "));
	currentClient.println( contentSize );
	currentClient.print("\r\n"); // break for content

	sent  = currentClient.print(head);
	if(sent == headSize )
	{
		// NOTE: WIFI board was not sending all the bytes
		//       when buffer > 3893, so I send one at a time
		//       which fixed the problem.
		sent = sendBuffer(buf, bodySize);
//		int bytes = 0;
//		for(int i=0; i<bodySize; i++)
//		{
//			sent = currentClient.write(buf[i]);
//			if( sent == 1 )
//			{
//				bytes++;
//			}
//			else
//			{
//				LOG_ERROR1(F("Error Sending Byte: "), i);
//			}
//		}
//		sent = bytes;
		if(sent == bodySize)
		{
			status = PortalStatusCode::SUCCESS;
			LOG_INFO1(F("Send Response - "), contentSize);
		}
		else
		{
			LOG_ERROR3(F("Error sending body - sent: "), sent, F(" size: "), bodySize);
			LOG_ERROR1(F("WiFI Write Code: "), currentClient.getWriteError() );
			status = PortalStatusCode::ERROR_SENDING_RESPONSE;
		}
	}
	else
	{
		LOG_ERROR3(F("Error sending head - sent: "), sent, F(" size: "), headSize);
		status = PortalStatusCode::ERROR_SENDING_RESPONSE;
	}

	// The HTTP response ends with another blank line:
	currentClient.print(F("\r\n")); // break = end of content

	return status;

}


PortalStatusCode PortalWebServer::send(int statusCode)
{
	PortalStatusCode status = PortalStatusCode::FAIL;

	int sent = 0;
	int len = strlen(wifiBuffer);

	LOG_DEBUG3(F("Sending HTTP Response:"), statusCode, F(" Size :"), len);

	switch (statusCode)
	{
	case 200:
		currentClient.print(F("HTTP/1.1 200 OK\r\n"));
		break;
	case 404:
		currentClient.print(F("HTTP/1.1 404 Not found\r\n"));
		break;
	default:
		currentClient.print(F("HTTP/1.1 500 Internal server error\r\n"));
		break;
	}
	currentClient.print(F("Content-type: text/html\r\n"));
	currentClient.print(F("Connection: closed\r\n"));
	currentClient.print(F("Content-Length: "));
	currentClient.print( len );
	currentClient.print("\r\n"); // break for content

	// TODO: Do we need to send one byte at a time here??
	sent = currentClient.print(wifiBuffer);
	LOG_DEBUG1(F("Sent to Client: "), sent);
	if(sent == len )
	{
		status = PortalStatusCode::SUCCESS;
		LOG_INFO1(F("Sent Content: "), len);
	}
	else
	{
		status = PortalStatusCode::ERROR_SENDING_RESPONSE;
		LOG_ERROR1(F("Error sending response - size mismatch"), len);
	}

	// The HTTP response ends with another blank line:
	currentClient.print(F("\r\n")); // break = end of content

	return status;
}
/**
 * Sends buffer to web client one byte at a time
 */
int PortalWebServer::sendBuffer(char *buf, int size)
{
	int sent = 0;
	int bytes = 0;
	for(int i=0; i<size; i++)
	{
		sent = currentClient.write(buf[i]);
		if( sent == 1 )
		{
			bytes++;
		}
		else
		{
			LOG_ERROR1(F("Error Sending Byte: "), i);
			break;
		}
	}

	return bytes;

}


String PortalWebServer::toString(PortalStatusCode c)
{
	String s = "";
	switch(c)
	{
	case PortalStatusCode::SUCCESS:
		s = String(F("SUCCESS"));
		break;
	case PortalStatusCode::FAIL:
		s = String(F("FAIL"));
		break;
	case PortalStatusCode::DNS_ERROR:
		s = String(F("DNS_ERROR"));
		break;
	case PortalStatusCode::WIFI_ERROR:
		s = String(F("WIFI_ERROR"));
		break;
	case PortalStatusCode::AP_CREATE_FAILURE:
		s = String(F("AP_CREATE_FAILURE"));
		break;
	case PortalStatusCode::AP_ERROR:
		s = String(F("AP_ERROR"));
		break;
	case PortalStatusCode::AP_NO_CLIENT:
		s = String(F("AP_NO_CLIENT"));
		break;
	case PortalStatusCode::AP_NO_CLIENT_ERROR:
		s = String(F("AP_NO_CLIENT_ERROR"));
		break;
	case PortalStatusCode::PARSE_ERROR:
		s = String(F("PARSE_ERROR"));
		break;
	case PortalStatusCode::CONFIG_SUCCESS:
		s = String(F("CONFIG_SUCCESS"));
		break;
	case PortalStatusCode::CONFIG_FAIL:
		s = String(F("CONFIG_FAIL"));
		break;
	case PortalStatusCode::ERROR_BUILDING_RESPONSE:
		s = String(F("ERROR_BUILDING_RESPONSE"));
		break;
	case PortalStatusCode::ERROR_SENDING_RESPONSE:
		s = String(F("ERROR_SENDING_RESPONSE"));
		break;
	case PortalStatusCode::ERROR_BUILDING_REQUEST:
		s = String(F("ERROR_BUILDING_REQUEST"));
		break;
	case PortalStatusCode::ERROR_SENDING_REQUEST:
		s = String(F("ERROR_SENDING_REQUEST"));
		break;
	case PortalStatusCode::OUT_OF_MEMORY:
		s = String(F("OUT_OF_MEMORY"));
		break;
	case PortalStatusCode::BUFFER_OVERRUN:
		s = String(F("BUFFER_OVERRUN"));
		break;
	case PortalStatusCode::UNDEFINED_URL:
		s = String(F("UNDEFINED_URL"));
		break;
	default:
		break;
	}
	return s;
}


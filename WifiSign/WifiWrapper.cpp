/*
 * WifiWrapper.cpp
 *
 *  Created on: Feb 22, 2016
 *      Author: tsasala
 */

#include "WifiWrapper.h"

WifiWrapper::WifiWrapper()
{
	config = 0;
}

/**
 * Initializes WIFI
 *
 */
WifiStatusCode WifiWrapper::initializeWifi(Configuration *config)
{
	this->config = config;
	return initializeWifi();
}

/**
 * Initializes the WIFI module
 *
 * The method allows initialize to be called without access to the config object.
 *
 */
WifiStatusCode WifiWrapper::initializeWifi()
{
	WifiStatusCode status = WifiStatusCode::UNKNOWN;

	LOG_DEBUG(F("Initializing WIFI..."));
	if (config == NULL)
	{
		LOG_DEBUG(F("ERROR - configuration object not defined."));
		return WifiStatusCode::NO_CONFIG;
	}

	// If connected, reset connection
	uint8_t ws = WiFi.status();
	LOG_DEBUG1(F("Current Connection Status: "), toStringWifiCode(ws));
	if (ws == WL_CONNECTED || ws == WL_AP_CONNECTED )
	{
		LOG_DEBUG(F("Disconnecting current session..."));
		WiFi.disconnect();
	}

	// Connect to specified network
	LOG_DEBUG1(F("Connecting to "), (char*) config->getWifiSsid());

	// We start by connecting to a WiFi network
	WiFi.setRetryCount( config->getWifiTries() );

	// NOTE - I changed the routine to implement a callback so it doens't block if callback is defined
	WiFi.begin((char*) config->getWifiSsid(), (char*) config->getWifiPassword());
	LOG_DEBUG1(F("Connection Status: "), toStringWifiCode(ws));

	//	WL_NO_SSID_AVAIL


	status = toWifiStatusCode(WiFi.status());
	if (status == WifiStatusCode::CONNECTED)
	{
		Serial.print(F(" Connected: "));
		LOG_DEBUG(WiFi.localIP());

		// pause after configuring WIFI
		//Helper::delayYield(250); // Give time to ESP

//		LOG_DEBUG(F("Setting up OTA..."));
//
//		sprintf(hostname, "lednode-%i", config->getNodeId() );
//
//		ArduinoOTA.setHostname(hostname);
//
//		ArduinoOTA.onStart([]() {
//			LOG_DEBUG("OTA Start");
//			setStatus(Uploading);
//		});
//
//		ArduinoOTA.onEnd([]() {
//			LOG_DEBUG("OTA End");
//			LOG_DEBUG("Rebooting...");
//		});
//
//		ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
//			Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
//			// TODO - put LED flash here or call back
//		});
//
//		ArduinoOTA.onError([](ota_error_t error) {
//			Serial.printf("Error[%u]: ", error);
//			if (error == OTA_AUTH_ERROR) LOG_DEBUG("Auth Failed");
//			else if (error == OTA_BEGIN_ERROR) LOG_DEBUG("Begin Failed");
//			else if (error == OTA_CONNECT_ERROR) LOG_DEBUG("Connect Failed");
//			else if (error == OTA_RECEIVE_ERROR) LOG_DEBUG("Receive Failed");
//			else if (error == OTA_END_ERROR) LOG_DEBUG("End Failed");
//		});
//		ArduinoOTA.begin();

	}
	else
	{
		LOG_ERROR1(F("Failed to connect: "), toStringWifiCode(WiFi.status()));
	}

	return status;

}

/**
 * Initializes WIFI AP
 *
 */
WifiStatusCode WifiWrapper::initializeAP(Configuration *config)
{
	WifiStatusCode status = WifiStatusCode::UNKNOWN;

	LOG_DEBUG(F("Initializing WIFI..."));
	if (config == NULL)
	{
		LOG_DEBUG(F("ERROR - configuration object not defined."));
		return WifiStatusCode::NO_CONFIG;
	}


	return status;
}

/**
 * Returns the WIFI Client
 *
 */
WiFiClient& WifiWrapper::getWifiClient()
{
	return wifi;
}

uint8_t WifiWrapper::connected()
{
	return (WiFi.status() == WL_CONNECTED);
}

void WifiWrapper::work()
{
	//ArduinoOTA.handle();
}

IPAddress WifiWrapper::getIpAddress()
{
	return WiFi.localIP();
}

void WifiWrapper::setRetryCount(uint8_t c)
{
	WiFi.setRetryCount(c);
}

uint8_t WifiWrapper::getRetryCount()
{
	return WiFi.getRetryCount();
}

void WifiWrapper::setRetryDelay(uint32_t c)
{
	WiFi.setRetryDelay(c);
}

uint32_t WifiWrapper::getRetryDelay()
{
	return WiFi.getRetryDelay();
}

/**
 * Sets the status update callback function pointer
 */
void WifiWrapper::setConnectionStatusCallback(void (*cb)())
{
	WiFi.setConnectionStatusCallback(cb);
}

/**
 * Sets the status update callback function pointer
 */
void WifiWrapper::setAccessPointStatusCallback(void (*cb)())
{
	WiFi.setAccessPointStatusCallback(cb);
}

/**
 * Sets the status update callback function pointer
 */
void WifiWrapper::setScanStatusCallback(void (*cb)())
{
	WiFi.setScanStatusCallback(cb);
}

WifiStatusCode WifiWrapper::toWifiStatusCode(uint8_t s)
{
	WifiStatusCode status = WifiStatusCode::UNKNOWN;
	switch (s)
	{
	case WL_IDLE_STATUS:
		status = WifiStatusCode::IDLE;
		break;
	case WL_CONNECTED:
		status = WifiStatusCode::CONNECTED;
		break;
	case WL_DISCONNECTED:
		status = WifiStatusCode::DISCONNECTED;
		break;
	case WL_SCAN_COMPLETED:
		status = WifiStatusCode::SCAN_COMPLETE;
		break;
	case WL_AP_LISTENING:
		status = WifiStatusCode::AP_LISTENING;
		break;
	case WL_AP_CONNECTED:
		status = WifiStatusCode::AP_CONNECTED;
		break;
	case WL_AP_FAILED:
		status = WifiStatusCode::AP_FAILED;
		break;
	case WL_CONNECTION_LOST:
		status = WifiStatusCode::CONNECTION_LOST;
		break;
	case WL_CONNECT_FAILED:
		status = WifiStatusCode::CONNECTION_FAILED;
		break;
	case WL_NO_SSID_AVAIL:
		status = WifiStatusCode::NO_SSID;
		break;
	case WL_NO_MODULE:
		status = WifiStatusCode::NO_MODULE;
		break;
	default:
		break;
	}
	return status;
}

void WifiWrapper::printWiFiStatus()
{
	LOG_INFO1(F("Nina W10 firmware: "), WiFi.firmwareVersion());

	// print the SSID of the network you're attached to:
	LOG_INFO1(F("SSID: "), WiFi.SSID());

	// print your WiFi shield's IP address:
	LOG_INFO1(F("IP Address  : "), WiFi.localIP());
	// print your WiFi shield's gateway:

	LOG_INFO1(F("IP Gateway  : "), WiFi.gatewayIP());

	// print your WiFi shield's gateway:
	LOG_INFO1(F("Subnet Mask : "), WiFi.subnetMask() );

	// print where to go in a browser:
	LOG_INFO1(F("To see this page in action, open a browser to http://"), WiFi.localIP() );
}

String WifiWrapper::toString(WifiStatusCode c)
{
	String s = "";
	switch (c)
	{
	case WifiStatusCode::IDLE:
		s = String(F("IDLE"));
		break;
	case WifiStatusCode::CONNECTED:
		s = String(F("CONNECTED"));
		break;
	case WifiStatusCode::DISCONNECTED:
		s = String(F("DISCONNECTED"));
		break;
	case WifiStatusCode::SCAN_COMPLETE:
		s = String(F("SCAN COMPLETE"));
		break;
	case WifiStatusCode::AP_LISTENING:
		s = String(F("AP LISTENING"));
		break;
	case WifiStatusCode::AP_CONNECTED:
		s = String(F("AP CONNECTED"));
		break;
	case WifiStatusCode::AP_FAILED:
		s = String(F("AP FAILED"));
		break;
	case WifiStatusCode::CONNECTION_LOST:
		s = String(F("CONNECTION LOST"));
		break;
	case WifiStatusCode::CONNECTION_FAILED:
		s = String(F("CONNECT FAILED"));
		break;
	case WifiStatusCode::NO_SSID:
		s = String(F("NO SSID"));
		break;
	case WifiStatusCode::NO_PASSWORD:
		s = String(F("NO PASSWORD"));
		break;
	case WifiStatusCode::NO_MODULE:
		s = String(F("NO MODULE"));
		break;
	case WifiStatusCode::OTA_AUTH_ERROR:
		s = String(F("OTA AUTHORIZATION ERROR"));
		break;
	case WifiStatusCode::OTA_CONNECTION_ERROR:
		s = String(F("OTA CONNECTION ERROR"));
		break;
	case WifiStatusCode::OTA_RECEIVE_ERROR:
		s = String(F("OTA RECEIVE ERROR"));
		break;
	case WifiStatusCode::OTA_END_ERROR:
		s = String(F("OTA END ERROR"));
		break;
	case WifiStatusCode::UNKNOWN:
		s = String(F("UNKNOWN"));
		break;
	default:
		break;
	}
	return s;
}


String WifiWrapper::toStringWifiCode(uint8_t c)
{
	String s = "";
	switch (c)
	{
	case WL_NO_MODULE:
		s = String(F("WL_NO_MODULE"));
		break;
	case WL_IDLE_STATUS:
		s = String(F("WL_IDLE_STATUS"));
		break;
	case WL_NO_SSID_AVAIL:
		s = String(F("WL_NO_SSID_AVAIL"));
		break;
	case WL_SCAN_COMPLETED:
		s = String(F("SCAN WL_SCAN_COMPLETED"));
		break;
	case WL_CONNECTED:
		s = String(F("WL_CONNECTED"));
		break;
	case WL_CONNECT_FAILED:
		s = String(F("WL_CONNECT_FAILED"));
		break;
	case WL_CONNECTION_LOST:
		s = String(F("WL_CONNECTION_LOST"));
		break;
	case WL_DISCONNECTED:
		s = String(F("WL_DISCONNECTED"));
		break;
	case WL_AP_LISTENING:
		s = String(F("WL_AP_LISTENING"));
		break;
	case WL_AP_CONNECTED:
		s = String(F("WL_AP_CONNECTED"));
		break;
	case WL_AP_FAILED:
		s = String(F("WL_AP_FAILED"));
		break;
	default:
		break;
	}
	return s;
}

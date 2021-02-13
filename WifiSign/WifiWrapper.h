/*
 * WifiWrapper.h
 *
 *  Created on: Feb 22, 2016
 *      Author: tsasala
 */

#pragma once

#include <Arduino.h>
#include <WiFiNINA.h>

#include "Debug.h"
#include "ClientGlobal.h"
#include "Configuration.h"
#include "Helper.h"

enum class WifiStatusCode {IDLE, CONNECTED, DISCONNECTED, SCAN_COMPLETE, AP_LISTENING, AP_CONNECTED, AP_FAILED, NO_MODULE, CONNECTION_FAILED, CONNECTION_LOST, NO_SSID, NO_PASSWORD, OTA_AUTH_ERROR,
	OTA_CONNECTION_ERROR, OTA_RECEIVE_ERROR, OTA_END_ERROR, NO_CONFIG, UNKNOWN };


// Define UDP settings for DNS
#define UDP_PACKET_SIZE 128           // MAX UDP packaet size = 512
#define DNSHEADER_SIZE 12             // DNS Header
#define DNSANSWER_SIZE 16             // DNS Answer = standard set with Packet Compression
#define DNSMAXREQUESTS 16             // trigger first DNS requests, to redirect to own web-page


class WifiWrapper
{
public:
	WifiWrapper();
	WifiStatusCode initializeWifi(Configuration* config);
	WifiStatusCode initializeWifi();
	WifiStatusCode initializeAP(Configuration* config);
	WifiStatusCode initiateConfigurationPort(Configuration* config);

	void waitForConfig();
	WiFiClient& getWifiClient();
	uint8_t connected();

	IPAddress getIpAddress();

    /**
     * Sets the number of times to try during a connection attempt
     */
    void setRetryCount(uint8_t c);

    /**
     * Gets the number of times to try during a connection attempt
     */
    uint8_t getRetryCount();

    /**
     * Sets the time duration to wait until checking for a retry
     */
    void setRetryDelay(uint32_t c);

    /**
     * Sets the time duration to wait until checking for a retry
     */
    uint32_t getRetryDelay();

    /**
     * Sets the status update callback function pointer
     */
	void setConnectionStatusCallback(void (*cb)());

    /**
     * Sets the status update callback function pointer
     */
	void setAccessPointStatusCallback(void (*cb)());

    /**
     * Sets the status update callback function pointer
     */
	void setScanStatusCallback(void (*cb)());

	/**
	 * Function that performs any necessary network work during "break" times
	 *
	 */
	void work();

	WifiStatusCode toWifiStatusCode(uint8_t s);

	String toString(WifiStatusCode c);
	String toStringWifiCode(uint8_t c);

	static void printWiFiStatus();

protected:
	WiFiClient wifi;
	Configuration* config;

	WifiStatusCode configureWifi();

};


//end of add your includes here
#ifdef __cplusplus
extern "C"
{
#endif


extern void wifiStatusCallback();


#ifdef __cplusplus
} // extern "C"
#endif

/*
 * PubSubWrapper.h
 *
 *  Created on: Feb 21, 2016
 *      Author: tsasala
 */

#pragma once

#include <Arduino.h>
#include <PubSubClient.h>

#include "Debug.h"
#include "ClientGlobal.h"
#include "Configuration.h"
#include "WifiWrapper.h"
#include "Command.h"
#include "Helper.h"


enum class MqttStatusCode {IDLE, CONNECTED, DISCONNECTED, UNKNOWN };


class PubSubWrapper
{
public:
	PubSubWrapper();
	uint8_t initialize(Configuration* config, WifiWrapper* wifi);
	void callback(char* topic, byte* payload, unsigned int length);

	uint8_t connect();
	uint8_t disconnect();
	uint8_t checkConnection();
	uint8_t connected();

	void work();
	void publish(char *channel);
	void publish( char *channel, char* buffer);
	void publish( char *channel, JsonObject& obj);
	uint8_t *getBuffer();


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


protected:
	PubSubClient pubsub;
	Configuration* config;
	uint8_t* cmdBuf;
	uint8_t retryCount;
    uint32_t retryDelay;
	void (*callbackConnectionStatus)();

};


//end of add your includes here
#ifdef __cplusplus
extern "C"
{
#endif

extern boolean isCommandAvailable();
extern void setCommandAvailable(boolean b);
extern boolean commandDelay(uint32_t time);

extern void pubsubCallback(char* topic, byte* payload, unsigned int length);

#ifdef __cplusplus
} // extern "C"
#endif


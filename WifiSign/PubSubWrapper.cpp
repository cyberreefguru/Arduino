/*
 * PubSubWrapper.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: tsasala
 */

#include "PubSubWrapper.h"

/**
 * Constructor
 */
PubSubWrapper::PubSubWrapper()
{
	config = 0;
	cmdBuf = 0;
	retryCount = 20;
	retryDelay = 250;
	callbackConnectionStatus = NULL;
}

/**
 * Initializes the wrapper
 */
uint8_t PubSubWrapper::initialize(Configuration* config, WifiWrapper* wifi)
{
	this->config = config;
	if( !config )
	{
		LOG_ERROR(F("Configuration is NULL - cannot configure MQTT client."));
		return false;
	}

	pubsub.setClient( (Client &)wifi->getWifiClient() );

	// Check if we are connected - if so, disconnect
	if( pubsub.connected() )
	{
		LOG_WARN(F("Connected already! Disconnecting from queue..."));
		disconnect();
	}

	// Set up mqtt server
	pubsub.setServer( (const char *)config->getMqttServer(), config->getMqttPort());
	pubsub.setCallback(pubsubCallback);

	// Check if we already allocated memory; if so, free it
	if( cmdBuf != 0 )
	{
		free(cmdBuf);
	}

	// Allocate memory
	cmdBuf = (uint8_t *)malloc(CMD_BUFFER_SIZE);
	if( cmdBuf == 0 )
	{
		LOG_ERROR(F("Unable to allocate json buffer memory!"));
		return false;
	}

	return connect();
}

/**
 * Connects to MQTT to server
 *
 */
uint8_t PubSubWrapper::connect()
{
	uint8_t count = 0;
	uint8_t flag = false;

	if( pubsub.connected() )
	{
		LOG_INFO(F("Queue connected."));
	}
	else
	{
		LOG_DEBUG(F("Attempting new queue connection..."));

		// Loop until we're reconnected or "timeout"
		while( count < config->getMqttTries() )
		{
			if(callbackConnectionStatus)
			{
				callbackConnectionStatus();
			}

			// Attempt to connect
			if (pubsub.connect( (char *)config->getMyChannel(), (char *)config->getMqttUser(), (char *)config->getMqttPassword() ) )
			{
				LOG_INFO1(F("Connected to MQTT Server: "), (char *)config->getMqttServer() );

				// subscribe to channels
				pubsub.subscribe( (char *)config->getAllChannel() );
				pubsub.subscribe( (char *)config->getMyChannel() );

				// Once connected, publish an announcement...
				LOG_DEBUG1(F("Announcing presence: "), (char *)config->getMyChannel());

				// Tell controller we're listening
				pubsub.publish((char *)config->getRegistrationChannel(), (char *)config->getMyChannel() );

				flag = true;
				break;
			}
			else
			{
				// Wait before retrying
//				Helper::delayYield(retryDelay);
			}
			count += 1;

		} // end while
	}

	if( !flag )
	{
		LOG_ERROR(F("Unable to bind to queue."));
	}

	return flag;
}

/**
 * Disconnects from MQTT server
 *
 */
uint8_t PubSubWrapper::disconnect()
{
	uint8_t flag = false;

	if( pubsub.connected() )
	{
		LOG_DEBUG(F("Disconnecting..."));
		pubsub.unsubscribe( (char *)config->getAllChannel() );
		pubsub.unsubscribe( (char *)config->getMyChannel() );
		pubsub.disconnect();
		LOG_INFO(F("Disconnected from MQTT Server."));
		flag = true;

	}
	else
	{
		LOG_WARN(F("Cannot disconnect -- not connected!"));
	}

	return flag;
}

uint8_t PubSubWrapper::connected()
{
	return pubsub.connected();
}

/**
 * Checks if we still have a valid connection to the server.
 * If there is no connection, an attempt will be made.
 *
 */
uint8_t PubSubWrapper::checkConnection()
{
	uint8_t flag = pubsub.connected();

	if (!flag)
	{
		flag = connect();
	}

	return flag;
}

/**
 * Calls the loop function to process queue messages
 *
 */
void PubSubWrapper::work()
{
	pubsub.loop();
}


/**
 * Handles all "receive" messages from MQTT
 *
 * Parses the messages and sends to correct handler
 *
 */
void PubSubWrapper::callback(char* topic, byte* payload, unsigned int length)
{
#ifdef __DEBUG
	LOG_DEBUG3( millis(), F(" - Message arrived ["), topic, F("]") );
		for (uint8_t i = 0; i < length; i++)
	{
		Serial.print((char) payload[i]);
	}
	Serial.println();
#endif

	if( length <= CMD_BUFFER_SIZE )
	{
		// Copy payload to command buffer
		memcpy( (void *)cmdBuf, (void *)payload, length);
		setCommandAvailable(true);
	}
	else
	{
		LOG_ERROR(F("Command buffer too small"));
	}

}

/**
 * Publishes a message to the specified channel
 *
 */
void PubSubWrapper::publish(char *channel)
{
	pubsub.publish(channel, (char *)cmdBuf);
}

/**
 * Publishes a message to the specified channel
 *
 */
void PubSubWrapper::publish(char *channel, char *buffer)
{
	pubsub.publish(channel, buffer);
}

/**
 * Publishes a message to the specified channel
 *
 */
void PubSubWrapper::publish(char *channel, JsonObject& obj)
{
	//obj.printTo((char *)cmdBuf, CMD_BUFFER_SIZE);
	pubsub.publish( channel, (char *)cmdBuf );
}

/**
 * Returns a pointer to the internal buffer
 *
 */
uint8_t* PubSubWrapper::getBuffer()
{
	return cmdBuf;
}

void PubSubWrapper::setRetryCount(uint8_t c)
{
	retryCount = c;
}

uint8_t PubSubWrapper::getRetryCount()
{
	return retryCount;
}

void PubSubWrapper::setRetryDelay(uint32_t c)
{
	retryDelay = c;
}

uint32_t PubSubWrapper::getRetryDelay()
{
	return retryDelay;
}

void PubSubWrapper::setConnectionStatusCallback(void (*cb)())
{
	this->callbackConnectionStatus = cb;
}


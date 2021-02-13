/*
 * Configuration.h
 *
 *  Created on: Oct 21, 2015
 *      Author: tsasala
 */
#pragma once

#include <Arduino.h>
#include "ClientGlobal.h"
//#include <FS.h>          // this needs to be first, or it all crashes and burns...
//#include <SD.h>
#include "FileSystemWrapper.h"
#include <ArduinoJson.h>
#include "Debug.h"

#define CONFIG_FILE_NAME			"/config.json"

#define KEY_VERSION					"v"
#define KEY_NODE_ID					"id"
#define KEY_HOST_NAME				"h"
#define KEY_AP_NAME					"apn"
#define KEY_WIFI_RETRIES			"wt"
#define KEY_WIFI_SSID				"wssid"
#define KEY_WIFI_PASSWORD			"wpass"
#define KEY_WEB_PORT				"wport"
#define KEY_MQTT_TRIES				"mt"
#define KEY_MQTT_SERVER				"ms"
#define KEY_MQTT_PORT				"mport"
#define KEY_MQTT_USER				"muser"
#define KEY_MQTT_PASSWORD			"mpass"
#define KEY_MQTT_CHANNEL_ALL		"mall"
#define KEY_MQTT_CHANNEL_REG		"mreg"
#define KEY_MQTT_CHANNEL_MY			"mmy"
#define KEY_MQTT_CHANNEL_RESPONSE	"mresp"

#define CONFIG_VERSION_1			1
#define CONFIG_VERSION_2			2
#define CONFIG_VERSION_3			3

#define DEFAULT_VERSION				CONFIG_VERSION_1
#define DEFAULT_NODE_ID				1

#define DEFAULT_WIFI_TRIES			20
#define DEFAULT_MQTT_TRIES			20

#define DEFAULT_HOST_NAME			"wifi_sign"
#define DEFAULT_AP_NAME				"WifiSignAP"

#define DEFAULT_WIFI_SSID			"IOT_Down_2G"
#define DEFAULT_WIFI_PASSWORD		"code5541iot"
#define DEFAULT_WEB_PORT			80
#define DEFAULT_MQTT_SERVER			"192.168.30.113"
#define DEFAULT_MQTT_USER			"mqtt_user"
#define DEFAULT_MQTT_PASSWORD		"mqtt"
#define DEFAULT_MQTT_PORT			1883

#define DEFAULT_MQTT_CHANNEL_ALL	"crg/matrix/all"
#define DEFAULT_MQTT_CHANNEL_REG	"crg/matrix/reg"
#define DEFAULT_MQTT_CHANNEL_MY		"crg/matrix/node/1"
#define DEFAULT_MQTT_CHANNEL_RESP	"crg/matrix/resp/1"

#define STRING_SIZE					32

enum class ConfigStatusCode {OK, FILE_SYSTEM_NOT_INITIALIZED, FILE_SYSTEM_NOT_DEFINED, FILE_NOT_FOUND,
	FILE_NOT_OPENED, FILE_NOT_OPEN_WRITE, FILE_NOT_DELETED, BUFFER_MEMORY_ERROR, SERIALIZE_ERROR, DESERIALIZE_ERROR,
	UNMATCHED_VERSION, UNKNOWN };


class Configuration
{
public:
	Configuration();
	ConfigStatusCode initialize(boolean readConfig);

	uint8_t getVersion();
	void setVersion(uint8_t v);

	uint8_t getNodeId();
	void setNodeId(uint8_t v);

	const uint8_t* getHostName() const;
	void setHostName(uint8_t *b);

	const uint8_t* getAPName() const;
	void setAPName(uint8_t *b);

	const uint8_t* getWifiSsid() const;
	void setWifiSsid(uint8_t *b);

	const uint8_t* getWifiPassword() const;
	void setWifiPassword(uint8_t *b);

	uint16_t getWebPort() const;
	void setWebPort(uint16_t port);

	uint8_t getWifiTries() const;
	void setWifiTries(uint8_t wifiTries);

	const uint8_t* getMqttServer() const;
	void setMqttServer(uint8_t *b);

	uint16_t getMqttPort() const;
	void setMqttPort(uint16_t port);

	const uint8_t* getMqttUser() const;
	void setMqttUser(uint8_t *b);

	const uint8_t* getMqttPassword() const;
	void setMqttPassword(uint8_t *b);

	uint8_t getMqttTries() const;
	void setMqttTries(uint8_t tries);


	const uint8_t* getAllChannel() const;
	void setAllChannel(uint8_t *b);

	const uint8_t* getRegistrationChannel() const;
	void setRegistrationChannel(uint8_t *b);

	const uint8_t* getMyChannel() const;
	void setMyChannel(uint8_t *b);

	const uint8_t* getMyResponseChannel() const;
	void setMyResponseChannel(uint8_t *b);

	String toString(ConfigStatusCode c);
	String toString();

	ConfigStatusCode write();
	ConfigStatusCode read();
	void dump();
	void dumpConfigFile();

protected:
	uint8_t version;
	uint8_t nodeId;

	uint8_t hostName[STRING_SIZE];
	uint8_t apName[STRING_SIZE];

	uint8_t wifiSsid[STRING_SIZE];
	uint8_t wifiPassword[STRING_SIZE];
	uint16_t webPort;
	uint8_t wifiTries;

	uint8_t mqttServer[STRING_SIZE];
	uint16_t mqttPort;
	uint8_t mqttUser[STRING_SIZE];
	uint8_t mqttPassword[STRING_SIZE];
	uint8_t mqttTries;

	uint8_t allChannel[STRING_SIZE];
	uint8_t regChannel[STRING_SIZE];
	uint8_t myChannel[STRING_SIZE];
	uint8_t myResponseChannel[STRING_SIZE];

	boolean configured;

	FileSystemWrapper fsw;

	void initializeVariables();

};

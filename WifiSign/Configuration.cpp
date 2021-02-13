/*
 * Configuration.cpp
 *
 *  Created on: Oct 21, 2015
 *      Author: tsasala
 */

#include "Configuration.h"

/**
 * Constructor
 *
 */
Configuration::Configuration()
{
	initializeVariables();
}

/**
 * Initializes the structure; returns false if failure during initialization
 *
 */
ConfigStatusCode Configuration::initialize(boolean readConfig)
{
	ConfigStatusCode flag = ConfigStatusCode::UNKNOWN;

	initializeVariables();

	if( readConfig )
	{
		flag = read();
		if( flag == ConfigStatusCode::FILE_NOT_FOUND )
		{
			LOG_ERROR("Config file does not exist - creating with defaults");
			flag = write();
			if( flag != ConfigStatusCode::OK)
			{
				LOG_ERROR("Error writing default configuration file!");
			}
		}
	}
	else
	{
		flag = ConfigStatusCode::OK;
	}
	return flag;

}

uint8_t Configuration::getNodeId()
{
	return nodeId;
}

void Configuration::setNodeId(uint8_t nodeId)
{
	this->nodeId = nodeId;
}

const uint8_t* Configuration::getAPName() const
{
	return apName;
}

void Configuration::setAPName(uint8_t *apn)
{
	strcpy( (char *)apName, (char *)apn );
}


uint8_t Configuration::getVersion()
{
	return version;
}

void Configuration::setVersion(uint8_t version)
{
	this->version = version;
}

const uint8_t* Configuration::getHostName() const
{
	return hostName;
}

void Configuration::setHostName(uint8_t *b)
{
	strcpy( (char *)hostName, (char *)b );
}

const uint8_t* Configuration::getWifiSsid() const
{
	return wifiSsid;
}

void Configuration::setWifiSsid(uint8_t *b)
{
	strcpy( (char *)wifiSsid, (char *)b );
}


const uint8_t* Configuration::getWifiPassword() const
{
	return wifiPassword;
}

void Configuration::setWifiPassword(uint8_t *b)
{
	strcpy( (char *)wifiPassword, (char *)b );
}

uint8_t Configuration::getWifiTries() const
{
	return wifiTries;
}

void Configuration::setWifiTries(uint8_t wifiTries)
{
	this->wifiTries = wifiTries;
}

uint16_t Configuration::getWebPort() const
{
	return webPort;
}

void Configuration::setWebPort(uint16_t port)
{
	this->webPort = port;
}


const uint8_t* Configuration::getMqttServer() const
{
	return mqttServer;
}

void Configuration::setMqttServer(uint8_t *b)
{
	strcpy( (char *)mqttServer, (char *)b );
}

uint16_t Configuration::getMqttPort() const
{
	return mqttPort;
}

void Configuration::setMqttPort(uint16_t port)
{
	this->mqttPort = port;
}

const uint8_t* Configuration::getMqttUser() const
{
	return mqttUser;
}

void Configuration::setMqttUser(uint8_t *b)
{
	strcpy( (char *)mqttUser, (char *)b );
}

const uint8_t* Configuration::getMqttPassword() const
{
	return mqttPassword;
}

void Configuration::setMqttPassword(uint8_t *b)
{
	strcpy( (char *)mqttPassword, (char *)b );
}

uint8_t Configuration::getMqttTries() const
{
	return mqttTries;
}

void Configuration::setMqttTries(uint8_t tries)
{
	this->mqttTries = tries;
}


const uint8_t* Configuration::getAllChannel() const
{
	return allChannel;
}

void Configuration::setAllChannel(uint8_t *b)
{
	strcpy( (char *)allChannel, (char *)b );
}

const uint8_t* Configuration::getRegistrationChannel() const
{
	return regChannel;
}

void Configuration::setRegistrationChannel(uint8_t *b)
{
	strcpy( (char *)regChannel, (char *)b );
}

const uint8_t* Configuration::getMyChannel() const
{
	return myChannel;
}

void Configuration::setMyChannel(uint8_t *b)
{
	strcpy( (char *)myChannel, (char *)b );
}

const uint8_t* Configuration::getMyResponseChannel() const
{
	return myResponseChannel;
}

void Configuration::setMyResponseChannel(uint8_t *b)
{
	strcpy( (char *)myResponseChannel, (char *)b );
}

/**
 * Reads the configuration
 *
 */
ConfigStatusCode Configuration::read()
{
	LOG_DEBUG(F("Read Configuration"));
	ConfigStatusCode flag = ConfigStatusCode::UNKNOWN;

	if( fsw.initialize() )
	{
		FatFileSystem *fatfs = fsw.getFileSystem();
		if( fatfs )
		{
			if(fatfs->exists( CONFIG_FILE_NAME ) )
			{
//				LOG_DEBUG(F("File System:"));
//				fatfs->ls(&Serial, "/", (LS_DATE|LS_SIZE) );

				File file = fatfs->open(CONFIG_FILE_NAME, FILE_READ);
				if ( file )
				{
					DynamicJsonDocument doc(1024);
					DeserializationError error = deserializeJson(doc, file);
					if (error == DeserializationError::Ok)
					{
						// Print config file
						// TODO - take out passwords

//						Serial.print("File=");
//						serializeJson(doc, Serial);
//						Serial.println();

						version = doc[KEY_VERSION] | DEFAULT_VERSION;
						if(version == CONFIG_VERSION_1)
						{
							nodeId = doc[KEY_NODE_ID] | DEFAULT_NODE_ID;
							strlcpy( (char *)hostName, doc[KEY_HOST_NAME] | DEFAULT_HOST_NAME, STRING_SIZE);
							strlcpy( (char *)apName, doc[KEY_AP_NAME] | DEFAULT_AP_NAME, STRING_SIZE);
							wifiTries = doc[KEY_WIFI_RETRIES] | DEFAULT_WIFI_TRIES;
							strlcpy( (char *)wifiSsid, doc[KEY_WIFI_SSID] | DEFAULT_WIFI_SSID, STRING_SIZE);
							strlcpy( (char *)wifiPassword, doc[KEY_WIFI_PASSWORD] | DEFAULT_WIFI_PASSWORD, STRING_SIZE);
							webPort = doc[KEY_WEB_PORT] | DEFAULT_WEB_PORT;
							mqttTries = doc[KEY_MQTT_TRIES] | DEFAULT_MQTT_TRIES;
							strlcpy( (char *)mqttServer, doc[KEY_MQTT_SERVER] | DEFAULT_MQTT_SERVER, STRING_SIZE);
							mqttPort = doc[KEY_MQTT_PORT] | DEFAULT_MQTT_PORT;
							strlcpy( (char *)mqttUser, doc[KEY_MQTT_USER] | DEFAULT_MQTT_USER, STRING_SIZE);
							strlcpy( (char *)mqttPassword, doc[KEY_MQTT_PASSWORD] | DEFAULT_MQTT_PASSWORD, STRING_SIZE);
							strlcpy( (char *)allChannel, doc[KEY_MQTT_CHANNEL_ALL] | DEFAULT_MQTT_CHANNEL_ALL, STRING_SIZE);
							strlcpy( (char *)regChannel, doc[KEY_MQTT_CHANNEL_REG] | DEFAULT_MQTT_CHANNEL_REG, STRING_SIZE);
							strlcpy( (char *)myChannel, doc[KEY_MQTT_CHANNEL_MY] | DEFAULT_MQTT_CHANNEL_MY, STRING_SIZE);
							strlcpy( (char *)myResponseChannel, doc[KEY_MQTT_CHANNEL_RESPONSE] | DEFAULT_MQTT_CHANNEL_RESP, STRING_SIZE);

							flag = ConfigStatusCode::OK;
						}
						else
						{
							flag = ConfigStatusCode::UNMATCHED_VERSION;
							LOG_ERROR1(F("Unmatched version: "), version);
						}
						LOG_DEBUG(F("Configuration file read."));
					}
					else
					{
						flag = ConfigStatusCode::DESERIALIZE_ERROR;
						LOG_ERROR1(F("deserializeJson() failed with code: "), error.c_str());
					}
				}
				else
				{
					flag = ConfigStatusCode::FILE_NOT_OPENED;
					LOG_ERROR(F("Unable to open config file"));
				}

				if( file )
				{
					file.close();
				}
			}
			else
			{
				LOG_ERROR(F("Config file not found."));
				flag = ConfigStatusCode::FILE_NOT_FOUND;
			}
		}
		else
		{
			flag = ConfigStatusCode::FILE_SYSTEM_NOT_DEFINED;
		}
	}
	else
	{
		flag = ConfigStatusCode::FILE_SYSTEM_NOT_INITIALIZED;

	}


	return flag;

}

/**
 * Writes the configuration
 */
ConfigStatusCode Configuration::write()
{
	LOG_DEBUG(F("Write Configuration"));
	ConfigStatusCode flag = ConfigStatusCode::UNKNOWN;

	if( fsw.initialize() )
	{
		FatFileSystem *fatfs = fsw.getFileSystem();
		if( fatfs )
		{
//			LOG_DEBUG(F("File System:"));
//			fatfs->ls(&Serial, "/", (LS_DATE|LS_SIZE) );

			// We must remove the file or new info will
			// be appended to existing file.
			if(fatfs->exists(CONFIG_FILE_NAME))
			{
				if( fatfs->remove(CONFIG_FILE_NAME) )
				{
					LOG_DEBUG("Old config file removed.")
				}
				else
				{
					LOG_ERROR("Error removing existing config file");
					return ConfigStatusCode::FILE_NOT_DELETED;
				}
			}

			File file = fatfs->open(CONFIG_FILE_NAME, FILE_WRITE);
			if ( file )
			{
				DynamicJsonDocument doc(JSON_OBJECT_SIZE(17));
				doc[KEY_VERSION] = version;
				doc[KEY_NODE_ID] = nodeId;
				doc[KEY_HOST_NAME] = hostName;
				doc[KEY_AP_NAME] = apName;
				doc[KEY_WIFI_RETRIES] = wifiTries;
				doc[KEY_WIFI_SSID] = wifiSsid;
				doc[KEY_WIFI_PASSWORD] = wifiPassword;
				doc[KEY_WEB_PORT] = webPort;
				doc[KEY_MQTT_TRIES] = mqttTries;
				doc[KEY_MQTT_SERVER] = mqttServer;
				doc[KEY_MQTT_PORT] = mqttPort;
				doc[KEY_MQTT_USER] = mqttUser;
				doc[KEY_MQTT_PASSWORD] = mqttPassword;
				doc[KEY_MQTT_CHANNEL_ALL] = allChannel;
				doc[KEY_MQTT_CHANNEL_REG] = regChannel;
				doc[KEY_MQTT_CHANNEL_MY] = myChannel;
				doc[KEY_MQTT_CHANNEL_RESPONSE] = myResponseChannel;

//				Serial.print("File=");
//				serializeJson(doc, Serial);
//				Serial.println();

				// Serialize JSON to file
				int s = serializeJson(doc, file);
				if ( s > 0)
				{
					flag = ConfigStatusCode::OK;
					LOG_INFO1(F("Config file written: "), s);
				}
				else
				{
					flag = ConfigStatusCode::SERIALIZE_ERROR;
					LOG_ERROR1(F("Failed to write to file: "), s);
				}
			}
			else
			{
				LOG_ERROR(F("Unable to open file for write"));
				flag = ConfigStatusCode::FILE_NOT_OPEN_WRITE;
			}
			if( file )
			{
				file.close();
			}
		}
		else
		{
			flag = ConfigStatusCode::FILE_SYSTEM_NOT_DEFINED;
		}
	}
	else
	{
		flag = ConfigStatusCode::FILE_SYSTEM_NOT_INITIALIZED;
	}

	return flag;
}

void Configuration::initializeVariables()
{
	version = DEFAULT_VERSION;
	nodeId = DEFAULT_NODE_ID;

	memset(hostName, 0, STRING_SIZE);
	sprintf( (char *)hostName, "%s", DEFAULT_HOST_NAME);

	memset(apName, 0, STRING_SIZE);
	sprintf( (char *)apName, "%s", DEFAULT_AP_NAME);

	wifiTries = DEFAULT_WIFI_TRIES;

	memset(wifiSsid, 0, STRING_SIZE);
	sprintf( (char *)wifiSsid, "%s", DEFAULT_WIFI_SSID );

	memset(wifiPassword, 0, STRING_SIZE);
	sprintf( (char *)wifiPassword, "%s", DEFAULT_WIFI_PASSWORD );

	webPort = DEFAULT_WEB_PORT;

	mqttTries = DEFAULT_MQTT_TRIES;

	memset(mqttServer, 0, STRING_SIZE);
	sprintf( (char *)mqttServer, "%s", DEFAULT_MQTT_SERVER );

	memset(mqttUser, 0, STRING_SIZE);
	sprintf( (char *)mqttUser, "%s", DEFAULT_MQTT_USER );

	memset(mqttPassword, 0, STRING_SIZE);
	sprintf( (char *)mqttPassword, "%s", DEFAULT_MQTT_PASSWORD );

	mqttPort = DEFAULT_MQTT_PORT;

	memset(allChannel, 0, STRING_SIZE);
	sprintf( (char *)allChannel, "%s", DEFAULT_MQTT_CHANNEL_ALL );

	memset(regChannel, 0, STRING_SIZE);
	sprintf( (char *)regChannel, "%s", DEFAULT_MQTT_CHANNEL_REG );

	memset(myChannel, 0, STRING_SIZE);
	sprintf((char *)myChannel, DEFAULT_MQTT_CHANNEL_MY );

	memset(myResponseChannel, 0, STRING_SIZE);
	sprintf((char *)myResponseChannel, DEFAULT_MQTT_CHANNEL_RESP );

}

String Configuration::toString(ConfigStatusCode c)
{
	String s = "";
	switch(c)
	{
	case ConfigStatusCode::OK:
		s = String(F("OK"));
		break;
	case ConfigStatusCode::FILE_SYSTEM_NOT_INITIALIZED:
		s = String(F("FILE_SYSTEM_NOT_INITIALIZED"));
		break;
	case ConfigStatusCode::FILE_SYSTEM_NOT_DEFINED:
		s = String(F("FILE_SYSTEM_NOT_DEFINED"));
		break;
	case ConfigStatusCode::FILE_NOT_FOUND:
		s = String(F("FILE_NOT_FOUND"));
		break;
	case ConfigStatusCode::FILE_NOT_OPENED:
		s = String(F("FILE_NOT_OPENED"));
		break;
	case ConfigStatusCode::FILE_NOT_OPEN_WRITE:
		s = String(F("FILE_NOT_OPEN_WRITE"));
		break;
	case ConfigStatusCode::BUFFER_MEMORY_ERROR:
		s = String(F("BUFFER_MEMORY_ERROR"));
		break;
	case ConfigStatusCode::SERIALIZE_ERROR:
		s = String(F("SERIALIZE_ERROR"));
		break;
	case ConfigStatusCode::DESERIALIZE_ERROR:
		s = String(F("DESERIALIZE_ERROR"));
		break;
	case ConfigStatusCode::UNMATCHED_VERSION:
		s = String(F("UNMATCHED_VERSION"));
		break;
	case ConfigStatusCode::FILE_NOT_DELETED:
		s = String(F("FILE_NOT_DELETED"));
		break;
	case ConfigStatusCode::UNKNOWN:
		s = String(F("UNKNOWN"));
		break;
	default:
		break;
	}
	return s;
}

void Configuration::dump()
{
	LOG_DEBUG(F("\nClient Configuration:"));
	LOG_DEBUG(toString());
}

String Configuration::toString()
{
	String s = "";

	s += F("Version         : ");
	s += version;
	s += F("\nNode ID         : ");
	s += nodeId;

	s += F("\nHost Name       : ");
	s += (char*)hostName;

	s += F("\nAP Name         : ");
	s += (char *)apName;

	s += F("\nWIFI Tries      : ");
	s += wifiTries;

	s += F("\nSSID            : ");
	s += (char *)wifiSsid;
	s += F("\nPassword        : ");
	s += (char *)wifiPassword;

	s += F("\nWeb Port        : ");
	s += webPort;

	s += F("\nMQTT Tries      : ");
	s += mqttTries;
	s += F("\nServer Address  : ");
	s += (char *)mqttServer;
	s += F("\nMQTT Port       : ");
	s += mqttPort;

	s += F("\nMQTT User       : ");
	s += (char *)mqttUser;
	s += F("\nMQTT Password   : ");
	s += (char *)mqttPassword;

	s += F("\nAll Channel     : ");
	s += (char *)allChannel;
	s += F("\nReg Channel     : ");
	s += (char *)regChannel;
	s += F("\nMy Channel      : ");
	s += (char *)myChannel;
	s += F("\nResponse Channel: ");
	s += (char *)myResponseChannel;
	return s;

}


void Configuration::dumpConfigFile()
{
	FatFileSystem *fatfs = fsw.getFileSystem();

	// Open file for reading
	File f = fatfs->open(CONFIG_FILE_NAME);
	if (!f)
	{
		Serial.println(F("Failed to open config file"));
	}
	else
	{
		// Extract each characters by one by one
		while (f.available())
		{
			Serial.print((char)f.read());
		}
		Serial.println();

		// Close the file
		f.close();
	}

}

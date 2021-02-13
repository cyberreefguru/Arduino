// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#pragma once

#define __DEBUG

#include <Arduino.h>

#include "Debug.h"
#include "WifiWrapper.h"
#include "PubSubWrapper.h"
#include "MatrixWrapper.h"
#include "ClientGlobal.h"
#include "Command.h"
#include "Configuration.h"
#include "Helper.h"
//#include "CaptivePortal.h"
#include "PortalWebServer.h"

#define NUM_PIXELS 4
#define HALF NUM_PIXELS/2

#define CMD_IP_ADDRESS "{\"cmd\": 22, \"t\": \"WIFI:%s MQTT:%s:%d\", \"x\":0, \"y\":24, \"fg\":16777215, \"bg\":0, \"sp\":10, \"r\":0, \"d\":0}"


void worker();

boolean isCommandAvailable();
void setCommandAvailable(boolean flag);
boolean commandDelay(uint32_t time);

void pubsubCallback(char* topic, byte* payload, unsigned int length);
void wifiConnectionStatusCallback();
void mqttConnectionStatusCallback();
void runConfigurationPortal();

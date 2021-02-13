/*
 * PortalWebServer.h
 *
 *  Created on: Feb 12, 2021
 *      Author: tsasala
 */

#pragma once

#include "Debug.h"
#include "WifiWrapper.h"
#include "Configuration.h"
#include "Parameter.h"
#include "mimetable.h"
#include "DNSServer.h"

enum class HttpMethod {ANY, GET, HEAD, POST, PUT, PATCH, DELETE, OPTIONS };
enum class HttpClientStatus { NONE, WAIT_READ, WAIT_CLOSE };
enum class HttpAuthMethod {BASIC, DIGEST};

enum class PortalStatusCode {SUCCESS, FAIL, DNS_ERROR, WIFI_ERROR, AP_CREATE_FAILURE, AP_ERROR, AP_NO_CLIENT, AP_NO_CLIENT_ERROR, PARSE_ERROR, CONFIG_SUCCESS, CONFIG_FAIL,
	ERROR_BUILDING_RESPONSE, ERROR_SENDING_RESPONSE, ERROR_BUILDING_REQUEST, ERROR_SENDING_REQUEST, OUT_OF_MEMORY, BUFFER_OVERRUN, UNDEFINED_URL};

#define MAX_WIFI_BUFFER_SIZE		1024*7

#define MAX_POST_PARAMETER_SIZE 	32

#define DEFAULT_DNS_PORT			53

#define DEFAULT_AP_CHANNEL			2

class PortalWebServer
{
public:
	PortalWebServer();
	virtual ~PortalWebServer();

	PortalStatusCode setupPortal();
	PortalStatusCode handlePortalClient();
	PortalStatusCode handlePortalRequest();

	PortalStatusCode runCaptivePortal();
	PortalStatusCode setupCaptivePortal();
	PortalStatusCode handleCaptiveClient();
	PortalStatusCode handleCaptiveRequest();

	void setConfiguration(Configuration &config);


protected:
	PortalStatusCode send(int code);
	PortalStatusCode sendResponse(char *buf);
	int sendBuffer(char *buf, int size);

	PortalStatusCode handleCaptiveRoot();
	PortalStatusCode handleConfigSave();

	boolean parseRequest();
	boolean parseForm(String boundary, uint32_t len);
	String urlDecode(const String& text);

	String toString(PortalStatusCode c);

    DNSServer 	dnsServer;
    WiFiClient	currentClient;
    HttpMethod	currentMethod;
    String		currentUri;
    Parameter 	currentParameters[MAX_POST_PARAMETER_SIZE];
    uint8_t 	currentParameterSize = 0;
    uint8_t 	wifiStatus;
    char		wifiBuffer[MAX_WIFI_BUFFER_SIZE];
    Configuration	myConfig;


    const char *head="<html> <head> <title>WiFi Sign Setup</title> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <style> * { box-sizing: border-box; } [class*='col-'] { float: left; padding: 15px; } [class*='col-'] { /* For mobile phones: */ width: 100%; } @media only screen and (min-width: 600px) { /* For tablets: */ .col-s-25 { width: 25%; } .col-s-50 { width: 50%; } .col-s-75 { width: 75%; } .col-s-100 { width: 100%; } } @media only screen and (min-width: 768px) { /* For desktop: */ .col-25 { width: 25%; } .col-50 { width: 50%; } .col-75 { width: 75%; } .col-100 { width: 100%; } } body { color: white; background-color: #333333; font-family: verdana; } .header { color: GoldenRod; font-weight: bolder; padding: 0px 10px 10px 25px; } .footer { background-color: #6622cc; color: #ffffff; text-align: center; font-size: 12px; padding: 1px 0px 1px 0px; } .footnote { padding: 0px 0px 10px 0px; font-style: italic; font-size: small; } .t_row { margin: 0px 10px 0px 10px } .t_row::after { content: ''; clear: both; display: table; } .f_row { display: flex; flex-direction: row; } .l_row { display: flex; flex-direction: row; } .f_input { font-size: 1em; padding: 2px 0px 2px 0px; margin: 0px 0px 10px 0px; } .f_label { font-weight: bold; padding: 0px 0px 5px 0px; } .f_note { font-style: italic; font-size: small; } .f_but { border: 0; border-radius: 0.3rem; background-color: #1fa3ec; color: #fff; line-height: 2.4rem; font-size: 1.2rem; } input:invalid { border: 3px solid red; } input[type=checkbox] { width: 20px; } </style> <script> function togglePass() { toggleType(document.getElementById('wpass')); toggleType(document.getElementById('mpass')); } function toggleType(field) { if (field.type === 'password') { field.type = 'text'; } else { field.type = 'password'; } } </script> </head>";
    const char *root_body="<body> <form method=POST action='/save_config'> <div class='t_row'> <div class='col-25 col-s-25'> </div> <div class='col-50 col-s-75'> <div class='f_row'> <div class='header'>WiFi Sign Setup</div> </div> <div class='footnote'> * Lower case, numbers, and _ only </div> <div class='f_row'> <label class='f_label' for='nid'>Node ID</label> <div class='f_note'>*</div> </div> <div class='f_row'> <input class='f_input' id='nid' required name='nid' type='number' value='%d'> </div> <div class='l_row'> <label class='f_label' for='hn'>Host Name</label> <div class='f_note'>*</div> </div> <div class='l_row'> <input class='f_input' id='hn' required name='hn' maxlength=32 pattern='[a-z0-9_]*' value='%s'> </div> <div class='l_row'> <label class='f_label' for='apn'>AP Name</label> </div> <div class='l_row'> <input class='f_input' id='apn' required name='apn' maxlength=32 pattern='[a-zA-Z0-9_]*' value='%s'> </div> <div class='f_row'> <label class='f_label' for='wssid'>WIFI SSID</label> <div class='f_note'>*</div> </div> <div class='f_row'> <input class='f_input' id='wssid' required name='wssid' maxlength=32 value='%s'> </div> <div class='f_row'> <label class='f_label' for='wpass'>WIFI Password</label> </div> <div class='f_row'> <input class='f_input' id='wpass' required name='wpass' type='password' maxlength=31 value='%s'> </div> <div class='f_row'> <label class='f_label' for='wtry'>WIFI Retries</label> </div> <div class='f_row'> <input class='f_input' id='wtry' required name='wtry' type='number' maxlength=2 value='%d'> </div> <div class='f_row'> <label class='f_label' for='mserv'>MQTT Server</label> </div> <div class='f_row'> <input class='f_input' id='mserv' required name='mserv' maxlength=32 value='%s'> </div> <div class='f_row'> <label class='f_label' for='mport'>MQTT Port</label> </div> <div class='f_row'> <input class='f_input' id='mport' required name='mport' type='number' value='%d'> </div> <div class='f_row'> <label class='f_label' for='muser'>MQTT User</label> </div> <div class='f_row'> <input class='f_input' id='muser' name='muser' maxlength=32 value='%s'> </div> <div class='f_row'> <label class='f_label' for='mpass'>MQTT Password</label> </div> <div class='f_row'> <input class='f_input' id='mpass' name='mpass' type='password' maxlength=32 value='%s'> </div> <div class='f_row'> <label class='f_label' for='mtry'>MQTT Retries</label> </div> <div class='f_row'> <input class='f_input' id='mtry' required name='mtry' type='number' value='%d'> </div> <div class='f_row'> <label class='f_label' for='mch'>MQTT Command Channel</label> <div class='f_note'>*</div> </div> <div class='f_row'> <input class='f_input' id='mch' name='mch' maxlength=32 value='%s'> </div> <div class='f_row'> <label class='f_label' for='regch'>Registration Channel</label> <div class='f_note'>*</div> </div> <div class='f_row'> <input class='f_input' id='regch' name='regch' maxlength=32 value='%s'> </div> <div class='f_row'> <label class='f_label' for='respch'>Response Channel</label> <div class='f_note'>*</div> </div> <div class='f_row'> <input class='f_input' id='respch' name='respch' maxlength=31 value='%s'> </div> <div class='f_row'> <label class='f_label' for='ach'>All Channel</label> <div class='f_note'>*</div> </div> <div class='f_row'> <input class='f_input' id='ach' name='ach' maxlength=31 value='%s'> </div> <div class='f_row'> <input class='f_input' type='checkbox' onclick='togglePass()'>Show Password </div> <div class='f_row'><button class='f_but' type='submit'>save settings</button></div> <form method='get' action='reboot'> <div class='f_row'> <button class='f_but' type='submit'>reboot device</button> </div> </form> </div> <div class='col-25 col-s-100'> </div> </div> <input name='version' value='1' hidden/> </form> <div class='footer'> <p>Copyright CyberReefGuru 2021</p></div></body></html>";
    const char *config_success="<body> <div class='t_row'> <div class='col-25 col-s-25'> </div> <div class='col-50 col-s-75'> <div class='header'>WiFi Sign Setup - Success</div> </div> <div class='col-25 col-s-100'> </div> </div> <form method=POST action='/save_config'> <div class='t_row'> <div class='col-25 col-s-25'> </div> <div class='col-50 col-s-75'> <div class='l_row'> Configuration saved! </div> </div> <div class='col-25 col-s-100'> </div> </div> </form> <div class='footer'> <p>Copyright CyberReefGuru 2021</p> </div> </body>";
    const char *config_failed="<body> <div class='t_row'> <div class='col-25 col-s-25'> </div> <div class='col-50 col-s-75'> <div class='header'>WiFi Sign Setup - Fail</div> </div> <div class='col-25 col-s-100'> </div> </div> <form method=POST action='/save_config'> <div class='t_row'> <div class='col-25 col-s-25'> </div> <div class='col-50 col-s-75'> <div class='l_row'> Configuration save failed! </div> </div> <div class='col-25 col-s-100'> </div> </div> </form> <div class='footer'> <p>Copyright CyberReefGuru 2021</p> </div> </body>";

};


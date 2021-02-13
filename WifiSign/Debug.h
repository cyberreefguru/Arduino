/**************************************************************************************************************************************
Sheepishly lifted from WiFiWebServer by K Hoang and I Grokhotkov
 ***************************************************************************************************************************************/
#pragma once

#include <stdio.h>

#define DEBUG_OUTPUT Serial

#define LOG_LEVEL_DISABLED	0
#define LOG_LEVEL_ERROR		1
#define LOG_LEVEL_WARN		2
#define LOG_LEVEL_INFO		3
#define LOG_LEVEL_DEBUG		4


// Change _LOG_LEVEL to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug

#ifndef _LOG_LEVEL
#define _LOG_LEVEL       4
#endif

///////////////////////////////////////

#define LOG_ERROR(x)         	if(_LOG_LEVEL>0) { DEBUG_OUTPUT.print("[ERROR] "); DEBUG_OUTPUT.println(x); }
#define LOG_ERROR0(x)        	if(_LOG_LEVEL>0) { DEBUG_OUTPUT.print(x); }
#define LOG_ERROR1(x,y)      	if(_LOG_LEVEL>0) { DEBUG_OUTPUT.print("[ERROR] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.println(y); }
#define LOG_ERROR2(x,y,z)    	if(_LOG_LEVEL>0) { DEBUG_OUTPUT.print("[ERROR] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.println(z); }
#define LOG_ERROR3(x,y,z,w)  	if(_LOG_LEVEL>0) { DEBUG_OUTPUT.print("[ERROR] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.print(z); DEBUG_OUTPUT.println(w); }
#define LOG_ERROR4(x,y,z,w,q)  	if(_LOG_LEVEL>3) { DEBUG_OUTPUT.print("[ERROR] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.print(z); DEBUG_OUTPUT.print(w); DEBUG_OUTPUT.println(q);}

///////////////////////////////////////

#define LOG_WARN(x)          	if(_LOG_LEVEL>1) { DEBUG_OUTPUT.print("[WARN] "); DEBUG_OUTPUT.println(x); }
#define LOG_WARN0(x)         	if(_LOG_LEVEL>1) { DEBUG_OUTPUT.print(x); }
#define LOG_WARN1(x,y)       	if(_LOG_LEVEL>1) { DEBUG_OUTPUT.print("[WARN] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.println(y); }
#define LOG_WARN2(x,y,z)     	if(_LOG_LEVEL>1) { DEBUG_OUTPUT.print("[WARN] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.println(z); }
#define LOG_WARN3(x,y,z,w)   	if(_LOG_LEVEL>1) { DEBUG_OUTPUT.print("[WARN] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.print(z); DEBUG_OUTPUT.println(w); }
#define LOG_WARN4(x,y,z,w,q)  	if(_LOG_LEVEL>3) { DEBUG_OUTPUT.print("[WARN] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.print(z); DEBUG_OUTPUT.print(w); DEBUG_OUTPUT.println(q);}

///////////////////////////////////////

#define LOG_INFO(x)          	if(_LOG_LEVEL>2) { DEBUG_OUTPUT.print("[INFO] "); DEBUG_OUTPUT.println(x); }
#define LOG_INFO0(x)         	if(_LOG_LEVEL>2) { DEBUG_OUTPUT.print(x); }
#define LOG_INFO1(x,y)       	if(_LOG_LEVEL>2) { DEBUG_OUTPUT.print("[INFO] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.println(y); }
#define LOG_INFO2(x,y,z)     	if(_LOG_LEVEL>2) { DEBUG_OUTPUT.print("[INFO] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.println(z); }
#define LOG_INFO3(x,y,z,w)		if(_LOG_LEVEL>2) { DEBUG_OUTPUT.print("[INFO] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.print(z); DEBUG_OUTPUT.println(w); }
#define LOG_INFO4(x,y,z,w,q)  	if(_LOG_LEVEL>3) { DEBUG_OUTPUT.print("[INFO] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.print(z); DEBUG_OUTPUT.print(w); DEBUG_OUTPUT.println(q);}

///////////////////////////////////////

#define LOG_DEBUG(x)         	if(_LOG_LEVEL>3) { DEBUG_OUTPUT.print("[DEBUG] "); DEBUG_OUTPUT.println(x); }
#define LOG_DEBUG0(x)        	if(_LOG_LEVEL>3) { DEBUG_OUTPUT.print(x); }
#define LOG_DEBUG1(x,y)      	if(_LOG_LEVEL>3) { DEBUG_OUTPUT.print("[DEBUG] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.println(y); }
#define LOG_DEBUG2(x,y,z)    	if(_LOG_LEVEL>3) { DEBUG_OUTPUT.print("[DEBUG] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.println(z); }
#define LOG_DEBUG3(x,y,z,w)  	if(_LOG_LEVEL>3) { DEBUG_OUTPUT.print("[DEBUG] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.print(z); DEBUG_OUTPUT.println(w); }
#define LOG_DEBUG4(x,y,z,w,q)  	if(_LOG_LEVEL>3) { DEBUG_OUTPUT.print("[DEBUG] "); DEBUG_OUTPUT.print(x); DEBUG_OUTPUT.print(y); DEBUG_OUTPUT.print(z); DEBUG_OUTPUT.print(w); DEBUG_OUTPUT.println(q);}


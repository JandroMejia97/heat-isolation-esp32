#ifndef __LIB_HTTP_SERVER_HTTP_SERVER_H__
#define __LIB_HTTP_SERVER_HTTP_SERVER_H__

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "SPIFFS.h"

#include "handler.h"
#include "credentials.h"

/**
 * @brief Initialize device
 */
void initialize();

/**
 * @brief Loop for handler
 */
void serverLoop();


#endif // __LIB_HTTP_SERVER_HTTP_SERVER_H__
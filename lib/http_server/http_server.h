#ifndef __LIB_HTTP_SERVER_HTTP_SERVER_H__
#define __LIB_HTTP_SERVER_HTTP_SERVER_H__

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "handler.h"
#include "credentials.h"

/**
 * @brief Initialize device
 */
void initialize();

#endif // __LIB_HTTP_SERVER_HTTP_SERVER_H__
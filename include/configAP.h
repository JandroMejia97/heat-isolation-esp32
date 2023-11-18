#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include "Server.cpp"
#include "ESP32_Utils_Server.cpp"

const char* ssid = "ESP32-Acces-Point";
const char* password = "12345678";
const char* hostname = "ESP32";
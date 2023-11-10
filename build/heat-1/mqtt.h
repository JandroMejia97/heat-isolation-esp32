#ifndef __LIB_MQTT_MQTT_H__
#define __LIB_MQTT_MQTT_H__

#include "UbidotsEsp32Mqtt.h"

/**
 * @brief ESP32 status
 */
typedef enum {
  INIT,
  CONNECTED,
  DISCONNECTED,
  READING_DATA,
  SENDING_DATA,
  DATA_ERROR,
} MQTT_Status_t;

/**
 * @brief Status of the MQTT client
 */
void setStatus(MQTT_Status_t status);

/**
 * @brief Function to get the status of the MQTT client
 * 
 * @return MQTT_Status_t - Status of the MQTT client
 */
MQTT_Status_t getStatus();

/**
 * @brief Function to check if the ESP32 is connected to the Ubidots MQTT broker
 * 
 * @return true - If the ESP32 is connected to the Ubidots MQTT broker
 * @return false - If the ESP32 is not connected to the Ubidots MQTT broker
 */
bool clientIsConnected();

/**
 * @brief Function to reconnect the ESP32 to the Ubidots MQTT broker
 */
void clientReconnect();

/**
 * @brief Client loop
 * 
 */
void clientLoop();

/**
 * @brief Function to publish data to the Ubidots MQTT broker
 * 
 * @param label - Label of the variable to publish
 * @param value - Value of the variable to publish
 */
void sendData(const char *label, const float value);

/**
 * @brief Function to get the status of the ESP32 as a string
 * 
 * @param status - Status of the ESP32
 * @return String - Status of the ESP32 as a string
 */
String MQTT_GetErrorAsString(MQTT_Status_t status);

/**
 * @brief Function to initialize the Ubidots client
 */
void initMQTT();


#endif // __LIB_MQTT_MQTT_H__

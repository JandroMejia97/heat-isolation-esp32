#ifndef __LIB_MQTT_MQTT_H__
#define __LIB_MQTT_MQTT_H__

#include "UbidotsEsp32Mqtt.h"

/**
 * @brief ESP32 status
 */
typedef enum {
  ESP_INIT,
  ESP_CONNECTED,
  ESP_DISCONNECTED,
  ESP_READING_DATA,
  ESP_SENDING_DATA,
  ESP_DATA_ERROR,
} MQTT_Status;

/**
 * @brief Status of the MQTT client
 */
void setStatus(MQTT_Status status);

/**
 * @brief Function to get the status of the MQTT client
 * 
 * @return MQTT_Status - Status of the MQTT client
 */
MQTT_Status getStatus();

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
void sendData(char *label, float value);

/**
 * @brief Function to get the status of the ESP32 as a string
 * 
 * @param status - Status of the ESP32
 * @return String - Status of the ESP32 as a string
 */
static String ESP_GetErrorAsString(MQTT_Status status);

/**
 * @brief Function to initialize the Ubidots client
 */
void initMQTT();


#endif // __LIB_MQTT_MQTT_H__

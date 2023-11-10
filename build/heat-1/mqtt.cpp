#include "mqtt.h"
#include "credentials.h"

#if defined WIFI_SSID_SECRETS 
  const char* WIFI_SSID = WIFI_SSID_SECRETS;
#else
  #error "WIFI_SSID not defined. Please define it before importing Ubidots library"
#endif

#if defined WIFI_PASSWORD_SECRETS
  const char* WIFI_PASSWORD = WIFI_PASSWORD_SECRETS;
#else
  #error "WIFI_PASSWORD not defined. Please define it before importing Ubidots library"
#endif

#if defined UBIDOTS_TOKEN_SECRETS
  const char* UBIDOTS_TOKEN = UBIDOTS_TOKEN_SECRETS;
#else
  #error "UBIDOTS_TOKEN not defined. Please define it before importing Ubidots library"
#endif

#ifndef PUBLISH_FREQUENCY
  #warning "PUBLISH_FREQUENCY not defined. Using default value"
  #define DEAFULT_PUBLISH_FREQUENCY 60000
  static const int PUBLISH_FREQUENCY = DEAFULT_PUBLISH_FREQUENCY;
#else
  static const int PUBLISH_FREQUENCY = PUBLISH_FREQUENCY;
#endif

// Set the DEVICE_LABEL
#ifndef DEVICE_LABEL
  #warning "DEVICE_LABEL not defined. Using default value"
  #define DEFAULT_DEVICE_LABEL "heater"
  const char *DEVICE_LABEL = DEFAULT_DEVICE_LABEL;
#else
  const char *DEVICE_LABEL = DEVICE_LABEL;
#endif

MQTT_Status_t MQTT_STATUS = INIT;

void setStatus(MQTT_Status_t status) {
  MQTT_STATUS = status;
}

MQTT_Status_t getStatus() {
  return MQTT_STATUS;
}

/**
 * @brief Public Ubidots client
 */
static Ubidots client(UBIDOTS_TOKEN);
static unsigned long timer;

bool clientIsConnected() {
  return client.connected();
}

void clientReconnect() {
  setStatus(DISCONNECTED);
  client.reconnect();
}

void clientLoop() {
  client.loop();
}

/**
 * @brief Callback function to handle the data received from the MQTT broker
 * 
 * @param topic - Topic where the data was published
 * @param payload - Data received
 * @param length - Length of the data received
 */
void callback(char *topic, byte *payload, unsigned int length);

void initMQTT() {
  Serial.println("MQTT - Initializing");
  // Connecting to a WiFi network
  client.connectToWifi(WIFI_SSID, WIFI_PASSWORD);
  // Set the debug mode
  client.setDebug(false);
  // Connecting to a mqtt broker
  client.setCallback(callback);
  client.setup();
  client.reconnect();
  setStatus(CONNECTED);
  timer = millis();
}

void sendData(const char *label, const float value) {
  Serial.println("MQTT - Sending data");
  // Check if the type is valid, and if the index is in range
  MQTT_STATUS = SENDING_DATA;
  // Add the value to the Ubidots client
  client.add(label, value);

  // Check if the time to publish the data has passed
  long diff = millis() - timer;
  diff = diff < 0 ? -diff : diff;
  if (diff > PUBLISH_FREQUENCY) {
    Serial.println("MQTT - Publishing data");
    // Publish the data to the Ubidots MQTT broker
    client.publish(DEVICE_LABEL);
    timer = millis();
  }
  setStatus(CONNECTED);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.printf("MQTT - Reading data - Topic: %s\n", topic);
  int i;
  char data[length];
  for (i = 0; i < length; i++) {
      data[i] = (char) payload[i];
  }
  data[i] = '\0';
  Serial.printf("Data: \n%s\n\n", data);
}

String MQTT_GetErrorAsString(MQTT_Status_t status) {
  switch (status) {
    case INIT:
      return "MQTT_INIT";
    case CONNECTED:
      return "MQTT_CONNECTED";
    case DISCONNECTED:
      return "MQTT_DISCONNECTED";
    case READING_DATA:
      return "MQTT_READING_DATA";
    case SENDING_DATA:
      return "MQTT_SENDING_DATA";
    case DATA_ERROR:
      return "MQTT_DATA_ERROR";
    default:
      return "MQTT_UNKNOWN_ERROR";
  }
}

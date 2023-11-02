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

MQTT_Status MQTT_STATUS = MQTT_INIT;

void setStatus(MQTT_Status status) {
  MQTT_STATUS = status;
}

MQTT_Status getStatus() {
  return MQTT_STATUS;
}

/**
 * @brief Public Ubidots client
 */
static Ubidots client(UBIDOTS_TOKEN);
static unsigned long timer;
static const int PUBLISH_FREQUENCY = 15000;

bool clientIsConnected() {
  return client.connected();
}

void clientReconnect() {
  setStatus(MQTT_DISCONNECTED);
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
  // Connecting to a WiFi network
  client.connectToWifi(WIFI_SSID, WIFI_PASSWORD);
  // Set the debug mode
  client.setDebug(true);
  // Connecting to a mqtt broker
  client.setCallback(callback);
  client.setup();
  client.reconnect();
  setStatus(MQTT_CONNECTED);
  timer = millis();
}

void publishData(char *label, int value) {
  // Check if the type is valid, and if the index is in range
  MQTT_STATUS = MQTT_SENDING_DATA;
  // Add the value to the Ubidots client
  client.add(label, value);

  // Check if the time to publish the data has passed
  long diff = millis() - timer;
  diff = diff < 0 ? -diff : diff;
  if (diff > PUBLISH_FREQUENCY) {
    // Publish the data to the Ubidots MQTT broker
    client.publish(DEVICE_LABEL);
    timer = millis();
  }
  setStatus(MQTT_CONNECTED);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  int i;
  char data[length];
  for (i = 0; i < length; i++) {
      data[i] = (char) payload[i];
  }
  data[i] = '\0';
  Serial.printf("Data: \n%s\n", data);
  Serial.println("-----------------------");
}

static String MQTT_GetErrorAsString(MQTT_Status status) {
  switch (status) {
    case MQTT_INIT:
      return "MQTT_INIT";
    case MQTT_CONNECTED:
      return "MQTT_CONNECTED";
    case MQTT_DISCONNECTED:
      return "MQTT_DISCONNECTED";
    case MQTT_READING_DATA:
      return "MQTT_READING_DATA";
    case MQTT_SENDING_DATA:
      return "MQTT_SENDING_DATA";
    case MQTT_DATA_ERROR:
      return "MQTT_DATA_ERROR";
    default:
      return "MQTT_UNKNOWN_ERROR";
  }
}

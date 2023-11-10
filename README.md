# ESP-32s Project with Arduino and PlatformIO

This project consists of an ESP-32s that uses Arduino libraries to receive data (temperature, air/soil humidity, and light intensity) through UART and send it to an MQTT server on Ubidots. PlatformIO is used as the integrated development environment (IDE).

## Before start

Before compiling the project, you must create a file named wifi_credentials.h with the following defined macros:

```c++
#define WIFI_SSID_SECRETS "<Network Name>"
#define WIFI_PASSWORD_SECRETS "<Password>"
#define UBIDOTS_TOKEN_SECRETS "<Ubidots Token>"
#define DEVICE_LABEL_SECRETS "<Device name>"
```

> ⚠️ Make sure to replace the example values with your own data before compiling.

## Libraries

The following libraries are used in this project:

* [Ubidots ESP MQTT](https://github.com/ubidots/esp32-mqtt/archive/refs/heads/main.zip)
* [PubSubClient](https://github.com/knolleary/pubsubclient/archive/refs/heads/master.zip)
* [Adafruit_MLX90614](https://github.com/adafruit/Adafruit-MLX90614-Library)
* [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)
* [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_DHT_Unified)

## References

* [Connect an ESP32-DevKitC to Ubidots over MQTT](https://help.ubidots.com/en/articles/748067-connect-an-esp32-devkitc-to-ubidots-over-mqtt)
* [MLX90614 Non-contact Infrared Temperature Sensor with ESP32](https://microcontrollerslab.com/mlx90614-non-contact-infrared-temperature-sensor-esp32/)
* [ESP32 with DHT11/DHT22 Temperature and Humidity Sensor using Arduino IDE](https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/)
* [FREERTOS en ESP32/ESP8266 (multi tarea)](https://www.electrosoftcloud.com/freertos-en-esp32-esp8266-multi-tarea/)
* [ESP32 Timers & Timer Interrupts](https://circuitdigest.com/microcontroller-projects/esp32-timers-and-timer-interrupts)
* [ESP32 Timers & Timer Interrupt Tutorial (Arduino IDE)](https://deepbluembedded.com/esp32-timers-timer-interrupt-tutorial-arduino-ide/)
* [PlatformIO project to Arduino IDE script](https://runningdeveloper.com/blog/platformio-project-to-arduino-ide)
# ESP-32s Project with Arduino and PlatformIO

This project consists of an ESP-32s that uses Arduino libraries to receive data (temperature, air/soil humidity, and light intensity) through UART and send it to an MQTT server on Ubidots. PlatformIO is used as the integrated development environment (IDE).

## Before start

Before compiling the project, you must create a file named *credentials.h* in the *http_server* folder with the following defined macros:

```c++
#define AP_SSID_SECRETS "YOUR_WIFI_SSID"
#define AP_PASSWORD_SECRETS "YOUR_WIFI_PASSWORD"
#define AP_HOSTNAME_SECRETS "YOUR_HOSTNAME"
```

> ⚠️ Make sure to replace the example values with your own data before compiling.

## Libraries

The following libraries are used in this project:

* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)
* [ArduinoJson](https://arduinojson.org/)
* [Adafruit_MLX90614](https://github.com/adafruit/Adafruit-MLX90614-Library)
* [DHT sensor library](https://github.com/adafruit/DHT-sensor-library)
* [Adafruit Unified Sensor](https://github.com/adafruit/Adafruit_DHT_Unified)

## References

* [MLX90614 Non-contact Infrared Temperature Sensor with ESP32](https://microcontrollerslab.com/mlx90614-non-contact-infrared-temperature-sensor-esp32/)
* [ESP32 with DHT11/DHT22 Temperature and Humidity Sensor using Arduino IDE](https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/)
* [PlatformIO project to Arduino IDE script](https://runningdeveloper.com/blog/platformio-project-to-arduino-ide)
* [ESP32 VSCode PlatformIO: Upload Files to Filesystem SPIFFS](https://randomnerdtutorials.com/esp32-vs-code-platformio-spiffs/)

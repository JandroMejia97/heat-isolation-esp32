# ESP-32s Project with Arduino and PlatformIO

This project consists of an ESP-32s that uses Arduino libraries to read the temperature and humidity of the environment and the temperature of an object. The ESP-32s has a web server that allows you to control a relay that is connected to a heating element.

## Before start

Before compiling the project, you must create a file named *credentials.h* in the *http_server* folder with the following defined macros:

```c++
#define AP_SSID_SECRETS "YOUR_WIFI_SSID"
#define AP_PASSWORD_SECRETS "YOUR_WIFI_PASSWORD"
#define AP_HOSTNAME_SECRETS "YOUR_HOSTNAME"
```

> [!WARNING]
> Make sure to replace the example values with your own data before compiling.

## Compiling and uploading

To compile and upload the project to the ESP-32s, you must have PlatformIO installed. If you don't have it, you can follow the instructions on the [official website](https://platformio.org/install/ide?install=vscode).

Once you have PlatformIO installed, you have build the file system image and upload it to the ESP-32s. To do this, you must follow the third step in the [official documentation](https://docs.platformio.org/en/latest/platforms/espressif32.html#uploading-files-to-file-system).

After that, you can compile and upload the project to the ESP-32s by clicking on the *Upload* button on the PlatformIO toolbar.

> [!NOTE]
> Before uploading the project, make sure that you have all the libraries installed. You can install them by clicking on the *Libraries* button on the PlatformIO toolbar and searching for the library name. Or you can download them from the [libraries](#libraries) section.

## Hardware

The following hardware is used in this project:

* ESP-32s
* MLX90614
* DHT11
* Relay
* Resistor 15W 1.2ΩJ

## Compile for Arduino IDE

To compile the project for Arduino IDE, you must run the following command:

```bash
source ./make-arduino.sh
```

This command will ask you for the version of the project that you want to assign to the output folder. The output folder will be named *build/heat-[version]*.

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

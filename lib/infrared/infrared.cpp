#include "infrared.h"

/** Initialize infrared sensor */
Adafruit_MLX90614 infrared;

void initInfrared() {
    Serial.println("MLX90614 - Initializing");
    infrared = Adafruit_MLX90614();
    if (!infrared.begin()) {
        Serial.println("MLX90614 - Could not find a valid sensor, check wiring!");
    } else {
        Serial.println("MLX90614 - Initialized");
    }
}

InfraredData readInfrared() {
    InfraredData data;
    data.ambientTemp = infrared.readAmbientTempC();
    data.objectTemp = infrared.readObjectTempC();
    return data;
}

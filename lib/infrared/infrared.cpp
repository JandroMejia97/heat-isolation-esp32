#include "infrared.h"

/** Initialize infrared sensor */
Adafruit_MLX90614 infrared = Adafruit_MLX90614();

void initInfrared() {
    Serial.println("MLX90614 - Initializing");
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

#include "infrared.h"

/** Initialize infrared sensor */
Adafruit_MLX90614 infrared = Adafruit_MLX90614();

void initInfrared() {
    infrared.begin();
}

InfraredData readInfrared() {
    InfraredData data;
    data.ambientTemp = infrared.readAmbientTempC();
    data.objectTemp = infrared.readObjectTempC();
    return data;
}

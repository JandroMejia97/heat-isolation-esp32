#include "dhts.h"

DHT internalDHT(INTERNAL_DHT_PIN, DHT_TYPE);
DHT roomDHT(ROOM_DHT_PIN, DHT_TYPE);
DHTReturn internalDHTReturn, roomDHTReturn;

void initDHTs() {
    Serial.println("DTH11 - Initializing");
    internalDHT.begin();
    roomDHT.begin();

    internalDHTReturn.tempLabel = INTERNAL_DHT_TEMP_LABEL;
    internalDHTReturn.humLabel = INTERNAL_DHT_HUM_LABEL;

    roomDHTReturn.tempLabel = ROOM_DHT_TEMP_LABEL;
    roomDHTReturn.humLabel = ROOM_DHT_HUM_LABEL;
    Serial.println("DTH11 - Initialized");
}

DHTReturn getInternalDHT() {
    return internalDHTReturn;
}

DHTReturn getRoomDHT() {
    return roomDHTReturn;
}

float getInternalTemperature() {
    return internalDHT.readTemperature();
}

float getInternalHumidity() {
    return internalDHT.readHumidity();
}

float getRoomTemperature() {
    return roomDHT.readTemperature();
}

float getRoomHumidity() {
    return roomDHT.readHumidity();
}


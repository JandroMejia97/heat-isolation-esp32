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
    internalDHTReturn.dht = &internalDHT;

    roomDHTReturn.tempLabel = ROOM_DHT_TEMP_LABEL;
    roomDHTReturn.humLabel = ROOM_DHT_HUM_LABEL;
    roomDHTReturn.dht = &roomDHT;
    Serial.println("DTH11 - Initialized");
}

DHTReturn getInternalDHT() {
    return internalDHTReturn;
}

DHTReturn getRoomDHT() {
    return roomDHTReturn;
}

DHT *getInternalDHTSensor() {
    return &internalDHT;
}

DHT *getRoomDHTSensor() {
    return &roomDHT;
}
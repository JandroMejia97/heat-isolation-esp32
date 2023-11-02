#include "dhts.h"

DHT internalDHT(INTERNAL_DHT_PIN, DHT_TYPE);
DHT roomDHT(ROOM_DHT_PIN, DHT_TYPE);
DHTReturn internalDHTReturn, roomDHTReturn;

void initDHTs() {
    internalDHTReturn.tempLabel = INTERNAL_DHT_TEMP_LABEL;
    internalDHTReturn.humLabel = INTERNAL_DHT_HUM_LABEL;
    internalDHTReturn.dht = &internalDHT;

    roomDHTReturn.tempLabel = ROOM_DHT_TEMP_LABEL;
    roomDHTReturn.humLabel = ROOM_DHT_HUM_LABEL;
    roomDHTReturn.dht = &roomDHT;
}

DHTReturn getInternalDHT() {
    return internalDHTReturn;
}

DHTReturn getRoomDHT() {
    return roomDHTReturn;
}
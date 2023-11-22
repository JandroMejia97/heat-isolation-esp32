#include "http_server.h"

void setup() {
    Serial.begin(115200);
    initialize();
}

void loop() {
    serverLoop();
}
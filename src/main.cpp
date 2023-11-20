#include "handler.h"

void setup() {
    Serial.begin(115200);
    initialize();
}

void loop() {
    loopHandler();
}
#include "tasks.h"

void setup() {
  Serial.begin(115200);
  initialSetup();
  setupTasks();
}

void loop() {
  // Empty
}
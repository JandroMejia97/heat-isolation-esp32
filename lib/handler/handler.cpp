#include "handler.h"

Dev_Status_t devStatus;

/** Data to send */
struct DataToSend {
    char *label;
    float value;
};

void readDhtTask(float temperature, const char *tempLabel);
void readDhtSensorTask(const char *tempLabel, DHT *sensor);
void readInfraredTask();
void compareDataTask();

/** Queue for sending data through MQTT */
QueueHandle_t dataQueue;

u_int8_t delayCounters = 0;
u_int8_t timerInterruptCounter = 0;

void initHandler() {
    Serial.println("Initial setup");
    devStatus = INIT;
    // Initialize sensors
    initInfrared();
    initDHTs();

    // Create queue for sending data
    dataQueue = xQueueCreate(DATA_QUEUE_SIZE, sizeof(DataToSend));

    if (dataQueue == NULL) {
        Serial.println("Failed to create data queue");
    } else {
        tasksEnabled = true;
    }
    pinMode(GPIO_NUM_4, OUTPUT);
    delay(1000);
    devStatus = OFF;
}

void statusHandler(void) {
    if (devStatus == OFF) {
        Serial.println("Device is off");
        digitalWrite(GPIO_NUM_4, LOW);
        return;
    }
    const float internalTemp = getInternalTemp();
    const float externalTemp = getExternalTemp();
    const float internalError = relError(desiredTemp, internalTemp);
    const float objectTemp = readInfrared().objectTemp;

    if (isnan(internalTemp) || isnan(objectTemp) || isnan(externalTemp)) {
        devStatus = ERROR;
    }
    switch(devStatus) {
        case ON:
            if (internalError <= MAX_RELATIVE_ERROR) {
                devStatus = COOLING_DOWN;
            } else if (desiredTemp > externalTemp) {
                devStatus = WARMING_UP;
            }
            Serial.printf("Status: %s\n", getDevStatusString());
            break;
        case OFF:
            if (digitalRead(GPIO_NUM_4) == HIGH) {
                digitalWrite(GPIO_NUM_4, LOW);
            }
            break;
        case WARMING_UP:
            if (internalError <= MAX_RELATIVE_ERROR || objectTemp >= MAX_OBJECT_TEMP) {
                Serial.println("Warming up done!");
                Serial.printf("Internal error: %f, Object temperature: %f\n", internalError, objectTemp);
                digitalWrite(GPIO_NUM_4, LOW);
                devStatus = WAITING;
            } else if (digitalRead(GPIO_NUM_4) == LOW) {
                Serial.println("Warming up...");
                digitalWrite(GPIO_NUM_4, HIGH);
            }
            break;
        case WAITING:
            if (internalError <= MAX_RELATIVE_ERROR) {
                devStatus = COOLING_DOWN;
            } else if (internalError > MAX_RELATIVE_ERROR && objectTemp < MIN_OBJECT_TEMP) {
                devStatus = WARMING_UP;
            }
            break;
        case COOLING_DOWN: {
            const float externalError = relError(externalTemp, internalTemp);
            if (externalError <= MAX_RELATIVE_ERROR) {
                devStatus = WAITING;
            }
            break;
        }
        case ERROR:
            Serial.printf("Error: Internal temperature: %f, Object temperature: %f, External temperature: %f\n", internalTemp, objectTemp, externalTemp);
            break;
   }
}

void loopHandler() {
    delay(10);
    delayCounters++;

    if (delayCounters % 250 == 0) {

        /* Serial.println("Status: " + String(getDevStatusString()));
        DHT* internalDHT = getInternalDHTSensor();
        readDhtSensorTask(INTERNAL_DHT_TEMP_LABEL, INTERNAL_DHT_HUM_LABEL, internalDHT);
        Serial.println("Status: " + String(getDevStatusString()));
        DHT* roomDHT = getRoomDHTSensor();
        readDhtSensorTask(ROOM_DHT_TEMP_LABEL, ROOM_DHT_HUM_LABEL, roomDHT);
        Serial.println("Status: " + String(getDevStatusString()));
        readInfraredTask();
        timerInterruptCounter++;
        delayCounters = 0; */
    }
}

void setDevStatus(Dev_Status_t status) {
    devStatus = status;
}

Dev_Status_t getDevStatus(void) {
    return devStatus;
}

const char *getDevStatusString(void) {
    switch(devStatus) {
        case INIT:
            return "INIT";
        case ON:
            return "ON";
        case OFF:
            return "OFF";
        case WARMING_UP:
            return "WARMING_UP";
        case WAITING:
            return "WAITING";
        case COOLING_DOWN:
            return "COOLING_DOWN";
        case ERROR:
            return "ERROR";
   }
}

void readDhtSensorTask(const char *tempLabel, DHT *sensor) {
    float temperature = sensor->readTemperature();
    readDhtTask(temperature, tempLabel);
}

void readDhtTask(float temperature, const char *tempLabel) {
    DataToSend tempToSend = DataToSend();
    tempToSend.label = (char *) tempLabel;
    tempToSend.value = 0;
    tempToSend.value = temperature;
    if (isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
    } else if (devStatus == COOLING_DOWN) {
        const TickType_t xTicksToWait = pdMS_TO_TICKS(10000);
        if (xQueueSend(dataQueue, &tempToSend, xTicksToWait) != pdPASS) {
            Serial.println("Failed to send temperature data to queue");
        }
    }
}

void readInfraredTask() {
    DataToSend objectDataToSend;
    objectDataToSend.label = INFRARED_OBJ_LABEL;

    InfraredData infraredData = readInfrared();
    objectDataToSend.value = infraredData.objectTemp;
    if (isnan(infraredData.objectTemp)) {
        Serial.println("Failed to read from infrared sensor!");
    } else if (devStatus == COOLING_DOWN) {
        const TickType_t xTicksToWait = pdMS_TO_TICKS(10000);
        if (xQueueSend(dataQueue, &objectDataToSend, xTicksToWait) != pdTRUE) {
            Serial.println("Failed to send object temperature data to queue");
        }
    }
}

float getInternalTemp() {
    return getInternalDHTSensor()->readTemperature();
}

float getExternalTemp() {
    return getRoomDHTSensor()->readTemperature();
}

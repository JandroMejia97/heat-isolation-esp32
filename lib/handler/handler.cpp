#include "handler.h"

u_int32_t coolingDownTimeInMS = DEFAULT_COOLING_DOWN_TIME_IN_MS;
u_int8_t desiredTemp = DEFAULT_DESIRED_TEMP_IN_C;

Dev_Status_t devStatus;

void readDhtSensorsTask();
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
    digitalWrite(GPIO_NUM_4, LOW);
    delay(1000);
    devStatus = OFF;
}

void statusHandler(void) {
    if (devStatus == OFF) {
        Serial.println("Device is off");
        digitalWrite(GPIO_NUM_4, LOW);
        return;
    }
    const InfraredData data = readInfrared();
    const float internalTemp = data.ambientTemp;
    const float externalTemp = getExternalTemp();
    const float internalError = relError(desiredTemp, internalTemp);

    if (isnan(internalTemp) || isnan(data.objectTemp) || isnan(externalTemp)) {
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
        case END:
        case OFF:
            if (digitalRead(GPIO_NUM_4) == HIGH) {
                digitalWrite(GPIO_NUM_4, LOW);
            }
            break;
        case WARMING_UP:
            Serial.printf("Internal error: %f, Object temperature: %f, Actual temperature: %f\n", internalError, data.objectTemp, data.ambientTemp);
            if (internalError <= MAX_RELATIVE_ERROR || data.objectTemp >= MAX_OBJECT_TEMP) {
                Serial.println("Warming up done!");
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
            } else if (internalError > MAX_RELATIVE_ERROR && data.objectTemp < MIN_OBJECT_TEMP) {
                devStatus = WARMING_UP;
            }
            break;
        case COOLING_DOWN: {
            const float externalError = relError(externalTemp, internalTemp);
            Serial.printf("Internal error: %.2f, External error: %.2f\n", internalError, externalError);
            if (externalError <= MAX_RELATIVE_ERROR) {
                Serial.println("Cooling down done!");
                digitalWrite(GPIO_NUM_4, LOW);
                devStatus = END;
            } else {
                Serial.println("Cooling down...");
            }
            break;
        }
        case ERROR:
            Serial.printf("Error: Internal temperature: %.2f, Object temperature: %.2f, External temperature: %.2f\n", internalTemp, data.objectTemp, externalTemp);
            break;
    }
}

void loopHandler() {
    delay(10);
    delayCounters++;

    if ((devStatus == ON || devStatus == WARMING_UP) && delayCounters % WARMING_UP_TIME_IN_MS == 0) {
        statusHandler();
    }

    if (devStatus == WAITING && delayCounters % WAITING_TIME_IN_MS == 0) {
        statusHandler();
    }

    if (devStatus == COOLING_DOWN && delayCounters % coolingDownTimeInMS == 0) {
        timerInterruptCounter++;
        delayCounters = 0;
    }

    if (timerInterruptCounter % coolingDownTimeInMS == 0) {
        for (int i = 0; i < 10000; i ++) {
            delay(1);
        }
        Serial.println("Status: " + String(getDevStatusString()));
        statusHandler();
        readDhtSensorsTask();
        timerInterruptCounter = 0;
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
        case END:
            return "END";
        case WARMING_UP:
            return "WARMING_UP";
        case WAITING:
            return "WAITING";
        case COOLING_DOWN:
            return "COOLING_DOWN";
        case ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

void readDhtSensorsTask() {
    DataToSend dataToSend;
    const float internalTemp = readInfrared().ambientTemp;
    const float externalTemp = getExternalTemp();
    const float internalError = relError(desiredTemp, internalTemp);

    if (isnan(internalTemp) || isnan(externalTemp)) {
        Serial.println("Failed to read from DHT sensor!");
    } else if (internalError > MAX_RELATIVE_ERROR) {
        dataToSend.internalTemp = internalTemp;
        dataToSend.externalTemp = externalTemp;
        if (xQueueSendToBack(dataQueue, &dataToSend, 0) != pdTRUE) {
            Serial.println("Failed to send data to queue");
        }
    }
}

void setCoolingDownTimeInMS(u_int32_t value) {
    coolingDownTimeInMS = value;
}

u_int8_t getDesiredTemp() {
    return desiredTemp;
}

void setDesiredTemp(u_int8_t value) {
    desiredTemp = value;
}

float getInternalTemp() {
    return getInternalDHTSensor()->readTemperature();
}

float getExternalTemp() {
    return getRoomDHTSensor()->readTemperature();
}

QueueHandle_t getResults() {
    return dataQueue;
}

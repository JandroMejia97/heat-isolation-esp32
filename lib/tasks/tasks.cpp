#include "tasks.h"

/** Data to send through MQTT */
struct DataToSend {
    char *label;
    float value;
};

void readDhtTask(float temperature, float humidity, const char *tempLabel, const char *humLabel);
void readInternalDhtTask(const char *tempLabel, const char *humLabel);
void readRoomDhtTask(const char *tempLabel, const char *humLabel);
void readInfraredTask();
void sendDataTask();
void compareDataTask();
void checkConnectionTask();
void IRAM_ATTR onTimer();

/** Queue for sending data through MQTT */
QueueHandle_t dataQueue;

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
const int timerTicks = 10000000;
int timerInterruptCounter = 0;

void initialSetup() {
    Serial.println("Initial setup");
    // Initialize sensors
    initInfrared();
    initDHTs();

    // Create queue for sending data
    dataQueue = xQueueCreate(40, sizeof(DataToSend));

    if (dataQueue == NULL) {
        Serial.println("Failed to create data queue");
    } else {
        tasksEnabled = true;
    }
    delay(1000);
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, false);
    /**
     * TimerTicks = (TIMER_INTERVAL_S) * (timer frequency / timer prescale)
     * TimerTicks = (5s) * (160MHz / 80) = 10,000,000
     */
    timerAlarmWrite(timer, timerTicks, true);
    timerAlarmEnable(timer);
}

void IRAM_ATTR onTimer(){
    if (tasksEnabled) {
        portENTER_CRITICAL_ISR(&timerMux);
        timerInterruptCounter++;

        /* if (timerInterruptCounter % 4 == 0) {
            checkConnectionTask();
        } */

        if (timerInterruptCounter % 3 == 0) {
            DHTReturn internalDHT = getInternalDHT();
            readInternalDhtTask(internalDHT.tempLabel, internalDHT.humLabel);
        }

        if (timerInterruptCounter % 3 == 0) {
            DHTReturn roomDHT = getRoomDHT();
            readRoomDhtTask(roomDHT.tempLabel, roomDHT.humLabel);
        }

        if (timerInterruptCounter % 3 == 0) {
            readInfraredTask();
        }

        if (timerInterruptCounter % 6 == 0) {
            timerInterruptCounter = 0;
        }
        portEXIT_CRITICAL_ISR(&timerMux);
    }
}

void readInternalDhtTask(const char *tempLabel, const char *humLabel) {
    Serial.printf("readDhtTask - %s\n", tempLabel);
    float temperature = getInternalTemperature();
    float humidity = getInternalHumidity();
    readDhtTask(temperature, humidity, tempLabel, humLabel);
}

void readRoomDhtTask(const char *tempLabel, const char *humLabel) {
    Serial.printf("readDhtTask - %s\n", tempLabel);
    float temperature = getRoomTemperature();
    float humidity = getRoomHumidity();
    readDhtTask(temperature, humidity, tempLabel, humLabel);
}

void readDhtTask(float temperature, float humidity, const char *tempLabel, const char *humLabel) {
    DataToSend tempToSend = DataToSend();
    DataToSend humToSend = DataToSend();
    tempToSend.label = (char *) tempLabel;
    tempToSend.value = 0;
    humToSend.label = (char *) humLabel;
    humToSend.value = 0;
    tempToSend.value = temperature;
    humToSend.value = humidity;
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
    } else {
        const TickType_t xTicksToWait = pdMS_TO_TICKS(10000);
        Serial.printf("Temperature: %.2f\n", temperature);
        if (xQueueSend(dataQueue, &tempToSend, xTicksToWait) != pdPASS) {
            Serial.println("Failed to send temperature data to queue");
        }
        Serial.printf("Humidity: %.2f\n", humidity);
        if (xQueueSend(dataQueue, &humToSend, xTicksToWait) != pdPASS) {
            Serial.println("Failed to send humidity data to queue");
        }
    }
}

void readInfraredTask() {
    DataToSend objectDataToSend;
    objectDataToSend.label = INFRARED_OBJ_LABEL;

    DataToSend ambientDataToSend;
    ambientDataToSend.label = INFRARED_ROOM_LABEL;
    Serial.printf("readInfraredTask - %s\n", objectDataToSend.label);
    InfraredData infraredData = readInfrared();
    objectDataToSend.value = infraredData.objectTemp;
    ambientDataToSend.value = infraredData.ambientTemp;
    if (isnan(infraredData.objectTemp) || isnan(infraredData.ambientTemp)) {
        Serial.println("Failed to read from infrared sensor!");
    } else {
        Serial.printf("Object temperature: %.2f\n", infraredData.objectTemp);
        const TickType_t xTicksToWait = pdMS_TO_TICKS(10000);
        if (xQueueSend(dataQueue, &objectDataToSend, xTicksToWait) != pdTRUE) {
            Serial.println("Failed to send object temperature data to queue");
        }
        Serial.printf("Ambient temperature: %.2f\n", infraredData.ambientTemp);
        if (xQueueSend(dataQueue, &ambientDataToSend, xTicksToWait) != pdTRUE) {
            Serial.println("Failed to send ambient temperature data to queue");
        }
    }
}

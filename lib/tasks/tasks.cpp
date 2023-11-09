#include "tasks.h"

/** Data to send through MQTT */
struct DataToSend {
    char *label;
    float value;
};

void readDhtTask(DHT *dht, const char *tempLabel, const char *humLabel);
void readInfraredTask();
void sendDataTask();
void compareDataTask();
void checkConnectionTask();
void IRAM_ATTR onTimer();

/** Queue for sending data through MQTT */
QueueHandle_t dataQueue;

hw_timer_t *timer = NULL;
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
        initMQTT();
        tasksEnabled = true;
    }

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &onTimer, false);
    /**
     * TimerTicks = (TIMER_INTERVAL_S) * (timer frequency / timer prescale)
     * TimerTicks = (5s) * (160MHz / 80) = 10,000,000
     */
    timerAlarmWrite(timer, 10000000, true);
    timerAlarmEnable(timer);
}

void IRAM_ATTR onTimer(){
    if (tasksEnabled) {
        timerInterruptCounter++;

        if (timerInterruptCounter % 2) {
            sendDataTask();
        }

        /* if (timerInterruptCounter % 4) {
            checkConnectionTask();
        } */

        if (timerInterruptCounter % 3) {
            DHTReturn internalDHT = getInternalDHT();
            readDhtTask(internalDHT.dht, internalDHT.tempLabel, internalDHT.humLabel);
        }

        if (timerInterruptCounter % 3) {
            DHTReturn roomDHT = getRoomDHT();
            readDhtTask(roomDHT.dht, roomDHT.tempLabel, roomDHT.humLabel);
        }

        if (timerInterruptCounter % 3) {
            readInfraredTask();
        }

        if (timerInterruptCounter % 6) {
            timerInterruptCounter = 0;
        }
    }
}

void readDhtTask(DHT *dht, const char *tempLabel, const char *humLabel) {
    DataToSend tempToSend = DataToSend();
    DataToSend humToSend = DataToSend();
    tempToSend.label = (char *) tempLabel;
    tempToSend.value = 0;
    humToSend.label = (char *) humLabel;
    humToSend.value = 0;

    Serial.printf("readDhtTask - %s\n", tempLabel);
    if (dht->read(true)) {
        Serial.println("Failed to read from DHT sensor!");
    }
    float temperature = dht->readTemperature();
    tempToSend.value = temperature;
    float humidity = dht->readHumidity();
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

void sendDataTask() {
    Serial.println("sendDataTask - Trying to send data");
    DataToSend dataToSend;
    if (xQueueReceive(dataQueue, &dataToSend, 0)) {
        Serial.printf("sendDataTask - %s: %.2f\n", dataToSend.label, dataToSend.value);
        sendData(dataToSend.label, dataToSend.value);
    } else {
        Serial.println("sendDataTask - No data to send");
    }
}

void checkConnectionTask() {
    Serial.println("checkConnectionTask - Checking connection");
    MQTT_Status_t status = getStatus();
    bool connected = clientIsConnected();
    Serial.printf("checkConnectionTask - Status: %s\n", MQTT_GetErrorAsString(status).c_str());
    if (!connected && status ==  CONNECTED) {
        tasksEnabled = false;
        clientReconnect();
        Serial.println("checkConnectionTask - Reconnecting");
    } else if (connected && status == DISCONNECTED) {
        tasksEnabled = true;
        setStatus(CONNECTED);
        Serial.println("checkConnectionTask - Connected");
    } else if (status ==  CONNECTED) {
        tasksEnabled = true;
        Serial.println("checkConnectionTask - Tasks enabled");
    }
}

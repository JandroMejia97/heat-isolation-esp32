#include "tasks.h"

/** Data to send through MQTT */
struct DataToSend {
    char *label;
    float value;
};

void readDhtTask(void *pvParameters);
void readInfraredTask(void *pvParameters);
void sendDataTask(void *pvParameters);
void compareDataTask(void *pvParameters);
void checkConnectionTask(void *pvParameters);

// Task handles for starting and stopping tasks
TaskHandle_t internalDhtTaskHandle = NULL;
TaskHandle_t roomDhtTaskHandle = NULL;
TaskHandle_t infraredTaskHandle = NULL;
TaskHandle_t sendDataTaskHandle = NULL;
TaskHandle_t compareDataTaskHandle = NULL;
TaskHandle_t checkConnectionTaskHandle = NULL;

/** Queue for sending data through MQTT */
QueueHandle_t dataQueue;

void initialSetup() {
    // Initialize sensors
    initInfrared();
    initDHTs();
    initMQTT();

    // Create queue for sending data
    dataQueue = xQueueCreate(30, sizeof(DataToSend));
}

void setupTasks() {
    // Create task to read temperature from DHT sensor
    DHTReturn internalDhtReturn = getInternalDHT();
    DHTReturn roomDhtReturn = getRoomDHT();
    xTaskCreate(
        readDhtTask,
        "readInternalDht",
        2048,
        &internalDhtReturn,
        0,
        &internalDhtTaskHandle
    );

    xTaskCreate(
        readDhtTask,
        "readroomDht",
        2048,
        &roomDhtReturn,
        0,
        &roomDhtTaskHandle
    );

    // Create task to read infrared sensor
    xTaskCreate(
        readInfraredTask,
        "readInfrared",
        2048,
        NULL,
        0,
        &infraredTaskHandle
    );

    // Create task to send data through MQTT
    xTaskCreate(
        sendDataTask,
        "sendData",
        2048,
        NULL,
        3,
        &sendDataTaskHandle
    );

    // Create task to compare data
    /* xTaskCreate(
      compareDataTask,
      "compareData",
      2048,
      NULL,
      1,
      &compareDataTaskHandle
    ); */

    // Create task to check connection
    xTaskCreate(
        checkConnectionTask,
        "checkConnection",
        2048,
        NULL,
        3,
        &checkConnectionTaskHandle
    );
}

void readDhtTask(void *pvParameters) {
    DHTReturn *dhtReturn = (DHTReturn *)pvParameters;
    DHT *dht = dhtReturn->dht;
    DataToSend tempToSend, humToSend;
    tempToSend.label = dhtReturn->tempLabel;
    tempToSend.value = 0;
    humToSend.label = dhtReturn->humLabel;
    humToSend.value = 0;

    while (1) {
        if (tasksEnabled) {
            float temperature = dht->readTemperature();
            float humidity = dht->readHumidity();
            if (isnan(temperature) || isnan(humidity)) {
                Serial.println("Failed to read from DHT sensor!");
            } else {
                const TickType_t xTicksToWait = pdMS_TO_TICKS(10000);
                tempToSend.value = temperature;
                Serial.printf("Temperature: %.2f\n", temperature);
                xQueueSend(dataQueue, &tempToSend, xTicksToWait);
                humToSend.value = humidity;
                Serial.printf("Humidity: %.2f\n", humidity);
                xQueueSend(dataQueue, &humToSend, xTicksToWait);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

void readInfraredTask(void *pvParameters) {
    DataToSend objectDataToSend;
    objectDataToSend.label = INFRARED_OBJ_LABEL;

    DataToSend ambientDataToSend;
    ambientDataToSend.label = INFRARED_ROOM_LABEL;

    while (1) {
        if (tasksEnabled) {
            InfraredData infraredData = readInfrared();
            if (isnan(infraredData.objectTemp) || isnan(infraredData.ambientTemp)) {
                Serial.println("Failed to read from infrared sensor!");
            } else {
                objectDataToSend.value = infraredData.objectTemp;
                ambientDataToSend.value = infraredData.ambientTemp;
                Serial.printf("Object temperature: %.2f\n", infraredData.objectTemp);
                Serial.printf("Ambient temperature: %.2f\n", infraredData.ambientTemp);
                const TickType_t xTicksToWait = pdMS_TO_TICKS(10000);
                xQueueSend(dataQueue, &objectDataToSend, xTicksToWait);
                xQueueSend(dataQueue, &ambientDataToSend, xTicksToWait);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

void sendDataTask(void *pvParameters) {
    while (1) {
        if (tasksEnabled) {
            DataToSend dataToSend;
            const TickType_t xTicksToWait = pdMS_TO_TICKS(15000);
            if (xQueueReceive(dataQueue, &dataToSend, xTicksToWait) == pdPASS) {
                Serial.printf("Sending data: %s: %.2f\n", dataToSend.label, dataToSend.value);
                sendData(dataToSend.label, dataToSend.value);
            } else {
                Serial.println("Queue is empty");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void checkConnectionTask(void *pvParameters) {
    while (1) {
        if (!clientIsConnected()) {
            tasksEnabled = false;
            Serial.println("MQTT client disconnected");
            clientReconnect();
        } else {
            tasksEnabled = true;
            Serial.println("WiFi connected");
            setStatus(CONNECTED);
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void stopTasks() {
    vTaskDelete(internalDhtTaskHandle);
    vTaskDelete(roomDhtTaskHandle);
    vTaskDelete(infraredTaskHandle);
    vTaskDelete(sendDataTaskHandle);
    vTaskDelete(compareDataTaskHandle);
}

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

// Statically allocate and initialize the spinlock
static portMUX_TYPE dhtSpinLock = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE infraredSpinLock = portMUX_INITIALIZER_UNLOCKED;
static portMUX_TYPE checkConnectionSpinLock = portMUX_INITIALIZER_UNLOCKED;

/** Queue for sending data through MQTT */
QueueHandle_t dataQueue;

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
    }
}

void setupTasks() {
    Serial.println("Setting up tasks");
    // Create task to read temperature from DHT sensor
    DHTReturn internalDhtReturn = getInternalDHT();
    DHTReturn roomDhtReturn = getRoomDHT();

    xTaskCreate(
        readDhtTask,
        "readroomDht",
        2048,
        &roomDhtReturn,
        0,
        &roomDhtTaskHandle
    );

    xTaskCreate(
        readDhtTask,
        "readInternalDht",
        2048,
        &internalDhtReturn,
        0,
        &internalDhtTaskHandle
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

    // Create task to compare data
    /* xTaskCreate(
      compareDataTask,
      "compareData",
      1024,
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
        1,
        &checkConnectionTaskHandle
    );

    Serial.println("Tasks set up");

     // Create task to send data through MQTT
    xTaskCreate(
        sendDataTask,
        "sendData",
        2048,
        NULL,
        1,
        &sendDataTaskHandle
    );

    // Start tasks
    vTaskStartScheduler();
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
            Serial.printf("readDhtTask - %s\n", dhtReturn->tempLabel);
            taskENTER_CRITICAL(&dhtSpinLock);
            float temperature = dht->readTemperature();
            tempToSend.value = temperature;
            float humidity = dht->readHumidity();
            humToSend.value = humidity;
            taskEXIT_CRITICAL(&dhtSpinLock);
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
        vTaskDelay(pdMS_TO_TICKS(15000));
    }
}

void readInfraredTask(void *pvParameters) {
    DataToSend objectDataToSend;
    objectDataToSend.label = INFRARED_OBJ_LABEL;

    DataToSend ambientDataToSend;
    ambientDataToSend.label = INFRARED_ROOM_LABEL;
    while (1) {
        if (tasksEnabled) {
            Serial.printf("readInfraredTask - %s\n", objectDataToSend.label);
            taskENTER_CRITICAL(&infraredSpinLock);
            InfraredData infraredData = readInfrared();
            objectDataToSend.value = infraredData.objectTemp;
            ambientDataToSend.value = infraredData.ambientTemp;
            taskEXIT_CRITICAL(&infraredSpinLock);
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
        vTaskDelay(pdMS_TO_TICKS(15000));
    }
}

void sendDataTask(void *pvParameters) {
    // Set wait time (in ticks)
    TickType_t xLastWakeTime;
    const TickType_t xFrequency =  pdMS_TO_TICKS(15000);
    while (1) {
        if (tasksEnabled) {
            // Get the actual execution tick
            xLastWakeTime = xTaskGetTickCount();
            Serial.println("sendDataTask - Trying to send data");
            DataToSend dataToSend;
            if (xQueueReceive(dataQueue, &dataToSend, 0)) {
                Serial.printf("sendDataTask - %s: %.2f\n", dataToSend.label, dataToSend.value);
                sendData(dataToSend.label, dataToSend.value);
            } else {
                Serial.println("sendDataTask - No data to send");
            }
            vTaskDelayUntil(&xLastWakeTime, xFrequency);
        } else {
            vTaskSuspend(NULL);
            Serial.println("Tasks are disabled");
        }
    }
}

void checkConnectionTask(void *pvParameters) {
    while (1) {
        Serial.println("checkConnectionTask - Checking connection");
        taskENTER_CRITICAL(&checkConnectionSpinLock);
        bool connected = clientIsConnected();
        MQTT_Status_t status = getStatus();
        taskEXIT_CRITICAL(&checkConnectionSpinLock);
        if (!connected && status ==  CONNECTED) {
            tasksEnabled = false;
            if (sendDataTaskHandle != NULL) {
                vTaskSuspend(sendDataTaskHandle);
            }
            clientReconnect();
            Serial.println("checkConnectionTask - Reconnecting");
        } else if (connected && status == DISCONNECTED) {
            tasksEnabled = true;
            setStatus(CONNECTED);
            if (sendDataTaskHandle != NULL) {
                vTaskResume(sendDataTaskHandle);
            }
            Serial.println("checkConnectionTask - Connected");
        } else if (status ==  CONNECTED) {
            tasksEnabled = true;
        }
        Serial.printf("checkConnectionTask - Status: %s\n", MQTT_GetErrorAsString(status).c_str());
        vTaskDelay(pdMS_TO_TICKS(20000));
    }
}

void stopTasks() {
    if (internalDhtTaskHandle != NULL) {
        vTaskDelete(internalDhtTaskHandle);
    }
    if (roomDhtTaskHandle != NULL) {
        vTaskDelete(roomDhtTaskHandle);
    }
    if (infraredTaskHandle != NULL) {
        vTaskDelete(infraredTaskHandle);
    }
    if (sendDataTaskHandle != NULL) {
        vTaskDelete(sendDataTaskHandle);
    }
    if (compareDataTaskHandle != NULL) {
        vTaskDelete(compareDataTaskHandle);
    }
}

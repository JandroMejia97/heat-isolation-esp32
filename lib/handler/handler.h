#ifndef __LIB_HANDLER_HANDLER_H__
#define __LIB_HANDLER_HANDLER_H__

/** Include libraries for sensors */
#include "infrared.h"
#include "dhts.h"
#include "utils.h"

#define DATA_QUEUE_SIZE 200
#define MAX_RELATIVE_ERROR 10
#define MAX_OBJECT_TEMP 70
#define MIN_OBJECT_TEMP 60
#define BLOCK_TIME_IN_MS 10
#define WARMING_UP_TIME_IN_MS 250
#define WAITING_TIME_IN_MS 500
#define DEFAULT_COOLING_DOWN_TIME_IN_MS 1000
#define DEFAULT_DESIRED_TEMP_IN_C 50

/** Flag if task should run */
static bool tasksEnabled = false;

/**
 * @brief Device status
 */
typedef enum {
  INIT,
  ON,
  OFF,
  WARMING_UP,
  WAITING,
  COOLING_DOWN,
  ERROR
} Dev_Status_t;

/**
 * @brief Data to send through HTTP
 */
struct DataToSend {
    float internalTemp;
    float externalTemp;
};

/**
 * @brief Get device status
 * 
 * @return Dev_Status_t 
 */
Dev_Status_t getDevStatus();

/**
 * @brief Set device status
 * 
 * @param status 
 */
void setDevStatus(Dev_Status_t status);

/**
 * @brief Set sampling time in ms
 */
void setCoolingDownTimeInMS(u_int32_t coolingDownTimeInMS);

/**
 * @brief Get desired temperature in C
 */
u_int8_t getDesiredTemp();

/**
 * @brief Set desired temperature in C
 */
void setDesiredTemp(u_int8_t desiredTemp);

/**
 * @brief Get results queue
 */
QueueHandle_t getResults();

/**
 * @brief Get device status as string
 * 
 * @return const char* 
 */
const char *getDevStatusString();

/**
 * @brief Initialize global variables and sensors
 */
void initHandler();

/**
 * @brief Handler for device status
 */
void loopHandler();

/**
 * @brief Get internal temperature
 */
float getInternalTemp();

/**
 * @brief Get external temperature
 */
float getExternalTemp();

#endif // __LIB_HANDLER_HANDLER_H__
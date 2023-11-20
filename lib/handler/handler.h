#ifndef __LIB_HANDLER_HANDLER_H__
#define __LIB_HANDLER_HANDLER_H__

/** Include libraries for sensors */
#include "infrared.h"
#include "dhts.h"
#include "utils.h"

#define BUFFER_SIZE 128
#define DATA_QUEUE_SIZE 100
#define MAX_RELATIVE_ERROR 10
#define MAX_OBJECT_TEMP 70
#define MIN_OBJECT_TEMP 60
#define BLOCK_TIME_IN_MS 10
#define WARMING_UP_TIME_IN_MS 250
#define WAITING_TIME_IN_MS 500
#define DEFAULT_COOLING_DOWN_TIME_IN_MS 1000

/** Flag if task should run */
static bool tasksEnabled = false;
static u_int8_t desiredTemp = 0;

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
 * @brief Get device status as string
 * 
 * @return const char* 
 */
const char *getDevStatusString();

/**
 * @brief Initialize global variables and sensors
 */
void initialize();

/**
 * @brief Handler for device status
 */
void loopHandler();

#endif // __LIB_HANDLER_HANDLER_H__
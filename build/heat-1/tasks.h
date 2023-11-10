#ifndef __LIB_TASKS_TASKS_H__
#define __LIB_TASKS_TASKS_H__

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

/** FreeRTOS config */
#define configMAX_TASK_NAME_LEN 20
#define configMAX_PRIORITIES    3

/** Include libraries for sensors */
#include "infrared.h"
#include "dhts.h"

/** MQTT libraries for publishing data */
#include "mqtt.h"

/** Flag if task should run */
static bool tasksEnabled = false;

/**
 * @brief Initialize global variables and sensors
 */
void initialSetup();

#endif // __LIB_TASKS_TASKS_H__
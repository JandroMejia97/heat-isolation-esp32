#ifndef __LIB_DHTS_DHTS_H__
#define __LIB_DHTS_DHTS_H__
#include <stdio.h>

#include "DHT.h"

#ifndef INTERNAL_DHT_PIN
#define INTERNAL_DHT_PIN GPIO_NUM_17
#endif

#ifndef INTERNAL_DHT_TEMP_LABEL
#define INTERNAL_DHT_TEMP_LABEL "internal_temp"
#endif

#ifndef INTERNAL_DHT_HUM_LABEL
#define INTERNAL_DHT_HUM_LABEL "internal_hum"
#endif

#ifndef ROOM_DHT_PIN
#define ROOM_DHT_PIN GPIO_NUM_16
#endif

#ifndef ROOM_DHT_TEMP_LABEL
#define ROOM_DHT_TEMP_LABEL "room_temp"
#endif

#ifndef ROOM_DHT_HUM_LABEL
#define ROOM_DHT_HUM_LABEL "room_hum"
#endif

#define DHT_TYPE DHT11

/**
 * @brief Struct for returning DHT data
 */
struct DHTReturn {
  char *tempLabel;
  char *humLabel;
  DHT *dht;
};

/**
 * @brief Initialize DHTs
 */
void initDHTs();

/**
 * @brief Get the Internal DHT object
 * 
 * @return DHTReturn 
 */
DHTReturn getInternalDHT();

/**
 * @brief Get the Room DHT object
 * 
 * @return DHTReturn 
 */
DHTReturn getRoomDHT();

DHT *getInternalDHTSensor();

DHT *getRoomDHTSensor();

#endif // __LIB_DHTS_DHTS_H__
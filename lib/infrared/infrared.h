#ifndef __LIB_INFRARED_INFRARED_H__
#define __LIB_INFRARED_INFRARED_H__
#include <Adafruit_MLX90614.h>

#ifndef INFRARED_SCL_PIN
#define INFRARED_SCL_PIN GPIO_NUM_21
#endif

#ifndef INFRARED_OBJ_LABEL
#define INFRARED_OBJ_LABEL "heater_object_temp"
#endif

#ifndef INFRARED_ROOM_LABEL
#define INFRARED_ROOM_LABEL "heater_room_temp"
#endif

#ifndef INFRARED_SDA_PIN
#define INFRARED_SDA_PIN GPIO_NUM_22
#endif

/**
 * @brief Struct for returning infrared data
 */
struct InfraredData {
    float ambientTemp;
    float objectTemp;
};

/**
 * @brief Initialize infrared sensor
 * 
 */
void initInfrared();

/**
 * @brief Read infrared sensor
 * 
 * @return InfraredData 
 */
InfraredData readInfrared();

#endif // __LIB_INFRARED_INFRARED_H__

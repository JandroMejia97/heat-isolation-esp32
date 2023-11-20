#ifndef __LIB_UTILS_UTILS_H__
#define __LIB_UTILS_UTILS_H__

#include <math.h>

float absError(float setpoint, float input);

float absFloat(float value);

float relError(float setpoint, float input);

float roundFloat(float value, int precision);

#endif // __LIB_UTILS_UTILS_H__
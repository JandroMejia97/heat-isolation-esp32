#include "utils.h"

float absError(float setpoint, float input) {
    return setpoint - input;
}

float absFloat(float value) {
    return value < 0 ? -value : value;
}

float relError(float setpoint, float input) {
    return absFloat(absError(setpoint, input)) / setpoint * 100;
}

float roundFloat(float value, int precision) {
    float multiplier = pow(10, precision);
    return round(value * multiplier) / multiplier;
}
#ifndef BSP_LINE_SENSORS_H
#define BSP_LINE_SENSORS_H

#include <stdbool.h>

#define NUM_OF_LINE_SENSORS 4

typedef enum {
   LINE_SENSOR_FL = 0,
   LINE_SENSOR_FR = 1,
   LINE_SENSOR_BL = 2, // TODO
   LINE_SENSOR_BR = 3,
} line_position_t;

void BSP_lineSensorsInit(void);

bool BSP_lineSensorIsReading(line_position_t sensor);

void BSP_lineSensorSet(line_position_t sensor, bool value);

#endif /* BSP_LINE_SENSORS_H */

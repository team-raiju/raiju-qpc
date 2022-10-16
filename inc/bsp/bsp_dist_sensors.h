#ifndef BSP_DIST_SENSORS_H
#define BSP_DIST_SENSORS_H

#include <stdbool.h>

#define NUM_OF_DIST_SENSORS 7

typedef enum {
   DIST_SENSOR_L = 5,
   DIST_SENSOR_FL = 4,
   DIST_SENSOR_DL = 0, // TODO
   DIST_SENSOR_F = 3,
   DIST_SENSOR_DR = 0, // TODO
   DIST_SENSOR_FR = 2,
   DIST_SENSOR_R = 1
} dist_position_t;

void BSP_distSensorsInit(void);

bool BSP_distSensorIsReading(dist_position_t sensor);

void BSP_distSensorSet(dist_position_t sensor, bool value);

void BSP_distSensorDisableAll();

#endif /* BSP_DIST_SENSORS_H */

#ifndef BSP_DIST_SENSOR_H
#define BSP_DIST_SENSOR_H

#include "bsp_gpio.h"

#define MAX_NUM_OF_DIST_SENSORS 9

typedef struct dist_sensor_gpio {
    io_port_t port;
    io_pin_t pin;
} dist_sensor_gpio_t;

io_port_t bsp_dist_sensor_port(uint8_t sensor_num);
io_pin_t bsp_dist_sensor_pin(uint8_t sensor_num);


#endif /* BSP_DIST_SENSOR_H */

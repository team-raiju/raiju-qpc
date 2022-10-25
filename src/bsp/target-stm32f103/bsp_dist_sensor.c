#include "bsp_dist_sensor.h"


dist_sensor_gpio_t dist_sensor_gpios[MAX_NUM_OF_DIST_SENSORS] = {
    {.pin = IO_PIN_0,  .port = IO_PORTB},  // Dist_1
    {.pin = IO_PIN_14, .port = IO_PORTC},  // Dist_2
    {.pin = IO_PIN_13, .port = IO_PORTC},  // Dist_3
    {.pin = IO_PIN_10, .port = IO_PORTC},  // Dist_4
    {.pin = IO_PIN_11, .port = IO_PORTC},  // Dist_5
    {.pin = IO_PIN_12, .port = IO_PORTC},  // Dist_6
    {.pin = IO_PIN_2,  .port = IO_PORTD},  // Dist_7
    {.pin = IO_PIN_4,  .port = IO_PORTB}, // Dist_8 - Disabled
    {.pin = IO_PIN_5,  .port = IO_PORTB}, // Dist_9 - Disabled
};

io_port_t bsp_dist_sensor_port(uint8_t sensor_num){

    if (sensor_num < MAX_NUM_OF_DIST_SENSORS){
        return dist_sensor_gpios[sensor_num].port;
    }

    return dist_sensor_gpios[0].port; // Error
    
}

io_pin_t bsp_dist_sensor_pin(uint8_t sensor_num){

    if (sensor_num < MAX_NUM_OF_DIST_SENSORS){
        return dist_sensor_gpios[sensor_num].pin;
    }

    return dist_sensor_gpios[0].pin; // Error
    
}
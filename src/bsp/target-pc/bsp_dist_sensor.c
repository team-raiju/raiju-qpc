#include "bsp_dist_sensor.h"


// Hardware GPIO mapping of Dist sensor inputs
dist_sensor_gpio_t dist_sensor_gpios[MAX_NUM_OF_DIST_SENSORS] = {
    {.pin = IO_PIN_0, .port = IO_PORTA}, // Dist_1
    {.pin = IO_PIN_1, .port = IO_PORTA}, // Dist_2
    {.pin = IO_PIN_2, .port = IO_PORTA}, // Dist_3
    {.pin = IO_PIN_3, .port = IO_PORTA}, // Dist_4
    {.pin = IO_PIN_4, .port = IO_PORTA}, // Dist_5
    {.pin = IO_PIN_5, .port = IO_PORTA}, // Dist_6
    {.pin = IO_PIN_6, .port = IO_PORTA}, // Dist_7
    {.pin = IO_PIN_7, .port = IO_PORTA}, // Dist_8 - Disabled
    {.pin = IO_PIN_8, .port = IO_PORTA}, // Dist_9 - Disabled
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
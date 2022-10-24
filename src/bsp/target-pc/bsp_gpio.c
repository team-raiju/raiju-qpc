

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "bsp_gpio.h"
#include "bsp_gpio_fake.h"

#define LINE_SENSOR_MAX_GPIO 9

static bsp_gpio_callback_t dist_callback_function = NULL;


void HAL_Fake_GPIO_EXTI_Callback(uint16_t sensor_num, bool sensor_level) {

    printf("Sensor Callback. Sensor = %d; Val = %d\r\n", sensor_num, sensor_level);


    if (dist_callback_function != NULL){
        dist_callback_function(sensor_num, sensor_level);
    }
}



void BSP_GPIO_Distance_Register_Callback(bsp_gpio_callback_t callback_function){

    dist_callback_function = callback_function;

}


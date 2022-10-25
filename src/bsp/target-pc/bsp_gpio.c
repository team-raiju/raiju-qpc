

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp_gpio.h"
#include "bsp_gpio_fake.h"

#define DIST_SENSOR_MAX_GPIOS 9
#define BUTTON_GPIO_PIN       IO_PIN_15


static bsp_gpio_callback_t dist_callback_function = NULL;
static bsp_button_callback_t button_callback_function = NULL;


static io_pin_t dist_sensor_pins[DIST_SENSOR_MAX_GPIOS] = {0};
static io_level_t gpio_emulation[4][16] = {0};
static uint8_t dist_sensor_amount = 0;


void HAL_Fake_GPIO_EXTI_Callback(uint16_t sensor_num) {

    printf("Sensor Callback. Pin = %d; Dist Sensor Pin = %d\r\n", sensor_num, dist_sensor_pins[sensor_num]);

    if (sensor_num == BUTTON_GPIO_PIN && button_callback_function != NULL){
        button_callback_function();
    }

    else if (dist_callback_function != NULL){
        dist_callback_function(dist_sensor_pins[sensor_num]);
    }

}

io_level_t BSP_GPIO_Read_Pin(io_port_t port, io_pin_t gpio_pin){
    return gpio_emulation[port][gpio_pin];
}

void BSP_GPIO_Write_Pin(io_port_t port, io_pin_t gpio_pin, io_level_t level){
    gpio_emulation[port][gpio_pin] = level;
}


void BSP_GPIO_Register_Distance_Callback(bsp_gpio_callback_t callback_function){

    dist_callback_function = callback_function;

}

void BSP_GPIO_Register_Button_Callback(bsp_button_callback_t callback_function){

    button_callback_function = callback_function;

}


void BSP_GPIO_Register_Distance_Pins(io_pin_t * pins, uint8_t amount){

    if (amount > DIST_SENSOR_MAX_GPIOS){
        return;
    }

    dist_sensor_amount = amount;

    memcpy(dist_sensor_pins, pins, dist_sensor_amount * sizeof(io_pin_t));

}


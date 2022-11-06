

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp_gpio.h"
#include "bsp_gpio_fake.h"
#include "bsp_gpio_mapping.h"


static bsp_gpio_dist_callback_t dist_callback_function = NULL;
static bsp_button_callback_t button_callback_function = NULL;
static io_level_t gpio_emulation[4][16] = {0};

static uint8_t gpio_dist_sensor_pins[BOARD_NUM_DIST_SENSORS] = {
    GPIO_DIST_SENSOR_1_PIN,
    GPIO_DIST_SENSOR_2_PIN,
    GPIO_DIST_SENSOR_3_PIN,
    // GPIO_DIST_SENSOR_4_PIN,
    // GPIO_DIST_SENSOR_5_PIN,
    GPIO_DIST_SENSOR_6_PIN,
    GPIO_DIST_SENSOR_7_PIN,
    GPIO_DIST_SENSOR_8_PIN,
    GPIO_DIST_SENSOR_9_PIN,
};

static uint8_t gpio_dist_sensor_ports[BOARD_NUM_DIST_SENSORS] = {
    GPIO_DIST_SENSOR_1_PORT,
    GPIO_DIST_SENSOR_2_PORT,
    GPIO_DIST_SENSOR_3_PORT,
    // GPIO_DIST_SENSOR_4_PORT,
    // GPIO_DIST_SENSOR_5_PORT,
    GPIO_DIST_SENSOR_6_PORT,
    GPIO_DIST_SENSOR_7_PORT,
    GPIO_DIST_SENSOR_8_PORT,
    GPIO_DIST_SENSOR_9_PORT,
};



static uint16_t BSP_GPIO_Pin_Mapping(io_pin_t io_pin){

    uint16_t hardware_pin_num[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    if (io_pin < sizeof(hardware_pin_num)/sizeof(uint16_t) ){
        return hardware_pin_num[io_pin];
    }

    return 0;

}


void HAL_Fake_GPIO_EXTI_Callback(uint16_t pin) {

    printf("Sensor Callback. Hardware Pin = %d\r\n", pin);

    // Button
    if (pin == BSP_GPIO_Pin_Mapping(GPIO_BUTTON_PIN)){
        if (button_callback_function != NULL){
            button_callback_function();
        }
        return;
    }

    // Distance Sensors
    for (int i = 0; i < BOARD_NUM_DIST_SENSORS; i++) {
        if (pin == BSP_GPIO_Pin_Mapping(gpio_dist_sensor_pins[i])){
            if (dist_callback_function != NULL){
                io_level_t state = BSP_GPIO_Read_Pin(gpio_dist_sensor_ports[i], gpio_dist_sensor_pins[i]);
                dist_callback_function(i, state);
            }
            return;
        }
    }

}

io_level_t BSP_GPIO_Read_Pin(io_port_t port, io_pin_t gpio_pin){
    return gpio_emulation[port][gpio_pin];
}

void BSP_GPIO_Write_Pin(io_port_t port, io_pin_t gpio_pin, io_level_t level){
    gpio_emulation[port][gpio_pin] = level;
}


void BSP_GPIO_Register_Distance_Callback(bsp_gpio_dist_callback_t callback_function){

    dist_callback_function = callback_function;

}

void BSP_GPIO_Register_Button_Callback(bsp_button_callback_t callback_function){

    button_callback_function = callback_function;

}


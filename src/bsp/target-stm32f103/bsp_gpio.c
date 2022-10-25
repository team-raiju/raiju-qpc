

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "bsp_gpio.h"
#include "gpio.h"


#define DIST_SENSOR_MAX_GPIOS 9


static bsp_gpio_callback_t dist_callback_function = NULL;

static io_pin_t dist_sensor_pins[DIST_SENSOR_MAX_GPIOS] = {0};
static uint8_t dist_sensor_amount = 0;


// Mapping from HAL to io_port_t
static GPIO_TypeDef* gpio_port_mapping[] = {
    GPIOA, //IO_PORTA
    GPIOB, //IO_PORTB
    GPIOC, //IO_PORTC
    GPIOD, //IO_PORTD
};

static uint16_t gpio_pin_mapping[] = {
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
};




void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

    if (dist_callback_function != NULL){
        for (int i = 0; i < dist_sensor_amount; i++) {
            if (GPIO_Pin == gpio_pin_mapping[dist_sensor_pins[i]]){
                dist_callback_function(dist_sensor_pins[i]);
                break;
            }
        }
    }
}

io_level_t BSP_GPIO_Read_Pin(io_port_t port, io_pin_t gpio_pin){
    return (io_level_t) HAL_GPIO_ReadPin(gpio_port_mapping[port], gpio_pin_mapping[gpio_pin]);
}

void BSP_GPIO_Write_Pin(io_port_t port, io_pin_t gpio_pin, io_level_t level){
    HAL_GPIO_WritePin(gpio_port_mapping[port], gpio_pin_mapping[gpio_pin], level);
}



void BSP_GPIO_Register_Distance_Callback(bsp_gpio_callback_t callback_function){

    dist_callback_function = callback_function;

}

void BSP_GPIO_Register_Distance_Pins(io_pin_t * pins, uint8_t amount){

    if (amount > DIST_SENSOR_MAX_GPIOS){
        return;
    }

    dist_sensor_amount = amount;

    memcpy(dist_sensor_pins, pins, dist_sensor_amount * sizeof(io_pin_t));

}

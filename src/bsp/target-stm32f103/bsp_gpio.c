

#include <stdbool.h>
#include <stdlib.h>

#include "bsp_gpio.h"
#include "gpio.h"


#define LINE_SENSOR_MAX_GPIO 9

typedef struct dist_sensor_gpio {
    GPIO_TypeDef * port;
    uint16_t pin;
} dist_sensor_gpio_t;

static bsp_gpio_callback_t dist_callback_function = NULL;


dist_sensor_gpio_t dist_sensor_gpios[LINE_SENSOR_MAX_GPIO] = {
    {.pin = GPIO_PIN_0,  .port = GPIOB}, // Dist_1
    {.pin = GPIO_PIN_14, .port = GPIOC}, // Dist_2
    {.pin = GPIO_PIN_13, .port = GPIOC}, // Dist_3
    {.pin = GPIO_PIN_10, .port = GPIOC}, // Dist_4
    {.pin = GPIO_PIN_11, .port = GPIOC}, // Dist_5
    {.pin = GPIO_PIN_12, .port = GPIOC}, // Dist_6
    {.pin = GPIO_PIN_2,  .port = GPIOD}, // Dist_7
    {.pin = GPIO_PIN_4,  .port = GPIOB}, // Dist_8
    {.pin = GPIO_PIN_5,  .port = GPIOB}, // Dist_9
};


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

    if (dist_callback_function != NULL){
        for (int i = 0; i < LINE_SENSOR_MAX_GPIO; i++) {
            if (GPIO_Pin == dist_sensor_gpios[i].pin){
                dist_callback_function(i, HAL_GPIO_ReadPin(dist_sensor_gpios[i].port, GPIO_Pin));
                break;
            }
        }
    }
}



void BSP_GPIO_Distance_Register_Callback(bsp_gpio_callback_t callback_function){

    dist_callback_function = callback_function;

}


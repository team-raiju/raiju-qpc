/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include "start_module.h"
#include "bsp_gpio.h"
#include "bsp_gpio_mapping.h"

void start_module_disable(void) {
    BSP_GPIO_Write_Pin(GPIO_START_MODULE_EN_PORT, GPIO_START_MODULE_EN_PIN, IO_LOW);
}

void start_module_enable(void) {
    BSP_GPIO_Write_Pin(GPIO_START_MODULE_EN_PORT, GPIO_START_MODULE_EN_PIN, IO_HIGH);
}

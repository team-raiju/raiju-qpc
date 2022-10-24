#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#include <stdint.h>
#include <stdbool.h>

typedef void( * bsp_gpio_callback_t) (uint16_t sensor_num, bool gpio_level);

void BSP_GPIO_Distance_Register_Callback(bsp_gpio_callback_t callback_function);


#endif /* BSP_GPIO_H */

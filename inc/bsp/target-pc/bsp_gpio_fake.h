#ifndef BSP_GPIO_FAKE_H
#define BSP_GPIO_FAKE_H

#include <stdint.h>

void HAL_Fake_GPIO_EXTI_Callback(uint16_t sensor_num, bool sensor_level);

#endif /* BSP_GPIO_FAKE_H */

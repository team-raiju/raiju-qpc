#ifndef BSP_GPIO_FAKE_H
#define BSP_GPIO_FAKE_H

#include <stdint.h>

void HAL_Fake_GPIO_EXTI_Callback(uint16_t pin);
void fake_init_dist_sensor_pins(void);

#endif /* BSP_GPIO_FAKE_H */

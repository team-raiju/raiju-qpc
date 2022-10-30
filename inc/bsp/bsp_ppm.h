#ifndef BSP_PPM_H
#define BSP_PPM_H

#include <stdint.h>

#define PPM_MIN_VALUE_MS     1000
#define PPM_STOPPED_VALUE_MS 1500
#define PPM_MAX_VALUE_MS     2000


typedef void( * bsp_ppm_callback_t) (uint8_t ppm_num, uint16_t ppm_value);

void bsp_ppm_init();
void bsp_ppm_start();
void bsp_ppm_stop();
uint16_t bsp_ppm_channel1();
uint16_t bsp_ppm_channel2();
uint16_t bsp_ppm_channel3();
uint16_t bsp_ppm_channel4();


void bsp_ppm_register_callback(bsp_ppm_callback_t callback_function);


#endif /* BSP_PPM_H */

#ifndef BSP_WS2812_H
#define BSP_WS2812_H

#include <stdint.h>

#define WS2812_MAX_LED_AMOUNT 16

void BSP_ws2812_init(void);
void BSP_ws2812_set(uint8_t num, uint8_t r, uint8_t g, uint8_t b);
void BSP_ws2812_send(void);

#endif /* BSP_WS2812_H */

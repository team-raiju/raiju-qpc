#ifndef BSP_LED_H
#define BSP_LED_H

#include <stdint.h>

void BSP_ledInit(void);
void BSP_ledOff(void);
void BSP_ledOn(void);
void BSP_ledToggle(void);
void BSP_ledStripe(uint8_t num, uint8_t r, uint8_t g, uint8_t b);
void BSP_ledStripeSetStrategyColor(uint8_t strategy_num);
void BSP_ledStripeSetAll(uint8_t r, uint8_t g, uint8_t b);
#endif /* BSP_LED_H */

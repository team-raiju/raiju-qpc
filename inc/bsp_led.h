#ifndef BSP_LED_H
#define BSP_LED_H

void BSP_ledInit(void);
void BSP_ledOff(void);
void BSP_ledOn(void);
void BSP_ledToggle(void);
void BSP_ledStrip(int num, int stat);

#endif /* BSP_LED_H */

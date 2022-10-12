#include <stdbool.h>
#include "bsp_led.h"
#include "main.h"
#include "gpio.h"


void BSP_ledInit(void) {
}


void BSP_ledOff(void) { 
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}



void BSP_ledOn(void)  { 
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

void BSP_ledToggle(void)  {
    static bool toggle = false;
    if (toggle){
        BSP_ledOff();
    } else {
        BSP_ledOn();
    }
    toggle = !toggle;

}


void BSP_ledStrip(int num, int stat) {
 
}
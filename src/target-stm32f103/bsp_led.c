#include <stdbool.h>
#include "bsp_led.h"
#include "main.h"
#include "gpio.h"


void BSP_ledInit(void) {
}


void BSP_ledOff(void) { 
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
}



void BSP_ledOn(void)  { 
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
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


void BSP_ledStripe(uint8_t num, uint8_t r, uint8_t g, uint8_t b) {
 
}

void BSP_ledStripeSetAll(uint8_t r, uint8_t g, uint8_t b){
    
}


void BSP_ledStripeSetStrategyColor(uint8_t strategy_num){

}
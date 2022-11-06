#include "bsp_buzzer.h"
#include "tim.h"

#define CPU_FREQUENCY 72000000


void BSP_buzzerInit(){
    MX_TIM12_Init();
}


void BSP_buzzerStart(){
    
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);

}

void BSP_buzzerStop(){
    
    HAL_TIM_PWM_Stop(&htim12, TIM_CHANNEL_1);

}

void BSP_buzzerSetPwmDutyCycle(uint8_t duty_cycle){
    
    __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_1, duty_cycle);
    
}

void BSP_buzzerSetFrequency(uint16_t frequency_hz){
    // frequency_hz = (CPU_FREQUENCY/ 100 * (preescaler_value + 1)) ->
    
    uint16_t preescaler_value = ((CPU_FREQUENCY/100) / frequency_hz) - 1;
    __HAL_TIM_SET_PRESCALER(&htim12, preescaler_value);
}
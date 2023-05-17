#include "tim.h"
#include "gpio.h"
#include "bsp_timer_input.h"
#include <stdbool.h>

void bsp_timer_5_init()
{
    MX_TIM5_Init();
    bsp_timer_5_start();
}

void bsp_timer_5_start()
{
    HAL_TIM_Base_Start(&htim5);
    HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_3);
    __HAL_TIM_SET_COUNTER(&htim5, 0);
}

void bsp_timer_5_stop()
{
    HAL_TIM_Base_Stop_IT(&htim5);
}

static uint16_t command_list[7];
static uint8_t command;
uint8_t cnt = 0;
bool packet_started = false;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {

    static uint16_t last_counter = 0;
    if (htim->Instance == TIM5) {

        uint16_t current_counter = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
        uint16_t diff = current_counter - last_counter;
        last_counter = current_counter;

        if (packet_started) {

            if (diff > 1150 && diff < 1300) {
                command_list[cnt] = 0;
            } else {
                command_list[cnt] = 1;
            }

            cnt++;

            if (cnt >= 7) {
                cnt = 0;
                packet_started = false;
                command = 0;
                for (int i = 0; i < 7; i++)
                {
                    command |= command_list[i] << i;
                }
                
            }
        }
        
        else if (diff > 2900 && diff < 3100) {
            cnt = 0;
            packet_started = true;
        }


    }

}
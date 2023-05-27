/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdbool.h>
#include "tim.h"
#include "gpio.h"
#include "bsp_tim_input_capture.h"
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/
#define TIM_NUMBER     TIM4
#define TIM_CHANNEL    TIM_CHANNEL_1

#define TIM_INSTANCE   (&htim4)
/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
static bsp_tim_capture_callback capture_callback = NULL;
static uint16_t last_counter = 0;
/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
    if (htim->Instance == TIM_NUMBER) {

        uint16_t current_counter = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL);
        uint16_t diff = current_counter - last_counter;
        last_counter = current_counter;

        if(capture_callback != NULL){
            capture_callback(diff);
        }
    }
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/
void bsp_tim_input_capture_init()
{
    MX_TIM4_Init();
    bsp_tim_input_capture_start();
}

void bsp_tim_input_capture_start()
{
    HAL_TIM_Base_Start(TIM_INSTANCE);
    HAL_TIM_IC_Start_IT(TIM_INSTANCE, TIM_CHANNEL);
    __HAL_TIM_SET_COUNTER(TIM_INSTANCE, 0);
}

void bsp_tim_input_capture_stop()
{
    HAL_TIM_Base_Stop_IT(TIM_INSTANCE);
}

void bsp_tim_capture_register_callback(bsp_tim_capture_callback callback_function){
    capture_callback = callback_function;
}

void bsp_tim_set_capture_level(bsp_tim_capture_edge_t edge)
{
    if (edge == BSP_TIM_RISING_EDGE) {
        __HAL_TIM_SET_CAPTUREPOLARITY(TIM_INSTANCE, TIM_CHANNEL, TIM_INPUTCHANNELPOLARITY_RISING);
    } else {
        __HAL_TIM_SET_CAPTUREPOLARITY(TIM_INSTANCE, TIM_CHANNEL, TIM_INPUTCHANNELPOLARITY_FALLING);
    }
}
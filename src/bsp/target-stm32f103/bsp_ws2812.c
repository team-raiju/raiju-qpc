/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdbool.h>
#include "bsp_ws2812.h"
#include "main.h"
#include "gpio.h"
#include "dma.h"
#include "tim.h"

#include "qpc.h"
#include "qk_port.h"


/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/
#define WS2812_PACKET_SIZE    24
#define FINAL_PADDING_SIZE 2 /* Used to guarantee that no extra bits will be sent at the final of the data stream */
#define WS2812_PWM_SIZE ((WS2812_PACKET_SIZE * WS2812_MAX_LED_AMOUNT) + FINAL_PADDING_SIZE)


#define PWM_BIT_1 12
#define PWM_BIT_0 6

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/
static uint32_t color_to_bits(uint8_t r, uint8_t g, uint8_t b);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

uint32_t led_data[WS2812_MAX_LED_AMOUNT];
uint16_t pwm_data[WS2812_PWM_SIZE];

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/
static uint32_t color_to_bits(uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t bits = 0;

    // blue
    for (int8_t i = 7; i >= 0; i--) {
        if (b & (1 << i)) {
            bits |= (1 << (23 - (7 - i)));
        }
    }

    // red
    for (int8_t i = 7; i >= 0; i--) {
        if (r & (1 << i)) {
            bits |= (1 << (15 - (7 - i)));
        }
    }

    // green
    for (int8_t i = 7; i >= 0; i--) {
        if (g & (1 << i)) {
            bits |= (1 << (7 - (7 - i)));
        }
    }

    return bits;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    QK_ISR_ENTRY();
    if (htim->Instance == TIM2) {
        HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
    }
    QK_ISR_EXIT();
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void BSP_ws2812_init()
{
    MX_TIM2_Init(); // Led Stripe Timer
}

void BSP_ws2812_set(uint8_t num, uint8_t r, uint8_t g, uint8_t b)
{
    if (num >= WS2812_MAX_LED_AMOUNT) {
        return;
    }

    led_data[num] = color_to_bits(r, g, b);
}

void BSP_ws2812_send()
{
    for (uint8_t i = 0; i < WS2812_MAX_LED_AMOUNT; i++) {
        for (int8_t j = (WS2812_PACKET_SIZE - 1); j >= 0; j--) {
            if (led_data[i] & (1 << j)) {
                pwm_data[(i * WS2812_PACKET_SIZE) + j] = PWM_BIT_1;
            } else {
                pwm_data[(i * WS2812_PACKET_SIZE) + j] = PWM_BIT_0;
            }
        }
    }

    for (int i = 0; i < FINAL_PADDING_SIZE; i++) {
        pwm_data[(WS2812_PACKET_SIZE * WS2812_MAX_LED_AMOUNT) + i] = 0;
    }

    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)pwm_data, WS2812_PWM_SIZE);
}

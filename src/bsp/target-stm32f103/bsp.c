#include <stdlib.h> /* for exit() */
#include <stdio.h>

#include "qpc.h" /* QP/C framework API */
#include "bsp.h" /* Board Support Package interface */
#include "main.h"
#include "gpio.h"
#include "dma.h"
#include "tim.h"
#include "adc.h"
#include "driving_service.h"
#include "buzzer_service.h"
#include "bsp_eeprom.h"
#include "bsp_i2c.h"
#include "bsp_gpio_mapping.h"
#include "bsp_gpio.h"

static uint32_t ms_counter = 0;
static uint32_t BSP_GetTick_us(void);

__weak void SystemClock_Config(void);

int _write(int file, char *ptr, int len)
{
  /* Implement your write code here, this is used by puts and printf for example */
  int i=0;
  for(i=0 ; i<len ; i++)
    ITM_SendChar((*ptr++));
  return len;
}

void SysTick_Handler(void)
{ /* system clock tick ISR */

    HAL_IncTick();

    QK_ISR_ENTRY(); /* inform QK about entering an ISR */

    QTIMEEVT_TICK_X(0U, &l_SysTick_Handler); /* process time events for rate 0 */

    QK_ISR_EXIT(); /* inform QK about exiting an ISR */
}

void BSP_init(void)
{
    HAL_Init();

    SystemClock_Config();
    SystemCoreClockUpdate();

    MX_GPIO_Init();
    MX_DMA_Init();

    MX_TIM6_Init();
    HAL_TIM_Base_Start_IT(&htim6);

    bsp_i2c_init(IO_I2C_1);

    HAL_Delay(20);

    BSP_eeprom_init();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // Buzzer interrupt
    QK_ISR_ENTRY();
    if (htim->Instance == TIM3) {
        BSP_GPIO_Toggle_Pin(IO_PORTB, IO_PIN_14);
    }

    // 0.1ms Timer
    if (htim->Instance == TIM6) {
        ms_counter += 6554;
    }

    QK_ISR_EXIT();
}

void delay_ms(uint32_t delay){
    uint32_t delay_us = 1000 * delay;

    uint32_t start = BSP_GetTick_us();
    volatile uint32_t wait = delay_us;

    if (wait < HAL_MAX_DELAY) {
        wait += 1;
    }

    while ((BSP_GetTick_us() - start) < wait) {
        __NOP();
    }
}

/* callback functions needed by the framework ------------------------------*/
void QF_onStartup(void)
{
    /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* All the code below is executed in HAL auto generated files */

    /* assing all priority bits for preemption-prio. and none to sub-prio. */
    // NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    // HAL_NVIC_SetPriority(SysTick_IRQn, QF_AWARE_ISR_CMSIS_PRI + 1, 0U);
    // Min possible value is QF_AWARE_ISR_CMSIS_PRI. Read
    // https://www.state-machine.com/doc/AN_QP_and_ARM-Cortex-M.pdf
    // For more details about interupt configuration
}

void QF_onCleanup(void)
{
}

void QK_onIdle(void)
{ /* called with interrupts enabled */

    /* We don't need to save power in this application */
    /* toggle an LED on and then off (not enough LEDs, see NOTE01) */
    // QF_INT_DISABLE();
    // QF_INT_ENABLE();
}

Q_NORETURN Q_onAssert(char const *const module, int_t const loc)
{
    (void)module;
    (void)loc;

    drive(0, 0);
    driving_disable();
    buzzer_start();

    volatile uint32_t timeout = 10000000;
    while (timeout > 0) {
        timeout--;
    }

    NVIC_SystemReset();
}

uint32_t BSP_GetTick(void)
{
    return HAL_GetTick();
}

static uint32_t BSP_GetTick_us(void)
{
    return (ms_counter * 1000) + (__HAL_TIM_GET_COUNTER(&htim6) * 100);
}

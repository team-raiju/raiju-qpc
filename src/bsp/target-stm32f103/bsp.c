#include <stdlib.h> /* for exit() */

#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */
#include "main.h"
#include "gpio.h"
#include "dma.h"
#include "tim.h"
#include "adc.h"
#include "driving_service.h"
#include "buzzer_service.h"
#include "bsp_eeprom.h"

__weak void SystemClock_Config(void);

void SysTick_Handler(void) {   /* system clock tick ISR */

    HAL_IncTick();

    QK_ISR_ENTRY();   /* inform QK about entering an ISR */

    QTIMEEVT_TICK_X(0U, &l_SysTick_Handler); /* process time events for rate 0 */

    QK_ISR_EXIT();             /* inform QK about exiting an ISR */
}


void BSP_init(void)   {

    HAL_Init();

    SystemClock_Config();
    SystemCoreClockUpdate();

    MX_GPIO_Init();
    MX_DMA_Init();

    HAL_Delay(20);

    BSP_eeprom_init();

}


/* callback functions needed by the framework ------------------------------*/
void QF_onStartup(void) {
        /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set priorities of ALL ISRs used in the system, see NOTE00
    *
    * !!!!!!!!!!!!!!!!!!!!!!!!!!!! CAUTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    * Assign a priority to EVERY ISR explicitly by calling NVIC_SetPriority().
    * DO NOT LEAVE THE ISR PRIORITIES AT THE DEFAULT VALUE!
    */
    NVIC_SetPriority(SysTick_IRQn,   QF_AWARE_ISR_CMSIS_PRI + 1);
    /* ... */

    /* enable IRQs... */
    
}


void QF_onCleanup(void) {

}

void QK_onIdle(void) { /* called with interrupts enabled */

    /* toggle an LED on and then off (not enough LEDs, see NOTE01) */
    QF_INT_DISABLE();
    QF_INT_ENABLE();
}


Q_NORETURN Q_onAssert(char const * const module, int_t const loc) {
    
    (void)module;
    (void)loc;

    drive(0, 0);
    driving_disable();
    buzzer_start();

    while (1);
    
    // NVIC_SystemReset();
}

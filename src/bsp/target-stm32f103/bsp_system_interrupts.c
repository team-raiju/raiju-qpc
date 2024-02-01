#include "main.h"
#include "bsp_system_interrupts.h"
#include "driving_service.h"
#include "buzzer_service.h"

void HardFault_Handler(void)
{
    drive(0, 0);
    driving_disable();
    buzzer_start();

    uint32_t timeout = 4000000;
    while (timeout > 0) {
        if (timeout % 100000 == 0) {
            buzzer_toggle();
        }
        timeout--;
    }

    NVIC_SystemReset();
}
#include <stdio.h>  
#include "qpc.h"  
#include "bsp.h"

#include "bsp_motors.h"

#ifdef Q_SPY
#include "qs_defines.h"
#endif

void BSP_buzzerInit(void){
    printf("Buzzer init \r\n"); 
}

void BSP_buzzerBeep(void) {
    printf("Buzzer Beep\n");
    QS_BEGIN_ID(SIMULATOR, AO_SumoHSM->prio)
       QS_I8(1, 1);
       QS_I8(1, 1);
    QS_END()

}
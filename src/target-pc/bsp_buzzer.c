#include "bsp_motors.h"


#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */
#include <stdio.h>  /* for printf()/fprintf() */
#include <stdlib.h> /* for exit() */

#ifdef Q_SPY
#include "qs_defines.h"
#endif

void BSP_buzzerInit(void){
    printf("Buzzer init \r\n"); 

}

void BSP_buzzer_beep(void) {
    printf("Buzzer Beep\n");
    QS_BEGIN_ID(SIMULATOR, AO_SumoHSM->prio)
       QS_I8(1, 1);
       QS_I8(1, 1);
    QS_END()

}
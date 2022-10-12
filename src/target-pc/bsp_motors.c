#include "bsp_motors.h"


#include "qpc.h"    /* QP/C framework API */
#include "bsp.h"    /* Board Support Package interface */
#include <stdio.h>  /* for printf()/fprintf() */
#include <stdlib.h> /* for exit() */

#ifdef Q_SPY
#include "qs_defines.h"
#endif

void BSP_motorsInit(void){
    printf("Motors init \r\n"); 

}

void BSP_motors(int vel_esq, int vel_dir) {

    if (vel_esq > 100){
        vel_esq = 100;
    } else if (vel_esq < -100){
        vel_esq = -100;
    }

    if (vel_dir > 100){
        vel_dir = 100;
    } else if (vel_dir < -100){
        vel_dir = -100;
    }

    printf("MOT %d,%d \n", vel_esq, vel_dir); 
    QS_BEGIN_ID(SIMULATOR, AO_SumoHSM->prio)
       QS_I8(1, 2);
       QS_I8(1, vel_esq);
       QS_I8(1, vel_dir);
    QS_END()

}
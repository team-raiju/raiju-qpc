#include <stdio.h>  
#include "qpc.h"  
#include "bsp.h"
#include "bsp_led.h"

#ifdef Q_SPY
#include "qs_defines.h"
#endif

void BSP_ledInit(void) {
    printf("LED INIT\n"); 
}


void BSP_ledOff(void) { 
    printf("LED OFF\n"); 
    QS_BEGIN_ID(SIMULATOR, AO_SumoHSM->prio)
       QS_I8(1, 0);
       QS_I8(1, 0);
    QS_END()
}



void BSP_ledOn(void)  { 
    printf("LED ON\n");  
    QS_BEGIN_ID(SIMULATOR, AO_SumoHSM->prio)
       QS_I8(1, 0);
       QS_I8(1, 1);
    QS_END()

}

void BSP_ledToggle(void)  {
    static bool toggle = false;
    if (toggle){
        BSP_ledOff();
    } else {
        BSP_ledOn();
    }
    toggle = !toggle;

}


void BSP_ledStrip(int num, int stat) {

    if (stat){
        printf("Led Strip Num %d ON\n", num); 
    } else {
        printf("Led Strip Num %d OFF\n", num); 
    }

}
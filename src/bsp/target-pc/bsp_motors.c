#include <stdio.h>  
#include "qpc.h"  
#include "bsp.h"
#include "bsp_motors.h"
#include "utils.h"

#ifdef Q_SPY
#include "qs_defines.h"
#endif

void BSP_motorsInit(void){
    printf("Motors init \r\n"); 
}

void BSP_motors(int16_t vel_left, int16_t vel_right) {

    vel_left = constrain(vel_left, -99, 99);
    vel_right = constrain(vel_right, -99, 99);

    // deadzone
    if (abs(vel_left) < 10) {
        vel_left = 0;
    }

    if (abs(vel_right) < 10) {
        vel_right = 0;
    }


    printf("MOT %d,%d \n", vel_left, vel_right); 
    QS_BEGIN_ID(SIMULATOR, AO_SumoHSM->prio)
       QS_I8(1, QS_MOTOR_ID);
       QS_I8(1, vel_left);
       QS_I8(1, vel_right);
    QS_END()

}
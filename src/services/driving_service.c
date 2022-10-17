#include "driving_service.h"
#include "bsp_motors.h"
// #include "bsp_gpio"
#include "utils.h"

#define MAX_SPEED 100


void driving_init(){
    BSP_motorsInit();
}

void driving_enable(){

}

void driving_disable(){

}

void drive(int16_t left, int16_t right){

    left = constrain(left, -99, 99);
    right = constrain(right, -99, 99);

    if (abs(left) < 10) {
        left = 0;
    }

    if (abs(right) < 10) {
        right = 0;
    }

    BSP_motors(left, right);

}
#include <stdio.h>  
#include <stdint.h>

#include "qpc.h"  
#include "bsp.h"
#include "bsp_line_sensors.h"


#ifdef Q_SPY
#include "qs_defines.h"
#endif

bool line_sensors[NUM_OF_LINE_SENSORS];

void BSP_lineSensorsInit(void){

}

bool BSP_lineSensorIsReading(line_position_t sensor){

    if (sensor >= NUM_OF_LINE_SENSORS){
        return false;
    }

    return line_sensors[sensor];

}

void BSP_lineSensorSet(line_position_t sensor, bool value){

    if (sensor >= NUM_OF_LINE_SENSORS){
        return;
    }

    line_sensors[sensor] = value;

}

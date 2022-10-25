/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "qpc.h"    
#include "bsp.h"

#include "distance_service.h"
#include "bsp_gpio.h"


/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/


/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

static void sensor_data_interrupt(uint8_t sensor_num, io_level_t state);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

volatile bool dist_sensor_is_active[NUM_OF_DIST_SENSORS];

static uint8_t sensor_num_to_position[NUM_OF_DIST_SENSORS] = {
    DIST_SENSOR_DR,
    DIST_SENSOR_R,
    DIST_SENSOR_FR,
    DIST_SENSOR_F,
    DIST_SENSOR_FL,
    DIST_SENSOR_L,
    DIST_SENSOR_DL,
};

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void sensor_data_interrupt(uint8_t sensor_num, io_level_t state){

    if (sensor_num > (NUM_OF_DIST_SENSORS - 1)){
        return;
    }

    dist_sensor_t sensor_position = sensor_num_to_position[sensor_num];
    dist_sensor_is_active[sensor_position] = state;

    QEvt evt = {.sig = DIST_SENSOR_CHANGE_SIG};
    QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
    

}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void distance_service_init() {
    BSP_GPIO_Register_Distance_Callback(sensor_data_interrupt);
}



bool distance_is_active(dist_sensor_t position){

    if (position > NUM_OF_DIST_SENSORS){
        return 0;
    }

    return dist_sensor_is_active[position];

}

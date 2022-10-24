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

#define SENSOR_DISABLED 0xff

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

enum dist_sensor_num {
    DIST_1,
    DIST_2,
    DIST_3,
    DIST_4,
    DIST_5,
    DIST_6,
    DIST_7,
    DIST_8,
    DIST_9,
};

uint8_t dist_sensor_gpio_mapping[] = {
    [DIST_1] = DIST_SENSOR_DR,
    [DIST_2] = DIST_SENSOR_R,
    [DIST_3] = DIST_SENSOR_FR,
    [DIST_4] = DIST_SENSOR_F,
    [DIST_5] = DIST_SENSOR_FL,
    [DIST_6] = DIST_SENSOR_L,
    [DIST_7] = DIST_SENSOR_DL,
    [DIST_8] = SENSOR_DISABLED,
    [DIST_9] = SENSOR_DISABLED,
};

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

static void sensor_data_interrupt(uint16_t sensor_num, bool gpio_level);

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

volatile bool dist_sensor_is_active[NUM_OF_DIST_SENSORS];

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void sensor_data_interrupt(uint16_t sensor_num, bool gpio_level){

    for (int i = 0; i < NUM_OF_DIST_SENSORS; i++) {
        if (sensor_num == i && dist_sensor_gpio_mapping[i] != SENSOR_DISABLED){
            dist_sensor_is_active[i] = gpio_level;

            QEvt evt = {.sig = DIST_SENSOR_CHANGE_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
        }
    }
    


}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void distance_service_init() {
    BSP_GPIO_Distance_Register_Callback(sensor_data_interrupt);
}



bool distance_is_active(dist_sensor_t position){

    if (position > NUM_OF_DIST_SENSORS){
        return 0;
    }

    return dist_sensor_is_active[position];

}

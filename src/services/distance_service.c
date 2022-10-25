/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include "qpc.h"    
#include "bsp.h"

#include "distance_service.h"
#include "bsp_gpio.h"
#include "bsp_gpio_mapping.h"


/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/


/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

static void sensor_data_interrupt(uint8_t sensor_num);

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

static void sensor_data_interrupt(uint8_t sensor_num){

    for (int i = 0; i < NUM_OF_DIST_SENSORS; i++) {
        if (sensor_num == i){
            dist_sensor_is_active[sensor_num_to_position[i]] = BSP_GPIO_Read_Pin(gpio_dist_sensor_pins[sensor_num], gpio_dist_sensor_ports[sensor_num]);

            QEvt evt = {.sig = DIST_SENSOR_CHANGE_SIG};
            QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
        }
    }
    


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

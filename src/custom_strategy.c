/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <string.h>
#include <stdbool.h>
#include "custom_strategy.h"
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/
#define STRATEGY_MAX_STEPS          8
/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

static uint8_t num_of_steps = 5;
static uint8_t current_step = 0;

// If type of movement is front or back, the value represents centimeters to move
// If type of movement is turn the value represents degrees to turn
static uint8_t cust_strategy_movements[STRATEGY_MAX_STEPS] = {
    90, 
    40, 
    90, 
    60,
    60
};


static movement_t type_of_movements[STRATEGY_MAX_STEPS] = {
    MOVE_RIGHT,
    MOVE_BACK,
    MOVE_LEFT,
    MOVE_FRONT,
    MOVE_RIGHT
};



/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void cust_strategy_reset() {
    current_step = 0;
}

void cust_strategy_increase_step() {
    current_step++;
}

uint8_t cust_strategy_num_steps(){
    return num_of_steps;
}

uint8_t cust_strategy_current_step() {
    return current_step;
}

movement_t cust_strategy_move_type(uint8_t step) {
    if (step >= STRATEGY_MAX_STEPS){
        return MOVE_FRONT;
    }

    return type_of_movements[step];
}

uint8_t cust_strategy_move(uint8_t step) {
    if (step >= STRATEGY_MAX_STEPS){
        return 10;
    }

    return cust_strategy_movements[step];
}

void cust_strategy_update_from_ble(uint8_t * last_data, uint8_t size){

    if (size < STRATEGY_MAX_STEPS + 3){
        return;
    }
    
    bool update_type_list = last_data[1];
    num_of_steps = last_data[2];

    if (update_type_list){
        memset(type_of_movements, 0, sizeof(type_of_movements));
    } else {
        memset(cust_strategy_movements, 0, sizeof(cust_strategy_movements));
    }

    for (int i = 0; i < STRATEGY_MAX_STEPS; i++)
    {
        if (update_type_list){
            type_of_movements[i] = last_data[3 + i];
        } else {
            cust_strategy_movements[i] = last_data[3 + i];
        }
    }
    
    
}
/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <string.h>
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

void cust_strategy_move_type_set(movement_t * movements_type, uint8_t size){
    if (size > STRATEGY_MAX_STEPS){
        return;
    }

    memcpy(type_of_movements, movements_type, size);
}

void cust_strategy_move_set(uint8_t * movements, uint8_t size){
    if (size > STRATEGY_MAX_STEPS){
        return;
    }

    memcpy(cust_strategy_movements, movements, size);
}
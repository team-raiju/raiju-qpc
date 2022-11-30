/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
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

static uint8_t num_of_steps = 4;
static uint8_t current_step = 0;

static uint8_t front_movements_cm[STRATEGY_MAX_STEPS] = {
    50, 0, 50, 0
};

static uint8_t back_movements_cm[STRATEGY_MAX_STEPS] = {
    0, 40, 0, 0
};

static uint8_t right_movements[STRATEGY_MAX_STEPS] = {
    0, 0, 0, 90
};

static uint8_t left_movements[STRATEGY_MAX_STEPS] = {
    0, 45, 0, 0
};

static movement_t type_of_movements[STRATEGY_MAX_STEPS] = {
    FRONT,
    BACK,
    FRONT,
    RIGHT,
};

// static uint8_t num_of_steps;
// static uint8_t current_step;
// static uint8_t front_movements_cm[STRATEGY_MAX_STEPS];
// static uint8_t back_movements_cm[STRATEGY_MAX_STEPS];
// static uint8_t right_movements[STRATEGY_MAX_STEPS];
// static uint8_t left_movements[STRATEGY_MAX_STEPS];
// static movement_t type_of_movements[STRATEGY_MAX_STEPS];

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
    return type_of_movements[step];
}

uint8_t cust_strategy_front(uint8_t step) {
    return front_movements_cm[step];
}

uint8_t cust_strategy_back(uint8_t step) {
    return back_movements_cm[step];
}

uint8_t cust_strategy_right(uint8_t step) {
    return right_movements[step];
}

uint8_t cust_strategy_left(uint8_t step) {
    return left_movements[step];
}
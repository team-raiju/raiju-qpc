/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <string.h>
#include <stdbool.h>
#include "custom_strategy.h"
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/
#define STRATEGY_MAX_STEPS 7
/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

static uint8_t num_of_steps = 3;
static uint8_t current_step = 0;

// If type of movement is front or back, the value represents centimeters to move
// If type of movement is left or right the value represents the set point degree
static uint16_t cust_strategy_movements[STRATEGY_MAX_STEPS] = { 315, 55, 45 };

static movement_t type_of_movements[STRATEGY_MAX_STEPS] = { MOVE_TURN, MOVE_FRONT, MOVE_TURN};

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void cust_strategy_reset()
{
    current_step = 0;
}

void cust_strategy_increase_step()
{
    current_step++;
}

uint8_t cust_strategy_num_steps()
{
    return num_of_steps;
}

uint8_t cust_strategy_current_step()
{
    return current_step;
}

movement_t cust_strategy_move_type(uint8_t step)
{
    if (step >= STRATEGY_MAX_STEPS) {
        return MOVE_FRONT;
    }

    if (step >= num_of_steps){
        return MOVE_FRONT;
    }

    return type_of_movements[step];
}

uint16_t cust_strategy_move(uint8_t step)
{
    if (step >= STRATEGY_MAX_STEPS) {
        return 10;
    }

    if (step >= num_of_steps){
        return 10;
    }

    return cust_strategy_movements[step];
}

int8_t cust_strategy_update_from_ble(uint8_t *strategy_data, uint8_t size)
{
    if (size != STRATEGY_MAX_STEPS + 2) {
        return -1;
    }

    if (strategy_data[1] > STRATEGY_MAX_STEPS) {
        return -1;
    }

    bool update_type_list = strategy_data[0];
    num_of_steps = strategy_data[1];

    if (update_type_list) {
        memset(type_of_movements, 0, sizeof(type_of_movements));
    } else {
        memset(cust_strategy_movements, 0, sizeof(cust_strategy_movements));
    }

    for (int i = 0; i < STRATEGY_MAX_STEPS; i++) {
        if (update_type_list) {
            movement_t move = strategy_data[2 + i];
            if (move < MAX_MOVEMENTS) {
                type_of_movements[i] = strategy_data[2 + i];
            } else {
                // Got an invalid movement so will stop here
                return 0;
            }
        } else {
            cust_strategy_movements[i] = strategy_data[2 + i];
        }
    }

    return 0;
}
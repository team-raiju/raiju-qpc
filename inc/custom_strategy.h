#ifndef CUSTOM_STARTEGY_H
#define CUSTOM_STARTEGY_H

#include <stdint.h>


typedef enum {
    MOVE_FRONT,
    MOVE_BACK,
    MOVE_LEFT,
    MOVE_RIGHT
} movement_t;


void cust_strategy_reset(); 
void cust_strategy_increase_step(); 
uint8_t cust_strategy_current_step();
uint8_t cust_strategy_num_steps();
movement_t cust_strategy_move_type(uint8_t step);
uint8_t cust_strategy_move(uint8_t step);
void cust_strategy_move_type_set(movement_t * movements_type, uint8_t size);
void cust_strategy_move_set(uint8_t * movements, uint8_t size);


#endif /* CUSTOM_STARTEGY_H */



#ifndef CUSTOM_STARTEGY_H
#define CUSTOM_STARTEGY_H

#include <stdint.h>


typedef enum {
    FRONT,
    BACK,
    LEFT,
    RIGHT
} movement_t;


void cust_strategy_reset(); 
void cust_strategy_increase_step(); 
uint8_t cust_strategy_current_step();
uint8_t cust_strategy_num_steps();
movement_t cust_strategy_move_type(uint8_t step);
uint8_t cust_strategy_front(uint8_t step);
uint8_t cust_strategy_back(uint8_t step); 
uint8_t cust_strategy_right(uint8_t step); 
uint8_t cust_strategy_left(uint8_t step); 


#endif /* CUSTOM_STARTEGY_H */



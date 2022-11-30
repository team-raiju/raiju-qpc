#ifndef CUSTOM_STARTEGY_H
#define CUSTOM_STARTEGY_H

#include <stdint.h>


typedef enum {
    FRONT,
    BACK,
    LEFT,
    RIGHT
} movement_t;


void reset_step(); 
void increase_step(); 
uint8_t get_current_step();
uint8_t get_num_of_steps();
movement_t get_movement_type(uint8_t step);
uint8_t get_front_movement(uint8_t step);
uint8_t get_back_movement(uint8_t step); 
uint8_t get_right_movement(uint8_t step); 
uint8_t get_left_movement(uint8_t step); 


#endif /* CUSTOM_STARTEGY_H */



#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

typedef struct sumo_parameters {

    uint8_t current_strategy;
    uint8_t current_pre_strategy;

    uint8_t enabled_distance_sensors;
    uint8_t enabled_line_sensors;

    uint8_t star_speed;
    uint8_t max_speed;
    uint8_t reverse_speed; 
    uint8_t turn_speed;

    uint8_t reverse_time_ms;
    uint8_t turn_180_time_ms;
    uint8_t step_wait_time_ms;
    

} sumo_parameters_t;

void report_sumo_parameters(sumo_parameters_t parameters);


#endif /* PARAMETERS_H */

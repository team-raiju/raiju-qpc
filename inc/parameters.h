#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

#define PARAMETERS_REPORT_SIZE  9

typedef struct sumo_parameters {

    uint8_t  strategy;
    uint8_t  pre_strategy;

    uint16_t enabled_distance_sensors;
    uint8_t  enabled_line_sensors;

    uint8_t  star_speed;
    uint8_t  max_speed;
    
    uint8_t  reverse_speed; 
    uint16_t reverse_time_ms;
    uint8_t  turn_speed;

    uint16_t turn_180_right_time_ms;
    uint16_t turn_180_left_time_ms;

    uint16_t step_wait_time_ms;
    uint16_t step_advance_time_ms;

    uint16_t time_ms_to_cross_at_60_vel;
    

} sumo_parameters_t;

void parameters_report(sumo_parameters_t params, uint8_t config_num);
void parameters_init(sumo_parameters_t *params);
void parameters_update_from_ble(sumo_parameters_t *params, uint8_t * last_data);
void parameters_set_strategy(sumo_parameters_t *params, uint8_t strategy);
void parameters_update_pre_strategy(sumo_parameters_t *params, uint8_t pre_strategy);
void parameters_set_strategy_led(sumo_parameters_t *params);
#endif /* PARAMETERS_H */

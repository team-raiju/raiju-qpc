#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

#define PARAMETERS_REPORT_SIZE  10

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

typedef enum {
    SIDE_LEFT,
    SIDE_RIGHT,
} side_t;

void parameters_report(sumo_parameters_t params, uint8_t config_num);
void parameters_init(sumo_parameters_t *params);
void parameters_update_from_ble(sumo_parameters_t *params, uint8_t * last_data);
void parameters_set_strategy(sumo_parameters_t *params, uint8_t strategy);
void parameters_update_pre_strategy(sumo_parameters_t *params, uint8_t pre_strategy);
void parameters_set_strategy_led(sumo_parameters_t *params);

uint16_t get_time_to_turn_ms(uint16_t degrees, uint8_t turn_speed, side_t side, sumo_parameters_t *params);
uint16_t get_time_to_move_ms(uint16_t distance_cm, uint8_t speed, sumo_parameters_t *params);

#endif /* PARAMETERS_H */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include <stdbool.h>

#define PARAMETERS_REPORT_SIZE  3

typedef struct sumo_parameters {

    uint8_t  strategy;                   /* Current strategy */
    uint8_t  pre_strategy;               /* Current pre strategy */
    uint8_t  calib_mode;                 /* Current calib mode */

    uint16_t enabled_distance_sensors;   /* Bits corresponding to which dist sensors are enabled bit 6-0 -> R,FR,DR,F,DL,FL,L*/
    uint8_t  enabled_line_sensors;       /* Bits corresponding to which line sensors are enabled bit 3-0 -> BL[3],BR[2],FL[1],FR[0]*/

    uint8_t  star_speed;                 /* Velocity to look for opponents whiles not seeing */
    uint8_t  max_speed;                  /* Max possible speed. Current UNUSED */
    
    uint8_t  reverse_speed;              /* When seeing line, absolute value of speed that will reverse the motor - default 100*/   
    uint16_t reverse_time_ms;            /* When seeing line, time that motor will be reversing at 100% */
    uint8_t  line_seen_turn_angle;       /* When seeing line, angle that will turn after going back */

    uint16_t turn_180_right_time_ms;     /* Time to turn 180 degrees right at 100%*/
    uint16_t turn_180_left_time_ms;      /* Time to turn 180 degrees left at 100%*/

    uint16_t step_wait_time_ms;          /* Time between two little steps */
    uint16_t step_advance_time_ms;       /* Time that will be going foward in little steps */

    uint16_t time_ms_to_cross_at_60_vel; /* Time that it takes to cross the arena at 60% speed*/

    uint16_t is_stucked_timeout_ms;      /* Time seeing the same sensor to consider that the robot is stucked */

    uint8_t attack_when_near;            /* Bool indicating if the robot must start auto when seeing near */   
    

} sumo_parameters_t;

typedef enum {
    SIDE_LEFT,
    SIDE_RIGHT,
} side_t;

void parameters_report(sumo_parameters_t params, uint8_t config_num);
void parameters_init(sumo_parameters_t *params);
void parameters_update_from_ble(sumo_parameters_t *params, uint8_t * last_data);
void report_raw_line_data_ble();
void parameters_set_strategy(sumo_parameters_t *params, uint8_t strategy);
void parameters_update_pre_strategy(sumo_parameters_t *params, uint8_t pre_strategy);
void parameters_update_calib_mode(sumo_parameters_t *params, uint8_t calib_mode);
void parameters_set_strategy_led(sumo_parameters_t *params);
void parameters_set_pre_strategy_led(sumo_parameters_t *params);
void parameters_set_calib_mode_led(sumo_parameters_t *params);

uint16_t get_time_to_turn_ms(uint16_t degrees, uint8_t turn_speed, side_t side, sumo_parameters_t *params);
uint16_t get_time_to_move_ms(uint16_t distance_cm, uint8_t speed, sumo_parameters_t *params);

#endif /* PARAMETERS_H */

#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <stdbool.h>
#include <stdint.h>
#include "parameters.h"

typedef enum {
    FORWARD,
    BACKWARD,

    TURN_RIGHT_45_STATIC,
    TURN_LEFT_45_STATIC,
    TURN_RIGHT_90_STATIC,
    TURN_LEFT_90_STATIC,

    TURN_RIGHT_45_MOVING,
    TURN_LEFT_45_MOVING,
    TURN_RIGHT_90_MOVING,
    TURN_LEFT_90_MOVING,

    STOP,
} movement_type_t;

typedef struct strategy_movements {
    movement_type_t movement;
    uint8_t lenght_cm;
} strategy_movements_t;

void control_reset(sumo_parameters_t params);
void control_update();
void navigation_reset(strategy_movements_t strategy_movement);
bool navigation_step();


float get_target_linear_speed_m_s();
float get_target_angular_speed_rad_s();

int16_t get_pwm_duty_l();
int16_t get_pwm_duty_r();

float get_integral_vel();
float get_integral_angular();

#endif /* NAVIGATION_H */

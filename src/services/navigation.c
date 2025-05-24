#include <stdint.h>
#include <stdio.h>
#include "driving_service.h"
#include "bsp_motors.h"
#include "bsp_gpio.h"
#include "bsp_gpio_mapping.h"
#include "utils.h"
#include "imu_service.h"
#include "adc_service.h"
#include "bsp.h"
#include "pid.h"
#include "navigation.h"

#define MOTOR_COUNTER_PERIOD_MAX 100
#define CONTROL_FREQUENCY_HZ     1000.0f

// Navigation variables
static uint32_t reference_time = 0;
static bool is_finished = false;
static movement_type_t current_movement = FORWARD;
static uint8_t current_lenght_cm = 0;



// Control Variables
static PID_controller angular_vel_pid;
static PID_controller linear_vel_pid;
static float target_linear_speed_m_s;
static float target_angular_speed_rad_s;
static int16_t pwm_duty_l;
static int16_t pwm_duty_r;

void control_reset()
{
    PID_init(&angular_vel_pid, 10.0, 0.2, 0.0, 200.0);
    PID_init(&linear_vel_pid, 10.0, 0.2, 0.0, 200.0);
}

void control_update()
{
    // float mean_velocity_m_s = bsp::encoders::get_filtered_velocity_m_s();
    // float linear_ratio = linear_vel_pid.calculate(target_linear_speed_m_s, mean_velocity_m_s);

    float linear_ratio = 0;
    float rotation_ratio = -PID_calculate(&angular_vel_pid, target_angular_speed_rad_s, get_imu_ang_vel_rad_s());

    float l_voltage = linear_ratio + rotation_ratio;
    float r_voltage = linear_ratio - rotation_ratio;

    // float bat_volts = adc_get_pwr_bat_mv() / 1000.0;
    float bat_volts = 50.0;

    pwm_duty_l = (l_voltage / bat_volts) * MOTOR_COUNTER_PERIOD_MAX;
    pwm_duty_r = (r_voltage / bat_volts) * MOTOR_COUNTER_PERIOD_MAX;

    /* Limit PWM */
    if (pwm_duty_l > MOTOR_COUNTER_PERIOD_MAX || pwm_duty_r > MOTOR_COUNTER_PERIOD_MAX) {
        uint32_t abs_diff = abs(pwm_duty_l - pwm_duty_r);
        abs_diff = min(abs_diff, 2 * MOTOR_COUNTER_PERIOD_MAX);
        if (pwm_duty_l > pwm_duty_r) {
            pwm_duty_l = MOTOR_COUNTER_PERIOD_MAX;
            pwm_duty_r = MOTOR_COUNTER_PERIOD_MAX - abs_diff;
        } else {
            pwm_duty_l = MOTOR_COUNTER_PERIOD_MAX - abs_diff;
            pwm_duty_r = MOTOR_COUNTER_PERIOD_MAX;
        }
    }

    if (pwm_duty_l < -MOTOR_COUNTER_PERIOD_MAX || pwm_duty_r < -MOTOR_COUNTER_PERIOD_MAX) {
        uint32_t abs_diff = abs(pwm_duty_l - pwm_duty_r);
        abs_diff = min(abs_diff, (2 * MOTOR_COUNTER_PERIOD_MAX));

        if (pwm_duty_l < pwm_duty_r) {
            pwm_duty_l = -MOTOR_COUNTER_PERIOD_MAX;
            pwm_duty_r = -MOTOR_COUNTER_PERIOD_MAX + abs_diff;
        } else {
            pwm_duty_l = -MOTOR_COUNTER_PERIOD_MAX + abs_diff;
            pwm_duty_r = -MOTOR_COUNTER_PERIOD_MAX;
        }
    }

    //printf("angular_vel: %f, target: %f, PWM L: %d, PWM R: %d\n", get_imu_ang_vel_rad_s(), target_angular_speed_rad_s ,pwm_duty_l, pwm_duty_r);
    drive(pwm_duty_l, pwm_duty_r);
}

void navigation_reset(strategy_movements_t strategy_movement)
{
    reference_time = BSP_GetTick();
    is_finished = false;
    current_movement = strategy_movement.movement;
    current_lenght_cm = strategy_movement.lenght_cm;

    printf("Movement: %d, Length: %d\r\n", current_movement, current_lenght_cm);
}

bool navigation_step()
{

    switch (current_movement) {
        case FORWARD:
        case BACKWARD: {
            float max_speed = 1.0;;
            float control_linear_speed = target_linear_speed_m_s;
            float acceleration = 5.0;
            float deceleration = 10.0;

            float forward_end_speed = 0.11;

            uint32_t elapsed_time = BSP_GetTick() - reference_time;
            uint32_t elapsed_lenght = current_lenght_cm * 3;

            if (elapsed_time < elapsed_lenght) {
                if (control_linear_speed < max_speed) {
                    control_linear_speed += acceleration / CONTROL_FREQUENCY_HZ;
                    control_linear_speed = min(control_linear_speed, max_speed);
                }
            } else {
                if (control_linear_speed > forward_end_speed) {
                    control_linear_speed -= deceleration / CONTROL_FREQUENCY_HZ;
                    control_linear_speed = max(control_linear_speed, 0.1);
                } else {
                    is_finished = true;
                    printf("Finished moving\r\n");
                }
            }

            target_linear_speed_m_s = control_linear_speed;
            target_angular_speed_rad_s = 0.0f;
            control_update();
            break;
        }

        case TURN_RIGHT_45_MOVING:
        case TURN_LEFT_45_MOVING:
        case TURN_RIGHT_90_MOVING:
        case TURN_LEFT_90_MOVING:
        case TURN_RIGHT_45_STATIC:
        case TURN_LEFT_45_STATIC:
        case TURN_RIGHT_90_STATIC:
        case TURN_LEFT_90_STATIC: {

            target_linear_speed_m_s = 0.0f;

            float angular_acceleration = 100.00;
            float angular_speed = 5.5850;
        
            uint16_t elapsed_t_accel = 50;
            uint16_t elapsed_t_max_ang_vel = elapsed_t_accel + 100;
            uint16_t elapsed_t_decel = elapsed_t_max_ang_vel + 50;
        
            int turn_sign = 1;
        
            uint32_t elapsed_time = BSP_GetTick() - reference_time;
        
            if (elapsed_time <= elapsed_t_accel) {
                float ideal_rad_s = abs(target_angular_speed_rad_s) + (angular_acceleration / CONTROL_FREQUENCY_HZ);
                ideal_rad_s = min(ideal_rad_s, angular_speed);
                target_angular_speed_rad_s = ideal_rad_s * turn_sign;
            } else if (elapsed_time <= elapsed_t_max_ang_vel) {
                float ideal_rad_s = angular_speed;
                target_angular_speed_rad_s = ideal_rad_s * turn_sign;
            } else if (elapsed_time <= elapsed_t_decel) {
                float ideal_rad_s = abs(target_angular_speed_rad_s) - (angular_acceleration / CONTROL_FREQUENCY_HZ);
                ideal_rad_s = max(ideal_rad_s, 0.0f);
                target_angular_speed_rad_s = ideal_rad_s * turn_sign;
            } else {
                target_angular_speed_rad_s = 0;
                is_finished = true;
            }
        
            control_update();
        
            break;
        }

        default:
            break;

    }

    return is_finished;
}
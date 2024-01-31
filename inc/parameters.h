#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>
#include <stdbool.h>

#define BLE_TRANSMIT_NUM_OF_PACKETS 3

typedef struct sumo_parameters {
    uint8_t strategy;     /* Current strategy */
    uint8_t pre_strategy; /* Current pre strategy */
    uint8_t calib_mode;   /* Current calib mode */

    uint16_t enabled_distance_sensors; /* Bits corresponding to which dist sensors are enabled bit 6-0 -> R,FR,DR,F,DL,FL,L*/
    uint8_t enabled_line_sensors; /* Bits corresponding to which line sensors are enabled bit 3-0 -> BL[3],BR[2],FL[1],FR[0]*/

    uint8_t star_speed; /* Velocity to look for opponents whiles not seeing */
    uint8_t max_speed;  /* Max possible speed. Current UNUSED */
    uint8_t star_full_speed_time_ms; /* Time that motors will be on max speed when making star */

    uint8_t reverse_speed;        /* When seeing line, absolute value of speed that will reverse the motor - default 100*/
    uint16_t reverse_time_ms;     /* When seeing line, time that motor will be reversing at 100% */
    uint8_t line_seen_turn_angle; /* When seeing line, angle that will turn after going back */

    uint16_t turn_180_right_time_ms; /* Time to turn 180 degrees right at 100%*/
    uint16_t turn_180_left_time_ms;  /* Time to turn 180 degrees left at 100%*/

    uint16_t step_wait_time_ms;    /* Time between two little steps */
    uint16_t step_advance_time_ms; /* Time that will be going foward in little steps */

    uint16_t time_ms_to_cross_at_max_vel; /* Time that it takes to cross the arena at 100% speed*/

    uint16_t is_stucked_timeout_ms; /* Time seeing the same sensor to consider that the robot is stucked */

    uint8_t attack_when_near; /* Bool indicating if the robot must start auto when seeing near */

    uint8_t current_state; /* Current state of the robot */

    uint16_t kp; /* Proportional constant */
    uint16_t kd; /* Derivative constant */
    uint16_t ki; /* Integral constant */

    uint8_t near_angle_th; /* Angle to consider that the robot is near to the desired angle */
    uint8_t inclinated_th; /* Angle to consider that the robot is inclinated */

} sumo_parameters_t;

typedef enum {
    SIDE_LEFT,
    SIDE_RIGHT,
} side_t;

typedef enum {
    PARAM_OK,
    PARAM_ERROR,
} param_error_t;

typedef enum {
    STATE_IDLE,
    RC_RUNNING,
    AUTO_RUNNING
} state_modes_t;

typedef enum {
    BLE_DATA_HDR_STRATEGY_AND_PRE,
    BLE_DATA_HDR_CALIB_MODE,
    BLE_DATA_HDR_CUST_STRATEGY,
    BLE_DATA_HDR_EN_DISTANCE_SENSORS,
    BLE_DATA_HDR_EN_LINE_SENSORS,
    BLE_DATA_HDR_STAR_SPEED,
    BLE_DATA_HDR_MAX_SPEED,
    BLE_DATA_HDR_REVERSE_SPEED,
    BLE_DATA_HDR_REVERSE_TIME_MS,
    BLE_DATA_HDR_LINE_TURN_ANGLE,
    BLE_DATA_HDR_TURN_180_RIGHT_TIME_MS,
    BLE_DATA_HDR_TURN_180_LEFT_TIME_MS,
    BLE_DATA_HDR_STEP_WAIT_TIME_MS,
    BLE_DATA_HDR_STEP_ADVANCE_TIME_MS,
    BLE_DATA_HDR_TIME_MS_TO_CROSS_AT_MAX_VEL,
    BLE_DATA_HDR_IS_STUCKED_TIMEOUT,
    BLE_DATA_HDR_ATTACK_WHEN_NEAR,
    BLE_DATA_HDR_STAR_FULL_SPEED_TIME_MS,
    BLE_DATA_HDR_KP,
    BLE_DATA_HDR_KD,
    BLE_DATA_HDR_KI,
    BLE_DATA_HDR_NEAR_ANGLE_TH,
    BLE_DATA_HDR_INCLINATED_TH,
    BLE_DATA_MAX_HDR
} ble_data_header_t;

void set_reference_voltage();
void parameters_report(sumo_parameters_t params, uint8_t config_num);
void parameters_init(sumo_parameters_t *params);
param_error_t parameters_update_from_ble(sumo_parameters_t *params, uint8_t *last_data);
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

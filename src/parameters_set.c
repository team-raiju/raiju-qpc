#include "parameters_set.h"
#include "bsp_eeprom.h"
#include "utils.h"
#include "distance_service.h"
#include "adc_service.h"

static void set_validate_en_distance_sensors(sumo_parameters_t * params, uint16_t new_data){
    params->enabled_distance_sensors = new_data;
    distance_service_set_mask(params->enabled_distance_sensors);
    BSP_eeprom_write(EN_DIST_SENSOR_ADDR, params->enabled_distance_sensors);
}

static void set_validate_en_line_sensors(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 0, 255)){
        params->enabled_line_sensors = new_data;
        adc_line_set_mask(params->enabled_line_sensors);
        BSP_eeprom_write(EN_LINE_SENSOR_ADDR, params->enabled_line_sensors);
    }
}

static void set_validate_star_speed(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 20, 100)){
        params->star_speed = new_data;
        BSP_eeprom_write(STAR_SPEED_ADDR, params->star_speed);
    }
}

static void set_validate_max_speed(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 20, 100)){
        params->max_speed = new_data;
        BSP_eeprom_write(MAX_SPEED_ADDR, params->max_speed);
    }
}

static void set_validate_reverse_speed(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 40, 100)){
        params->reverse_speed = new_data;
        BSP_eeprom_write(REVERSE_SPEED_ADDR, params->reverse_speed);
    }
}

static void set_validate_reverse_time_ms(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 30, 1000)){
        params->reverse_time_ms = new_data;
        BSP_eeprom_write(REVERSE_TIME_MS_ADDR, params->reverse_time_ms);
    }
}

static void set_validate_line_seen_turn_angle(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 10, 255)){
        params->line_seen_turn_angle = new_data;
        BSP_eeprom_write(LINE_TURN_ANGLE_ADDR, params->line_seen_turn_angle);
    }
}

static void set_validate_turn_180_right_time_ms(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 30, 1000)){
        params->turn_180_right_time_ms = new_data;
        BSP_eeprom_write(TURN_180_RIGHT_TIME_ADDR, params->turn_180_right_time_ms);
    }
}

static void set_validate_turn_180_left_time_ms(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 30, 1000)){
        params->turn_180_left_time_ms = new_data;
        BSP_eeprom_write(TURN_180_LEFT_TIME_ADDR, params->turn_180_left_time_ms);
    }
}

static void set_validate_step_wait_time_ms(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 100, 10000)){
        params->step_wait_time_ms = new_data;
        BSP_eeprom_write(STEP_WAIT_TIME_MS_ADDR, params->step_wait_time_ms);
    }
}

static void set_validate_step_advance_time_ms(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 10, 1000)){
        params->step_advance_time_ms = new_data;
        BSP_eeprom_write(STEP_ADVANCE_TIME_MS_ADDR, params->step_advance_time_ms);
    }
}

static void set_validate_time_ms_to_cross_at_60_vel(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 100, 5000)){
        params->time_ms_to_cross_at_60_vel = new_data;
        BSP_eeprom_write(TIME_MS_TO_CROSS_AT_60_ADDR, params->time_ms_to_cross_at_60_vel);
    }
}

static void set_validate_is_stucked_timeout_ms(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 100, 20000)){
        params->is_stucked_timeout_ms = new_data;
        BSP_eeprom_write(TIMEOUT_IS_STUCKED, params->is_stucked_timeout_ms);
    }
}

static void set_validate_attack_when_near(sumo_parameters_t * params, uint16_t new_data){
    if(IS_BETWEEN(new_data, 0, 1)){
        params->attack_when_near = new_data;
    }
}


void set_validade_new_param_uint16_t(ble_data_header_t header, sumo_parameters_t * params, uint16_t new_data) {

    switch (header)
    {
    case BLE_DATA_HDR_EN_DISTANCE_SENSORS:
        set_validate_en_distance_sensors(params, new_data);
        break;
    case BLE_DATA_HDR_EN_LINE_SENSORS:
        set_validate_en_line_sensors(params, new_data);
        break;
    case BLE_DATA_HDR_STAR_SPEED:
        set_validate_star_speed(params, new_data);
        break;
    case BLE_DATA_HDR_MAX_SPEED:
        set_validate_max_speed(params, new_data);
        break;
    case BLE_DATA_HDR_REVERSE_SPEED:
        set_validate_reverse_speed(params, new_data);
        break;
    case BLE_DATA_HDR_REVERSE_TIME_MS:
        set_validate_reverse_time_ms(params, new_data);
        break;
    case BLE_DATA_HDR_LINE_TURN_ANGLE:
        set_validate_line_seen_turn_angle(params, new_data);
        break;
    case BLE_DATA_HDR_TURN_180_RIGHT_TIME_MS:
        set_validate_turn_180_right_time_ms(params, new_data);
        break;
    case BLE_DATA_HDR_TURN_180_LEFT_TIME_MS:
        set_validate_turn_180_left_time_ms(params, new_data);
        break;
    case BLE_DATA_HDR_STEP_WAIT_TIME_MS:
        set_validate_step_wait_time_ms(params, new_data);
        break;
    case BLE_DATA_HDR_STEP_ADVANCE_TIME_MS:
        set_validate_step_advance_time_ms(params, new_data);
        break;
    case BLE_DATA_HDR_TIME_MS_TO_CROSS_AT_60_VEL:
        set_validate_time_ms_to_cross_at_60_vel(params, new_data);
        break;
    case BLE_DATA_HDR_IS_STUCKED_TIMEOUT:
        set_validate_is_stucked_timeout_ms(params, new_data);
        break;
    case BLE_DATA_HDR_ATTACK_WHEN_NEAR:
        set_validate_attack_when_near(params, new_data);
        break;
    default:
        break;
    }

}

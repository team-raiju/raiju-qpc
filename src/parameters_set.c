#include "parameters_set.h"
#include "bsp_eeprom.h"
#include "utils.h"
#include "distance_service.h"
#include "adc_service.h"
#include "qpc.h"
#include "bsp.h"

static param_error_t set_validate_en_distance_sensors(sumo_parameters_t *params, uint16_t new_data)
{
    params->enabled_distance_sensors = new_data;
    distance_service_set_mask(params->enabled_distance_sensors);
    BSP_eeprom_write(EN_DIST_SENSOR_ADDR, params->enabled_distance_sensors);
    return PARAM_OK;
}

static param_error_t set_validate_en_line_sensors(sumo_parameters_t *params, uint16_t new_data)
{
    if (new_data <= 255) {
        params->enabled_line_sensors = new_data;
        adc_line_set_mask(params->enabled_line_sensors);
        BSP_eeprom_write(EN_LINE_SENSOR_ADDR, params->enabled_line_sensors);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_star_speed(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 20, 100)) {
        params->star_speed = new_data;
        BSP_eeprom_write(STAR_SPEED_ADDR, params->star_speed);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_star_full_speed_timeout(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 1, 100)) {
        params->star_full_speed_time_ms = new_data;
        BSP_eeprom_write(STAR_FULL_SPEED_TIME_MS_ADDR, params->star_full_speed_time_ms);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_max_speed(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 20, 100)) {
        params->max_speed = new_data;
        BSP_eeprom_write(MAX_SPEED_ADDR, params->max_speed);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_reverse_speed(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 40, 100)) {
        params->reverse_speed = new_data;
        BSP_eeprom_write(REVERSE_SPEED_ADDR, params->reverse_speed);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_reverse_time_ms(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 30, 1000)) {
        params->reverse_time_ms = new_data;
        BSP_eeprom_write(REVERSE_TIME_MS_ADDR, params->reverse_time_ms);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_line_seen_turn_angle(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 10, 255)) {
        params->line_seen_turn_angle = new_data;
        BSP_eeprom_write(LINE_TURN_ANGLE_ADDR, params->line_seen_turn_angle);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_turn_180_right_time_ms(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 30, 1000)) {
        params->turn_180_right_time_ms = new_data;
        BSP_eeprom_write(TURN_180_RIGHT_TIME_ADDR, params->turn_180_right_time_ms);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_turn_180_left_time_ms(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 30, 1000)) {
        params->turn_180_left_time_ms = new_data;
        BSP_eeprom_write(TURN_180_LEFT_TIME_ADDR, params->turn_180_left_time_ms);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_step_wait_time_ms(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 100, 10000)) {
        params->step_wait_time_ms = new_data;
        BSP_eeprom_write(STEP_WAIT_TIME_MS_ADDR, params->step_wait_time_ms);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_step_advance_time_ms(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 10, 1000)) {
        params->step_advance_time_ms = new_data;
        BSP_eeprom_write(STEP_ADVANCE_TIME_MS_ADDR, params->step_advance_time_ms);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_time_ms_to_cross_at_max_vel(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 10, 700)) {
        params->time_ms_to_cross_at_max_vel = new_data;
        BSP_eeprom_write(TIME_MS_TO_CROSS_AT_100_ADDR, params->time_ms_to_cross_at_max_vel);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_is_stucked_timeout_ms(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 100, 20000)) {
        params->is_stucked_timeout_ms = new_data;
        BSP_eeprom_write(TIMEOUT_IS_STUCKED_ADDR, params->is_stucked_timeout_ms);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_attack_when_near(uint16_t new_data)
{
    if (new_data <= 1) {
        QEvt evt = { .sig = BLE_ATTACK_NEAR_SIG };
        QHSM_DISPATCH(&AO_SumoHSM->super, &evt, SIMULATOR);
        /* Param OK is never return because the variable is not set here */
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_kp(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 100, 5000)) {
        params->kp = new_data;
        BSP_eeprom_write(EE_KP_ADDR, params->kp);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_kd(sumo_parameters_t *params, uint16_t new_data)
{
    if (new_data <= 5000) {
        params->kd = new_data;
        BSP_eeprom_write(EE_KD_ADDR, params->kd);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_ki(sumo_parameters_t *params, uint16_t new_data)
{
    if (new_data <= 5000) {
        params->ki = new_data;
        BSP_eeprom_write(EE_KI_ADDR, params->ki);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_near_angle_th(sumo_parameters_t *params, uint16_t new_data)
{
    if (new_data <= 30) {
        params->near_angle_th = new_data;
        BSP_eeprom_write(EE_NEAR_ANGLE_TH_ADDR, params->near_angle_th);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_inclinated_th(sumo_parameters_t *params, uint16_t new_data)
{
    if (IS_BETWEEN(new_data, 10, 100)) {
        params->inclinated_th = new_data;
        BSP_eeprom_write(EE_INCLINATED_TH_ADDR, params->inclinated_th);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

static param_error_t set_validate_imu_enabled(sumo_parameters_t *params, uint16_t new_data)
{
    if (new_data == 0 || new_data == 1) {
        params->imu_enabled = new_data;
        BSP_eeprom_write(EE_IMU_ENABLED_ADDR, params->imu_enabled);
        return PARAM_OK;
    }
    return PARAM_ERROR;
}

param_error_t set_validade_new_param_uint16_t(ble_data_header_t header, sumo_parameters_t *params, uint16_t new_data)
{
    switch (header) {
    case BLE_DATA_HDR_EN_DISTANCE_SENSORS:
        return set_validate_en_distance_sensors(params, new_data);
    case BLE_DATA_HDR_EN_LINE_SENSORS:
        return set_validate_en_line_sensors(params, new_data);
    case BLE_DATA_HDR_STAR_SPEED:
        return set_validate_star_speed(params, new_data);
    case BLE_DATA_HDR_MAX_SPEED:
        return set_validate_max_speed(params, new_data);
    case BLE_DATA_HDR_REVERSE_SPEED:
        return set_validate_reverse_speed(params, new_data);
    case BLE_DATA_HDR_REVERSE_TIME_MS:
        return set_validate_reverse_time_ms(params, new_data);
    case BLE_DATA_HDR_LINE_TURN_ANGLE:
        return set_validate_line_seen_turn_angle(params, new_data);
    case BLE_DATA_HDR_TURN_180_RIGHT_TIME_MS:
        return set_validate_turn_180_right_time_ms(params, new_data);
    case BLE_DATA_HDR_TURN_180_LEFT_TIME_MS:
        return set_validate_turn_180_left_time_ms(params, new_data);
    case BLE_DATA_HDR_STEP_WAIT_TIME_MS:
        return set_validate_step_wait_time_ms(params, new_data);
    case BLE_DATA_HDR_STEP_ADVANCE_TIME_MS:
        return set_validate_step_advance_time_ms(params, new_data);
    case BLE_DATA_HDR_TIME_MS_TO_CROSS_AT_MAX_VEL:
        return set_validate_time_ms_to_cross_at_max_vel(params, new_data);
    case BLE_DATA_HDR_IS_STUCKED_TIMEOUT:
        return set_validate_is_stucked_timeout_ms(params, new_data);
    case BLE_DATA_HDR_ATTACK_WHEN_NEAR:
        return set_validate_attack_when_near(new_data);
    case BLE_DATA_HDR_STAR_FULL_SPEED_TIME_MS:
        return set_validate_star_full_speed_timeout(params, new_data);
    case BLE_DATA_HDR_KP:
        return set_validate_kp(params, new_data);
    case BLE_DATA_HDR_KD:
        return set_validate_kd(params, new_data);
    case BLE_DATA_HDR_KI:
        return set_validate_ki(params, new_data);
    case BLE_DATA_HDR_NEAR_ANGLE_TH:
        return set_validate_near_angle_th(params, new_data);
    case BLE_DATA_HDR_INCLINATED_TH:
        return set_validate_inclinated_th(params, new_data);
    case BLE_DATA_HDR_IMU_ENABLED:
        return set_validate_imu_enabled(params, new_data);
    default:
        break;
    }
    return PARAM_ERROR;
}

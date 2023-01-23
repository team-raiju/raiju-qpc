/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "parameters.h"
#include "ble_service.h"
#include "utils.h"
#include "bsp_eeprom.h"
#include "distance_service.h"
#include "adc_service.h"
#include "led_service.h"
#include "custom_strategy.h"

/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

#define NUM_OF_STRATEGIES        3
#define NUM_OF_PRE_STRATEGIES    9
#define NUM_OF_CALIB_MODES       5

#define ARENA_LENGHT_CM          154.0f
#define SUMO_LENGHT_CM           20.0f
#define REFERENCE_DIST_CM        (ARENA_LENGHT_CM - SUMO_LENGHT_CM)

#define REFERENCE_SPEED          60.0f

#define REFERENCE_TURN_SPEED     100.0f



/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

typedef enum {
    BLE_DATA_HDR_CUST_STRATEGY,
    BLE_DATA_HDR_STRATEGY,
    BLE_DATA_HDR_PRE_STRATEGY,
    BLE_DATA_HDR_CALIB_MODE,
    BLE_DATA_HDR_EN_DISTANCE_SENSORS,
    BLE_DATA_HDR_EN_LINE_SENSORS,
    BLE_DATA_HDR_STAR_SPEED,
    BLE_DATA_HDR_MAX_SPEED,
    BLE_DATA_HDR_REVERSE_SPEED,
    BLE_DATA_HDR_REVERSE_TIME_MS,
    BLE_DATA_HDR_TURN_SPEED,
    BLE_DATA_HDR_TURN_180_RIGHT_TIME_MS,
    BLE_DATA_HDR_TURN_180_LEFT_TIME_MS,
    BLE_DATA_HDR_STEP_WAIT_TIME_MS,
    BLE_DATA_HDR_STEP_ADVANCE_TIME_MS,
    BLE_DATA_HDR_TIME_MS_TO_CROSS_AT_60_VEL,
    BLE_DATA_HDR_IS_STUCKED_TIMEOUT,
    BLE_DATA_HDR_ATTACK_WHEN_NEAR,
    BLE_DATA_MAX_HDR
} ble_data_header_t;


typedef union{
    uint8_t _raw[BLE_RECEIVE_PACKET_SIZE - 2];
    struct {
        uint8_t header;
        uint8_t param_data[BLE_RECEIVE_PACKET_SIZE - 3];
    };

} ble_update_param_packet_t;

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/
static void read_and_update_parameter_16_bit(uint16_t eeprom_addr, uint16_t * updated_data);
static void read_and_update_parameter_8_bit(uint16_t eeprom_addr, uint8_t * updated_data);
/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

static color_name_t strategy_colors[NUM_OF_STRATEGIES] = {
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_ORANGE,
    // COLOR_PINK,
    // COLOR_YELLOW,
};

static color_name_t pre_strategy_colors[NUM_OF_PRE_STRATEGIES] = {
    COLOR_GREEN,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_BLUE,
    COLOR_ORANGE,
    COLOR_ORANGE,
    COLOR_PINK,
    COLOR_PINK,
    COLOR_YELLOW,
    // COLOR_YELLOW,
};

static color_name_t calib_mode_colors[NUM_OF_CALIB_MODES] = {
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_ORANGE,
    COLOR_PINK,
    COLOR_YELLOW,
};

static const char * strategy_names[] = {
    "star",
    "small steps",
    "defensive"
};


static sumo_parameters_t init_parameters_default = {
    .strategy = 0,
    .pre_strategy = 0,
    .calib_mode = 0,
    .enabled_distance_sensors = 0b111111111,
    .enabled_line_sensors = 0b001111,
    .star_speed = 60,
    .max_speed = 100,
    .reverse_speed = 100,
    .reverse_time_ms = 250,
    .turn_speed = 100,
    .turn_180_right_time_ms = 300,
    .turn_180_left_time_ms = 300,
    .step_wait_time_ms = 1500,
    .step_advance_time_ms = 150,
    .time_ms_to_cross_at_60_vel = 600,
    .is_stucked_timeout = 3000,
    .attack_when_near = 0,
};

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void read_and_update_parameter_16_bit(uint16_t eeprom_addr, uint16_t * updated_data){
    uint16_t eeprom_data;

    if (BSP_eeprom_read(eeprom_addr, &eeprom_data) ==  EEPROM_OK){
        *updated_data = eeprom_data;
    } else {
        BSP_eeprom_write(eeprom_addr, *updated_data);
    }

}

static void read_and_update_parameter_8_bit(uint16_t eeprom_addr, uint8_t * updated_data){
    uint16_t eeprom_data;

    if (BSP_eeprom_read(eeprom_addr, &eeprom_data) ==  EEPROM_OK){
        *updated_data = eeprom_data;
    } else {
        BSP_eeprom_write(eeprom_addr, *updated_data);
    }

}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void parameters_init(sumo_parameters_t *params){

    sumo_parameters_t temp_params = init_parameters_default;

    read_and_update_parameter_16_bit(EN_DIST_SENSOR_ADDR, &temp_params.enabled_distance_sensors);
    read_and_update_parameter_8_bit(EN_LINE_SENSOR_ADDR, &temp_params.enabled_line_sensors);

    read_and_update_parameter_8_bit(STAR_SPEED_ADDR, &temp_params.star_speed);
    read_and_update_parameter_8_bit(MAX_SPEED_ADDR, &temp_params.max_speed);

    read_and_update_parameter_8_bit(REVERSE_SPEED_ADDR, &temp_params.reverse_speed);
    read_and_update_parameter_16_bit(REVERSE_TIME_MS_ADDR, &temp_params.reverse_time_ms);
    read_and_update_parameter_8_bit(TURN_SPEED_ADDR, &temp_params.turn_speed);

    read_and_update_parameter_16_bit(STEP_WAIT_TIME_MS_ADDR, &temp_params.step_wait_time_ms);
    read_and_update_parameter_16_bit(STEP_ADVANCE_TIME_MS_ADDR, &temp_params.step_advance_time_ms);

    read_and_update_parameter_16_bit(TURN_180_RIGHT_TIME_ADDR, &temp_params.turn_180_right_time_ms);
    read_and_update_parameter_16_bit(TURN_180_LEFT_TIME_ADDR, &temp_params.turn_180_left_time_ms);
    read_and_update_parameter_16_bit(TIME_MS_TO_CROSS_AT_60_ADDR, &temp_params.time_ms_to_cross_at_60_vel);

    read_and_update_parameter_16_bit(TIMEOUT_IS_STUCKED, &temp_params.is_stucked_timeout);

    *params = temp_params;

    distance_service_set_mask(params->enabled_distance_sensors);
    adc_line_set_mask(params->enabled_line_sensors);

}


void parameters_report(sumo_parameters_t params, uint8_t config_num){

    char buffer[20] = {0};
    switch (config_num)
    {
        case 0:
            snprintf(buffer, 20, "ss:%d:%s", 0, strategy_names[0]);
            break;
        case 1:
            snprintf(buffer, 20, "ss:%d:%s", 1, strategy_names[1]);
            break;
        case 2:
            snprintf(buffer, 20, "ss:%d:%s", 2, strategy_names[2]);
            break;
        case 3:
            snprintf(buffer, 20, "sens:%hu:%hu", params.enabled_distance_sensors, params.enabled_line_sensors);
            break;
        case 4:
            snprintf(buffer, 20, "rev:%hu:%hu", params.reverse_speed, params.reverse_time_ms);
            break;
        case 5:
            snprintf(buffer, 20, "turn:%hu:%hu", params.turn_180_left_time_ms, params.turn_180_right_time_ms);
            break;
        case 6:
            snprintf(buffer, 20, "step:%hu", params.step_wait_time_ms);
            break;
        case 7:
            snprintf(buffer, 20, "str:%hu:%hu", params.strategy, params.pre_strategy);
            break;
        case 8:
            snprintf(buffer, 20, "mms:%hu", params.max_speed);
            break;
        case 9: {
            uint16_t dist_mask = distance_get_all_active();
            uint16_t line_mask = adc_line_get_all();
            snprintf(buffer, 20, "stat:%u:%u", dist_mask, line_mask);
            break;
        }
        
    }

    ble_service_send_data((uint8_t *)buffer, 20);

}

void report_raw_line_data_ble(bool front) {
    char buffer[20] = {0};

    if (front) {
        uint32_t line_fr = adc_get_raw_line(LINE_FR);
        uint32_t line_fl = adc_get_raw_line(LINE_FL);
        snprintf(buffer, 20, "linef:%lu:%lu", line_fl, line_fr);

    } else {
        uint32_t line_br = adc_get_raw_line(LINE_BR);
        uint32_t line_bl = adc_get_raw_line(LINE_BL);
        snprintf(buffer, 20, "lineb:%lu:%lu", line_br, line_bl);
    }

    ble_service_send_data((uint8_t *)buffer, 20);


}

void parameters_update_from_ble(sumo_parameters_t *params, uint8_t * data){

    ble_update_param_packet_t ble_packet;
    memcpy(ble_packet._raw, data, BLE_RECEIVE_PACKET_SIZE - 2);

    switch (ble_packet.header)
    {
    case BLE_DATA_HDR_CUST_STRATEGY:
        cust_strategy_update_from_ble(ble_packet.param_data, sizeof(ble_packet.param_data));
        break;
    case BLE_DATA_HDR_STRATEGY:
        parameters_set_strategy(params, ble_packet.param_data[0]);
        parameters_set_strategy_led(params);
        break;
    case BLE_DATA_HDR_PRE_STRATEGY:
        parameters_update_pre_strategy(params, ble_packet.param_data[0]);
        parameters_set_pre_strategy_led(params);
        break;
    case BLE_DATA_HDR_CALIB_MODE:
        parameters_update_calib_mode(params, ble_packet.param_data[0]);
        parameters_set_calib_mode_led(params);
        break;
    case BLE_DATA_HDR_EN_DISTANCE_SENSORS:
        params->enabled_distance_sensors = TWO_BYTES_TO_UINT16(ble_packet.param_data[0], ble_packet.param_data[1]);
        break;
    case BLE_DATA_HDR_EN_LINE_SENSORS:
        params->enabled_line_sensors = ble_packet.param_data[0];
        break;
    case BLE_DATA_HDR_STAR_SPEED:
        params->star_speed = ble_packet.param_data[0];
        break;
    case BLE_DATA_HDR_MAX_SPEED:
        params->max_speed = ble_packet.param_data[0];
        break;
    case BLE_DATA_HDR_REVERSE_SPEED:
        params->reverse_speed = ble_packet.param_data[0];
        break;
    case BLE_DATA_HDR_REVERSE_TIME_MS:
        params->reverse_time_ms = TWO_BYTES_TO_UINT16(ble_packet.param_data[0], ble_packet.param_data[1]);
        break;
    case BLE_DATA_HDR_TURN_SPEED:
        params->turn_speed = ble_packet.param_data[0];
        break;
    case BLE_DATA_HDR_TURN_180_RIGHT_TIME_MS:
        params->turn_180_right_time_ms  = TWO_BYTES_TO_UINT16(ble_packet.param_data[0], ble_packet.param_data[1]);
        break;
    case BLE_DATA_HDR_TURN_180_LEFT_TIME_MS:
        params->turn_180_left_time_ms  = TWO_BYTES_TO_UINT16(ble_packet.param_data[0], ble_packet.param_data[1]);
        break;
    case BLE_DATA_HDR_STEP_WAIT_TIME_MS:
        params->step_wait_time_ms = TWO_BYTES_TO_UINT16(ble_packet.param_data[0], ble_packet.param_data[1]);
        break;
    case BLE_DATA_HDR_STEP_ADVANCE_TIME_MS:
        params->step_advance_time_ms = TWO_BYTES_TO_UINT16(ble_packet.param_data[0], ble_packet.param_data[1]);
        break;
    case BLE_DATA_HDR_TIME_MS_TO_CROSS_AT_60_VEL:
        params->time_ms_to_cross_at_60_vel = TWO_BYTES_TO_UINT16(ble_packet.param_data[0], ble_packet.param_data[1]);
        break;
    case BLE_DATA_HDR_IS_STUCKED_TIMEOUT:
        params->is_stucked_timeout = TWO_BYTES_TO_UINT16(ble_packet.param_data[0], ble_packet.param_data[1]);
        break;
    case BLE_DATA_HDR_ATTACK_WHEN_NEAR:
        params->attack_when_near = ble_packet.param_data[0];
        break;
    default:
        break;
    }



    // TODO: Save in eeprom
}

void parameters_set_strategy(sumo_parameters_t *params, uint8_t strategy){

    if (strategy >= NUM_OF_STRATEGIES){
        strategy = 0;
    }

    params->strategy = strategy;

}

void parameters_update_pre_strategy(sumo_parameters_t *params, uint8_t pre_strategy){

    if (pre_strategy >= NUM_OF_PRE_STRATEGIES){
        pre_strategy = 0;
    }

    params->pre_strategy = pre_strategy;

}

void parameters_update_calib_mode(sumo_parameters_t *params, uint8_t calib_mode){

    if (calib_mode >= NUM_OF_CALIB_MODES){
        calib_mode = 0;
    }

    params->calib_mode = calib_mode;

}


void parameters_set_strategy_led(sumo_parameters_t *params){

    if (params->strategy < NUM_OF_STRATEGIES){
        led_stripe_set_range_color(0, (LED_STRIPE_NUM / 2), strategy_colors[params->strategy]);
    } else {
        led_stripe_set_range_color(0, (LED_STRIPE_NUM / 2), COLOR_BLACK);
    }

}

void parameters_set_pre_strategy_led(sumo_parameters_t *params){

    if (params->pre_strategy < NUM_OF_PRE_STRATEGIES){
        if (params->pre_strategy % 2 == 0){
            led_stripe_set_range_color(((LED_STRIPE_NUM / 4) * 2), ((LED_STRIPE_NUM / 4) * 3), pre_strategy_colors[params->pre_strategy]);
            led_stripe_set_range_color(((LED_STRIPE_NUM / 4) * 3), ((LED_STRIPE_NUM / 4) * 4), COLOR_BLACK);
        } else {
            led_stripe_set_range_color(((LED_STRIPE_NUM / 4) * 2), ((LED_STRIPE_NUM / 4) * 4), pre_strategy_colors[params->pre_strategy]);
        }
    } else {
        led_stripe_set_range_color((LED_STRIPE_NUM / 2), LED_STRIPE_NUM, COLOR_BLACK);
    }

}

void parameters_set_calib_mode_led(sumo_parameters_t *params){

    if (params->calib_mode < NUM_OF_CALIB_MODES){
        led_stripe_set_range_color(0, (LED_STRIPE_NUM), calib_mode_colors[params->calib_mode]);
    } else {
        led_stripe_set_range_color(0, (LED_STRIPE_NUM / 2), COLOR_BLACK);
    }

}

uint16_t get_time_to_turn_ms(uint16_t degrees, uint8_t turn_speed, side_t side, sumo_parameters_t *params) {

    // TODO: Account for non linear effects on multiplicators
    double angle_multiplicator = degrees / 180.0;
    double speed_multiplicator = turn_speed / REFERENCE_TURN_SPEED;

    uint16_t turn_time_ms;

    if (side == SIDE_LEFT){
        turn_time_ms = (angle_multiplicator * params->turn_180_left_time_ms) * speed_multiplicator;
    } else {
        turn_time_ms = (angle_multiplicator * params->turn_180_right_time_ms) * speed_multiplicator;
    }

    return turn_time_ms;

}

uint16_t get_time_to_move_ms(uint16_t distance_cm, uint8_t speed, sumo_parameters_t *params) {

    double reference_speed_cm_per_ms = REFERENCE_DIST_CM / (double) params->time_ms_to_cross_at_60_vel;
    double speed_multiplicator = speed / REFERENCE_SPEED;

    // TODO: Account for non linear effects on speed multiplicator
    uint16_t move_time_ms = distance_cm / (reference_speed_cm_per_ms * speed_multiplicator);

    return move_time_ms;

}

/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "parameters.h"
#include "parameters_set.h"
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

#define NUM_OF_STRATEGIES     3
#define NUM_OF_PRE_STRATEGIES 9
#define NUM_OF_CALIB_MODES    6

#define ARENA_LENGHT_CM   154.0f
#define SUMO_LENGHT_CM    20.0f
#define REFERENCE_DIST_CM (ARENA_LENGHT_CM - SUMO_LENGHT_CM)

#define REFERENCE_SPEED 100.0f

#define REFERENCE_TURN_SPEED 100.0f

#define BLE_PACKET_TRANSMIT_SIZE 20
#define PARAMS_PACKET_VERSION    0

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/
typedef union {
    uint8_t _raw[BLE_RECEIVE_PACKET_SIZE - 2];
    struct {
        uint8_t header;
        uint8_t param_data[BLE_RECEIVE_PACKET_SIZE - 3];
    };

} ble_update_param_packet_t;

typedef union {
    uint8_t _raw[BLE_PACKET_TRANSMIT_SIZE];
    struct {
        uint8_t version;
        uint8_t header;

        uint8_t strategy;
        uint8_t pre_strategy;
        uint8_t calib_mode;
        uint16_t en_dist_sensors;
        uint8_t en_line_sensors;
        uint8_t star_speed;
        uint8_t max_speed;
        uint8_t reverse_speed;
        uint16_t reverse_time_ms;
        uint8_t line_seen_turn_angle;
        uint16_t turn_180_right_time_ms;
        uint16_t turn_180_left_time_ms;
        uint8_t star_full_speed_time_ms;

        uint8_t empty[1];
    } __attribute__((packed, scalar_storage_order("big-endian")));

} ble_transmit_packet_0_t;

typedef union {
    uint8_t _raw[BLE_PACKET_TRANSMIT_SIZE];
    struct {
        uint8_t version;
        uint8_t header;

        uint16_t step_wait_time_ms;
        uint16_t step_advance_time_ms;
        uint16_t time_ms_to_cross_at_max_vel;
        uint16_t is_stucked_timeout_ms;

        uint8_t attack_when_near;
        uint8_t empty[9];
    } __attribute__((packed, scalar_storage_order("big-endian")));

} ble_transmit_packet_1_t;

typedef union {
    uint8_t _raw[BLE_PACKET_TRANSMIT_SIZE];
    struct {
        uint8_t version;
        uint8_t header;

        uint16_t dist_mask;
        uint16_t line_mask;

        uint16_t ctrl_battery;
        uint16_t pwr_battery;

        uint8_t empty[10];

    } __attribute__((packed, scalar_storage_order("big-endian")));

} ble_transmit_packet_2_t;

typedef union {
    uint8_t _raw[BLE_PACKET_TRANSMIT_SIZE];
    struct {
        uint8_t version;
        uint8_t header;

        uint32_t line_fr;
        uint32_t line_fl;
        uint32_t line_br;
        uint32_t line_bl;
        uint8_t empty[2];

    } __attribute__((packed, scalar_storage_order("big-endian")));

} ble_transmit_packet_3_t;

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/
static void read_and_update_parameter_16_bit(uint16_t eeprom_addr, uint16_t *updated_data);
static void read_and_update_parameter_8_bit(uint16_t eeprom_addr, uint8_t *updated_data);
/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/

static color_name_t strategy_colors[NUM_OF_STRATEGIES] = {
    COLOR_GREEN, COLOR_BLUE, COLOR_ORANGE,
    // COLOR_PINK,
    // COLOR_YELLOW,
};

static color_name_t pre_strategy_colors[NUM_OF_PRE_STRATEGIES] = {
    COLOR_BLACK, COLOR_GREEN, COLOR_BLUE, COLOR_BLUE, COLOR_ORANGE, COLOR_ORANGE, COLOR_PINK, COLOR_PINK, COLOR_PURPLE,
    // COLOR_WHITE,
    // COLOR_YELLOW,
};

static color_name_t calib_mode_colors[NUM_OF_CALIB_MODES] = {
    COLOR_GREEN, COLOR_BLUE, COLOR_ORANGE, COLOR_PINK, COLOR_PURPLE, COLOR_RED,
};

// static const char * strategy_names[] = {
//     "star",
//     "small steps",
//     "defensive"
// };

static sumo_parameters_t init_parameters_default = {
    .strategy = 0,
    .pre_strategy = 0,
    .calib_mode = 0,
    .enabled_distance_sensors = 0b001111111,
    .enabled_line_sensors = 0b001111,
    .star_speed = 50,
    .star_full_speed_time_ms = 50,
    .max_speed = 100,
    .reverse_speed = 100,
    .reverse_time_ms = 140,
    .line_seen_turn_angle = 120,
    .turn_180_right_time_ms = 95,
    .turn_180_left_time_ms = 95,
    .step_wait_time_ms = 1500,
    .step_advance_time_ms = 150,
    .time_ms_to_cross_at_max_vel = 200,
    .is_stucked_timeout_ms = 2000,
    .attack_when_near = 0,
};

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

static void read_and_update_parameter_16_bit(uint16_t eeprom_addr, uint16_t *updated_data)
{
    uint16_t eeprom_data;

    if (BSP_eeprom_read(eeprom_addr, &eeprom_data) == EEPROM_OK) {
        *updated_data = eeprom_data;
    } else {
        BSP_eeprom_write(eeprom_addr, *updated_data);
    }
}

static void read_and_update_parameter_8_bit(uint16_t eeprom_addr, uint8_t *updated_data)
{
    uint16_t eeprom_data;

    if (BSP_eeprom_read(eeprom_addr, &eeprom_data) == EEPROM_OK) {
        *updated_data = eeprom_data;
    } else {
        BSP_eeprom_write(eeprom_addr, *updated_data);
    }
}

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void parameters_init(sumo_parameters_t *params)
{
    sumo_parameters_t temp_params = init_parameters_default;

    read_and_update_parameter_16_bit(EN_DIST_SENSOR_ADDR, &temp_params.enabled_distance_sensors);
    read_and_update_parameter_8_bit(EN_LINE_SENSOR_ADDR, &temp_params.enabled_line_sensors);

    read_and_update_parameter_8_bit(STAR_SPEED_ADDR, &temp_params.star_speed);
    read_and_update_parameter_8_bit(MAX_SPEED_ADDR, &temp_params.max_speed);
    read_and_update_parameter_8_bit(STAR_FULL_SPEED_TIME_MS_ADDR, &temp_params.star_full_speed_time_ms);

    read_and_update_parameter_8_bit(REVERSE_SPEED_ADDR, &temp_params.reverse_speed);
    read_and_update_parameter_16_bit(REVERSE_TIME_MS_ADDR, &temp_params.reverse_time_ms);
    read_and_update_parameter_8_bit(LINE_TURN_ANGLE_ADDR, &temp_params.line_seen_turn_angle);

    read_and_update_parameter_16_bit(STEP_WAIT_TIME_MS_ADDR, &temp_params.step_wait_time_ms);
    read_and_update_parameter_16_bit(STEP_ADVANCE_TIME_MS_ADDR, &temp_params.step_advance_time_ms);

    read_and_update_parameter_16_bit(TURN_180_RIGHT_TIME_ADDR, &temp_params.turn_180_right_time_ms);
    read_and_update_parameter_16_bit(TURN_180_LEFT_TIME_ADDR, &temp_params.turn_180_left_time_ms);
    read_and_update_parameter_16_bit(TIME_MS_TO_CROSS_AT_100_ADDR, &temp_params.time_ms_to_cross_at_max_vel);

    read_and_update_parameter_16_bit(TIMEOUT_IS_STUCKED_ADDR, &temp_params.is_stucked_timeout_ms);

    *params = temp_params;

    distance_service_set_mask(params->enabled_distance_sensors);
    adc_line_set_mask(params->enabled_line_sensors);
}

void parameters_report(sumo_parameters_t params, uint8_t config_num)
{
    char buffer[BLE_PACKET_TRANSMIT_SIZE] = { 0 };
    switch (config_num) {
    case 0: {
        ble_transmit_packet_0_t packet_0 = { 0 };
        packet_0.version = PARAMS_PACKET_VERSION;
        packet_0.header = 0;
        packet_0.strategy = params.strategy;
        packet_0.pre_strategy = params.pre_strategy;
        packet_0.calib_mode = params.calib_mode;
        packet_0.en_dist_sensors = params.enabled_distance_sensors;
        packet_0.en_line_sensors = params.enabled_line_sensors;
        packet_0.star_speed = params.star_speed;
        packet_0.star_full_speed_time_ms = params.star_full_speed_time_ms;
        packet_0.max_speed = params.max_speed;
        packet_0.reverse_speed = params.reverse_speed;
        packet_0.reverse_time_ms = params.reverse_time_ms;
        packet_0.line_seen_turn_angle = params.line_seen_turn_angle;
        packet_0.turn_180_right_time_ms = params.turn_180_right_time_ms;
        packet_0.turn_180_left_time_ms = params.turn_180_left_time_ms;
        memcpy(buffer, packet_0._raw, BLE_PACKET_TRANSMIT_SIZE);
        break;
    }
    case 1: {
        ble_transmit_packet_1_t packet_1 = { 0 };
        packet_1.version = PARAMS_PACKET_VERSION;
        packet_1.header = 1;
        packet_1.step_wait_time_ms = params.step_wait_time_ms;
        packet_1.step_advance_time_ms = params.step_advance_time_ms;
        packet_1.time_ms_to_cross_at_max_vel = params.time_ms_to_cross_at_max_vel;
        packet_1.is_stucked_timeout_ms = params.is_stucked_timeout_ms;
        packet_1.attack_when_near = params.attack_when_near;
        memcpy(buffer, packet_1._raw, BLE_PACKET_TRANSMIT_SIZE);
        break;
    }
    case 2: {
        ble_transmit_packet_2_t packet_2 = { 0 };
        packet_2.version = PARAMS_PACKET_VERSION;
        packet_2.header = 2;
        packet_2.dist_mask = distance_get_all_active();
        packet_2.line_mask = adc_line_get_all();
        packet_2.ctrl_battery = adc_get_ctrl_bat_mv();
        packet_2.pwr_battery = adc_get_pwr_bat_mv();

        memcpy(buffer, packet_2._raw, BLE_PACKET_TRANSMIT_SIZE);
        break;
    }

    default: {
        return;
    }
    }

    ble_service_send_data((uint8_t *)buffer, BLE_PACKET_TRANSMIT_SIZE);
}

void report_raw_line_data_ble()
{
    char buffer[BLE_PACKET_TRANSMIT_SIZE] = { 0 };
    ble_transmit_packet_3_t packet_3;

    packet_3.version = PARAMS_PACKET_VERSION;
    packet_3.header = 3;

    packet_3.line_fr = adc_get_raw_line(LINE_FR);
    packet_3.line_fl = adc_get_raw_line(LINE_FL);
    packet_3.line_br = adc_get_raw_line(LINE_BR);
    packet_3.line_bl = adc_get_raw_line(LINE_BL);

    memcpy(buffer, packet_3._raw, BLE_PACKET_TRANSMIT_SIZE);

    ble_service_send_data((uint8_t *)buffer, BLE_PACKET_TRANSMIT_SIZE);
}

param_error_t parameters_update_from_ble(sumo_parameters_t *params, uint8_t *data)
{
    ble_update_param_packet_t ble_packet;
    memcpy(ble_packet._raw, data, BLE_RECEIVE_PACKET_SIZE - 2);

    switch (ble_packet.header) {
    case BLE_DATA_HDR_STRATEGY_AND_PRE:
        parameters_set_strategy(params, ble_packet.param_data[0]);
        parameters_update_pre_strategy(params, ble_packet.param_data[1]);
        parameters_set_strategy_led(params);
        parameters_set_pre_strategy_led(params);
        break;
    case BLE_DATA_HDR_CALIB_MODE:
        parameters_update_calib_mode(params, ble_packet.param_data[0]);
        parameters_set_calib_mode_led(params);
        break;
    case BLE_DATA_HDR_CUST_STRATEGY: {
        uint8_t ret;
        ret = cust_strategy_update_from_ble(ble_packet.param_data, sizeof(ble_packet.param_data));
        return (ret == 0) ? PARAM_OK : PARAM_ERROR;
        break;
    }
    default: {
        uint16_t new_param = TWO_BYTES_TO_UINT16(ble_packet.param_data[0], ble_packet.param_data[1]);
        return set_validade_new_param_uint16_t(ble_packet.header, params, new_param);
    }
    }

    return PARAM_OK;
}

void parameters_set_strategy(sumo_parameters_t *params, uint8_t strategy)
{
    if (strategy >= NUM_OF_STRATEGIES) {
        strategy = 0;
    }

    params->strategy = strategy;
}

void parameters_update_pre_strategy(sumo_parameters_t *params, uint8_t pre_strategy)
{
    if (pre_strategy >= NUM_OF_PRE_STRATEGIES) {
        pre_strategy = 0;
    }

    params->pre_strategy = pre_strategy;
}

void parameters_update_calib_mode(sumo_parameters_t *params, uint8_t calib_mode)
{
    if (calib_mode >= NUM_OF_CALIB_MODES) {
        calib_mode = 0;
    }

    params->calib_mode = calib_mode;
}

void parameters_set_strategy_led(sumo_parameters_t *params)
{
    if (params->strategy < NUM_OF_STRATEGIES) {
        led_stripe_prepare_range_color(0, (LED_STRIPE_NUM / 2), strategy_colors[params->strategy]);
    } else {
        led_stripe_prepare_range_color(0, (LED_STRIPE_NUM / 2), COLOR_BLACK);
    }

    led_stripe_send();
}

void parameters_set_pre_strategy_led(sumo_parameters_t *params)
{
    if (params->pre_strategy < NUM_OF_PRE_STRATEGIES) {
        if (params->pre_strategy % 2 == 0) {
            led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 2), ((LED_STRIPE_NUM / 4) * 3),
                                           pre_strategy_colors[params->pre_strategy]);
            led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 3), ((LED_STRIPE_NUM / 4) * 4), COLOR_BLACK);
        } else {
            led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 2), ((LED_STRIPE_NUM / 4) * 4),
                                           pre_strategy_colors[params->pre_strategy]);
        }
    } else {
        led_stripe_prepare_range_color((LED_STRIPE_NUM / 2), LED_STRIPE_NUM, COLOR_BLACK);
    }

    led_stripe_send();
}

void parameters_set_pre_and_strategy_leds(sumo_parameters_t *params)
{
    if (params->strategy < NUM_OF_STRATEGIES) {
        led_stripe_prepare_range_color(0, (LED_STRIPE_NUM / 2), strategy_colors[params->strategy]);
    } else {
        led_stripe_prepare_range_color(0, (LED_STRIPE_NUM / 2), COLOR_BLACK);
    }

    if (params->pre_strategy < NUM_OF_PRE_STRATEGIES) {
        if (params->pre_strategy % 2 == 0) {
            led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 2), ((LED_STRIPE_NUM / 4) * 3),
                                           pre_strategy_colors[params->pre_strategy]);
            led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 3), ((LED_STRIPE_NUM / 4) * 4), COLOR_BLACK);
        } else {
            led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 2), ((LED_STRIPE_NUM / 4) * 4),
                                           pre_strategy_colors[params->pre_strategy]);
        }
    } else {
        led_stripe_prepare_range_color((LED_STRIPE_NUM / 2), LED_STRIPE_NUM, COLOR_BLACK);
    }

    led_stripe_send();
}

void parameters_set_calib_mode_led(sumo_parameters_t *params)
{
    if (params->calib_mode < NUM_OF_CALIB_MODES) {
        led_stripe_prepare_range_color(0, (LED_STRIPE_NUM), calib_mode_colors[params->calib_mode]);
    } else {
        led_stripe_prepare_range_color(0, (LED_STRIPE_NUM / 2), COLOR_BLACK);
    }

    led_stripe_send();
}

uint16_t get_time_to_turn_ms(uint16_t degrees, uint8_t turn_speed, side_t side, sumo_parameters_t *params)
{
    double turn_angle_dividers[] = { 12.00, 6.00, 4.00, 3.00, 2.40, 2.00, 1.71, 1.50, 1.33, 1.20, 1.09, 1.00 };

    // Find experimentally
    // double turn_angle_dividers[] = {
    //     7.90,  // time to turn 15 degrees
    //     4.30,  // time to turn 30 degrees
    //     3.10,  // time to turn 45 degrees
    //     2.40,  // time to turn 60 degrees
    //     2.00,  // time to turn 75 degrees
    //     1.70,  // time to turn 90 degrees
    //     1.50,  // time to turn 105 degrees
    //     1.30,  // time to turn 120 degrees
    //     1.25, // time to turn 135 degrees
    //     1.14, // time to turn 150 degrees
    //     1.06, // time to turn 165 degrees
    //     1.00,  // time to turn 180 degrees
    // };

    uint8_t index;
    if (degrees >= 180) {
        index = 11;
    } else if (degrees < 15) {
        index = 0;
    } else {
        index = round((degrees / 15.0) - 1);
    }

    double angle_multiplicator = degrees / (15.0 * (index + 1));
    double speed_multiplicator = REFERENCE_TURN_SPEED / (double) turn_speed;
    double battery_multiplicator = 1 / adc_get_pwr_bat_percent();
    uint16_t turn_time_ms;

    if (side == SIDE_LEFT) {
        double reference_turn_time = params->turn_180_left_time_ms / turn_angle_dividers[index];
        turn_time_ms = (angle_multiplicator * reference_turn_time) * speed_multiplicator * battery_multiplicator;
    } else {
        double reference_turn_time = params->turn_180_right_time_ms / turn_angle_dividers[index];
        turn_time_ms = (angle_multiplicator * reference_turn_time) * speed_multiplicator * battery_multiplicator;
    }

    return turn_time_ms;
}

uint16_t get_time_to_move_ms(uint16_t distance_cm, uint8_t speed, sumo_parameters_t *params)
{
    double distance_dividers[] = { 8, 4, 2.67, 2, 1.6, 1.33, 1.14, 1 };

    // Find experimentally
    // double distance_dividers[] = {
    //     6.00, // time cross 1/8 arena = 16.75cm
    //     5.00, // time cross 2/8 arena = 33.50cm
    //     3.00, // time cross 3/8 arena = 50.25cm
    //     2.00, // time cross 4/8 arena = 67.00cm
    //     1.60, // time cross 5/8 arena = 83.75cm
    //     1.33, // time cross 6/8 arena = 100.50cm
    //     1.14, // time cross 7/8 arena = 117.25cm
    //     1.00  // time cross complete arena (154 - 20) = 134cm
    // };

    uint8_t index;
    if (distance_cm >= 134) {
        index = 7;
    } else if (distance_cm < 17) {
        index = 0;
    } else {
        index = round((distance_cm / 16.75) - 1);
    }

    double distance_multiplicator = distance_cm / (16.75 * (index + 1));
    double battery_multiplicator = 1 / adc_get_pwr_bat_percent();
    double speed_multiplicator = REFERENCE_SPEED / (double) speed;

    double reference_move_time_ms = params->time_ms_to_cross_at_max_vel / distance_dividers[index];
    uint16_t move_time_ms = (distance_multiplicator * reference_move_time_ms) * speed_multiplicator * battery_multiplicator;

    return move_time_ms;
}

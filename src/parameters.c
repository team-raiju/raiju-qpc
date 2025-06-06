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
#define NUM_OF_PRE_STRATEGIES 20
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

        uint16_t kp;
        uint16_t kd;
        uint16_t ki;

        uint8_t near_angle_th;
        uint8_t inclinated_th;

        uint8_t empty[1];
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
static double battery_multiplicator = 1.0;

static color_name_t strategy_colors[NUM_OF_STRATEGIES] = {
    COLOR_BLUE, COLOR_GREEN, COLOR_ORANGE,
    // COLOR_PINK,
    // COLOR_YELLOW,
};

static color_name_t pre_strategy_colors[] = { COLOR_BLUE,  COLOR_GREEN, COLOR_ORANGE, COLOR_PURPLE,
                                              COLOR_WHITE, COLOR_PINK,  COLOR_YELLOW };

static color_name_t calib_mode_colors[NUM_OF_CALIB_MODES] = { COLOR_BLUE,   COLOR_GREEN, COLOR_ORANGE,
                                                              COLOR_PURPLE, COLOR_WHITE, COLOR_PINK };

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
    .star_speed = 40,
    .star_full_speed_time_ms = 40,
    .max_speed = 100,
    .reverse_speed = 100,
    .reverse_time_ms = 140,
    .line_seen_turn_angle = 135,
    .turn_180_right_time_ms = 75,
    .turn_180_left_time_ms = 75,
    .step_wait_time_ms = 1500,
    .step_advance_time_ms = 60,
    .time_ms_to_cross_at_max_vel = 210,
    .is_stucked_timeout_ms = 1800,
    .attack_when_near = 0,
    .kp = 0,
    .kd = 0,
    .ki = 0,
    .near_angle_th = 0,
    .inclinated_th = 10,
    .current_state = STATE_IDLE,
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

    read_and_update_parameter_16_bit(KP, &temp_params.kp);
    read_and_update_parameter_16_bit(KD, &temp_params.kd);
    read_and_update_parameter_16_bit(KI, &temp_params.ki);

    read_and_update_parameter_8_bit(NEAR_ANGLE_TH, &temp_params.near_angle_th);
    read_and_update_parameter_8_bit(INCLINATED_TH, &temp_params.inclinated_th);

    read_and_update_parameter_8_bit(EE_CURRENT_STATE_ADDR, &temp_params.current_state);

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
        packet_1.kp = params.kp;
        packet_1.kd = params.kd;
        packet_1.ki = params.ki;
        packet_1.near_angle_th = params.near_angle_th;
        packet_1.inclinated_th = params.inclinated_th;
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
    uint8_t index;
    uint8_t colors_arr_size = sizeof(pre_strategy_colors) / sizeof(color_name_t);

    /* Invalid param */
    if (params->pre_strategy >= NUM_OF_PRE_STRATEGIES) {
        led_stripe_prepare_range_color((LED_STRIPE_NUM / 2), LED_STRIPE_NUM, COLOR_BLACK);
        led_stripe_send();
        return;
    }
    /* Invalid param */
    if (params->pre_strategy >= (3 * colors_arr_size)) {
        led_stripe_prepare_range_color((LED_STRIPE_NUM / 2), LED_STRIPE_NUM, COLOR_BLACK);
        led_stripe_send();
        return;
    }

    /* First Half Led RED and other half first color of "pre_strategy_colors" array */
    if (params->pre_strategy >= 2 * colors_arr_size) {
        index = (params->pre_strategy - 2 * colors_arr_size);
        led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 2), ((LED_STRIPE_NUM / 4) * 3), pre_strategy_colors[index]);
        led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 3), ((LED_STRIPE_NUM / 4) * 4), COLOR_RED);
        led_stripe_send();
        return;
    }

    /* For even number half black and half other color */
    /* For odd number all same color*/
    index = params->pre_strategy / 2;
    if (params->pre_strategy % 2 == 0) {
        led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 2), ((LED_STRIPE_NUM / 4) * 3), pre_strategy_colors[index]);
        led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 3), ((LED_STRIPE_NUM / 4) * 4), COLOR_BLACK);
    } else {
        led_stripe_prepare_range_color(((LED_STRIPE_NUM / 4) * 2), ((LED_STRIPE_NUM / 4) * 4), pre_strategy_colors[index]);
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

void set_reference_voltage()
{
    battery_multiplicator = 1 / adc_get_pwr_bat_percent();
    battery_multiplicator = constrain(battery_multiplicator, 0.9, 1.0);
}

uint16_t get_time_to_turn_ms(uint16_t degrees, uint8_t turn_speed, side_t side, sumo_parameters_t *params)
{
    // Find experimentally. turn 45, 90, 135, 180
    double turn_angle_dividers[] = { 2, 1.36, 1.2, 1 };

    uint8_t index;
    if (degrees >= 180) {
        index = 3;
    } else if (degrees < 45) {
        index = 0;
    } else {
        index = round((degrees / 45.0) - 1);
    }

    double angle_multiplicator = degrees / (45.0 * (index + 1));
    double speed_multiplicator = REFERENCE_TURN_SPEED / (double)turn_speed;

    uint16_t turn_time_ms;

    if (side == SIDE_LEFT) {
        double reference_turn_time = params->turn_180_left_time_ms / turn_angle_dividers[index];
        turn_time_ms = (angle_multiplicator * reference_turn_time) * speed_multiplicator * battery_multiplicator;
    } else {
        double reference_turn_time = params->turn_180_right_time_ms / turn_angle_dividers[index];
        turn_time_ms = (angle_multiplicator * reference_turn_time) * speed_multiplicator * battery_multiplicator;
    }

    turn_time_ms = constrain(turn_time_ms, 1, 1000);
    return turn_time_ms;
}

uint16_t get_time_to_move_ms(uint16_t distance_cm, uint8_t speed, sumo_parameters_t *params)
{
#ifndef Q_SPY
    // Find experimentally. Move 33.5cm, 67cm, 100.5, 134cm
    double distance_dividers[] = { 3.2, 1.875, 1.30, 1 };

    uint8_t index;
    if (distance_cm >= 134) {
        index = 3;
    } else if (distance_cm < 17) {
        index = 0;
    } else {
        index = round((distance_cm / 33.5) - 1);
    }

    double distance_multiplicator = distance_cm / (33.5 * (index + 1));
    double speed_multiplicator = REFERENCE_SPEED / (double)speed;

    double reference_move_time_ms = params->time_ms_to_cross_at_max_vel / distance_dividers[index];
    uint16_t move_time_ms = (distance_multiplicator * reference_move_time_ms) * speed_multiplicator * battery_multiplicator;
    move_time_ms = constrain(move_time_ms, 2, 600);

#else
    double reference_speed_cm_per_ms = REFERENCE_DIST_CM / (double)params->time_ms_to_cross_at_max_vel;
    double speed_multiplicator = speed / REFERENCE_SPEED;
    uint16_t move_time_ms = (distance_cm / (reference_speed_cm_per_ms * speed_multiplicator));
    move_time_ms = constrain(move_time_ms, 2, 1200);
#endif

    return move_time_ms;
}

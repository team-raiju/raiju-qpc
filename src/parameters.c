#include <stdio.h>
#include <string.h>
#include "parameters.h"
#include "ble_service.h"
#include "utils.h"
#include "bsp_eeprom.h"

const char * strategy_names[] = {
    "star",
    "small steps"
};


static sumo_parameters_t init_parameters_default = {
    .current_strategy = 0,
    .current_pre_strategy = 0,
    .enabled_distance_sensors = 0xff,
    .enabled_line_sensors = 0xff,
    .max_speed = 100,
    .reverse_speed = 100,
    .reverse_time_ms = 250,
    .turn_speed = 100,
    .star_speed = 60,
    .step_wait_time_ms = 3000,
    .step_advance_time_ms = 150,
    .turn_180_time_ms = 800,
};

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


void parameters_init(sumo_parameters_t *params){

    read_and_update_parameter_16_bit(TURN_180_TIME_ADDR, &init_parameters_default.turn_180_time_ms);
    read_and_update_parameter_8_bit(STAR_SPEED_ADDR, &init_parameters_default.star_speed);

    *params = init_parameters_default;

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
            snprintf(buffer, 20, "sens:%hu:%hu", params.enabled_distance_sensors, params.enabled_line_sensors);
            break;
        case 3:
            snprintf(buffer, 20, "rev:%hu:%hu", params.reverse_speed, params.reverse_time_ms);
            break;
        case 4:
            snprintf(buffer, 20, "turn:%hu:%hu", params.turn_speed, params.turn_180_time_ms);
            break;
        case 5:
            snprintf(buffer, 20, "step:%hu", params.step_wait_time_ms);
            break;
        case 6:
            snprintf(buffer, 20, "str:%hu:%hu", params.current_pre_strategy, params.current_strategy);
            break;
        case 7:
            snprintf(buffer, 20, "mms:%hu", params.max_speed);
            break;
    }

    ble_service_send_data((uint8_t *)buffer, 20);

}

void parameters_update_from_ble(sumo_parameters_t *params, uint8_t * last_data){

    ble_rcv_packet_t ble_packet;
    memcpy(ble_packet._raw, last_data, BLE_RECEIVE_PACKET_SIZE);

    params->enabled_distance_sensors = ble_packet.enabledDistanceSensors;
    params->enabled_line_sensors = ble_packet.enabledLineSensors;
    params->reverse_speed = ble_packet.reverseSpeed;
    params->reverse_time_ms = ble_packet.reverseTimeMs;
    params->turn_speed = ble_packet.turnSpeed;
    params->turn_180_time_ms  = ble_packet.turnTimeMs;
    params->step_wait_time_ms = ble_packet.stepWaitTimeMs;
    params->current_pre_strategy = ble_packet.preStrategy;
    params->current_strategy = ble_packet.strategy;
    params->max_speed = ble_packet.maxMotorSpeed;

    // Save in eeprom
}
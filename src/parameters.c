#include <stdio.h>
#include "parameters.h"
#include "ble_service.h"
#include "utils.h"

const char * strategy_names[] = {
    "STAR_STRATEGY",
    "SMALL_STEPS_STRATEGY"
};



void report_sumo_parameters(sumo_parameters_t parameters){

    char buffer[20];
    snprintf(buffer, 20, "turn:%hu:%hu",  parameters.turn_180_time_ms,  parameters.turn_180_time_ms);
    ble_service_send_data((uint8_t *)buffer, 20);

    for (long unsigned int i = 0; i < len(strategy_names); i++) {
        snprintf(buffer, 20, "ss:%ld:%s", i, i > 0 ? strategy_names[i] : "-");
        ble_service_send_data((uint8_t *)buffer, 20);

    }

    snprintf(buffer, 20, "sens:%hu:%hu", parameters.enabled_distance_sensors, parameters.enabled_line_sensors);
    ble_service_send_data((uint8_t *)buffer, 20);

    snprintf(buffer, 20, "rev:%hu:%hu", parameters.reverse_speed, parameters.reverse_time_ms);
    ble_service_send_data((uint8_t *)buffer, 20);

    snprintf(buffer, 20, "turn:%hu:%hu", parameters.turn_speed, parameters.turn_180_time_ms);
    ble_service_send_data((uint8_t *)buffer, 20);

    snprintf(buffer, 20, "step:%hu", parameters.step_wait_time_ms);
    ble_service_send_data((uint8_t *)buffer, 20);

    snprintf(buffer, 20, "str:%hu:%hu", parameters.current_pre_strategy, parameters.current_strategy);
    ble_service_send_data((uint8_t *)buffer, 20);

    snprintf(buffer, 20, "mms:%hu", parameters.max_speed);
    ble_service_send_data((uint8_t *)buffer, 20);

}
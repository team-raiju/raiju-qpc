#ifndef LOGGER_H
#define LOGGER_H

#include <stdint.h>

typedef union LogData {

    uint8_t data[14];

    struct {
        uint16_t velocity_ms : 13;
        uint16_t target_velocity_ms : 13;

        uint16_t angular_speed_rad_s : 13;
        uint16_t target_rad_s : 13;

        uint16_t angle: 13;

        uint16_t pwm_left : 11;
        uint16_t pwm_right : 11;

        uint16_t battery : 9;

        uint8_t integral_vel : 8;
        uint8_t integral_angular : 8;


    } __attribute__((packed)) fields;
} LogData_t;


typedef struct ParamInfo {
    uint16_t max_store_value; //Max value to store
    float min_param_value; //Min measured value
    float max_param_value; //Max measured value
    float scale; // For max usage efficiency = max_store_value / max_param_value
} ParamInfo_t;

void logger_init();
void logger_reset();
void logger_update();
void logger_save_size();
void logger_save_to_memory();
void logger_print();


#endif /* LOGGER_H */

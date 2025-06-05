/***************************************************************************************************
 * INCLUDES
 **************************************************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "logger.h"
#include "utils.h"
#include "navigation.h"
#include "imu_service.h"
#include "adc_service.h"
#include "bsp.h"
/***************************************************************************************************
 * LOCAL DEFINES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL TYPEDEFS
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL VARIABLES
 **************************************************************************************************/
static LogData_t logdata[7];
static uint8_t log_data_idx;
static uint32_t addr_offset;
static uint8_t ram_logger[30000]; //Max log number is 30000 / sizeof(LogData)

static ParamInfo_t paramInfoArray[] = {
    {8191, -8, 10, 455.0f},     // velocity_ms
    {8191, -8, 10, 455.0f},     // target_velocity_ms
    {8191, -70, 70, 58.0f},     // angular_speed_rad_s
    {8191, -70, 70, 58.0f},     // target_rad_s
    {8191, -180, 180, 22.0f},   // angle
    {2048, -1000, 1000, 1.0f},  // pwm_left
    {2048, -1000, 1000, 1.0f},  // pwm_right
    {512, 35000, 51000, 0.032f},   // battery
    {255, -100, 100, 1.274f},   // integral_vel
    {255, -500, 500, 0.254f},   // integral_angular
};

/***************************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************************/

/***************************************************************************************************
 * LOCAL FUNCTIONS
 **************************************************************************************************/

/***************************************************************************************************
 * GLOBAL FUNCTIONS
 **************************************************************************************************/

void logger_init() {
    memset(ram_logger, 0, sizeof(ram_logger));
    logger_reset();
}

void logger_reset() {
    log_data_idx = 0;
    addr_offset = 0;
}

void logger_save_size() {

}


void logger_update() {
    if (addr_offset + sizeof(logdata) >= sizeof(ram_logger)) {
        return;
    }


    float param_velocity_ms = 0;
    param_velocity_ms *= paramInfoArray[0].scale;

    float param_target_velocity_ms = get_target_linear_speed_m_s() + abs(paramInfoArray[1].min_param_value);
    param_target_velocity_ms *= paramInfoArray[1].scale;

    float param_angular_speed_rad_s = get_imu_ang_vel_rad_s() + abs(paramInfoArray[2].min_param_value);
    param_angular_speed_rad_s *= paramInfoArray[2].scale;

    float param_target_rad_s = get_target_angular_speed_rad_s() + abs(paramInfoArray[3].min_param_value);
    param_target_rad_s *= paramInfoArray[3].scale;

    float angle_rad = get_imu_angle_z() + abs(paramInfoArray[4].min_param_value);
    angle_rad *= paramInfoArray[4].scale;

    float pwm_left = get_pwm_duty_l() + abs(paramInfoArray[5].min_param_value);
    pwm_left *= paramInfoArray[5].scale;

    float pwm_right = get_pwm_duty_r() + abs(paramInfoArray[6].min_param_value);
    pwm_right *= paramInfoArray[6].scale;

    float battery = adc_get_pwr_bat_mv() + abs(paramInfoArray[7].min_param_value);
    battery *= paramInfoArray[7].scale;

    float integral_vel = get_integral_vel() + abs(paramInfoArray[8].min_param_value);
    integral_vel *= paramInfoArray[8].scale;

    float integral_angular = get_integral_angular() + abs(paramInfoArray[9].min_param_value);
    integral_angular *= paramInfoArray[9].scale;

    logdata[log_data_idx].fields.velocity_ms = min(param_velocity_ms, (float)paramInfoArray[0].max_store_value);
    logdata[log_data_idx].fields.target_velocity_ms =
        min(param_target_velocity_ms, (float)paramInfoArray[1].max_store_value);
    logdata[log_data_idx].fields.angular_speed_rad_s =
        min(param_angular_speed_rad_s, (float)paramInfoArray[2].max_store_value);
    logdata[log_data_idx].fields.target_rad_s = min(param_target_rad_s, (float)paramInfoArray[3].max_store_value);
    logdata[log_data_idx].fields.angle = min(angle_rad, (float)paramInfoArray[4].max_store_value);
    logdata[log_data_idx].fields.pwm_left = min(pwm_left, (float)paramInfoArray[5].max_store_value);
    logdata[log_data_idx].fields.pwm_right = min(pwm_right, (float)paramInfoArray[6].max_store_value);
    logdata[log_data_idx].fields.battery = min(battery, (float)paramInfoArray[7].max_store_value);
    logdata[log_data_idx].fields.integral_vel = min(integral_vel, (float)paramInfoArray[8].max_store_value);
    logdata[log_data_idx].fields.integral_angular =
        min(integral_angular, (float)paramInfoArray[9].max_store_value);

    log_data_idx++;
    if (log_data_idx >= (sizeof(logdata) / sizeof(logdata[0]))) {
        log_data_idx = 0;
        memcpy(ram_logger + addr_offset, (uint8_t*)(logdata), sizeof(logdata));
        addr_offset += sizeof(logdata);
    }
}

void logger_save_to_memory() {

}

void logger_print() {
    printf("t;Vel;TargetVel;AngVel;TargetAngVel;Angle;PWM_L;PWM_R,Bat,IntVel,IntW\r\n");
    delay_ms(100);

    uint32_t saved_size = addr_offset;
    // if (bsp::eeprom::read_u32(bsp::eeprom::ADDR_LOGGER_SIZE, &saved_size) != bsp::eeprom::OK) {
    //     std::printf("Error reading logger saved size\r\n");
    //     return;
    // }

    for (uint16_t i = 0; i < saved_size; i += sizeof(LogData_t)) {
        LogData_t read_logdata;

        if (i + sizeof(read_logdata.data) > sizeof(ram_logger)) {
            break;
        }

        memcpy(read_logdata.data, ram_logger + i, sizeof(read_logdata.data));

        uint16_t idx = i / sizeof(LogData_t);

        printf(
            "%d;%0.4f;%0.4f;%0.4f;%0.4f;%0.4f;%0.4f;%0.4f;%0.4f;%0.4f;%0.4f\r\n", idx,
            (read_logdata.fields.velocity_ms / paramInfoArray[0].scale) - abs(paramInfoArray[0].min_param_value),
            (read_logdata.fields.target_velocity_ms / paramInfoArray[1].scale) - abs(paramInfoArray[1].min_param_value),
            (read_logdata.fields.angular_speed_rad_s / paramInfoArray[2].scale) - abs(paramInfoArray[2].min_param_value),
            (read_logdata.fields.target_rad_s / paramInfoArray[3].scale) - abs(paramInfoArray[3].min_param_value),
            (read_logdata.fields.angle / paramInfoArray[4].scale) - abs(paramInfoArray[4].min_param_value),
            (read_logdata.fields.pwm_left / paramInfoArray[5].scale) - abs(paramInfoArray[5].min_param_value),
            (read_logdata.fields.pwm_right / paramInfoArray[6].scale) - abs(paramInfoArray[6].min_param_value),
            (read_logdata.fields.battery / paramInfoArray[7].scale) - abs(paramInfoArray[7].min_param_value),
            (read_logdata.fields.integral_vel / paramInfoArray[8].scale) - abs(paramInfoArray[8].min_param_value),
            (read_logdata.fields.integral_angular / paramInfoArray[9].scale) - abs(paramInfoArray[9].min_param_value));

        delay_ms(25);
    }
}

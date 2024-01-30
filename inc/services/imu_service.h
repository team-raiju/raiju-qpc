#ifndef IMU_SERVICE_H
#define IMU_SERVICE_H

#include <stdint.h>

void imu_service_init(void);

float get_imu_angle_z();

void reset_imu_angle_z();

void reset_inclination();

void start_g_bias_calculation();

void stop_g_bias_calculation();

void imu_set_setpoint(float set_point_);

float imu_get_setpoint();

bool near_set_point();

void imu_set_base_speed(int8_t base_speed_);

int8_t imu_pid_process(int8_t *left_speed, int8_t *right_speed);

bool get_imu_inclination_stat();


#endif /* IMU_SERVICE_H */

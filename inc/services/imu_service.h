#ifndef IMU_SERVICE_H
#define IMU_SERVICE_H

#include <stdint.h>

void imu_service_init(void);

float get_imu_angle_z();

void reset_imu_angle_z();

void start_g_bias_calculation();

void stop_g_bias_calculation();

void imu_set_setpoint(float set_point_);

void imu_set_base_speed(uint8_t base_speed_);

int8_t imu_pid_process(int8_t *left_speed, int8_t *right_speed);


#endif /* IMU_SERVICE_H */
